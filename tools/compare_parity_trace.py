#!/usr/bin/env python3
"""Compare HelloOgre parity trace JSONL/log output with tolerances."""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path


PREFIX = "[ParityTrace]"


def _load_records(path: Path, strict_json: bool) -> list[dict]:
    records: list[dict] = []
    with path.open("r", encoding="utf-8-sig", errors="replace") as handle:
        for line_number, raw_line in enumerate(handle, 1):
            line = raw_line.strip()
            if not line:
                continue
            if PREFIX in line:
                line = line.split(PREFIX, 1)[1].strip()
            if not line.startswith("{"):
                continue
            try:
                record = json.loads(line)
            except json.JSONDecodeError as exc:
                message = f"{path}:{line_number}: invalid parity JSON skipped: {exc}"
                if strict_json:
                    raise SystemExit(message) from exc
                print(f"[ParityCompare] warning: {message}", file=sys.stderr)
                continue
            if record.get("type") == "sample":
                records.append(record)
    return records


def _distance(a: list[float] | None, b: list[float] | None) -> float:
    if not a or not b or len(a) < 3 or len(b) < 3:
        return math.inf
    return math.sqrt(sum((float(a[index]) - float(b[index])) ** 2 for index in range(3)))


def _agent_by_index(record: dict) -> dict[int, dict]:
    result: dict[int, dict] = {}
    for agent in record.get("agents", []):
        try:
            result[int(agent.get("index"))] = agent
        except (TypeError, ValueError):
            continue
    return result


def compare(old_records: list[dict], new_records: list[dict], args: argparse.Namespace) -> tuple[bool, list[str]]:
    failures: list[str] = []
    warnings: list[str] = []

    if not old_records:
        failures.append("old trace has no sample records")
    if not new_records:
        failures.append("new trace has no sample records")
    if failures:
        return False, failures

    if len(old_records) != len(new_records):
        warnings.append(f"sample count differs: old={len(old_records)} new={len(new_records)}")

    compared_samples = min(len(old_records), len(new_records))
    max_position_error = 0.0
    max_target_error = 0.0

    for sample_index in range(compared_samples):
        old = old_records[sample_index]
        new = new_records[sample_index]
        label = f"sample#{sample_index + 1}"

        time_error = abs(float(old.get("tMs", 0)) - float(new.get("tMs", 0)))
        if time_error > args.time_tolerance_ms:
            failures.append(f"{label}: time differs by {time_error:.1f}ms")

        old_agents = _agent_by_index(old)
        new_agents = _agent_by_index(new)
        if len(old_agents) != len(new_agents):
            failures.append(f"{label}: agent count differs old={len(old_agents)} new={len(new_agents)}")

        for agent_index in sorted(set(old_agents) & set(new_agents)):
            old_agent = old_agents[agent_index]
            new_agent = new_agents[agent_index]
            prefix = f"{label} agent#{agent_index}"
            if old_agent.get("team") != new_agent.get("team"):
                failures.append(f"{prefix}: team differs old={old_agent.get('team')} new={new_agent.get('team')}")
            if bool(old_agent.get("alive")) != bool(new_agent.get("alive")):
                failures.append(f"{prefix}: alive differs old={old_agent.get('alive')} new={new_agent.get('alive')}")

            position_error = _distance(old_agent.get("pos"), new_agent.get("pos"))
            max_position_error = max(max_position_error, position_error if math.isfinite(position_error) else 0.0)
            if position_error > args.position_tolerance:
                failures.append(f"{prefix}: pos error {position_error:.3f} > {args.position_tolerance:.3f}")

            target_error = _distance(old_agent.get("target"), new_agent.get("target"))
            if math.isfinite(target_error):
                max_target_error = max(max_target_error, target_error)
                if target_error > args.target_tolerance:
                    failures.append(f"{prefix}: target error {target_error:.3f} > {args.target_tolerance:.3f}")

        old_tactics = old.get("tactics", {})
        new_tactics = new.get("tactics", {})
        for key in ("events", "dangerCells", "teamCells", "luaDangerActive", "luaTeamActive"):
            if key not in old_tactics or key not in new_tactics:
                continue
            old_value = int(old_tactics.get(key, 0) or 0)
            new_value = int(new_tactics.get(key, 0) or 0)
            if abs(old_value - new_value) > args.cell_tolerance:
                failures.append(f"{label}: tactics.{key} differs old={old_value} new={new_value}")

    status = "PASS" if not failures else "FAIL"
    messages = [
        f"[ParityCompare] {status} samples={compared_samples} old={len(old_records)} new={len(new_records)} "
        f"maxPosError={max_position_error:.3f} maxTargetError={max_target_error:.3f}"
    ]
    messages.extend(f"[ParityCompare] warning: {warning}" for warning in warnings)
    shown_failures = failures[: args.max_failures]
    messages.extend(f"[ParityCompare] failure: {failure}" for failure in shown_failures)
    hidden_count = len(failures) - len(shown_failures)
    if hidden_count > 0:
        messages.append(f"[ParityCompare] ... {hidden_count} more failure(s) omitted; rerun with --max-failures {len(failures)} to show all")
    return not failures, messages


