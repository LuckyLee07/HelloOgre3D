#!/usr/bin/env python3
"""Check isolated logs produced by run_agent_animation_probe.py."""
import collections
import json
import math
from pathlib import Path
import re
import statistics
import sys

POSE = re.compile(r'\[AnimPose\] id=2 simMs=(\d+) wallUs=(\d+) alpha=([\d.]+) clip=([\d.]+) hand=([^ ]+) foot=([^\n]+)')


def check(output):
    result = {'status': 'PASS', 'simulation_hz': 30, 'rates': []}
    reference_shots = None
    for directory in sorted(Path(output).glob('probe-*'), key=lambda p: int(p.name.split('-')[1]) if p.is_dir() else 10000):
        if not directory.is_dir():
            continue
        rate = int(directory.name.split('-')[1])
        metadata = json.loads((directory / 'metadata.json').read_text())
        text = (directory / 'stdout.log').read_text(errors='replace')
        assert metadata['exit_code'] == 0, directory
        assert '[AnimProbe] result=PASS failures=0 shots=4' in text, directory
        assert 'physical-input=disabled' in text
        assert not re.search(r'call_func error|OGRE EXCEPTION|stack traceback|Assertion failed|result=FAIL', text)
        checks = re.findall(r'\[AnimCheck\].*case=(\S+) result=PASS', text)
        assert len(checks) == 21, checks
        rows = [dict(sim=int(a), wall=int(b), alpha=float(c), clip=float(d),
                     hand=tuple(map(float, e.split(','))), foot=tuple(map(float, f.split(','))))
                for a, b, c, d, e, f in POSE.findall(text)]
        assert len(rows) > 100
        assert all(math.isfinite(v) for r in rows for v in (*r['hand'], *r['foot'], r['clip']))
        groups = collections.defaultdict(list)
        for row in rows:
            groups[row['sim']].append(row)
        assert all(len({r['clip'] for r in group}) == 1 for group in groups.values()), 'display changed an action clock'
        active = [group for t, group in groups.items() if 3500 < t < 4700 and len(group) > 1]
        continuous = [group for group in active if max(math.dist(group[0]['foot'], r['foot']) for r in group) > .0001]
        if rate >= 60:
            assert len(continuous) > 20, 'missing bone samples between simulation steps'
        periods = [(b['wall'] - a['wall']) / 1000 for a, b in zip(rows, rows[1:]) if a['sim'] > 300]
        fps = 1000 / statistics.median(periods)
        assert abs(fps - rate) / rate < .15, (rate, fps)
        shots = re.findall(r'\[WeaponShot\].*position=(.*?) forward=(.*?) speed=48', text)
        assert len(shots) == 4
        if reference_shots is None:
            reference_shots = shots
        assert shots == reference_shots, 'render cadence changed shot transforms'
        result['rates'].append(dict(requested_hz=rate, measured_median_hz=round(fps, 2),
                                    checks=checks, shots=len(shots), ammo_remaining=6,
                                    pose_frames=len(rows), continuous_gait_steps=len(continuous),
                                    clock_changes_within_step=0, binary_sha256=metadata['binary_sha256']))
    assert result['rates'], 'no probe directories'
    result['identical_shot_transforms'] = reference_shots
    return result


if __name__ == '__main__':
    print(json.dumps(check(sys.argv[1]), indent=2))
