#!/usr/bin/env python3
"""Run the current Relay Outpost input route and require a natural victory.

The replay enters through InputManager. It never changes actor health, teleports
units or invokes the product fixture. A successful process exit alone is not a
pass: the second wave must clear before victory, followed by restart and quit.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
REPLAY = ROOT / 'tools' / 'replays' / 'sandbox19-product' / 'ranged-and-regroup.txt'
ERROR = re.compile(r'call_func error|call_string error|lua_pcall error|Assertion failed|'
                   r'OGRE EXCEPTION|PANIC:|stack traceback|\]\s+FAIL\b|'
                   r'self test result:\s*false', re.IGNORECASE)


def sha256(path):
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(block)
    return digest.hexdigest()


def ordered_events(text):
    events = (
        '[Sandbox19Match] phase=WAVE wave=1',
        '[Sandbox19Match] phase=ADVANCE',
        '[Sandbox19Match] phase=WAVE wave=2',
        '[Sandbox19Match] phase=REGROUP',
        '[Sandbox19Match] phase=VICTORY',
        '[Sandbox19Match] phase=WAVE wave=1',
        '[Sandbox19Pause] paused=true',
        '[InputReplay] synthetic=true completed reason=quit',
    )
    positions = []
    cursor = 0
    for event in events:
        found = text.find(event, cursor)
        positions.append(found)
        if found < 0:
            positions.extend([-1] * (len(events) - len(positions)))
            return events, positions
        cursor = found + len(event)
    return events, positions


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--timeout', type=int, default=150, help='wall seconds (default: 150)')
    parser.add_argument('--executable', type=Path, default=ROOT / 'bin' /
                        ('HelloOgre3D.exe' if os.name == 'nt' else 'HelloOgre3D'))
    args = parser.parse_args()
    if args.timeout < 110:
        parser.error('timeout must allow the 104-second replay to finish')
    executable = args.executable.resolve()
    if not executable.is_file() or not REPLAY.is_file():
        parser.error('executable or replay is missing')

    (ROOT / 'tmp').mkdir(exist_ok=True)
    output = Path(tempfile.mkdtemp(prefix='relay-natural-' + time.strftime('%Y%m%d-%H%M%S') + '-',
                                   dir=ROOT / 'tmp'))
    stdout_path, stderr_path = output / 'stdout.log', output / 'stderr.log'
    env = {key: value for key, value in os.environ.items() if not key.startswith('HELLO_')}
    env.update(HELLO_SANDBOX_SAMPLE='Sandbox19', HELLO_SAMPLE_PRESET='Sandbox19',
               HELLO_WINDOW_BACKGROUND='1', HELLO_AUDIO_SILENT='1',
               HELLO_WINDOW_WIDTH='1280', HELLO_WINDOW_HEIGHT='800',
               HELLO_INPUT_REPLAY=str(REPLAY))
    print(f'[RelayNaturalGate] synthetic-input=true fixture=false output={output}', flush=True)
    timed_out = False
    exit_code = None
    start = time.monotonic()
    with stdout_path.open('wb') as stdout, stderr_path.open('wb') as stderr:
        try:
            child = subprocess.run([str(executable)], cwd=ROOT / 'bin', env=env,
                                   stdout=stdout, stderr=stderr, timeout=args.timeout,
                                   creationflags=subprocess.CREATE_NO_WINDOW if os.name == 'nt' else 0)
            exit_code = child.returncode
        except subprocess.TimeoutExpired:
            timed_out = True

    text = stdout_path.read_text(errors='replace') + '\n' + stderr_path.read_text(errors='replace')
    events, positions = ordered_events(text)
    missing = [event for event, position in zip(events, positions) if position < 0]
    checks = {
        'physical_input_disabled': '[WindowMode] background=true physical-input=disabled' in text,
        'focus_executed': re.search(r'\[Sandbox19Order\].*kind=focus result=executing', text) is not None,
        'rally_executed': re.search(r'\[Sandbox19Order\].*kind=rally result=executing', text) is not None,
        'natural_victory': re.search(r'\[Sandbox19Match\] phase=VICTORY[^\n]*director=none', text) is not None,
        'second_briefing': text.count('[Sandbox19] ready playerId=') >= 2,
        'ordered_lifecycle': not missing,
    }
    errors = [line for line in text.splitlines() if ERROR.search(line)]
    passed = exit_code == 0 and not timed_out and all(checks.values()) and not errors
    summary = {
        'status': 'PASS' if passed else 'FAIL', 'synthetic_input': True, 'product_fixture': False,
        'natural_play_verified': passed, 'external_input_verified': False,
        'executable': str(executable), 'binary_sha256': sha256(executable),
        'replay_sha256': sha256(REPLAY), 'wall_seconds': round(time.monotonic() - start, 2),
        'exit_code': exit_code, 'timed_out': timed_out, 'checks': checks,
        'missing_ordered_events': missing, 'errors': errors,
        'match_events': [line for line in text.splitlines() if '[Sandbox19Match]' in line],
    }
    (output / 'summary.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2) + '\n')
    print(f'[RelayNaturalGate] status={summary["status"]} victory={checks["natural_victory"]} '
          f'exit={exit_code} summary={output / "summary.json"}', flush=True)
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
