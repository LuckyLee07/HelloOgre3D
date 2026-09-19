"""Build original Crossfire mechanical short sounds with no external recordings.

The generator is deterministic, uses only the Python standard library, and emits
44.1 kHz mono PCM16 WAVs. RuntimeUiSound supplies gain, pan and master volume.
Use --check to compare checked-in assets without rewriting them.
"""
import argparse
import hashlib
import io
import math
from pathlib import Path
import random
import struct
import wave

RATE = 44100
ROOT = Path(__file__).resolve().parents[1] / "bin/res/audio/crossfire"
TAU = math.tau


def blank(seconds):
    return [0.0] * round(seconds * RATE)


def add_noise(out, start, duration, gain, decay, seed, lowpass=5000, highpass=90):
    """Band-limited transient: impact grit, pressure body or mechanical air."""
    rng = random.Random(seed)
    lo, slow = 0.0, 0.0
    a = 1 - math.exp(-TAU * lowpass / RATE)
    b = 1 - math.exp(-TAU * highpass / RATE)
    begin = round(start * RATE)
    for n in range(min(round(duration * RATE), len(out) - begin)):
        t = n / RATE
        lo += a * (rng.uniform(-1, 1) - lo)
        slow += b * (lo - slow)
        attack = min(1.0, t / .0009)
        tail = min(1.0, max(0.0, duration - t) / .015)
        out[begin + n] += gain * attack * tail * math.exp(-t / decay) * (lo - slow)


def add_tone(out, start, duration, hz, end_hz, gain, decay, harmonics=(1, .22, .08)):
    """Damped motor/resonator with a pitch fall and small overtone beating."""
    begin, phase = round(start * RATE), 0.0
    for n in range(min(round(duration * RATE), len(out) - begin)):
        t = n / RATE
        f = end_hz + (hz - end_hz) * math.exp(-t / max(.012, duration * .22))
        phase += TAU * f / RATE
        body = sum(level * math.sin(phase * (i + 1) + .08 * math.sin(TAU * 23 * t) * i)
                   for i, level in enumerate(harmonics))
        attack = min(1.0, t / .0025)
        tail = min(1.0, max(0.0, duration - t) / .04)
        out[begin + n] += gain * attack * tail * math.exp(-t / decay) * body


def add_metal(out, start, scale, gain, seed, duration=.28):
    """Inharmonic damped modes make a small machined plate/casing response."""
    rng = random.Random(seed)
    for index, ratio in enumerate((1.0, 1.47, 2.13, 3.38, 4.71)):
        add_tone(out, start + index * .0004, duration, scale * ratio, scale * ratio * .995,
                 gain * rng.uniform(.8, 1.0) / (1 + index * 1.6),
                 .08 / (1 + index * .5), (1,))


def latch(out, start, scale=1.0, seed=1):
    add_noise(out, start, .07, .34, .006, seed, 6400, 850)
    add_metal(out, start, 870 * scale, .14, seed + 1, .16)
    add_tone(out, start, .08, 230 * scale, 145 * scale, .12, .013, (1, .3))


