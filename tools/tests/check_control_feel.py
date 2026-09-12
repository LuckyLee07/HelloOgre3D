#!/usr/bin/env python3
"""Check isolated stdout from sandbox19_control_feel.txt with HELLO_CAMERA_TRACE=1.

Example run environment (bin/HelloOgre3D, cwd=bin): HELLO_SANDBOX_SAMPLE=Sandbox19,
HELLO_WINDOW_BACKGROUND=1, HELLO_AUDIO_SILENT=1, HELLO_CAMERA_TRACE=1,
HELLO_INPUT_REPLAY=<absolute path to tools/tests/sandbox19_control_feel.txt>.
The replay is synthetic; this does not measure hardware input latency or hand feel.
"""
import json
import math
from pathlib import Path
import re
import sys


def vector(line, key, parenthesized=False):
    pattern = re.escape(key) + (r'=\(([^)]+)\)' if parenthesized else r'=(\S+ \S+ \S+)')
    return tuple(map(float, re.search(pattern, line).group(1).replace(',', ' ').split()))


def yaw(v):
    return math.degrees(math.atan2(v[0], v[2]))


def delta(a, b):
    return (a - b + 180) % 360 - 180


def check(path):
    text = Path(path).read_text(errors='replace')
    assert '[InputReplay] synthetic=true completed reason=quit' in text, 'replay did not complete'
    assert 'physical-input=disabled' in text, 'hardware input was not isolated'
    assert not re.search(r'OGRE EXCEPTION|call_func error|stack traceback|Assertion failed', text)
    player = re.search(r'\[InputReplay\].*started player=(\d+)', text).group(1)
    events, poses, shots = {}, [], []
    for line in text.splitlines():
        if '[InputReplay]' in line and 'dispatch ' in line:
            name = re.search(r'event=(.*?) paused=', line).group(1)
            events[name] = dict(camera=yaw(vector(line, 'cameraForward', True)),
                                pitch=vector(line, 'cameraForward', True)[1],
                                body=yaw(vector(line, 'playerForward', True)),
                                paused='paused=true' in line)
        if '[PresentationTrace]' in line:
            poses.append(dict(time=int(re.search(r'simMs=(\d+)', line).group(1)),
                              body=yaw(vector(line, 'bodyForward')),
                              displayed=yaw(vector(line, 'displayedForward'))))
        if f'[WeaponShot] owner={player} ' in line:
            assert poses, 'missing pose trace'
            shots.append(dict(time=poses[-1]['time'], body=poses[-1]['body'],
                              direction=yaw(vector(line, 'forward'))))
    get = lambda name: events[name]
    observed = get('566 mouse_up 1600 400 2')
    assert abs(delta(observed['camera'], 180)) < .1
    assert abs(observed['body']) < .1, 'free orbit forced the stationary body to turn'
    assert 3 <= len(shots) <= 6, f'unexpected player shots: {shots}'
    assert abs(delta(shots[0]['direction'], 180)) < 10, 'first shot fired along the old facing'
    assert abs(delta(shots[0]['body'], 180)) < 8.5, 'shot began before the body settled'
    turn = [p for p in poses if 1056 <= p['time'] <= 1500]
    steps = [abs(delta(b['body'], a['body'])) for a, b in zip(turn, turn[1:])]
    assert steps and 0 < max(steps) <= 18.0, 'body snapped during the 180-degree turn'
    for name in ('2800 key_down SPACE', '3300 key_up SPACE', '3700 key_up S'):
        assert abs(delta(get(name)['body'], get(name)['camera'])) < .1, 'backpedal changed facing'
    assert abs(delta(get('4800 key_up W')['body'], get('4800 key_up W')['camera'])) < .1
    assert abs(delta(get('5400 key_down E')['camera'], get('5800 key_up Q')['camera'])) < .1
    q_degrees = delta(get('5400 key_down E')['camera'], get('5000 key_down Q')['camera'])
    assert 25 < q_degrees < 35, f'keyboard turn rate outside expected range: {q_degrees}'
    assert get('6500 key_up E')['paused'] and not get('7400 mouse_down 600 400 2')['paused']
    assert abs(delta(get('6200 key_press ESC')['camera'], get('7400 mouse_down 600 400 2')['camera'])) < .1
    assert get('7466 mouse_up 700 600 2')['pitch'] < get('7400 mouse_down 600 400 2')['pitch']
    assert abs(delta(get('7900 mouse_down 400 300 0')['camera'], get('8400 mouse_up 500 300 0')['camera'])) < .1
    return dict(status='PASS', synthetic=True, player=player, max_body_step_degrees=max(steps),
                first_shot_direction_degrees=shots[0]['direction'], player_shots=len(shots),
                keyboard_400ms_degrees=q_degrees,
                checks=['free-orbit', '180-degree turn', 'first-shot alignment', 'backpedal fire',
                        'shortest turn', 'opposing keys', 'pause release', 'pitch sign', 'selection vs orbit'])


if __name__ == '__main__':
    print(json.dumps(check(sys.argv[1]), indent=2))
