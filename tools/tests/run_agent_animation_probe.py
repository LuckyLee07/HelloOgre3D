#!/usr/bin/env python3
"""Run the animation contract probe in an isolated macOS resource tree.

Requires a matching Release bin/HelloOgre3D. No tracked sample is replaced.
Usage: python3 tools/tests/run_agent_animation_probe.py --rates 30 60 120
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import time
from check_agent_animation import check


def main():
    root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rates', type=int, nargs='+', default=[30, 60, 120], choices=[30, 60, 120])
    parser.add_argument('--output', type=Path, default=root / 'tmp' / time.strftime('animation-probe-%Y%m%d-%H%M%S'))
    args = parser.parse_args()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    runtime = output / 'runtime'
    rbin = runtime / 'bin'
    rbin.mkdir(parents=True)
    (runtime / 'media').symlink_to(root / 'media', target_is_directory=True)
    for name in ('HelloOgre3D', 'Sandbox.cfg', 'SandboxResources.cfg'):
        shutil.copy2(root / 'bin' / name, rbin / name)
    (rbin / 'res').mkdir()
    for path in (root / 'bin/res').iterdir():
        if path.name == 'scripts':
            shutil.copytree(path, rbin / 'res/scripts')
        else:
            (rbin / 'res' / path.name).symlink_to(path, target_is_directory=path.is_dir())
    shutil.copyfile(Path(__file__).with_name('agent_animation_probe.lua'), rbin / 'res/scripts/samples/Sandbox3.lua')
    replay = output / 'replay.txt'
    replay.write_text('20000 quit\n') # Enables real WeaponShot diagnostics; Lua owns termination.
    binary_hash = hashlib.sha256((rbin / 'HelloOgre3D').read_bytes()).hexdigest()
    for rate in args.rates:
        directory = output / ('probe-' + str(rate))
        directory.mkdir()
        env = {k: v for k, v in os.environ.items() if not k.startswith('HELLO_')}
        env.update(HELLO_SANDBOX_SAMPLE='Sandbox3', HELLO_SIM_HZ='30', HELLO_WINDOW_BACKGROUND='1',
                   HELLO_AUDIO_SILENT='1', HELLO_INPUT_REPLAY=str(replay), HELLO_WINDOW_WIDTH='960',
                   HELLO_WINDOW_HEIGHT='540', HELLO_ANIMATION_TRACE='1', HELLO_ANIMATION_POSE_TRACE='1',
                   HELLO_RENDER_VSYNC='0', HELLO_RENDER_MAX_FPS=str(rate), HELLO_RENDER_CAPTURE='1',
                   HELLO_RENDER_CAPTURE_DIR=str(directory), HELLO_RENDER_CAPTURE_MS='3500,4100,4600,5000,5400',
                   HELLO_RENDER_CAPTURE_CLOCK='simulation')
        with (directory / 'stdout.log').open('wb') as stream:
            child = subprocess.Popen([str(rbin / 'HelloOgre3D')], cwd=rbin, env=env, stdout=stream, stderr=subprocess.STDOUT)
            try:
                code = child.wait(timeout=32)
            except subprocess.TimeoutExpired:
                child.terminate()
                try:
                    child.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    child.kill(); child.wait()
                code = 'timeout'
        metadata = dict(exit_code=code, binary_sha256=binary_hash, environment={k: v for k, v in env.items() if k.startswith('HELLO_')})
        (directory / 'metadata.json').write_text(json.dumps(metadata, indent=2))
        assert code == 0, f'{directory}: process {code}; inspect stdout.log'
        print(f'[AnimationProbe] rate={rate} completed log={directory / "stdout.log"}', flush=True)
    result = check(output)
    (output / 'results.json').write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