def make_sounds():
    sounds = {}
    s = blank(.19)
    latch(s, 0, 1.12, 101)
    add_noise(s, .022, .1, .07, .028, 102, 3400, 1400)
    sounds["select"] = (s, .24)

    s = blank(.31)
    latch(s, 0, .87, 111)
    latch(s, .08, 1.21, 112)
    add_tone(s, .075, .22, 520, 515, .06, .065, (1, .12, .06))
    sounds["order"] = (s, .26)

    s = blank(.34)
    # Pressure crack + low body + delayed action cycling: one actual round.
    add_noise(s, 0, .1, 1.25, .012, 121, 8900, 550)
    add_noise(s, .003, .25, .5, .05, 122, 1350, 65)
    add_tone(s, 0, .2, 220, 79, .6, .038, (1, .2, .06))
    add_metal(s, .042, 960, .2, 123, .22)
    add_noise(s, .082, .12, .29, .014, 124, 4500, 650)
    add_metal(s, .093, 1530, .07, 125, .16)
    sounds["shot"] = (s, .36)

    s = blank(.48)
    # Shield has a glassy dispersed resonance; no low mechanical damage thud.
    add_noise(s, 0, .1, .28, .009, 131, 7400, 2700)
    for hz, level in ((680, .23), (1117, .16), (1783, .09), (2670, .055)):
        add_tone(s, .002, .46, hz * 1.18, hz, level, .12, (1, .12))
    add_noise(s, .055, .35, .13, .1, 132, 4500, 1800)
    sounds["shield"] = (s, .30)

    s = blank(.32)
    add_noise(s, 0, .12, .7, .007, 141, 7200, 1300)
    add_tone(s, .002, .17, 174, 98, .23, .033, (1, .3))
    add_metal(s, .001, 520, .35, 142, .28)
    add_metal(s, .065, 1370, .09, 143, .2)
    sounds["hit"] = (s, .30)

    s = blank(.93)
    add_noise(s, 0, .6, .85, .1, 151, 2600, 40)
    add_tone(s, 0, .52, 154, 43, .5, .105, (1, .32, .11))
    for index, when in enumerate((.016, .073, .154, .281, .427)):
        add_noise(s, when, .08, .6 / (1 + index), .009, 152 + index, 7300, 1200)
        add_metal(s, when, 760 + index * 113, .18 / (1 + index * .4), 160 + index, .35)
    add_tone(s, .11, .72, 980, 135, .14, .14, (1, .1, .035))
    sounds["destroy"] = (s, .36)

    s = blank(1.05)
    latch(s, 0, .78, 171)
    add_noise(s, .04, .76, .12, .25, 172, 1300, 160)
    add_tone(s, .07, .72, 80, 220, .16, .31, (1, .3, .14, .06))
    for index, hz in enumerate((330, 440, 660)):
        add_metal(s, .17 + index * .15, hz, .18, 173 + index, .48)
    sounds["start"] = (s, .28)

    s = blank(1.52)
    add_noise(s, 0, .63, .16, .18, 181, 1200, 130)
    for index, hz in enumerate((330, 440, 660)):
        when = index * .17
        latch(s, when, .85 + index * .16, 182 + index)
        add_tone(s, when, 1.12, hz, hz, .15, .34, (1, .28, .1, .04))
        add_metal(s, when, hz * 2.02, .07, 186 + index, .55)
    sounds["win"] = (s, .29)

    s = blank(1.34)
    add_noise(s, 0, .72, .2, .2, 191, 2300, 85)
    add_tone(s, 0, 1.2, 250, 54, .23, .37, (1, .26, .09))
    for index, hz in enumerate((392, 311, 196)):
        add_metal(s, index * .19, hz, .12, 192 + index, .49)
    latch(s, .82, .54, 199)
    sounds["lose"] = (s, .27)
    return sounds


def encode(samples, peak):
    # Remove DC; use a short onset ramp to keep impact attacks, a 12 ms tail,
    # and conservative peaks to leave headroom for overlapping voices.
    mean = sum(samples) / len(samples)
    samples = [value - mean for value in samples]
    fade = round(.012 * RATE)
    for index in range(fade):
        samples[-index - 1] *= index / fade
    attack = round(.0006 * RATE)
    for index in range(attack):
        samples[index] *= index / attack
    factor = peak / max(abs(value) for value in samples)
    pcm = b"".join(struct.pack("<h", round(value * factor * 32767)) for value in samples)
    result = io.BytesIO()
    with wave.open(result, "wb") as out:
        out.setparams((1, 2, RATE, 0, "NONE", "not compressed"))
        out.writeframes(pcm)
    return result.getvalue()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.check:
        args.output.mkdir(parents=True, exist_ok=True)
    failures = []
    for name, (samples, peak) in make_sounds().items():
        data = encode(samples, peak)
        path = args.output / (name + ".wav")
        if args.check:
            if not path.is_file() or path.read_bytes() != data:
                failures.append(name)
        else:
            path.write_bytes(data)
        print(f"{name:8s} {len(samples) / RATE:.2f}s  peak={peak:.2f}  sha256={hashlib.sha256(data).hexdigest()}")
    if failures:
        raise SystemExit("Generated audio differs: " + ", ".join(failures))
    print("Verified" if args.check else "Generated", "9 original PCM16 WAV files in", args.output)


if __name__ == "__main__":
    main()
