#!/usr/bin/env bash
set -euo pipefail
project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${project_root}/bin"
export HELLO_SANDBOX_SAMPLE=Sandbox20
export HELLO_SAMPLE_PRESET=Sandbox20
export HELLO_RENDER_FSAA="${HELLO_RENDER_FSAA:-4}"
exec ./HelloOgre3D
