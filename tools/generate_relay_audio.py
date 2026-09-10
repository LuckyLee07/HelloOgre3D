"""Generate original, deterministic Relay Outpost PCM feedback (no source recordings)."""
import math
from pathlib import Path
import random
import struct
import wave

RATE = 22050
ROOT = Path(__file__).resolve().parents[1] / "bin/res/audio/relay"
TONES = {
    "select": [(650, .045), (850, .055)],
    "accepted": [(520, .07), (780, .11)],
    "failed": [(260, .11), (190, .12)],
    "start": [(390, .1), (520, .1), (780, .18)],
    "victory": [(523, .16), (659, .16), (784, .16), (1047, .4)],
    "defeat": [(392, .18), (330, .18), (262, .35)],
    "pause": [(440, .1)],
}


def write(name, samples):
    with wave.open(str(ROOT / (name + ".wav")), "wb") as out:
        out.setparams((1, 2, RATE, 0, "NONE", "not compressed"))
        out.writeframes(b"".join(struct.pack("<h", max(-32767, min(32767, int(s * 32767)))) for s in samples))


def main():
    ROOT.mkdir(parents=True, exist_ok=True)
    for name, notes in TONES.items():
        samples = []
        for frequency, seconds in notes:
            length = int(seconds * RATE)
            for n in range(length):
                t = n / RATE
                envelope = min(1, t / .008) * min(1, (seconds - t) / .035)
                samples.append(.20 * envelope * (math.sin(2 * math.pi * frequency * t) + .18 * math.sin(4 * math.pi * frequency * t)))
        write(name, samples)
    rng = random.Random(19)
    for name, duration, frequency in [("shot", .07, 150), ("hit", .09, 75)]:
        samples = []
        for n in range(int(duration * RATE)):
            t = n / RATE
            samples.append(.22 * math.exp(-t * 65) * (rng.uniform(-1, 1) + .45 * math.sin(2 * math.pi * frequency * t)))
        write(name, samples)
    print("Generated 9 original PCM WAV files in", ROOT)


if __name__ == "__main__":
    main()
