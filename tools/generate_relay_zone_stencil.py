"""Generate the project-authored A1 wall stencil used by Sandbox19."""

from pathlib import Path

from PIL import Image, ImageDraw


SIZE = (1024, 512)
SCALE = 4
OUTPUT = Path(__file__).resolve().parents[1] / 'media/textures/sandbox19/zone_a1_stencil.png'


def scaled(points):
    return [(x * SCALE, y * SCALE) for x, y in points]


def main():
    mask = Image.new('L', (SIZE[0] * SCALE, SIZE[1] * SCALE), 0)
    draw = ImageDraw.Draw(mask)
    paint = 210
    for polygon in (
        [(128, 430), (280, 70), (342, 70), (205, 430)],
        [(280, 70), (342, 70), (500, 430), (418, 430)],
        [(217, 288), (404, 288), (426, 338), (196, 338)],
        [(575, 159), (652, 70), (714, 70), (714, 430), (632, 430), (632, 178), (575, 212)],
        [(565, 394), (768, 394), (768, 430), (565, 430)],
    ):
        draw.polygon(scaled(polygon), fill=paint)
    mask = mask.resize(SIZE, Image.Resampling.LANCZOS)
    image = Image.new('RGBA', SIZE, (43, 46, 41, 0))
    image.putalpha(mask)
    # The existing vertical VisualPlane maps its U axis upward in world space.
    image = image.transpose(Image.Transpose.ROTATE_90)
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    image.save(OUTPUT, optimize=True)
    print(OUTPUT)


if __name__ == '__main__':
    main()
