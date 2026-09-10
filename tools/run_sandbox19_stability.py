#!/usr/bin/env python3
"""Run the Relay Outpost synthetic product fixture with --product-fixture.

The old three-wave stability/probe/low-health runner has been retired. Its flags
now fail before starting a process; ordinary play waits for the player's START.
The product fixture changes health/positions and kills actors deliberately. It
checks real BT execution and lifecycle, not natural matches or external input.
This runner terminates its own child after evidence collection; graceful window
closure and normal-play stability remain separate checks.
"""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import time
import tempfile

ROOT = Path(__file__).resolve().parents[1]
ERROR = re.compile(r'call_func error|call_string error|lua_pcall error|Assertion failed|'
                   r'OGRE EXCEPTION|PANIC:|stack traceback|\]\s+FAIL\b|'
                   r'self test result:\s*false|RuntimeDiagProbeCleanup\] result=false')
REQUIRED = (
    '[Sandbox19ProductSelfTest] PASS all=true synthetic=true',
    '[Sandbox19ArenaSelfTest] PASS all-spawns-connected relay-routes',
    '[Sandbox19ArenaSelfTest] PASS relay-static-collision',
)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--product-fixture', action='store_true',
                        help='explicitly opt into the new synthetic product gate')
    parser.add_argument('--timeout', type=int, default=90, help='wall time limit in seconds (default: 90)')
    # Recognize legacy calls only to explain the changed contract, never silently
    # replace their ordinary-play meaning with synthetic success.
    parser.add_argument('--rounds', type=int, help=argparse.SUPPRESS)
    parser.add_argument('--timeout-per-round', type=int, help=argparse.SUPPRESS)
    parser.add_argument('--probe', action='store_true', help=argparse.SUPPRESS)
    parser.add_argument('--scripted-victory', action='store_true', help=argparse.SUPPRESS)
    parser.add_argument('--low-health', action='store_true', help=argparse.SUPPRESS)
    parser.add_argument('--executable', type=Path, default=ROOT / 'bin' /
                        ('HelloOgre3D.exe' if os.name == 'nt' else 'HelloOgre3D'))
    args = parser.parse_args()
    if (not args.product_fixture or args.rounds is not None or args.timeout_per_round is not None
            or args.probe or args.scripted_victory or args.low_health):
        parser.error('the old three-wave stability gate is retired; use --product-fixture --timeout 90 '
                     'for the explicitly synthetic Relay Outpost product gate. This does not validate '
                     'ordinary complete matches, probe cleanup or the old low-health scenario. No game was started.')
    if args.timeout < 10:
        parser.error('timeout must be at least 10 seconds')
    (ROOT / 'tmp').mkdir(exist_ok=True)
    output = Path(tempfile.mkdtemp(prefix='relay-product-fixture-' + time.strftime('%Y%m%d-%H%M%S') + '-',
                                   dir=ROOT / 'tmp'))
    log = output / 'Sandbox19.log'
    env = {k: v for k, v in os.environ.items() if not k.startswith('HELLO_')}
    env['HELLO_WINDOW_BACKGROUND'] = '1'
    env['HELLO_AUDIO_SILENT'] = '1'
    env.update(HELLO_SANDBOX_SAMPLE='Sandbox19', HELLO_SANDBOX19_PRODUCT_TEST='1')
    deadline = time.monotonic() + args.timeout
    exit_before_evidence = None
    reason = 'wall-timeout'
    text = ''
    with log.open('wb') as stream:
        child = subprocess.Popen([str(args.executable.resolve())], cwd=ROOT / 'bin',
                                 env=env, stdout=stream, stderr=subprocess.STDOUT,
                                 creationflags=subprocess.CREATE_NO_WINDOW if os.name == 'nt' else 0)
        print(f'[RelayProductGate] synthetic=true pid={child.pid} log={log}', flush=True)
        try:
            while time.monotonic() < deadline:
                text = log.read_text(errors='replace')
                exit_before_evidence = child.poll()
                if ERROR.search(text):
                    failure = re.search(r'\[Sandbox19ProductSelfTest\] FAIL case=([^\s]+)', text)
                    reason = failure.group(1) if failure else 'runtime-error'
                    break
                if all(marker in text for marker in REQUIRED):
                    reason = 'evidence-complete' if exit_before_evidence in (None, 0) else 'nonzero-exit'
                    break
                if exit_before_evidence is not None:
                    reason = 'early-exit'
                    break
                time.sleep(0.2)
        finally:
            if child.poll() is None:
                child.terminate()
                try:
                    child.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    child.kill()
                    child.wait()
    text = log.read_text(errors='replace')
    errors = [line for line in text.splitlines() if ERROR.search(line)]
    missing = [marker for marker in REQUIRED if marker not in text]
    passed = reason == 'evidence-complete' and not errors and not missing
    summary = dict(mode='relay-product-fixture', synthetic=True, executable=str(args.executable.resolve()), status='PASS' if passed else 'FAIL',
                   reason=reason, early_exit=exit_before_evidence,
                   graceful_shutdown_verified=False, natural_play_verified=False,
                   external_input_verified=False, missing_markers=missing, errors=errors,
                   evidence=[line for line in text.splitlines()
                             if any(marker in line for marker in ('[Sandbox19ProductSelfTest]', '[Sandbox19ArenaSelfTest]',
                                                                 '[Sandbox19Order]'))])
    (output / 'summary.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2) + '\n')
    print(f'[RelayProductGate] synthetic=true status={summary["status"]} reason={reason} summary={output / "summary.json"}', flush=True)
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
