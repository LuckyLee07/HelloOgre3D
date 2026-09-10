#!/usr/bin/env python3
"""Bounded desktop smoke for M1 and its related samples (macOS executable).

Terminates only the child it starts after the observation window. This verifies
runtime evidence, not graceful shutdown or complete-match stability.
"""
import argparse
import os
from pathlib import Path
import re
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
REQUIRED = {
    'Sandbox19': ['[Sandbox19ProductSelfTest] PASS all=true',
                  '[Sandbox19ArenaSelfTest] PASS all-spawns-connected',
                  '[Sandbox19ArenaSelfTest] PASS relay-static-collision'],
    'Sandbox6': ['[ConfigManager] preset=Sandbox6 sample=Sandbox6'],
    'Sandbox7': ['[ConfigManager] preset=Sandbox7 sample=Sandbox7'],
    'Sandbox8': ['[ConfigManager] preset=Sandbox8 sample=Sandbox8'],
    'Sandbox12': ['[TeamBlackboardSmoke] PASS', '[TeamBlackboardLifecycleSelfTest] PASS'],
    'Sandbox17': ['[Chapter9TacticsSmoke] PASS'],
}
ERROR = re.compile(r'call_func error|call_string error|Assertion failed|OGRE EXCEPTION|PANIC:|stack traceback|\]\s+FAIL\b|self test result:\s*false')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--samples', nargs='+', choices=REQUIRED, default=['Sandbox19', 'Sandbox8', 'Sandbox12', 'Sandbox17'])
    parser.add_argument('--seconds', type=float, default=30)
    args = parser.parse_args()
    if args.seconds <= 0:
        parser.error('--seconds must be positive')
    output = ROOT / 'tmp' / ('m1-smoke-' + time.strftime('%Y%m%d-%H%M%S'))
    output.mkdir(parents=True, exist_ok=False)
    failed = False
    for sample in args.samples:
        env = {k: v for k, v in os.environ.items() if not k.startswith('HELLO_')}
        env['HELLO_WINDOW_BACKGROUND'] = '1'
        env['HELLO_AUDIO_SILENT'] = '1'
        env.update(HELLO_SANDBOX_SAMPLE=sample, HELLO_SANDBOX_SMOKE_TEST='1')
        if sample == 'Sandbox19':
            env['HELLO_SANDBOX19_PRODUCT_TEST'] = '1'
        log = output / (sample + '.log')
        early_exit = None
        with log.open('wb') as stream:
            child = subprocess.Popen([str(ROOT / 'bin/HelloOgre3D')], cwd=ROOT / 'bin',
                                     env=env, stdout=stream, stderr=subprocess.STDOUT)
            try:
                try:
                    early_exit = child.wait(timeout=args.seconds)
                except subprocess.TimeoutExpired:
                    pass
            finally:
                if child.poll() is None:
                    child.terminate()
                    try:
                        child.wait(timeout=5)
                    except subprocess.TimeoutExpired:
                        child.kill()
                        child.wait()
        text = log.read_text(errors='replace')
        missing = [marker for marker in REQUIRED[sample] if marker not in text]
        errors = [line for line in text.splitlines() if ERROR.search(line)]
        ok = early_exit is None and not missing and not errors
        failed |= not ok
        print(f'[M1Smoke] sample={sample} status={"PASS" if ok else "FAIL"} log={log}', flush=True)
        if not ok:
            print(f'  early_exit={early_exit} missing={missing} errors={errors[:5]}', flush=True)
    return int(failed)


if __name__ == '__main__':
    raise SystemExit(main())
