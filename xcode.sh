#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR/premake"

if ! command -v premake5 >/dev/null 2>&1; then
  echo "premake5 not found. Install with: brew install premake"
  exit 1
fi

premake5 --file=premake.lua xcode4

echo "Xcode project generated under build/."
