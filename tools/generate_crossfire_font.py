#!/usr/bin/env python3
"""Bake the Crossfire UTF-8 subset into Gorilla's printable-ASCII font banks.

Requires Pillow and fonttools. Pass a Noto Sans SC 2.004 font with --font for
first generation / new characters. The checked-in OFL subset suffices for
repeat generation. Runtime uses only PNG + Gorilla metadata + Lua mapping.
"""
import argparse
import hashlib
import math
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont
from fontTools import subset
from fontTools.ttLib import TTFont

ROOT = Path(__file__).resolve().parents[1]
DEST = ROOT / "media/fonts/crossfire"
LUA = ROOT / "bin/res/scripts/samples/crossfire_hud_glyphs.lua"
SIZES = {9: (16, 20), 14: (20, 26), 24: (32, 40)}


def source_characters():
    sources = sorted((ROOT / "bin/res/scripts/samples").glob("crossfire_*.lua"))
    sources = [p for p in sources if p != LUA]
    sources.append(ROOT / "bin/res/scripts/samples/Sandbox20.lua")
    return sorted({c for p in sources for c in p.read_text(encoding="utf-8") if ord(c) > 127})


def write_text(path, text):
    path.write_bytes(text.replace("\r\n", "\n").replace("\n", "\r\n").encode("utf-8"))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--font", type=Path, default=DEST / "CrossfireUI-Subset.ttf")
    args = parser.parse_args()
    characters = source_characters()
    font = TTFont(args.font)
    cmap = font.getBestCmap()
    required = set(characters) | {chr(i) for i in range(32, 127)}
    missing = sorted(c for c in required if ord(c) not in cmap)
    if missing:
        raise SystemExit("Font lacks characters; pass the original Noto Sans SC font: " + "".join(missing))
    DEST.mkdir(parents=True, exist_ok=True)
    # Keep an OFL-renamed source subset, so generation works offline on either OS.
    if args.font.resolve() != (DEST / "CrossfireUI-Subset.ttf").resolve():
        options = subset.Options()
        options.name_IDs = [0, 1, 2, 3, 4, 5, 6, 13, 14]
        options.name_legacy = True
        options.recalc_timestamp = False
        sub = subset.Subsetter(options=options)
        sub.populate(unicodes=[ord(c) for c in required])
        sub.subset(font)
        for entry in font["name"].names:
            if entry.nameID in (1, 3, 4, 6):
                value = "CrossfireUI-Subset" if entry.nameID == 6 else "Crossfire UI Subset"
                entry.string = value.encode(entry.getEncoding())
        font.save(DEST / "CrossfireUI-Subset.ttf")
    source = DEST / "CrossfireUI-Subset.ttf"
    image = Image.new("RGBA", (2048, 4096), (255, 255, 255, 0))
    image.paste(Image.open(ROOT / "media/fonts/dejavu/dejavu.png").convert("RGBA"), (0, 0))
    draw = ImageDraw.Draw(image)
    x, y, row_height = 1, 514, 0
    atlas = (ROOT / "media/fonts/dejavu/dejavu.gorilla").read_text().replace(
        "file fonts/dejavu/dejavu.png", "file fonts/crossfire/crossfire.png")
    metadata = []
    for font_id, (size, line_height) in SIZES.items():
        native = ImageFont.truetype(str(source), size)
        ascii_id, bank_id = 100 + font_id, 1000 + font_id * 100
        widths = {}
        groups = [(ascii_id, [chr(i) for i in range(33, 127)])]
        groups.extend((bank_id + n // 94, characters[n:n + 94]) for n in range(0, len(characters), 94))
        space = math.ceil(native.getlength(" "))
        for bank, glyphs in groups:
            atlas += (f"\n[Font.{bank}]\noffset 0 0\nlineheight {line_height}\n"
                      f"spacelength {space}\nbaseline {size}\nkerning 0\nmonowidth {size}\nrange 33 126\n")
            for i in range(94):
                char = glyphs[i] if i < len(glyphs) else "?"
                advance = math.ceil(native.getlength(char))
                box = native.getbbox(char, anchor="ls")
                glyph_width = max(advance, box[2]) + max(0, -box[0])
                if x + glyph_width + 2 > image.width:
                    x = 1; y += row_height + 2; row_height = 0
                if y + line_height >= image.height:
                    raise SystemExit("Atlas overflow")
                draw.text((x + max(0, -box[0]), y + size), char, font=native, anchor="ls", fill=(255, 255, 255, 255))
                atlas += f"glyph_{33+i} {x} {y} {glyph_width} {line_height} {advance}\n"
                if bank == ascii_id:
                    widths[33+i] = advance
                x += glyph_width + 2
                row_height = max(row_height, line_height)
        metadata.append(f" [{font_id}]={{ascii={ascii_id},bank={bank_id},space={space},cjk={size},line={line_height},widths={{" +
                        ",".join(f"[{code}]={advance}" for code, advance in widths.items()) + "}},")
    height = 2 ** math.ceil(math.log2(y + row_height + 2))
    image.crop((0, 0, image.width, height)).save(DEST / "crossfire.png", optimize=True)
    write_text(DEST / "crossfire.gorilla", atlas)
    entries = [f' ["{c}"]={index},' for index, c in enumerate(characters)]
    write_text(LUA, "-- Generated by tools/generate_crossfire_font.py; do not edit.\nreturn {fonts={\n" +
               "\n".join(metadata) + "\n},index={\n" + "\n".join(entries) + "\n}}\n")
    print(f"Crossfire atlas: {len(characters)} Unicode glyphs, {image.width}x{height}, "
          f"source sha256={hashlib.sha256(source.read_bytes()).hexdigest()}")


if __name__ == "__main__":
    main()
