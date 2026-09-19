#!/usr/bin/env python3
"""Compare Crossfire tactics through normal replayed input, not a win/loss gate.

Each case starts a fresh, silent background process and profile. PASS describes
technical execution only; VICTORY, DEFEAT and INCOMPLETE are strategy outcomes.
Ground clicks are calibrated from real layout logs, with independent holdouts.
Update the explicit ground contract below when encounter routes change.
"""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import re
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
NATIVE_NAME = 'HelloOgre3D.exe' if os.name == 'nt' else 'HelloOgre3D'
PROJECTION_TOLERANCE_PX = 2.5
TIMEOUT_SECONDS = 55
ERROR = re.compile(r'call_func error|call_string error|lua_pcall error|Assertion failed|'
                   r'OGRE EXCEPTION|PANIC:|stack traceback|result=FAIL|'
                   r'rejected reason=invalid|\]\s+FAIL\b')
CAMERA = re.compile(r'\[CrossfireCamera\] width=(\d+) height=(\d+) '
                    r'distance=([\d.]+) yaw=([\d.-]+) pitch=([\d.-]+) '
                    r'safe=(\d+),(\d+),(\d+),(\d+)')
LAYOUT = re.compile(r'\[CrossfireLayout\] key=(\w+) screen=(-?\d+),(-?\d+)')
READY = re.compile(r'\[Crossfire\] ready drones=(\d+),(\d+) sentry=\d+ '
                   r'paused=true level=(\d+) enemies=(\d+)')
# The extra right-side anchor avoids the unstable fit obtained from two
# rounded points on z=5 plus the nearby route3_2 point. Four others stay held out.
FIT_POINTS = {'route1_1': (-5, 5), 'front': (-5, 9),
              'route3_2': (7, 4), 'route2_2': (8, 4.5)}
CHECK_POINTS = {'route1_2': (5, 5), 'route2_1': (-5, 6),
                'route3_1': (-7, 6), 'flank3': (-7, 10)}
# (level, initial two destinations or clear both routes, later normal clicks)
CASES = {
    'l1_same_side': (1, [(-5, 5), (-5, 9)], []),
    'l1_split': (1, [(-5, 5), (5, 5)], []),
    'l1_evade': (1, [(-5, 5), (5, 5)], [(7000, 1, (-5, 9))]),
    'l2_wait': (2, None, []),
    'l2_reposition': (2, None, [(7000, 1, (-5, 6)), (7350, 2, (8, 4.5))]),
    'l2_split': (2, [(-5, 6), (8, 4.5)], []),
    'l3_rear_cross': (3, [(-7, 9.5), (7, 4)],
                      [(8500, 1, 'enemy3_2'), (8850, 2, 'enemy3_1')]),
    'l3_auto': (3, [(-7, 6), (7, 4)], []),
    'l3_cross': (3, [(-7, 6), (7, 4)],
                 [(8500, 1, 'enemy3_2'), (8850, 2, 'enemy3_1')]),
    'l3_cross_shift': (3, [(-7, 6), (7, 4)],
                       [(8500, 1, 'enemy3_2'), (8850, 2, 'enemy3_1'),
                        (10400, 1, (-7, 10)), (14700, 1, 'enemy3_2')]),
}


