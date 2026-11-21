const fs = require('node:fs');
const path = require('node:path');
const { execFileSync } = require('node:child_process');
const { createHash } = require('node:crypto');
const assert = require('node:assert/strict');

const root = path.resolve(__dirname, '..', 'outputs');
const build = path.join(root, 'plugin-build');
const output = path.join(root, 'unison-study-0.26-alpha.2');
const validator = path.join(build, 'MemoryLabValidate_artefacts/Release/MemoryLabValidate.exe');
const smoke = path.join(build, 'MemoryLabSmoke_artefacts/Release/MemoryLabSmoke.exe');
const bundle = path.join(build, 'MemoryLab_artefacts/Release/VST3/Memory Lab Study.vst3');
const hash = audio => createHash('sha256').update(audio).digest('hex');
fs.mkdirSync(output, { recursive: true });

function capture(name, patch, mode) {
    const stem = path.join(output, name);
    fs.writeFileSync(stem + '.log', execFileSync(validator, ['--capture-stereo-patch', bundle, stem, patch, mode]));
    return ['.f32', '.right.f32'].map(suffix => fs.readFileSync(stem + suffix));
}

const preserved = [];
for (const version of ['filter-modulation-study-0.23', 'oscillator-pwm-study-0.24',
    'voice-path-study-0.25', 'stereo-spread-study-0.26']) {
    const folder = path.join(root, version);
    const study = JSON.parse(fs.readFileSync(path.join(folder, 'measurements.json'), 'utf8'));
    for (const fixture of study.fixtures) {
        const { name } = fixture;
        const stem = 'preserved-' + version.slice(-4) + '-' + name;
        const mode = name === 'combined-filter-routes' ? 'stress' : fixture.sha256 ? 'single' : 'default';
        const [left, right] = capture(stem, path.join(folder, name + '.mlpatch'), mode);
        assert.equal(hash(left), fixture.sha256 ?? fixture.left, stem + ' left');
        assert.equal(hash(right), fixture.sha256 ?? fixture.right, stem + ' right');
        preserved.push(stem);
    }
}

const bank = path.join(output, 'bank');
execFileSync(smoke, ['--factory-patches', bank]);
const template = fs.readFileSync(path.join(bank, '1.mlpatch'), 'utf8');
const fixtures = [];
function render(name, settings) {
    let patch = template;
    const controls = { drive: 0, voiceDrive: 0, volume: .07, cutoff: 3000,
        sawLevel: .4, pulseLevel: 0, noiseLevel: 0, unisonDetune: 0, ...settings };
    for (const [id, value] of Object.entries(controls)) {
        const pattern = new RegExp('(<PARAM id="' + id + '" value=")[^"]+');
        assert.match(patch, pattern);
        patch = patch.replace(pattern, (_, prefix) => prefix + value);
    }
    const file = path.join(output, name + '.mlpatch');
    fs.writeFileSync(file, patch);
    execFileSync(smoke, ['--check-patch-file', file]);
    const audio = capture(name, file, 'single');
    let peak = 0;
    for (const channel of audio) for (let i = 0; i < channel.length; i += 4) {
        const sample = channel.readFloatLE(i);
        assert(Number.isFinite(sample), name);
        peak = Math.max(peak, Math.abs(sample));
    }
    assert(peak > 0 && peak < 1, name + ' headroom');
    fixtures.push({ name, peak, left: hash(audio[0]), right: hash(audio[1]) });
    return audio;
}

const [single] = render('single', {});
const [two] = render('two-detuned', { unisonSize: 1, unisonDetune: 18 });
const [four] = render('four-detuned', { unisonSize: 2, unisonDetune: 18 });
const [left, right] = render('four-detuned-wide', { unisonSize: 2, unisonDetune: 18, stereoSpread: 1 });
assert(!single.equals(two) && !two.equals(four), 'stack sizes must change the detuned note');
assert(!left.equals(right), 'stereo separation');
fs.writeFileSync(path.join(output, 'measurements.json'), JSON.stringify({ preserved, fixtures }, null, 2) + '\n');
console.log('PASS ' + preserved.length + ' earlier stereo captures and ' + fixtures.length + ' stack fixtures');
