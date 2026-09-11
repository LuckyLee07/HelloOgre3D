#!/usr/bin/env python3
"""Run a versioned, paired HelloOgre3D AI experiment.

The manifest is the experiment contract. Each run is isolated from inherited
HELLO_* variables, uses the internal input replay path, and preserves raw logs,
input snapshots, source/build identity, a machine-readable result, and a paired
comparison. Synthetic input is never reported as manual play evidence.
"""

from __future__ import annotations

import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import statistics
import subprocess
import sys
import time
from typing import Any, Iterable


ROOT = Path(__file__).resolve().parents[1]
TOOL_SCHEMA_VERSION = 1
ID_PATTERN = re.compile(r"^[a-z0-9][a-z0-9._-]{0,79}$")
CONFIG_LINE_PATTERN = re.compile(r"\[ConfigManager\]\s+(.+)$", re.MULTILINE)
CONFIG_PATTERN = re.compile(
    r"\[ConfigManager\]\s+preset=\s*(\S+)\s+sample=\s*(\S+)\s+seed=\s*(-?\d+)"
)
EXPERIMENT_PATTERN = re.compile(r"\[(?:AIExperiment|Sandbox19Experiment)\]\s+(.+)$")
KV_PATTERN = re.compile(r"([A-Za-z][A-Za-z0-9]*)=([^\s]+)")
FOCUS_EXECUTION_PATTERN = re.compile(
    r"\[Sandbox19Order\].*\bkind=focus\b.*\bresult=executing\b.*\blatencyMs=(\d+)"
)
WINDOW_ERROR_RULES = (
    ("graphics-device", re.compile(r"OGRE EXCEPTION.*Cannot create device", re.IGNORECASE)),
    ("ogre-exception", re.compile(r"OGRE EXCEPTION", re.IGNORECASE)),
    ("lua-error", re.compile(r"call_func error|call_string error|lua_pcall|stack traceback|PANIC:", re.IGNORECASE)),
    ("assertion", re.compile(r"Assertion failed", re.IGNORECASE)),
)
NUMERIC_METRICS = (
    "first_contact_ms",
    "wave1_clear_ms",
    "focus_execution_count",
    "focus_squad_execution_latency_ms",
    "final_commander_hp",
    "final_ally_alive",
    "final_ally_hp",
    "final_enemy_alive",
    "final_enemy_hp",
    "orders_issued",
    "orders_completed",
    "orders_failed",
    "orders_replaced",
    "orders_cancelled",
)
EXPECTED_INT_FIELDS = {
    "ai_tick_ms": (1, 60_000),
    "ai_max_per_frame": (1, 100_000),
    "commander_health": (1, 1_000_000),
    "ally_health": (1, 1_000_000),
    "enemy_health": (1, 1_000_000),
}
GENERIC_PROFILE = "event-comparison"


class ManifestError(ValueError):
    pass


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def resolve_repo_file(value: str, label: str) -> Path:
    if not isinstance(value, str) or not value.strip():
        raise ManifestError(f"{label} must be a non-empty repository-relative path")
    candidate = (ROOT / value).resolve()
    try:
        candidate.relative_to(ROOT)
    except ValueError as exc:
        raise ManifestError(f"{label} escapes the repository: {value}") from exc
    if not candidate.is_file():
        raise ManifestError(f"{label} does not exist: {value}")
    return candidate


def require_int(value: Any, label: str, minimum: int, maximum: int) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or not minimum <= value <= maximum:
        raise ManifestError(f"{label} must be an integer in [{minimum}, {maximum}]")
    return value


def validate_event(line: str, label: str) -> tuple[int, str]:
    if not isinstance(line, str) or not line.strip() or "\n" in line or "\r" in line:
        raise ManifestError(f"{label} must be one non-empty replay line")
    body = line.split("#", 1)[0].strip()
    parts = body.split()
    if len(parts) < 2:
        raise ManifestError(f"{label} has an invalid replay line: {line!r}")
    try:
        timestamp = int(parts[0])
    except ValueError as exc:
        raise ManifestError(f"{label} has a non-integer timestamp: {line!r}") from exc
    if not 0 <= timestamp <= 3_600_000:
        raise ManifestError(f"{label} timestamp is outside InputReplay bounds: {timestamp}")
    command = parts[1]
    expected_parts = {
        "key_down": 3,
        "key_up": 3,
        "key_press": 3,
        "mouse_down": 5,
        "mouse_up": 5,
        "mouse_move": 4,
        "wheel": 5,
        "quit": 2,
    }
    if command not in expected_parts or len(parts) != expected_parts[command]:
        raise ManifestError(f"{label} has an unsupported replay event: {line!r}")
    return timestamp, body


def build_replay(manifest: dict[str, Any], case: dict[str, Any]) -> list[str]:
    replay = manifest["replay"]
    merged: list[tuple[int, int, str]] = []
    for source_order, (label, lines) in enumerate(
        (("replay.common_events", replay["common_events"]),
         (f"case {case['id']} events", case.get("events", [])))
    ):
        for index, line in enumerate(lines):
            timestamp, body = validate_event(line, f"{label}[{index}]")
            merged.append((timestamp, source_order, body))
    merged.sort(key=lambda item: (item[0], item[1]))
    events = [body for _, _, body in merged]
    quits = [index for index, line in enumerate(events) if line.split()[1] == "quit"]
    if quits != [len(events) - 1]:
        raise ManifestError(f"case {case['id']} must have exactly one final quit event")
    quit_ms = int(events[-1].split()[0])
    if quit_ms <= manifest["horizon_ms"]:
        raise ManifestError(f"case {case['id']} quit must occur after horizon_ms")
    return events