def compare_anim(old_records, new_records, args):
    failures = []
    n = min(len(old_records), len(new_records))
    max_prog_err = 0.0
    for i in range(n):
        oa = (old_records[i].get("agents") or [{}])[0]
        na = (new_records[i].get("agents") or [{}])[0]
        label = f"sample#{i+1}"
        if oa.get("state") != na.get("state"):
            failures.append(f"{label}: state old={oa.get('state')} new={na.get('state')}")
        op, npv = oa.get("progress"), na.get("progress")
        if op is not None and npv is not None:
            err = abs(float(op) - float(npv))
            max_prog_err = max(max_prog_err, err)
            if err > args.progress_tolerance:
                failures.append(f"{label}: progress err {err:.3f} > {args.progress_tolerance:.3f}")
    status = "PASS" if not failures else "FAIL"
    msgs = [f"[ParityCompare] ANIM {status} samples={n} maxProgressError={max_prog_err:.3f}"]
    msgs += [f"[ParityCompare] failure: {f}" for f in failures[:args.max_failures]]
    return not failures, msgs


def _progress_deltas(records):
    deltas, prev = [], None
    for r in records:
        a = (r.get("agents") or [{}])[0]
        p = a.get("progress")
        if p is None: continue
        p = float(p)
        if prev is not None:
            d = p - prev
            if d < 0: d += 1.0
            deltas.append(d)
        prev = p
    return deltas


def _delta_ms(records):
    out = []
    for r in records:
        a = (r.get("agents") or [{}])[0]
        if a.get("deltaTimeMs") is not None:
            out.append(float(a["deltaTimeMs"]))
    return out


def report_jitter(label, records):
    pds = _progress_deltas(records)
    dms = _delta_ms(records)
    def stats(xs):
        if not xs: return (0,0,0,0)
        m = sum(xs)/len(xs)
        var = sum((x-m)**2 for x in xs)/len(xs)
        return (m, var**0.5, min(xs), max(xs))
    pm, psd, pmin, pmax = stats(pds)
    dm, dsd, dmin, dmax = stats(dms)
    stalls = sum(1 for d in pds if d < 1e-4)
    return [
        f"[Jitter] {label} frames={len(records)} "
        f"progDelta(mean={pm:.4f} std={psd:.4f} min={pmin:.4f} max={pmax:.4f} stalls={stalls}) "
        f"deltaMs(mean={dm:.2f} std={dsd:.2f} min={dmin:.2f} max={dmax:.2f})"
    ]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("old_trace", type=Path)
    parser.add_argument("new_trace", type=Path)
    parser.add_argument("--position-tolerance", type=float, default=1.05)
    parser.add_argument("--target-tolerance", type=float, default=1.5)
    parser.add_argument("--time-tolerance-ms", type=float, default=250.0)
    parser.add_argument("--cell-tolerance", type=int, default=8)
    parser.add_argument("--max-failures", type=int, default=60)
    parser.add_argument("--strict-json", action="store_true")
    parser.add_argument("--anim", action="store_true", help="动画 parity 模式：逐帧对齐 state/progress")
    parser.add_argument("--anim-jitter", action="store_true", help="模式 B：各自输出 progress/deltaTime 抖动统计，不判 PASS/FAIL")
    parser.add_argument("--progress-tolerance", type=float, default=0.05)
    args = parser.parse_args()

    old_records = _load_records(args.old_trace, args.strict_json)
    new_records = _load_records(args.new_trace, args.strict_json)

    if args.anim_jitter:
        for message in report_jitter("legacy", old_records):
            print(message)
        for message in report_jitter("modern", new_records):
            print(message)
        return 0

    if args.anim:
        ok, messages = compare_anim(old_records, new_records, args)
        for message in messages:
            print(message)
        return 0 if ok else 1

    ok, messages = compare(old_records, new_records, args)
    for message in messages:
        print(message)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
