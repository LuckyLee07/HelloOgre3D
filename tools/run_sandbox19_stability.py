#!/usr/bin/env python3
"""Run complete Sandbox19 matches and restarts in a desktop session.

Ordinary/probe runs do not issue combat commands or kill units. The separately
labelled scripted-victory mode exercises all-wave victory/restart transitions.
The child is terminated after evidence collection; this is not a window-close test.
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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rounds', type=int, default=3)
    parser.add_argument('--timeout-per-round', type=int, default=120)
    parser.add_argument('--probe', action='store_true')
    parser.add_argument('--scripted-victory', action='store_true')
    parser.add_argument('--executable', type=Path, default=ROOT / 'bin' /
                        ('HelloOgre3D.exe' if os.name == 'nt' else 'HelloOgre3D'))
    args = parser.parse_args()
    if args.rounds < 1 or args.timeout_per_round < 10:
        parser.error('rounds must be positive and timeout-per-round at least 10 seconds')
    mode = 'scripted-victory' if args.scripted_victory else 'ordinary'
    if args.probe:
        mode += '-probe'
    (ROOT / 'tmp').mkdir(exist_ok=True)
    output = Path(tempfile.mkdtemp(prefix='stability-' + time.strftime('%Y%m%d-%H%M%S') + '-' + mode + '-',
                                   dir=ROOT / 'tmp'))
    log = output / 'Sandbox19.log'
    env = {k: v for k, v in os.environ.items() if not k.startswith('HELLO_')}
    env.update(HELLO_SANDBOX_SAMPLE='Sandbox19',
               HELLO_SANDBOX19_STABILITY_ROUNDS=str(args.rounds),
               HELLO_SANDBOX19_STABILITY_TIMEOUT=str(args.timeout_per_round))
    if args.probe:
        env['HELLO_RUNTIME_DIAGNOSTIC_SELF_TEST'] = '1'
    if args.scripted_victory:
        env['HELLO_SANDBOX19_STABILITY_SCRIPTED_VICTORY'] = '1'
    expected = '[Sandbox19Stability] PASS rounds=' + str(args.rounds) + ' '
    deadline = time.monotonic() + args.rounds * args.timeout_per_round + 30
    exit_before_evidence = None
    reason = 'wall-timeout'
    text = ''
    with log.open('wb') as stream:
        child = subprocess.Popen([str(args.executable.resolve())], cwd=ROOT / 'bin',
                                 env=env, stdout=stream, stderr=subprocess.STDOUT)
        print(f'[StabilityRunner] mode={mode} pid={child.pid} log={log}', flush=True)
        try:
            while time.monotonic() < deadline:
                text = log.read_text(errors='replace')
                exit_before_evidence = child.poll()
                if ERROR.search(text):
                    failure = re.search(r'\[Sandbox19Stability\] FAIL reason=([^\s]+)', text)
                    reason = failure.group(1) if failure else 'runtime-error'
                    break
                if exit_before_evidence is not None:
                    reason = 'early-exit'
                    break
                probe_ok = not args.probe or ('[RuntimeDiagProbeCleanup] result=true' in text
                                               and re.search(r'\[RuntimeDiag\] self test result:\s*true', text))
                if expected in text and probe_ok:
                    reason = 'evidence-complete'
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
    passed = reason == 'evidence-complete' and not errors
    summary = dict(mode=mode, executable=str(args.executable.resolve()), rounds=args.rounds, status='PASS' if passed else 'FAIL',
                   reason=reason, early_exit=exit_before_evidence,
                   graceful_shutdown_verified=False, errors=errors,
                   evidence=[line for line in text.splitlines()
                             if '[Sandbox19Stability]' in line or '[RuntimeDiagProbeCleanup]' in line])
    (output / 'summary.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2) + '\n')
    print(f'[StabilityRunner] status={summary["status"]} reason={reason} summary={output / "summary.json"}', flush=True)
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