def write_json(path, data):
    temporary = path.with_suffix(path.suffix + '.new')
    temporary.write_bytes((json.dumps(data, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))
    temporary.replace(path)


def file_hash(path):
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(chunk)
    return digest.hexdigest()


def fingerprint(args):
    """Keep per-file manifests as well as deterministic tree hashes."""
    result = {'executables': {}, 'lua': {}, 'errors': []}
    for name, path in (('requested', args.executable), ('native_in_cwd', args.cwd / NATIVE_NAME)):
        try:
            result['executables'][name] = {'path': str(path), 'sha256': file_hash(path)}
        except OSError as error:
            result['executables'][name] = {'path': str(path), 'sha256': None}
            # A directly specified native executable can live outside cwd.
            if name == 'requested' or args.launcher_default:
                result['errors'].append(f'{name}: {error}')
    for name, path in (('source', ROOT / 'bin/res/scripts'), ('runtime', args.cwd / 'res/scripts')):
        files = {}
        try:
            for item in sorted(path.rglob('*.lua')):
                files[item.relative_to(path).as_posix()] = file_hash(item)
            if not files:
                raise OSError(f'No Lua files in {path}')
        except OSError as error:
            result['errors'].append(f'{name}: {error}')
        canonical = json.dumps(files, sort_keys=True, separators=(',', ':')).encode('utf-8')
        result['lua'][name] = {'path': str(path), 'sha256': hashlib.sha256(canonical).hexdigest(),
                               'files': files}
    return result


def read_camera(text):
    cameras = []
    for match in CAMERA.finditer(text):
        values = match.groups()
        camera = {'width': int(values[0]), 'height': int(values[1]),
                  'distance': float(values[2]), 'yaw': float(values[3]),
                  'pitch': float(values[4]), 'safe': list(map(int, values[5:])),
                  'raw': match.group(0)}
        if camera not in cameras:
            cameras.append(camera)
    if len(cameras) != 1:
        raise ValueError(f'Expected one stable camera, found {len(cameras)}')
    camera = cameras[0]
    left, top, right, bottom = camera['safe']
    if not (0 <= left < right <= camera['width'] and 0 <= top < bottom <= camera['height']):
        raise ValueError('Invalid camera safe rectangle')
    return camera


def validate_click(point, camera):
    if len(point) != 2 or not all(math.isfinite(value) for value in point):
        raise ValueError(f'Invalid projected point: {point}')
    left, top, right, bottom = camera['safe']
    if not (left <= point[0] < right and top <= point[1] < bottom):
        raise ValueError(f'Click {point} outside logged safe rectangle {camera["safe"]}')


def project(h, point):
    x, z = point
    denominator = h[6] * x + h[7] * z + 1
    if not math.isfinite(denominator) or abs(denominator) < 1e-8:
        raise ValueError(f'Invalid projection denominator at {point}')
    result = ((h[0] * x + h[1] * z + h[2]) / denominator,
              (h[3] * x + h[4] * z + h[5]) / denominator)
    if not all(math.isfinite(value) for value in result):
        raise ValueError(f'Non-finite projection at {point}')
    return result


def fit_projection(points):
    matrix = []
    for key, (x, z) in FIT_POINTS.items():
        u, v = points[key]
        matrix.extend([[x, z, 1, 0, 0, 0, -u*x, -u*z, u],
                       [0, 0, 0, x, z, 1, -v*x, -v*z, v]])
    for col in range(8):
        pivot = max(range(col, 8), key=lambda row: abs(matrix[row][col]))
        matrix[col], matrix[pivot] = matrix[pivot], matrix[col]
        divisor = matrix[col][col]
        if abs(divisor) < 1e-9:
            raise ValueError('Singular ground projection')
        matrix[col] = [value / divisor for value in matrix[col]]
        for row in range(8):
            if row != col:
                scale = matrix[row][col]
                matrix[row] = [value - scale * base for value, base in zip(matrix[row], matrix[col])]
    return [row[8] for row in matrix]


def calibrate(text):
    camera = read_camera(text)
    points = {}
    for key, x, y in LAYOUT.findall(text):
        point = (int(x), int(y))
        if key in points and points[key] != point:
            raise ValueError(f'Conflicting layout points for {key}')
        validate_click(point, camera)
        points[key] = point
    missing = (set(FIT_POINTS) | set(CHECK_POINTS) | {'enemy3_1', 'enemy3_2'}) - set(points)
    if missing:
        raise ValueError(f'Missing layout points: {sorted(missing)}')
    h = fit_projection(points)
    errors = {}
    for key, world in CHECK_POINTS.items():
        actual = project(h, world)
        errors[key] = math.hypot(actual[0] - points[key][0], actual[1] - points[key][1])
        if errors[key] > PROJECTION_TOLERANCE_PX:
            raise ValueError(f'Projection holdout {key}: {errors[key]:.3f}px exceeds '
                             f'{PROJECTION_TOLERANCE_PX}px')
    return {'camera': camera, 'points': points, 'homography': h,
            'holdout_errors_px': errors, 'tolerance_px': PROJECTION_TOLERANCE_PX}


def make_replay(name, calibration):
    level, routes, later = CASES[name]
    events, clicks = [], []

    def click(at, slot, target):
        if isinstance(target, str):
            point = calibration['points'][target]
        else:
            point = tuple(round(value) for value in project(calibration['homography'], target))
        validate_click(point, calibration['camera'])
        x, y = point
        events.extend([(at, f'key_press {slot}'), (at + 100, f'mouse_down {x} {y} 0'),
                       (at + 150, f'mouse_up {x} {y} 0')])
        clicks.append({'at_ms': at + 100, 'slot': slot, 'target': target, 'screen': point})

    if routes:
        click(300, 1, routes[0])
        click(600, 2, routes[1])
    else:
        events.extend([(300, 'key_press 1'), (400, 'key_press X'),
                       (600, 'key_press 2'), (700, 'key_press X')])
    events.append((1000, 'key_press SPACE'))
    for at, slot, target in later:
        click(at, slot, target)
    events.append((31000 if name == 'l1_same_side' else 26000, 'quit'))
    replay = ''.join(f'{at} {event}\n' for at, event in sorted(events))
    return replay, clicks


def strategy_result(text):
    ready = READY.search(text)
    if not ready:
        return {'outcome': 'UNAVAILABLE', 'reason': 'No ready marker'}
    allies = {int(ready[1]), int(ready[2])}
    health, damage, flanks, blocks = {}, 0.0, 0, 0
    for source, target, blocked, hit, hp in re.findall(
            r'\[CrossfireImpact\] source=(\d+) target=(\d+) blocked=(\d+) '
            r'damage=([\d.]+) health=([\d.-]+)', text):
        source, target, hit = int(source), int(target), float(hit)
        if target in allies:
            health[target] = float(hp)
            damage += hit
        elif source in allies:
            blocks += int(blocked)
            flanks += int(hit > 0)
    match_lines = re.findall(r'\[CrossfireMatch\][^\r\n]*', text)
    fields = dict(re.findall(r'(\w+)=(\S+)', match_lines[-1])) if match_lines else {}
    outcome = fields.get('result', 'INCOMPLETE')
    return {'outcome': outcome, 'elapsedMs': int(fields['elapsedMs']) if 'elapsedMs' in fields else None,
            'living': int(fields['living']) if 'living' in fields else
                      sum(health.get(agent, 1) > 0 for agent in allies),
            'damage': int(fields['damage']) if 'damage' in fields else damage,
            'metrics_source': 'match_summary' if fields else 'observed_impacts_before_quit',
            'health_observed': {str(agent): health.get(agent) for agent in sorted(allies)},
            'flankHits': int(fields.get('flankHits', flanks)),
            'blockedShots': int(fields.get('blocked', blocks)),
            'alerts': [{'slot': int(slot), 'reason': reason} for slot, reason in
                       re.findall(r'\[CrossfireAlert\] slot=(\d+) reason=(\w+)', text)],
            'accepted': len(re.findall(r'\[CrossfireOrder\][^\r\n]* result=accepted\b', text)),
            'input_rejected': re.findall(r'\[CrossfireInput\][^\r\n]*=rejected[^\r\n]*', text),
            'rejected': re.findall(r'\[CrossfireOrder\][^\r\n]* result=(out_of_range|unavailable)\b', text),
            'raw_match_lines': match_lines}


def execute_run(args, dest, replay, level, probe=False, expected_camera=None, expected_orders=None):
    dest.mkdir()
    data = replay.encode('utf-8')
    (dest / 'input.txt').write_bytes(data)
    env = {key: value for key, value in os.environ.items() if not key.startswith('HELLO_')}
    env.update(HELLO_WINDOW_BACKGROUND='1', HELLO_AUDIO_SILENT='1', HELLO_RENDER_FSAA='4',
               HELLO_WINDOW_WIDTH=str(args.width), HELLO_WINDOW_HEIGHT=str(args.height),
               HELLO_CROSSFIRE_QUICKSTART='1', HELLO_CROSSFIRE_LEVEL=str(level),
               HELLO_INPUT_REPLAY=str(dest / 'input.txt'), HELLO_INPUT_REPLAY_WAIT_FOR_PLAYER='0',
               HELLO_CROSSFIRE_PROFILE=str(dest / 'profile.cfg'),
               HELLO_RENDER_CAPTURE='0' if probe else '1')
    if not args.launcher_default:
        env.update(HELLO_SANDBOX_SAMPLE='Sandbox20', HELLO_SAMPLE_PRESET='Sandbox20')
    if probe:
        env['HELLO_CROSSFIRE_LAYOUT_PROBE'] = '1'
    else:
        env.update(HELLO_RENDER_CAPTURE_DIR=str(dest), HELLO_RENDER_CAPTURE_MS='6400,8200,18000,24800',
                   HELLO_RENDER_CAPTURE_MAX_DELTA_MS='500')
    before = fingerprint(args)
    row = {'directory': str(dest), 'level': level, 'technical_status': 'FAIL',
           'input_sha256': hashlib.sha256(data).hexdigest(), 'input_events': len(replay.splitlines()),
           'requested_window': [args.width, args.height], 'timeout_seconds': TIMEOUT_SECONDS,
           'environment': {key: value for key, value in env.items() if key.startswith('HELLO_')},
           'provenance': before, 'returncode': None, 'issues': list(before['errors'])}
    write_json(dest / 'result.json', row)
    start = time.monotonic()
    with (dest / 'stdout.log').open('wb') as stdout, (dest / 'stderr.log').open('wb') as stderr:
        try:
            process = subprocess.run([str(args.executable)], cwd=args.cwd, env=env,
                                     stdout=stdout, stderr=stderr, timeout=TIMEOUT_SECONDS)
            row['returncode'] = process.returncode
            if process.returncode != 0:
                row['issues'].append(f'Process exited {process.returncode}')
        except subprocess.TimeoutExpired:
            row['issues'].append(f'Process timed out after {TIMEOUT_SECONDS}s (killed and waited)')
        except OSError as error:
            row['issues'].append(f'Launch failed: {error}')
    row['wall_seconds'] = round(time.monotonic() - start, 3)
    text = '\n'.join((dest / filename).read_text(encoding='utf-8', errors='replace')
                     for filename in ('stdout.log', 'stderr.log'))
    row['raw_lines'] = [line for line in text.splitlines() if
                        re.search(r'\[(?:Crossfire\w*|WindowMode|InputReplay)\]|' + ERROR.pattern, line)]
    row['window_lines'] = [line for line in text.splitlines() if '[WindowMode]' in line]
    row['captures'] = {path.name: file_hash(path) for path in sorted(dest.glob('capture_*.png'))}
    row['strategy'] = None if probe else strategy_result(text)
    if row['strategy'] and row['strategy']['outcome'] != 'UNAVAILABLE':
        strategy = row['strategy']
        if strategy['input_rejected'] or strategy['rejected']:
            row['issues'].append('Planned route/target input was rejected; case is not comparable')
        if expected_orders is not None and strategy['accepted'] != expected_orders:
            row['issues'].append(f'Expected {expected_orders} accepted orders, got {strategy["accepted"]}')
    row['issues'].extend('Runtime error: ' + line for line in text.splitlines() if ERROR.search(line))
    if 'background=true physical-input=disabled' not in text:
        row['issues'].append('Missing confirmation of disabled physical input')
    if '[InputReplay] synthetic=true completed reason=quit' not in text:
        row['issues'].append('Replay did not finish through its quit event')
    loaded = re.findall(r'\[InputReplay\] synthetic=true loaded events=(\d+)', text)
    if loaded != [str(row['input_events'])]:
        row['issues'].append(f'Replay event count mismatch: {loaded}')
    ready = list(READY.finditer(text))
    if len(ready) != 1 or int(ready[0][3]) != level:
        row['issues'].append('Missing, repeated or wrong-level ready marker')
    if re.search(r'\[Crossfire(?:Physics|Queries)\]', text):
        row['issues'].append('Unexpected direct physics/query fixture')
    try:
        row['camera'] = read_camera(text)
        if (row['camera']['width'], row['camera']['height']) != (args.width, args.height):
            row['issues'].append('Actual camera dimensions differ from requested window')
        if expected_camera is not None and row['camera'] != expected_camera:
            row['issues'].append('Camera differs from calibrated probe')
    except ValueError as error:
        row['issues'].append(str(error))
    after = fingerprint(args)
    if after != before:
        row['issues'].append('Executable or Lua files changed during this run')
        row['provenance_after'] = after
    if row['strategy'] and row['strategy']['outcome'] not in ('VICTORY', 'DEFEAT', 'INCOMPLETE'):
        row['issues'].append('Strategy log unavailable or has unknown outcome')
    row['technical_status'] = 'FAIL' if row['issues'] else 'PASS'
    write_json(dest / 'result.json', row)
    return row, text


def create_output(requested):
    base = (ROOT / 'tmp').resolve()
    base.mkdir(exist_ok=True)
    if requested is None:
        return Path(tempfile.mkdtemp(prefix='crossfire-tactics-' + time.strftime('%Y%m%d-%H%M%S') + '-',
                                     dir=base))
    # Resolve existing parent symlinks before accepting any output path.
    if requested.exists() or requested.is_symlink():
        raise ValueError('--output must be a new directory')
    destination = requested.resolve()
    if destination == base or base not in destination.parents:
        raise ValueError('--output must be inside the project tmp directory')
    destination.mkdir(parents=True, exist_ok=False)
    return destination


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable', type=Path, default=ROOT / 'bin' / NATIVE_NAME)
    parser.add_argument('--cwd', type=Path, default=ROOT / 'bin', help='Runtime bin directory (including res/scripts)')
    parser.add_argument('--launcher-default', action='store_true',
                        help='Let the packaged launcher select Sandbox20 without sample overrides')
    parser.add_argument('--cases', default=','.join(CASES), help='Comma-separated names: ' + ','.join(CASES))
    parser.add_argument('--repeat', type=int, choices=(1, 2, 3), default=1)
    parser.add_argument('--width', type=int, default=1280)
    parser.add_argument('--height', type=int, default=720)
    parser.add_argument('--output', type=Path, help='New directory inside project tmp; default: a unique tmp directory')
    args = parser.parse_args(argv)
    selected = [name.strip() for name in args.cases.split(',')]
    if any(name not in CASES for name in selected) or len(set(selected)) != len(selected):
        parser.error('--cases requires unique known case names')
    if not (640 <= args.width <= 3840 and 360 <= args.height <= 2160):
        parser.error('Window must be between 640x360 and 3840x2160')
    args.executable, args.cwd = args.executable.resolve(), args.cwd.resolve()
    try:
        output = create_output(args.output)
    except (OSError, ValueError) as error:
        parser.error(str(error))
    summary = {'schema': 1, 'meaning': 'technical PASS does not imply strategy victory; '
               'INCOMPLETE means no match result before the replay quit',
               'output': str(output), 'cases': selected, 'repeat': args.repeat, 'runs': [],
               'technical_status': 'RUNNING'}
    print(f'Output: {output}', flush=True)
    write_json(output / 'summary.json', summary)
    probe, text = execute_run(args, output / 'probe', '1200 quit\n', 1, probe=True)
    calibration = None
    if probe['technical_status'] == 'PASS':
        try:
            calibration = calibrate(text)
            # Check all selected clicks before starting the first case.
            for name in selected:
                make_replay(name, calibration)
            probe['calibration'] = calibration
        except (ValueError, KeyError) as error:
            probe['issues'].append(str(error))
            probe['technical_status'] = 'FAIL'
            calibration = None
    write_json(output / 'probe/result.json', probe)
    summary['probe'] = probe
    write_json(output / 'summary.json', summary)
    for repetition in range(1, args.repeat + 1):
        for name in selected:
            dest = output / f'{name}-{repetition:02d}'
            if calibration is None:
                row = {'directory': str(dest), 'technical_status': 'NOT_RUN',
                       'issues': ['Calibration probe failed; no input sent'], 'strategy': None}
                dest.mkdir()
            elif fingerprint(args) != probe['provenance']:
                row = {'directory': str(dest), 'technical_status': 'FAIL',
                       'issues': ['Executable or Lua changed after calibration; no input sent'],
                       'strategy': None}
                dest.mkdir()
            else:
                replay, clicks = make_replay(name, calibration)
                row, _ = execute_run(args, dest, replay, CASES[name][0],
                                     expected_camera=calibration['camera'], expected_orders=2 + len(CASES[name][2]))
                row['clicks'] = clicks
                if row['provenance'] != probe['provenance']:
                    row['issues'].append('Executable or Lua differs from calibrated probe')
                    row['technical_status'] = 'FAIL'
            row.update(case=name, repetition=repetition)
            write_json(dest / 'result.json', row)
            summary['runs'].append(row)
            write_json(output / 'summary.json', summary)
            print(json.dumps({'case': name, 'repetition': repetition,
                              'technical_status': row['technical_status'],
                              'strategy': row['strategy'], 'issues': row['issues']}), flush=True)
    ok = probe['technical_status'] == 'PASS' and all(row['technical_status'] == 'PASS' for row in summary['runs'])
    summary['technical_status'] = 'PASS' if ok else 'FAIL'
    write_json(output / 'summary.json', summary)
    print(f'Technical execution: {summary["technical_status"]}; results: {output / "summary.json"}', flush=True)
    return 0 if ok else 1


if __name__ == '__main__':
    raise SystemExit(main())
