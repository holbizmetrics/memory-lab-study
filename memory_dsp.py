"""Original output-drive hypotheses. Models are not claims about Relic circuitry."""
import json
import math
from pathlib import Path
import wave

import numpy as np
from scipy.io import wavfile
from scipy.signal import lfilter, resample_poly

STRENGTHS = (2., 5., 12.)


def parameters(expression, variant):
    p = json.loads(expression)
    if not isinstance(p, dict) or set(p) != {'kind', 'bias', 'depth', 'attack_ms', 'release_ms'}:
        raise ValueError('parameter fields must match the contract')
    kinds = {'v1': 'static', 'v2': 'biased', 'v3': 'sag'}
    if p['kind'] != kinds[variant]:
        raise ValueError('variant must preserve its assigned hypothesis')
    for name in ('bias', 'depth', 'attack_ms', 'release_ms'):
        if type(p[name]) not in (int, float) or not math.isfinite(p[name]):
            raise ValueError('parameters must be finite numbers')
    if not 0 <= p['bias'] <= .08 or not 0 <= p['depth'] <= 1.5:
        raise ValueError('bias/depth outside declared bounds')
    if not 2 <= p['attack_ms'] <= 30 or not 50 <= p['release_ms'] <= 400:
        raise ValueError('follower times outside declared bounds')
    if p['kind'] == 'static' and (p['bias'] or p['depth']):
        raise ValueError('static reference must be unbiased and memoryless')
    if p['kind'] == 'biased' and (p['depth'] or p['bias'] < .005):
        raise ValueError('biased variant must have bias and no follower depth')
    if p['kind'] == 'sag' and (p['bias'] or p['depth'] < .2):
        raise ValueError('sag variant must have follower depth and no bias')
    return p


def process(x, sr, drive, p):
    if drive == 0:
        return np.array(x, copy=True)
    if p['kind'] == 'static':
        return np.tanh(drive*x)/drive
    if p['kind'] == 'biased':
        center = math.tanh(drive*p['bias'])
        slope = max(1-center*center, 1e-6)
        return (np.tanh(drive*(x+p['bias']))-center)/(drive*slope)
    follower = 0.
    attack = math.exp(-1/(sr*p['attack_ms']*.001))
    release = math.exp(-1/(sr*p['release_ms']*.001))
    out = np.empty_like(x)
    # Fixed calibration scale, shared across candidates; no per-note matching.
    scale = .25
    for i, sample in enumerate(x):
        level = abs(float(sample))/scale
        coefficient = attack if level > follower else release
        follower = coefficient*follower + (1-coefficient)*level
        gain = 1/(1+p['depth']*follower)
        out[i] = math.tanh(drive*gain*float(sample))/drive
    return out


def render(source, sr, drive, p):
    high = resample_poly(source, 4, 1, window=('kaiser', 8.6))
    driven = process(high, sr*4, drive, p)
    low = resample_poly(driven, 1, 4, window=('kaiser', 8.6))[:len(source)]
    pole = math.exp(-2*math.pi*8/sr)
    raw = lfilter([1., -1.], [1., -pole], low)
    if not np.all(np.isfinite(raw)):
        raise ValueError('nonfinite audio')
    rms = float(np.sqrt(np.mean(raw**2)))
    if rms < 1e-12:
        raise ValueError('silent render')
    gain = 10**(-24/20)/rms
    matched = raw*gain
    if np.max(np.abs(matched)) >= .98:
        raise ValueError('RMS matching would exceed playback headroom; no limiter used')
    return raw, matched, gain


def metrics(audio, sr):
    power = np.abs(np.fft.rfft(audio*np.hanning(len(audio))))**2
    freq = np.fft.rfftfreq(len(audio), 1/sr)
    return {'rms_dbfs': float(20*np.log10(max(np.sqrt(np.mean(audio**2)), 1e-15))),
            'peak_dbfs': float(20*np.log10(max(np.max(np.abs(audio)), 1e-15))),
            'dc': float(np.mean(audio)),
            'spectral_centroid_hz': float(np.sum(freq*power)/max(np.sum(power), 1e-20)),
            'energy_above_5khz_fraction': float(np.sum(power[freq>5000])/max(np.sum(power), 1e-20))}


def memory_probe(p):
    sr = 48000
    probe = np.full(2400, .1)
    silence = np.zeros(2400)
    conditioning = np.full(2400, .7)
    a = process(np.concatenate((silence, probe)), sr, 5., p)[2400:]
    b = process(np.concatenate((conditioning, probe)), sr, 5., p)[2400:]
    # Measured before the common DC blocker, which itself has memory.
    return float(np.sqrt(np.mean((a-b)**2)))


def write_pcm(path, audio, sr):
    ints = np.rint(audio*(2**23-1)).astype(np.int32)
    packed = np.empty((len(ints), 3), dtype=np.uint8)
    for byte in range(3):
        packed[:, byte] = (ints >> (byte*8)) & 255
    with wave.open(str(path), 'wb') as f:
        f.setnchannels(1); f.setsampwidth(3); f.setframerate(sr); f.writeframes(packed.tobytes())


def load_source(path):
    sr, audio = wavfile.read(path)
    if sr != 48000 or audio.ndim != 1 or audio.dtype.kind != 'f':
        raise ValueError('expected companion mono 48 kHz float raw clean source')
    audio = audio.astype(np.float64)
    if not np.all(np.isfinite(audio)):
        raise ValueError('invalid source audio')
    return sr, audio


def evaluate(candidate, source_path, output):
    """Numerical admissibility and diagnostic history test, never musical ranking."""
    try:
        p = parameters(candidate['expression'], candidate['variant'])
        sr, source = load_source(source_path)
        output.mkdir(parents=True, exist_ok=False)
        probe = memory_probe(p)
        if p['kind'] != 'sag' and probe != 0 or p['kind'] == 'sag' and probe <= 1e-7:
            raise ValueError('model does not exhibit its declared memory behavior')
        if not np.array_equal(process(source, sr, 0, p), source):
            raise ValueError('zero-drive null failed')
        clips = []
        for drive in STRENGTHS:
            raw, matched, gain = render(source, sr, drive, p)
            name = f'drive-{drive:g}'
            wavfile.write(output/(name+'-raw.wav'), sr, raw.astype(np.float32))
            write_pcm(output/(name+'.wav'), matched, sr)
            clips.append({'drive': drive, 'file': str(output/(name+'.wav')), 'seconds': len(matched)/sr,
                          'gain': gain, 'raw_metrics': metrics(raw, sr), 'matched_metrics': metrics(matched, sr)})
        record = {'ok': True, 'score': 1, 'parameters': p, 'history_probe_rms_difference': probe,
                  'clips': clips, 'musical_verdict': 'UNASSESSED: requires listening'}
        (output/'measurements.json').write_text(json.dumps(record, indent=2), encoding='utf-8')
        return record
    except (ValueError, KeyError, OSError, TypeError) as error:
        return {'ok': False, 'score': 0, 'error': str(error), 'musical_verdict': 'NOT_EVALUATED'}
