#!/usr/bin/env python3
"""Crossfire runtime gate: real input route and separately labelled physics fixture."""
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
ERROR = re.compile(r'call_func error|call_string error|lua_pcall error|Assertion failed|'
                   r'OGRE EXCEPTION|PANIC:|stack traceback|result=FAIL|rejected reason=invalid|\]\s+FAIL\b')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--mode', choices=('all', 'natural', 'front', 'physics', 'campaign', 'interface', 'controls', 'queries', 'pacing'), default='all')
    parser.add_argument('--executable', type=Path, default=ROOT / 'bin' /
                        ('HelloOgre3D.exe' if os.name == 'nt' else 'HelloOgre3D'))
    parser.add_argument('--cwd', type=Path, default=ROOT / 'bin')
    parser.add_argument('--launcher-default', action='store_true', help='require the launcher to choose Sandbox20 without environment selection')
    parser.add_argument('--width', type=int, default=1280)
    parser.add_argument('--height', type=int, default=800)
    parser.add_argument('--capture-ms', help='Optional comma-separated render capture times for focused visual verification.')
    args = parser.parse_args()
    if args.capture_ms and (not re.fullmatch(r'\d+(,\d+)*', args.capture_ms) or len(args.capture_ms.split(',')) > 240):
        parser.error('--capture-ms requires at most 240 comma-separated integer milliseconds')
    output = Path(tempfile.mkdtemp(prefix='crossfire-gate-' + time.strftime('%Y%m%d-%H%M%S') + '-', dir=ROOT / 'tmp'))
    all_ok = True
    summary = {'executable': str(args.executable.resolve()), 'runs': {}}
    for mode in (('natural', 'front', 'physics', 'campaign', 'interface', 'controls', 'queries', 'pacing') if args.mode == 'all' else (args.mode,)):
        dest = output / mode
        dest.mkdir()
        replay = ROOT / 'tools/replays/crossfire/flank-and-retry.txt'
        if mode == 'front':
            replay = ROOT / 'tools/replays/crossfire/front-only.txt'
        if mode in ('physics', 'queries'):
            replay = dest / 'fixture-quit.txt'
            replay.write_text('8000 quit\n' if mode == 'queries' else '12000 key_press R\n12500 key_press SPACE\n13500 key_press R\n14000 quit\n')
        if mode in ('campaign', 'interface', 'controls', 'pacing'):
            replay = ROOT / 'tools/replays/crossfire' / (('campaign' if mode == 'controls' else mode) + '.txt')
        if mode == 'interface' and (args.width, args.height) != (1280, 800):
            contents = replay.read_text()
            for old, point in {
                '848 380': (args.width // 2 + 208, args.height // 2 - 20),
                '808 449': (args.width // 2 + 168, args.height // 2 + 49),
                '522 589': (args.width // 2 - 118, args.height // 2 + 189),
                '1060 81': (args.width - 220, 81),
            }.items():
                contents = contents.replace(old, f'{point[0]} {point[1]}')
            replay = dest / 'interface-sized.txt'
            replay.write_text(contents)
        if mode == 'controls':
            contents = replay.read_text().replace('400 key_press RETURN\n',
                                                  '400 key_press RETURN\n500 key_press SPACE\n')
            contents = re.sub(r'(mouse_(?:down|up) \d+ \d+) 1', r'\1 0', contents)
            replay = dest / 'left-click-campaign.txt'
            replay.write_text(contents)
        if mode == 'pacing':
            sized = dest / 'pacing-sized.txt'
            sized.write_text(replay.read_text().replace('718 29', f'{args.width // 2 + 78} 29')
                             .replace('1060 81', f'{args.width - 220} 81'))
            replay = sized
        previous_ms = -1
        for line_number, line in enumerate(replay.read_text().splitlines(), 1):
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            event_ms = int(line.split()[0])
            if event_ms < previous_ms:
                raise ValueError(f'{replay}:{line_number}: replay time moves backwards')
            previous_ms = event_ms
        env = {k: v for k, v in os.environ.items() if not k.startswith('HELLO_')}
        env.update(HELLO_SANDBOX_SAMPLE='Sandbox20', HELLO_SAMPLE_PRESET='Sandbox20',
                   HELLO_WINDOW_BACKGROUND='1', HELLO_AUDIO_SILENT='1',
                   HELLO_WINDOW_WIDTH=str(args.width), HELLO_WINDOW_HEIGHT=str(args.height), HELLO_RENDER_FSAA='4',
                   HELLO_INPUT_REPLAY=str(replay), HELLO_INPUT_REPLAY_WAIT_FOR_PLAYER='0',
                   HELLO_RENDER_CAPTURE='1', HELLO_RENDER_CAPTURE_DIR=str(dest),
                   HELLO_RENDER_CAPTURE_MS='1700,9000,15000,22500' if mode == 'natural' else '1000,6500',
                   HELLO_RENDER_CAPTURE_MAX_DELTA_MS='500')
        env['HELLO_CROSSFIRE_PROFILE'] = str(dest / 'profile.cfg')
        if mode in ('natural', 'front', 'physics', 'queries'):
            env['HELLO_CROSSFIRE_QUICKSTART'] = '1'
        if mode in ('campaign', 'controls'):
            env['HELLO_RENDER_CAPTURE_MS'] = '300,12000,17000,28000,39000,50000,65000,70000'
        if mode == 'pacing':
            env['HELLO_RENDER_CAPTURE_MS'] = '3700,5500,10400,15000,24800,26000'
        if mode == 'front':
            env['HELLO_RENDER_CAPTURE_MS'] = '6500,37000,39000'
        if mode == 'interface':
            env['HELLO_RENDER_CAPTURE_MS'] = '500,2300,5300,7300,9000,12300,16800'
        if args.capture_ms:
            env['HELLO_RENDER_CAPTURE_MS'] = args.capture_ms
        if args.launcher_default:
            env.pop('HELLO_SANDBOX_SAMPLE')
            env.pop('HELLO_SAMPLE_PRESET')
        if mode == 'physics':
            env['HELLO_CROSSFIRE_PHYSICS_TEST'] = '1'
        if mode == 'queries':
            env['HELLO_CROSSFIRE_QUERY_TEST'] = '1'
        # Coordinates come from the actual viewport/camera, then still travel
        # through the ordinary mouse input path. Never infer a win from a probe.
        if mode in ('natural', 'front', 'campaign', 'controls', 'pacing'):
            probe_replay = dest / 'layout-probe.txt'
            probe_replay.write_text('1200 quit\n')
            probe_env = dict(env, HELLO_CROSSFIRE_LAYOUT_PROBE='1',
                             HELLO_INPUT_REPLAY=str(probe_replay),
                             HELLO_RENDER_CAPTURE='0', HELLO_CROSSFIRE_QUICKSTART='1')
            with (dest / 'layout-probe.log').open('wb') as log:
                probe = subprocess.run([str(args.executable.resolve())], cwd=args.cwd.resolve(),
                                       env=probe_env, stdout=log, stderr=log, timeout=30)
            probe_text = (dest / 'layout-probe.log').read_text(errors='replace')
            if probe.returncode != 0 or ERROR.search(probe_text):
                raise RuntimeError(f'Layout probe failed; inspect {dest / "layout-probe.log"}')
            points = {key: (x, y) for key, x, y in re.findall(
                r'\[CrossfireLayout\] key=(\w+) screen=(\d+),(\d+)', probe_text)}
            mapping = {(675,284):'route1_1',(489,342):'route1_2',
                       (666,272):'route2_1',(455,368):'route2_2',
                       (701,261):'route3_1',(508,287):'enemy3_2',
                       (638,281):'enemy3_1',(643,233):'front',(735,158):'flank3'}
            def remap(match):
                key = mapping.get((int(match[2]), int(match[3])))
                if key is None: return match[0]
                line_prefix = match.string[match.string.rfind('\n', 0, match.start()) + 1:match.start()]
                if key == 'route2_2' and int(line_prefix.split()[0]) > 38000:
                    key = 'route3_2'
                if key not in points: raise RuntimeError('Missing projected point ' + key)
                return match[1] + ' '.join(points[key]) + match[4]
            contents = re.sub(r'(mouse_(?:down|up) )(\d+) (\d+)( [01])', remap, replay.read_text())
            replay = dest / 'projected-input.txt'
            replay.write_text(contents)
            env['HELLO_INPUT_REPLAY'] = str(replay)
        exit_code, timed_out = None, False
        with (dest / 'stdout.log').open('wb') as out, (dest / 'stderr.log').open('wb') as err:
            try:
                child = subprocess.run([str(args.executable.resolve())], cwd=args.cwd.resolve(),
                                       env=env, stdout=out, stderr=err, timeout=100 if mode in ('campaign', 'controls') else 55)
                exit_code = child.returncode
            except subprocess.TimeoutExpired:
                timed_out = True
        text = (dest / 'stdout.log').read_text(errors='replace') + '\n' + (dest / 'stderr.log').read_text(errors='replace')
        checks = {
            'normal_exit': exit_code == 0 and not timed_out,
            'no_runtime_errors': ERROR.search(text) is None,
            'background_input_disabled': '[WindowMode] background=true physical-input=disabled' in text,
            'graceful_quit': '[InputReplay] synthetic=true completed reason=quit' in text,
        }
        if mode in ('campaign', 'controls'):
            victories = re.findall(r'\[CrossfireMatch\] result=VICTORY[^\n]*level=(\d+)', text)
            checks.update(
                title_entry='[CrossfireScreen] screen=title level=1' in text,
                three_natural_wins=victories == ['1', '2', '3'],
                completed='[CrossfireCampaign] completed=3' in text,
                level_transitions=all(f'reason=next level={level}' in text for level in (1, 2, 3)),
                four_agents='level=3 screen=battle agents=4' in text,
                crossed_focus=len(re.findall(r'kind=attack result=accepted', text)) >= 2,
                routes_reachable='reachable=false' not in text,
                nav_rebuilt=text.count('[CrossfireScene]') == 4,
                records_saved=(dest / 'profile.cfg').exists(),
                fixture_absent='[CrossfirePhysics]' not in text,
            )
        elif mode == 'interface':
            opened = re.findall(r'\[CrossfireMenu\] overlay=(settings|help|closed) tick=(\d+)', text)
            checks.update(
                title_entry='[CrossfireScreen] screen=title level=1' in text,
                all_previews=all(f'reason=preview level={level}' in text for level in (1, 2, 3)),
                menu_clock_frozen=len(opened) >= 4 and len({tick for _, tick in opened[:4]}) == 1,
                no_commands_through_modal='[CrossfireOrder]' not in text and '[CrossfirePlan]' not in text,
                settings_changed='[CrossfireSettings] volume=' in text,
                settings_saved=(dest / 'profile.cfg').exists(),
                restart='[CrossfireRestart] cleared=true' in text,
                routes_reachable='reachable=false' not in text,
            )
        elif mode == 'pacing':
            starts = [tuple(map(int, m)) for m in re.findall(r'\[CrossfireStep\] event=started tick=(\d+) until=(\d+)', text)]
            stops = [tuple(map(int, m)) for m in re.findall(r'\[CrossfireStep\] event=finished tick=(\d+) until=(\d+)', text)]
            menus = re.findall(r'\[CrossfireMenu\] overlay=(settings|help|closed) tick=(\d+)', text)
            restart_tail = text.rsplit('[CrossfireRestart] cleared=true', 1)[-1]
            modal_chunks = re.findall(r'\[CrossfireMenu\] overlay=(?:settings|help)[^\n]*\n(.*?)\[CrossfireMenu\] overlay=closed', text, re.S)
            checks.update(
                step_keyboard_and_button=len(starts) == 4 and all(b-a == 2000 for a,b in starts),
                two_second_stop=len(stops) == 2 and len(starts) == 4 and all(
                    deadline == starts[i][1] and 0 <= tick-deadline <= 34
                    for (tick,deadline),i in zip(stops, (0,2))),
                pause_clock_frozen=len(menus) == 8 and all(menus[i][1] == menus[i+1][1] for i in range(0,8,2)),
                modal_blocks_shortcut=len(modal_chunks) == 4 and all('[CrossfireStep] event=started' not in c for c in modal_chunks),
                empty_execute_guard=text.count('execute=blocked reason=no_plan') == 2,
                retry_cancels_step=text.count('[CrossfireRestart] cleared=true') == 2 and '[CrossfireStep]' not in restart_tail,
                continuous_after_retry=text.count('[CrossfireMatch] result=VICTORY') == 1,
                restored_then_reset='[CrossfireSceneState] outcome=VICTORY' in text and 'outcome=idle' in text.split('[CrossfireSceneState] outcome=VICTORY')[-1],
                fixture_absent='[CrossfirePhysics]' not in text and '[CrossfireQueries]' not in text,
                routes_reachable='reachable=false' not in text,
            )
        elif mode == 'queries':
            checks['physical_queries'] = re.search(r'\[CrossfireQueries\] result=PASS synthetic=true passed=\d+ failed=0', text) is not None
        elif mode == 'physics':
            checks['physical_fixture'] = 'result=PASS synthetic=true passed=7 failed=0' in text
            checks['fixture_retry'] = text.count('[CrossfireRestart] cleared=true') == 2
        elif mode == 'front':
            checks.update(front_defeat='[CrossfireMatch] result=DEFEAT' in text,
                          offline_then_reset='[CrossfireSceneState] outcome=DEFEAT' in text and 'outcome=idle' in text.split('[CrossfireSceneState] outcome=DEFEAT')[-1],
                          two_drones_lost=text.count('[CrossfireDestroyed] slot=') == 2,
                          retry=text.count('[CrossfireRestart] cleared=true') == 1,
                          no_flank_damage='blocked=0 damage=10.0' not in text)
        else:
            accepted = re.findall(r'\[CrossfireOrder\].*result=accepted[^\n]*tick=(\d+)', text)
            pause = re.findall(r'\[CrossfirePause\] paused=(true|false) tick=(\d+)', text)
            checks.update(
                fixture_absent='[CrossfirePhysics]' not in text,
                simultaneous_commit=len(accepted) >= 2 and accepted[0] == accepted[1],
                paused_clock=len(pause) >= 3 and pause[1][0] == 'true' and pause[2] == ('false', pause[1][1]),
                queued_hold_replaced='kind=hold tick=' in text and text.count('[CrossfirePlan]') >= 4,
                routes_arrived=text.count('result=arrived') >= 2,
                front_blocks='blocked=1 damage=0.0' in text,
                flank_hits='blocked=0 damage=10.0' in text,
                natural_victory='[CrossfireMatch] result=VICTORY' in text,
                retries_clean=text.count('[CrossfireRestart] cleared=true') == 2 and text.count('[Crossfire] ready') == 3,
            )
        if mode in ('controls', 'campaign', 'front', 'natural'):
            reveals = re.findall(r'\[CrossfireOutcome\] phase=reveal result=(VICTORY|DEFEAT) tick=(\d+)', text)
            reports = re.findall(r'\[CrossfireOutcome\] phase=report result=(VICTORY|DEFEAT) elapsedUiMs=(\d+) tick=(\d+)', text)
            pools = [tuple(map(int,m)) for m in re.findall(r'\[CrossfireEffects\] event=report capacity=(\d+) active=(\d+) peak=(\d+) emitted=(\d+) reused=(\d+) evicted=(\d+)', text)]
            checks.update(
                outcome_reveal_then_report=bool(reveals) and len(reveals)==len(reports) and all(
                    (outcome,tick)==reveals[i] and 900<=int(age)<=1000 for i,(outcome,age,tick) in enumerate(reports)),
                bounded_effects_expired=len(pools)==len(reports) and all(
                    capacity==28 and active==0 and 0<peak<=28 and emitted>0 and reused>0 and evicted==0
                    for capacity,active,peak,emitted,reused,evicted in pools),
                power_sequence=('result=VICTORY' not in text or text.count('[CrossfireScenePower] elapsedMs=900')==text.count('[CrossfireMatch] result=VICTORY')),
            )
        if mode == 'controls':
            checks['empty_execute_blocked'] = '[CrossfireInput] execute=blocked reason=no_plan' in text
        if mode in ('campaign', 'interface', 'controls'):
            reload_dir = dest / 'reload'
            reload_dir.mkdir()
            reload_replay = reload_dir / 'quit.txt'
            reload_replay.write_text('2000 quit\n')
            reload_env = dict(env, HELLO_INPUT_REPLAY=str(reload_replay),
                              HELLO_RENDER_CAPTURE_DIR=str(reload_dir), HELLO_RENDER_CAPTURE_MS='1000')
            with (reload_dir / 'stdout.log').open('wb') as out, (reload_dir / 'stderr.log').open('wb') as err:
                try:
                    child = subprocess.run([str(args.executable.resolve())], cwd=args.cwd.resolve(),
                                           env=reload_env, stdout=out, stderr=err, timeout=25)
                    reload_ok = child.returncode == 0
                except subprocess.TimeoutExpired:
                    reload_ok = False
            reload_text = (reload_dir / 'stdout.log').read_text(errors='replace') + (reload_dir / 'stderr.log').read_text(errors='replace')
            checks['reload_clean'] = reload_ok and ERROR.search(reload_text) is None
            checks['saved_values_reloaded'] = ('volume=1.00 muted=false hints=false' in reload_text if mode == 'interface'
                                              else 'hints=true completed=3' in reload_text)
        passed = all(checks.values())
        all_ok &= passed
        summary['runs'][mode] = {'passed': passed, 'checks': checks, 'exit_code': exit_code,
                                 'timed_out': timed_out, 'replay_sha256': hashlib.sha256(replay.read_bytes()).hexdigest()}
        print(f'[CrossfireGate] mode={mode} result={"PASS" if passed else "FAIL"} output={dest}', flush=True)
        if not passed:
            print(json.dumps(checks, ensure_ascii=False), flush=True)
    (output / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    return 0 if all_ok else 1


if __name__ == '__main__':
    raise SystemExit(main())
