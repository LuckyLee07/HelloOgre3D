#!/usr/bin/env python3
from __future__ import annotations

import argparse
import base64
from pathlib import Path


def _decode(text: str) -> str:
    return base64.b64decode(text.encode("ascii")).decode("utf-8")


def cmd_write(args: argparse.Namespace) -> int:
    path = Path(args.file)
    path.parent.mkdir(parents=True, exist_ok=True)
    data = _decode(args.content_b64)
    path.write_text(data, encoding="utf-8")
    print(f"wrote: {path}")
    return 0


def cmd_replace(args: argparse.Namespace) -> int:
    path = Path(args.file)
    if not path.exists():
        print(f"file not found: {path}")
        return 2

    text = path.read_text(encoding="utf-8")
    old = _decode(args.old_b64)
    new = _decode(args.new_b64)

    count = text.count(old)
    if count == 0:
        print("pattern not found")
        return 3

    if args.expected_count is not None and count != args.expected_count:
        print(f"unexpected match count: {count}, expected: {args.expected_count}")
        return 4

    replaced = text.replace(old, new, args.max_replacements if args.max_replacements > 0 else -1)
    path.write_text(replaced, encoding="utf-8")
    print(f"updated: {path} (matches: {count})")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Codex local file operations")
    sub = parser.add_subparsers(dest="cmd", required=True)

    w = sub.add_parser("write")
    w.add_argument("--file", required=True)
    w.add_argument("--content-b64", required=True)
    w.set_defaults(func=cmd_write)

    r = sub.add_parser("replace")
    r.add_argument("--file", required=True)
    r.add_argument("--old-b64", required=True)
    r.add_argument("--new-b64", required=True)
    r.add_argument("--expected-count", type=int)
    r.add_argument("--max-replacements", type=int, default=0)
    r.set_defaults(func=cmd_replace)

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
