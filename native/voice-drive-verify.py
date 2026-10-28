"""Verify the study's saved float WAVs independently of the C++ renderer."""
import hashlib
from html.parser import HTMLParser
import json
import math
from pathlib import Path
import struct
import sys

folder = Path(sys.argv[1])
report = folder / "verification.json"
if report.exists():
    raise SystemExit("Verification report already exists; preserve the run.")
rows = []
for path in sorted(folder.glob("*.wav")):
    data = path.read_bytes()
    assert data[:4] == b"RIFF" and data[8:16] == b"WAVEfmt "
    assert struct.unpack_from("<IHHIIHH", data, 16) == (16, 3, 1, 48000, 192000, 4, 32)
    assert data[36:40] == b"data"
    size = struct.unpack_from("<I", data, 40)[0]
    assert size == 144000 * 4 and len(data) == 44 + size
    audio = struct.unpack_from(f"<{size // 4}f", data, 44)
    assert all(math.isfinite(x) for x in audio)
    rms = math.sqrt(sum(x * x for x in audio) / len(audio))
    peak = max(map(abs, audio))
    if not path.stem.endswith("-raw"):
        assert abs(20 * math.log10(rms) + 24) < 0.001
        assert peak < 0.95
    rows.append(dict(file=path.name, rms_dbfs=20 * math.log10(rms), peak=peak,
                     sha256=hashlib.sha256(data).hexdigest()))
assert len(rows) == 12
html = (folder / "index.html").read_text()
class PlayerParser(HTMLParser):
    players = 0
    def handle_starttag(self, tag, attrs):
        if tag == "audio":
            attrs = dict(attrs)
            assert "controls" in attrs and "autoplay" not in attrs
            assert (folder / attrs["src"]).exists()
            self.players += 1
parser = PlayerParser()
parser.feed(html)
assert parser.players == 6
root = Path(__file__).resolve().parent.parent
sources = [root / "native" / name for name in ("voice.hpp", "voice-drive-study.cpp")]
sources.append(root / "outputs/native-build/memorylab_voice_drive.exe")
report.write_text(json.dumps(dict(clips=rows, hashes={str(p.relative_to(root)):
    hashlib.sha256(p.read_bytes()).hexdigest() for p in sources}), indent=2))
print("PASS: 12 decoded float WAVs; six playback clips at -24 dBFS RMS within .001 dB, peak headroom; six explicit audio controls without autoplay. Hashes recorded.")