def load_manifest(path: Path) -> dict[str, Any]:
    try:
        manifest = json.loads(path.read_text(encoding="utf-8-sig"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ManifestError(f"cannot read manifest: {exc}") from exc
    if not isinstance(manifest, dict):
        raise ManifestError("manifest root must be an object")
    if manifest.get("$schema_version") != TOOL_SCHEMA_VERSION:
        raise ManifestError(f"$schema_version must be {TOOL_SCHEMA_VERSION}")
    profile = manifest.setdefault("profile", "sandbox19-focus-order")
    if profile not in ("sandbox19-focus-order", GENERIC_PROFILE):
        raise ManifestError(f"unsupported profile: {profile}")
    experiment_id = manifest.get("experiment_id")
    if not isinstance(experiment_id, str) or not ID_PATTERN.fullmatch(experiment_id):
        raise ManifestError("experiment_id must be a lowercase stable id")
    for key in ("question", "sample", "preset"):
        if not isinstance(manifest.get(key), str) or not manifest[key].strip():
            raise ManifestError(f"{key} must be a non-empty string")
    manifest["horizon_ms"] = require_int(manifest.get("horizon_ms"), "horizon_ms", 1_000, 3_500_000)
    manifest["timeout_seconds"] = require_int(manifest.get("timeout_seconds"), "timeout_seconds", 10, 900)
    manifest["repetitions"] = require_int(manifest.get("repetitions"), "repetitions", 1, 20)
    seeds = manifest.get("seeds")
    if not isinstance(seeds, list) or not seeds:
        raise ManifestError("seeds must be a non-empty integer list")
    manifest["seeds"] = [require_int(seed, f"seeds[{index}]", 0, 2_147_483_647)
                         for index, seed in enumerate(seeds)]
    if len(set(manifest["seeds"])) != len(manifest["seeds"]):
        raise ManifestError("seeds must be unique")
    window = manifest.get("window")
    if not isinstance(window, dict):
        raise ManifestError("window must be an object")
    window["width"] = require_int(window.get("width"), "window.width", 640, 3840)
    window["height"] = require_int(window.get("height"), "window.height", 360, 2160)
    build = manifest.get("build")
    if not isinstance(build, dict) or not all(isinstance(build.get(k), str) and build[k] for k in ("configuration", "architecture")):
        raise ManifestError("build must name configuration and architecture")
    variable = manifest.get("variable")
    if not isinstance(variable, dict) or not isinstance(variable.get("name"), str) or not variable["name"]:
        raise ManifestError("variable must name the one changed factor")
    cases = manifest.get("cases")
    if not isinstance(cases, list) or len(cases) != 2:
        raise ManifestError("paired experiments require exactly two cases")
    case_ids: set[str] = set()
    for index, case in enumerate(cases):
        if not isinstance(case, dict):
            raise ManifestError(f"cases[{index}] must be an object")
        case_id = case.get("id")
        if not isinstance(case_id, str) or not ID_PATTERN.fullmatch(case_id) or case_id in case_ids:
            raise ManifestError(f"cases[{index}].id must be a unique lowercase stable id")
        case_ids.add(case_id)
        if "value" not in case or isinstance(case["value"], (dict, list)):
            raise ManifestError(f"case {case_id} value must be a JSON scalar")
        if not isinstance(case.get("events", []), list):
            raise ManifestError(f"case {case_id} events must be a list")
        if profile == "sandbox19-focus-order":
            case["expected_focus_execution_count"] = require_int(
                case.get("expected_focus_execution_count"),
                f"case {case_id} expected_focus_execution_count", 0, 100_000,
            )
        else:
            expected_events = case.get("expected_events")
            if not isinstance(expected_events, dict):
                raise ManifestError(f"case {case_id} expected_events must be an object")
            for list_name in ("required", "forbidden"):
                values = expected_events.get(list_name, [])
                if (not isinstance(values, list)
                        or any(not isinstance(value, str) or not value for value in values)):
                    raise ManifestError(f"case {case_id} expected_events.{list_name} must be a string list")
            fields = expected_events.get("fields", {})
            if (not isinstance(fields, dict)
                    or any(not isinstance(name, str) or not isinstance(values, dict)
                           for name, values in fields.items())):
                raise ManifestError(f"case {case_id} expected_events.fields must map event names to objects")
    replay = manifest.get("replay")
    if not isinstance(replay, dict) or replay.get("format_version") != 1:
        raise ManifestError("replay.format_version must be 1")
    replay.setdefault("wait_for_player", True)
    if not isinstance(replay["wait_for_player"], bool):
        raise ManifestError("replay.wait_for_player must be a boolean")
    if not isinstance(replay.get("common_events"), list):
        raise ManifestError("replay.common_events must be a list")
    config_files = manifest.get("config_files")
    if not isinstance(config_files, list) or not config_files:
        raise ManifestError("config_files must be a non-empty path list")
    for index, value in enumerate(config_files):
        resolve_repo_file(value, f"config_files[{index}]")
    for case in cases:
        build_replay(manifest, case)
    expected = manifest.get("expected")
    if not isinstance(expected, dict):
        raise ManifestError("expected must be an object")
    if profile == "sandbox19-focus-order" and expected.get("director") != "none":
        raise ManifestError("expected.director currently requires 'none'")
    if not isinstance(expected.get("spawn_mode"), str) or not expected["spawn_mode"]:
        raise ManifestError("expected.spawn_mode must be a non-empty string")
    if not isinstance(expected.get("ai_scheduler_enabled"), bool):
        raise ManifestError("expected.ai_scheduler_enabled must be a boolean")
    for key, bounds in EXPECTED_INT_FIELDS.items():
        if profile == "sandbox19-focus-order" or key in expected:
            expected[key] = require_int(expected.get(key), f"expected.{key}", *bounds)
    for key in ("agent_count", "light_team_count"):
        if key in expected:
            expected[key] = require_int(expected[key], f"expected.{key}", 0, 100_000)
    if profile == GENERIC_PROFILE:
        metrics = manifest.get("metrics")
        if not isinstance(metrics, list) or not metrics:
            raise ManifestError("event-comparison profile requires a non-empty metrics list")
        metric_names: set[str] = set()
        for index, metric in enumerate(metrics):
            if not isinstance(metric, dict):
                raise ManifestError(f"metrics[{index}] must be an object")
            for key in ("name", "label", "event", "field"):
                if not isinstance(metric.get(key), str) or not metric[key]:
                    raise ManifestError(f"metrics[{index}].{key} must be a non-empty string")
            if metric["name"] in metric_names:
                raise ManifestError(f"duplicate metric name: {metric['name']}")
            if "optional" in metric and not isinstance(metric["optional"], bool):
                raise ManifestError(f"metrics[{index}].optional must be a boolean")
            metric_names.add(metric["name"])
    return manifest


def git_output(arguments: list[str], binary: bool = False) -> bytes | str:
    completed = subprocess.run(
        ["git", *arguments], cwd=ROOT, check=True, stdout=subprocess.PIPE,
        stderr=subprocess.PIPE, text=not binary,
    )
    return completed.stdout


def workspace_metadata(allow_dirty: bool, output: Path) -> dict[str, Any]:
    head = str(git_output(["rev-parse", "HEAD"])).strip()
    status = str(git_output(["status", "--porcelain=v1", "--untracked-files=all"]))
    tracked_diff = bytes(git_output(["diff", "--binary", "HEAD", "--"], binary=True))
    tracked_clean = not tracked_diff
    if not tracked_clean and not allow_dirty:
        raise ManifestError("tracked workspace changes exist; commit them or rerun with --allow-dirty to snapshot the patch")
    (output / "workspace-status.txt").write_text(status, encoding="utf-8", newline="\n")
    patch_name = None
    patch_hash = None
    if tracked_diff:
        patch_name = "workspace.patch"
        (output / patch_name).write_bytes(tracked_diff)
        patch_hash = hashlib.sha256(tracked_diff).hexdigest()
    return {
        "head": head,
        "tracked_clean": tracked_clean,
        "workspace_clean": not status,
        "status_lines": status.splitlines(),
        "tracked_patch": patch_name,
        "tracked_patch_sha256": patch_hash,
    }


def snapshot_inputs(manifest_path: Path, manifest: dict[str, Any], output: Path) -> list[dict[str, Any]]:
    inputs = output / "inputs"
    inputs.mkdir()
    normalized_manifest = inputs / "manifest.json"
    normalized_manifest.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8", newline="\n")
    records = [{"path": "manifest.json", "sha256": sha256_file(normalized_manifest)}]
    tool_copy = inputs / "run_ai_experiment.py"
    shutil.copy2(Path(__file__).resolve(), tool_copy)
    records.append({"path": tool_copy.relative_to(inputs).as_posix(), "sha256": sha256_file(tool_copy)})
    original_copy = inputs / "manifest-source.json"
    shutil.copy2(manifest_path, original_copy)
    records.append({"path": original_copy.relative_to(inputs).as_posix(), "sha256": sha256_file(original_copy)})
    for value in manifest["config_files"]:
        source = resolve_repo_file(value, "config file")
        target = inputs / "repository" / Path(value)
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        records.append({"path": target.relative_to(inputs).as_posix(), "sha256": sha256_file(target)})
    return records


def parse_int(fields: dict[str, str], key: str) -> int | None:
    value = fields.get(key)
    if value is None:
        return None
    try:
        return int(value)
    except ValueError:
        return None


def health_total(value: str | None) -> int | None:
    if value is None or value == "-":
        return None
    try:
        return sum(int(part) for part in value.split(","))
    except ValueError:
        return None


def fatal_from_text(text: str) -> tuple[str | None, str | None]:
    for category, pattern in WINDOW_ERROR_RULES:
        match = pattern.search(text)
        if match:
            line_start = text.rfind("\n", 0, match.start()) + 1
            line_end = text.find("\n", match.end())
            if line_end < 0:
                line_end = len(text)
            return category, text[line_start:line_end].strip()
    return None, None


def parse_config_evidence(text: str) -> dict[str, Any] | None:
    candidates: list[dict[str, Any]] = []
    for match in CONFIG_LINE_PATTERN.finditer(text):
        fields = dict(KV_PATTERN.findall(match.group(1)))
        if not all(key in fields for key in ("preset", "sample", "seed")):
            continue
        seed = parse_int(fields, "seed")
        if seed is None:
            continue
        candidate: dict[str, Any] = {
            "preset": fields["preset"],
            "sample": fields["sample"],
            "seed": seed,
        }
        if "spawnMode" in fields:
            candidate["spawn_mode"] = fields["spawnMode"]
        if fields.get("aiScheduler") in ("true", "false"):
            candidate["ai_scheduler_enabled"] = fields["aiScheduler"] == "true"
        if "tickMs" in fields:
            candidate["ai_tick_ms"] = parse_int(fields, "tickMs")
        if "maxPerFrame" in fields:
            candidate["ai_max_per_frame"] = parse_int(fields, "maxPerFrame")
        if "agents" in fields:
            candidate["agent_count"] = parse_int(fields, "agents")
        if "light" in fields:
            candidate["light_team_count"] = parse_int(fields, "light")
        candidates.append(candidate)
    if candidates:
        return max(candidates, key=len)
    match = CONFIG_PATTERN.search(text)
    if match:
        return {
            "preset": match.group(1),
            "sample": match.group(2),
            "seed": int(match.group(3)),
        }
    return None


def analyze_run_log(
    text: str,
    expected_run_id: str,
    expected_seed: int,
    expected_sample: str,
    expected_preset: str,
    horizon_ms: int,
    exit_code: int | None,
    wall_timeout: bool = False,
    forced_error: str | None = None,
    require_window_evidence: bool = False,
    expected_director: str | None = None,
    expected_conditions: dict[str, Any] | None = None,
    expected_focus_execution_count: int | None = None,
    expected_events: dict[str, Any] | None = None,
    metric_definitions: list[dict[str, Any]] | None = None,
) -> dict[str, Any]:
    error_category, error_line = fatal_from_text(text)
    if forced_error is not None:
        error_category = forced_error
    config = parse_config_evidence(text)
    events: list[dict[str, Any]] = []
    for line in text.splitlines():
        match = EXPERIMENT_PATTERN.search(line)
        if not match:
            continue
        fields = dict(KV_PATTERN.findall(match.group(1)))
        if fields.get("runId") != expected_run_id:
            continue
        event: dict[str, Any] = {}
        for key, value in fields.items():
            if re.fullmatch(r"-?\d+", value):
                event[key] = int(value)
            elif value in ("true", "false"):
                event[key] = value == "true"
            else:
                event[key] = value
        events.append(event)
    terminal = next((event for event in reversed(events) if event.get("event") == "terminal"), None)
    horizon = next((event for event in reversed(events) if event.get("event") == "horizon"), None)
    final = terminal or horizon or (events[-1] if events else None)
    first_contact = next((event for event in events if event.get("event") == "first-contact"), None)
    wave1_clear = next((event for event in events
                        if event.get("event") == "phase" and event.get("state") == "ADVANCE"), None)
    focus_execution_latencies = [int(value) for value in FOCUS_EXECUTION_PATTERN.findall(text)]
    replay_complete = "[InputReplay] synthetic=true completed reason=quit" in text
    ready = (("[Sandbox19] ready" in text) if expected_events is None else
             any(event.get("event") == "ready" for event in events))
    window_ok = bool(re.search(
        r"\[WindowMode\] background=true hidden=false noActivate=true offscreen=true foregroundUnchanged=true", text))
    input_disabled = "[WindowMode] background=true physical-input=disabled" in text
    reasons: list[str] = []
    if wall_timeout:
        reasons.append("wall-timeout")
    if error_category:
        reasons.append(error_category)
    if exit_code not in (None, 0) and not error_category:
        reasons.append("nonzero-exit")
    if config is None:
        reasons.append("missing-config")
    else:
        required_config = {"preset": expected_preset, "sample": expected_sample, "seed": expected_seed}
        if any(config.get(key) != value for key, value in required_config.items()):
            reasons.append("config-mismatch")
        if expected_conditions is not None:
            for key in ("spawn_mode", "ai_scheduler_enabled", "ai_tick_ms", "ai_max_per_frame",
                        "agent_count", "light_team_count"):
                if key in expected_conditions and config.get(key) != expected_conditions.get(key):
                    reasons.append("config-mismatch")
                    break
    if not ready:
        reasons.append("missing-ready")
    if not events:
        reasons.append("missing-experiment-events")
    else:
        ready_event = next((event for event in events if event.get("event") == "ready"), None)
        required_event_fields = (
            ("schema", "event", "simulationMs", "matchElapsedMs") if expected_events is not None else
            ("schema", "event", "simulationMs", "matchElapsedMs", "state", "wave", "commanderHp",
             "allyAlive", "allyHp", "enemyAlive", "enemyHp", "issued", "completed", "failed",
             "replaced", "cancelled", "active", "director", "spawnMode", "aiSchedulerEnabled", "aiTickMs",
             "aiMaxPerFrame", "commanderMaxHp", "allyMaxHp", "enemyMaxHp")
        )
        if ready_event is None or events[0] is not ready_event:
            reasons.append("missing-initial-ready-event")
        if any(any(field not in event or event[field] is None for field in required_event_fields)
               for event in events):
            reasons.append("malformed-experiment-event")
        if any(event.get("schema") != TOOL_SCHEMA_VERSION for event in events):
            reasons.append("experiment-schema-mismatch")
        simulation_times = [event.get("simulationMs") for event in events]
        if all(isinstance(value, int) for value in simulation_times) and simulation_times != sorted(simulation_times):
            reasons.append("nonmonotonic-experiment-events")
        if expected_conditions is not None and ready_event is not None:
            event_key_by_condition = {
                "director": "director",
                "spawn_mode": "spawnMode",
                "ai_scheduler_enabled": "aiSchedulerEnabled",
                "ai_tick_ms": "aiTickMs",
                "ai_max_per_frame": "aiMaxPerFrame",
                "commander_health": "commanderMaxHp",
                "ally_health": "allyMaxHp",
                "enemy_health": "enemyMaxHp",
                "agent_count": "agentCount",
                "light_team_count": "lightTeamCount",
            }
            expected_event = {event_key_by_condition[key]: value for key, value in expected_conditions.items()
                              if key in event_key_by_condition}
            if any(ready_event.get(key) != value for key, value in expected_event.items()):
                reasons.append("experiment-config-mismatch")
        if expected_events is not None:
            event_names = [event.get("event") for event in events]
            if any(name not in event_names for name in expected_events.get("required", [])):
                reasons.append("missing-required-event")
            if any(name in event_names for name in expected_events.get("forbidden", [])):
                reasons.append("forbidden-event")
            for event_name, expected_fields in expected_events.get("fields", {}).items():
                matched_event = next((event for event in events if event.get("event") == event_name), None)
                if (matched_event is None
                        or any(matched_event.get(key) != value for key, value in expected_fields.items())):
                    reasons.append("event-field-mismatch")
                    break
    if not replay_complete:
        reasons.append("missing-replay-complete")
    if terminal is None and horizon is None:
        reasons.append("missing-terminal-or-horizon")
    if require_window_evidence and not (window_ok and input_disabled):
        reasons.append("missing-background-evidence")
    if expected_director is not None and events and any(event.get("director") != expected_director for event in events):
        reasons.append("director-mismatch")
    if (expected_focus_execution_count is not None
            and len(focus_execution_latencies) != expected_focus_execution_count):
        reasons.append("focus-execution-count-mismatch")
    if terminal is not None and terminal.get("state") in ("VICTORY", "DEFEAT"):
        outcome = terminal["state"]
    elif horizon is not None and (horizon.get("matchElapsedMs") or 0) >= horizon_ms:
        outcome = "TIME_LIMIT"
    else:
        outcome = "UNRESOLVED"
        reasons.append("unresolved-outcome")
    if metric_definitions is not None:
        metrics = {}
        for definition in metric_definitions:
            source = next((event for event in events if event.get("event") == definition["event"]), None)
            value = source.get(definition["field"]) if source is not None else None
            numeric_value = value if isinstance(value, (int, float)) and not isinstance(value, bool) else None
            metrics[definition["name"]] = numeric_value
            field_missing = source is not None and definition["field"] not in source
            if source is not None and numeric_value is None and not (field_missing and definition.get("optional", False)):
                reasons.append("invalid-metric-value")
    else:
        metrics = {
            "first_contact_ms": first_contact.get("matchElapsedMs") if first_contact else None,
            "wave1_clear_ms": wave1_clear.get("matchElapsedMs") if wave1_clear else None,
            "focus_execution_count": len(focus_execution_latencies),
            "focus_squad_execution_latency_ms": max(focus_execution_latencies) if focus_execution_latencies else None,
            "final_commander_hp": final.get("commanderHp") if final else None,
            "final_ally_alive": final.get("allyAlive") if final else None,
            "final_ally_hp": health_total(final.get("allyHp") if final else None),
            "final_enemy_alive": final.get("enemyAlive") if final else None,
            "final_enemy_hp": health_total(final.get("enemyHp") if final else None),
            "orders_issued": final.get("issued") if final else None,
            "orders_completed": final.get("completed") if final else None,
            "orders_failed": final.get("failed") if final else None,
            "orders_replaced": final.get("replaced") if final else None,
            "orders_cancelled": final.get("cancelled") if final else None,
        }
    status = "PASS" if not reasons else "FAIL"
    return {
        "status": status,
        "classification": reasons[0] if reasons else "evidence-complete",
        "reasons": reasons,
        "outcome": outcome,
        "exit_code": exit_code,
        "config": config,
        "ready": ready,
        "replay_complete": replay_complete,
        "background_window_verified": window_ok,
        "physical_input_disabled": input_disabled,
        "error_line": error_line,
        "event_count": len(events),
        "events": events,
        "final_snapshot": final,
        "metrics": metrics,
    }


def stop_child(child: subprocess.Popen[Any]) -> None:
    if child.poll() is not None:
        return
    child.terminate()
    try:
        child.wait(timeout=5)
    except subprocess.TimeoutExpired:
        child.kill()
        child.wait()


def read_current_log(path: Path, previous_mtime_ns: int | None) -> str:
    try:
        stat = path.stat()
        if previous_mtime_ns is not None and stat.st_mtime_ns == previous_mtime_ns:
            return ""
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def run_one(
    executable: Path,
    manifest: dict[str, Any],
    case: dict[str, Any],
    seed: int,
    repetition: int,
    run_index: int,
    batch_id: str,
    output: Path,
) -> dict[str, Any]:
    run_id = f"{batch_id}-{case['id']}-s{seed}-r{repetition}"
    run_dir = output / "runs" / f"{run_index:03d}-{case['id']}-s{seed}-r{repetition}"
    run_dir.mkdir(parents=True)
    replay_lines = build_replay(manifest, case)
    replay_path = run_dir / "input-replay.txt"
    replay_path.write_text("# hello-input-replay v1\n" + "\n".join(replay_lines) + "\n",
                           encoding="utf-8", newline="\n")
    stdout_path = run_dir / "stdout.log"
    engine_log = executable.parent / "Sandbox.log"
    try:
        previous_mtime_ns = engine_log.stat().st_mtime_ns
    except OSError:
        previous_mtime_ns = None
    env = {key: value for key, value in os.environ.items() if not key.startswith("HELLO_")}
    env.update({
        "HELLO_WINDOW_BACKGROUND": "1",
        "HELLO_WINDOW_WIDTH": str(manifest["window"]["width"]),
        "HELLO_WINDOW_HEIGHT": str(manifest["window"]["height"]),
        "HELLO_AUDIO_SILENT": "1",
        "HELLO_SANDBOX_SAMPLE": manifest["sample"],
        "HELLO_SAMPLE_PRESET": manifest["preset"],
        "HELLO_SAMPLE_SEED": str(seed),
        "HELLO_INPUT_REPLAY": str(replay_path.resolve()),
        "HELLO_INPUT_REPLAY_WAIT_FOR_PLAYER": "1" if manifest["replay"]["wait_for_player"] else "0",
        "HELLO_EXPERIMENT_RUN_ID": run_id,
        "HELLO_EXPERIMENT_HORIZON_MS": str(manifest["horizon_ms"]),
        "HELLO_EXPERIMENT_CASE_ID": case["id"],
        "HELLO_EXPERIMENT_CASE_VALUE": json.dumps(case["value"], ensure_ascii=True, separators=(",", ":")),
    })
    started_at = utc_now()
    start = time.monotonic()
    wall_timeout = False
    forced_error = None
    creation_flags = subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
    with stdout_path.open("wb") as stream:
        child = subprocess.Popen([str(executable)], cwd=executable.parent, env=env,
                                 stdout=stream, stderr=subprocess.STDOUT,
                                 creationflags=creation_flags)
        try:
            deadline = start + manifest["timeout_seconds"]
            while child.poll() is None:
                current = read_current_log(engine_log, previous_mtime_ns)
                error_category, _ = fatal_from_text(current)
                if error_category:
                    forced_error = error_category
                    break
                if time.monotonic() >= deadline:
                    wall_timeout = True
                    break
                time.sleep(0.2)
        finally:
            stop_child(child)
    wall_seconds = time.monotonic() - start
    engine_text = read_current_log(engine_log, previous_mtime_ns)
    if engine_text:
        (run_dir / "engine.log").write_text(engine_text, encoding="utf-8", newline="\n")
    stdout_text = stdout_path.read_text(encoding="utf-8", errors="replace")
    has_engine_events = "[Sandbox19Experiment]" in engine_text or "[AIExperiment]" in engine_text
    analysis_text = engine_text if has_engine_events or fatal_from_text(engine_text)[0] else stdout_text
    result = analyze_run_log(
        analysis_text, run_id, seed, manifest["sample"], manifest["preset"], manifest["horizon_ms"],
        child.returncode, wall_timeout=wall_timeout, forced_error=forced_error,
        require_window_evidence=os.name == "nt",
        expected_director=manifest.get("expected", {}).get("director"),
        expected_conditions=manifest.get("expected"),
        expected_focus_execution_count=case.get("expected_focus_execution_count"),
        expected_events=case.get("expected_events"),
        metric_definitions=manifest.get("metrics"),
    )
    result.update({
        "run_id": run_id,
        "case_id": case["id"],
        "case_value": case["value"],
        "seed": seed,
        "repetition": repetition,
        "execution_index": run_index,
        "started_at_utc": started_at,
        "wall_seconds": round(wall_seconds, 3),
        "synthetic_input": True,
        "direct_state_mutation": manifest.get("direct_state_mutation", False),
        "replay_sha256": sha256_file(replay_path),
        "log": str((run_dir / ("engine.log" if engine_text else "stdout.log")).relative_to(output)).replace("\\", "/"),
    })
    (run_dir / "result.json").write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n",
                                         encoding="utf-8", newline="\n")
    return result


def metric_stats(values: Iterable[int | float | None]) -> dict[str, Any] | None:
    numbers = [value for value in values if isinstance(value, (int, float)) and not isinstance(value, bool)]
    if not numbers:
        return None
    return {
        "count": len(numbers),
        "values": numbers,
        "min": min(numbers),
        "median": statistics.median(numbers),
        "max": max(numbers),
    }


def build_comparison(manifest: dict[str, Any], results: list[dict[str, Any]], planned_runs: int) -> dict[str, Any]:
    metric_names = ([definition["name"] for definition in manifest["metrics"]]
                    if manifest.get("metrics") else list(NUMERIC_METRICS))
    case_summaries = []
    for case in manifest["cases"]:
        case_runs = [run for run in results if run["case_id"] == case["id"]]
        valid_runs = [run for run in case_runs if run["status"] == "PASS"]
        case_summaries.append({
            "case_id": case["id"],
            "value": case["value"],
            "run_count": len(case_runs),
            "infrastructure_pass": len(valid_runs),
            "failure_classifications": dict(sorted(Counter(
                run["classification"] for run in case_runs if run["status"] != "PASS").items())),
            "outcomes": dict(sorted(Counter(run["outcome"] for run in valid_runs).items())),
            "metrics": {metric: metric_stats(run["metrics"].get(metric) for run in valid_runs)
                        for metric in metric_names},
        })
    control, variant = manifest["cases"]
    paired = []
    by_key = {(run["case_id"], run["seed"], run["repetition"]): run for run in results}
    for seed in manifest["seeds"]:
        for repetition in range(1, manifest["repetitions"] + 1):
            left = by_key.get((control["id"], seed, repetition))
            right = by_key.get((variant["id"], seed, repetition))
            deltas = {}
            if left and right and left["status"] == right["status"] == "PASS":
                for metric in metric_names:
                    left_value = left["metrics"].get(metric)
                    right_value = right["metrics"].get(metric)
                    deltas[metric] = (right_value - left_value
                                      if isinstance(left_value, (int, float)) and isinstance(right_value, (int, float))
                                      else None)
            paired.append({
                "seed": seed,
                "repetition": repetition,
                "complete": left is not None and right is not None,
                "control_outcome": left["outcome"] if left else None,
                "variant_outcome": right["outcome"] if right else None,
                "both_infrastructure_pass": bool(left and right and left["status"] == right["status"] == "PASS"),
                "variant_minus_control": deltas,
            })
    infrastructure_ok = len(results) == planned_runs and all(run["status"] == "PASS" for run in results)
    return {
        "$schema_version": TOOL_SCHEMA_VERSION,
        "experiment_id": manifest["experiment_id"],
        "status": "PASS" if infrastructure_ok else "FAIL",
        "synthetic_input": True,
        "manual_play_verified": False,
        "planned_run_count": planned_runs,
        "executed_run_count": len(results),
        "variable": manifest["variable"],
        "case_summaries": case_summaries,
        "paired_runs": paired,
        "limitations": manifest.get("limitations", [
            "InputReplay uses internal synthetic events and does not verify physical input or feel.",
            "Fixed seeds do not guarantee frame-identical Bullet simulation across runs or machines.",
            "This small diagnostic comparison does not establish a general win-rate effect.",
        ]),
    }


def format_range(stats: dict[str, Any] | None) -> str:
    if stats is None:
        return "—"
    return f"{stats['median']:g} [{stats['min']:g}, {stats['max']:g}]"


def write_markdown_summary(
    manifest: dict[str, Any], metadata: dict[str, Any], comparison: dict[str, Any], output: Path
) -> None:
    lines = [
        f"# {manifest['experiment_id']} 成对实验摘要",
        "",
        f"- 状态：`{comparison['status']}`（表示运行与证据是否完整，不代表某一策略获胜）",
        f"- 问题：{manifest['question']}",
        f"- 版本：`{metadata['workspace']['head']}`；tracked clean=`{str(metadata['workspace']['tracked_clean']).lower()}`",
        f"- 条件：sample `{manifest['sample']}`，preset `{manifest['preset']}`，"
        f"{manifest['window']['width']}×{manifest['window']['height']}，观察窗 {manifest['horizon_ms']}ms",
        f"- 单变量：`{manifest['variable']['name']}`；内部输入回放，`synthetic_input=true`",
        "",
    ]
    generic_metrics = manifest.get("metrics")
    if generic_metrics:
        metric_labels = [definition["label"] for definition in generic_metrics]
        lines.extend([
            "| 用例 | 值 | 运行/通过 | 结果 | " + " | ".join(metric_labels) + " |",
            "|---|---:|---:|---|" + "---:|" * len(metric_labels),
        ])
    else:
        lines.extend([
            "| 用例 | 值 | 运行/通过 | 游戏结果 | 首次接敌 ms | 第一段清场 ms | 集火执行数 | 集火接管 ms | 最终队友存活 | 最终队友 HP | 最终敌人存活 | 最终敌人 HP |",
            "|---|---:|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|",
        ])
    for case in comparison["case_summaries"]:
        metrics = case["metrics"]
        outcomes = ", ".join(f"{name}×{count}" for name, count in case["outcomes"].items()) or "—"
        if generic_metrics:
            values = " | ".join(format_range(metrics[definition["name"]]) for definition in generic_metrics)
            lines.append(
                f"| `{case['case_id']}` | `{json.dumps(case['value'], ensure_ascii=False)}` | "
                f"{case['run_count']}/{case['infrastructure_pass']} | {outcomes} | {values} |"
            )
        else:
            lines.append(
                f"| `{case['case_id']}` | `{json.dumps(case['value'], ensure_ascii=False)}` | "
                f"{case['run_count']}/{case['infrastructure_pass']} | {outcomes} | "
                f"{format_range(metrics['first_contact_ms'])} | {format_range(metrics['wave1_clear_ms'])} | "
                f"{format_range(metrics['focus_execution_count'])} | "
                f"{format_range(metrics['focus_squad_execution_latency_ms'])} | "
                f"{format_range(metrics['final_ally_alive'])} | {format_range(metrics['final_ally_hp'])} | "
                f"{format_range(metrics['final_enemy_alive'])} | {format_range(metrics['final_enemy_hp'])} |"
            )
    lines.extend([
        "",
        "逐局原始值、成对差值、配置指纹与日志见同目录 `summary.json`、`metadata.json` 和 `runs/`。",
        *(manifest.get("summary_limitations", comparison["limitations"]) if generic_metrics else [
            "固定 seed 不保证 Bullet 跨运行逐帧一致；本实验规模只用于诊断当前场景，不能外推一般胜率。",
            "人工输入、操控手感与听感未由本实验验证。",
        ]),
        "",
    ])
    (output / "summary.md").write_text("\n".join(lines), encoding="utf-8", newline="\n")


def execution_matrix(manifest: dict[str, Any]) -> list[tuple[dict[str, Any], int, int]]:
    matrix = []
    for seed in manifest["seeds"]:
        for repetition in range(1, manifest["repetitions"] + 1):
            cases = manifest["cases"] if repetition % 2 == 1 else list(reversed(manifest["cases"]))
            for case in cases:
                matrix.append((case, seed, repetition))
    return matrix


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest", type=Path, help="versioned experiment manifest JSON")
    parser.add_argument("--output", type=Path, help="new output directory; defaults under tmp/")
    parser.add_argument("--executable", type=Path,
                        default=ROOT / "bin" / ("HelloOgre3D.exe" if os.name == "nt" else "HelloOgre3D"))
    parser.add_argument("--allow-dirty", action="store_true",
                        help="allow tracked changes and preserve git diff --binary in the output")
    parser.add_argument("--dry-run", action="store_true", help="validate and print the run matrix without starting the game")
    parser.add_argument("--continue-on-infrastructure-error", action="store_true",
                        help="continue remaining runs after a startup/runtime infrastructure failure")
    args = parser.parse_args()
    try:
        manifest_path = args.manifest.resolve()
        manifest = load_manifest(manifest_path)
        executable = args.executable.resolve()
        if not executable.is_file():
            raise ManifestError(f"executable does not exist: {executable}")
        matrix = execution_matrix(manifest)
        if args.dry_run:
            print(f"[AIExperiment] dry-run id={manifest['experiment_id']} runs={len(matrix)}")
            for index, (case, seed, repetition) in enumerate(matrix, 1):
                print(f"  {index:02d} case={case['id']} value={case['value']!r} seed={seed} repetition={repetition}")
            return 0
        if args.output:
            output = args.output.resolve()
        else:
            stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
            output = ROOT / "tmp" / f"ai-experiment-{manifest['experiment_id']}-{stamp}"
        if output.exists():
            raise ManifestError(f"output already exists: {output}")
        output.mkdir(parents=True)
        workspace = workspace_metadata(args.allow_dirty, output)
        input_files = snapshot_inputs(manifest_path, manifest, output)
        metadata = {
            "$schema_version": TOOL_SCHEMA_VERSION,
            "experiment_id": manifest["experiment_id"],
            "created_at_utc": utc_now(),
            "platform": platform.platform(),
            "machine": platform.machine(),
            "python": platform.python_version(),
            "build": manifest["build"],
            "workspace": workspace,
            "executable": {"path": str(executable), "sha256": sha256_file(executable)},
            "inputs": input_files,
            "synthetic_input": True,
        }
        (output / "metadata.json").write_text(json.dumps(metadata, ensure_ascii=False, indent=2) + "\n",
                                               encoding="utf-8", newline="\n")
        batch_id = datetime.now().strftime("exp-%Y%m%d%H%M%S")
        results = []
        for index, (case, seed, repetition) in enumerate(matrix, 1):
            print(f"[AIExperiment] run={index}/{len(matrix)} case={case['id']} seed={seed} repetition={repetition}", flush=True)
            result = run_one(executable, manifest, case, seed, repetition, index, batch_id, output)
            results.append(result)
            print(f"[AIExperiment] run={index} status={result['status']} outcome={result['outcome']} "
                  f"classification={result['classification']}", flush=True)
            if result["status"] != "PASS" and not args.continue_on_infrastructure_error:
                print("[AIExperiment] stopping after infrastructure failure", flush=True)
                break
        comparison = build_comparison(manifest, results, len(matrix))
        (output / "summary.json").write_text(json.dumps(comparison, ensure_ascii=False, indent=2) + "\n",
                                              encoding="utf-8", newline="\n")
        write_markdown_summary(manifest, metadata, comparison, output)
        print(f"[AIExperiment] status={comparison['status']} output={output}", flush=True)
        return 0 if comparison["status"] == "PASS" else 1
    except ManifestError as exc:
        parser.error(str(exc))
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
