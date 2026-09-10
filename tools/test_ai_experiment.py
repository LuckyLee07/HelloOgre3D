#!/usr/bin/env python3
"""Contract tests for the M2 experiment log parser."""

from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import run_ai_experiment as experiment


WINDOW = """
[WindowMode] background=true hidden=true foregroundUnchanged=true pixels=1280x800
[WindowMode] hidden=true physical-input=disabled
"""
CONFIG = "[ConfigManager] preset=Sandbox19 sample=Sandbox19 seed=20260710 agents=7 light=3 spawnMode=fixed aiScheduler=true tickMs=50 maxPerFrame=6 spawnPoints=7"
EXPECTED = {
    "director": "none",
    "spawn_mode": "fixed",
    "ai_scheduler_enabled": True,
    "ai_tick_ms": 50,
    "ai_max_per_frame": 6,
    "commander_health": 160,
    "ally_health": 240,
    "enemy_health": 100,
}
EVENT_CONFIG = " spawnMode=fixed aiSchedulerEnabled=true aiTickMs=50 aiMaxPerFrame=6 commanderMaxHp=160 allyMaxHp=240 enemyMaxHp=100"


def analyze(body, exit_code=0, wall_timeout=False, forced_error=None, expected_focus_count=None):
    body = body.replace(
        "[ConfigManager] preset= Sandbox19 sample= Sandbox19 seed= 20260710 agents= 7 spawnMode= fixed deterministic= true",
        CONFIG,
    ).replace(" director=none", " director=none" + EVENT_CONFIG)
    return experiment.analyze_run_log(
        WINDOW + body,
        expected_run_id="m2-test",
        expected_seed=20260710,
        expected_sample="Sandbox19",
        expected_preset="Sandbox19",
        horizon_ms=30000,
        exit_code=exit_code,
        wall_timeout=wall_timeout,
        forced_error=forced_error,
        require_window_evidence=True,
        expected_director="none",
        expected_conditions=EXPECTED,
        expected_focus_execution_count=expected_focus_count,
    )


class ExperimentLogContractTest(unittest.TestCase):
    def test_manifest_defaults_replay_to_wait_for_player(self):
        manifest = experiment.load_manifest(
            Path(__file__).resolve().parent / "experiments" / "sandbox19-focus-order.json"
        )
        self.assertTrue(manifest["replay"]["wait_for_player"])

    def test_terminal_victory_is_valid_game_outcome(self):
        result = analyze("""
[ConfigManager] preset= Sandbox19 sample= Sandbox19 seed= 20260710 agents= 7 spawnMode= fixed deterministic= true
[Sandbox19] ready playerId=229 role=commander mission=relay-outpost
[Sandbox19Experiment] schema=1 runId=m2-test event=ready simulationMs=0 matchElapsedMs=0 state=PREPARE wave=0 commanderHp=160 allyAlive=2 allyHp=240,240 enemyAlive=0 enemyHp=- issued=0 completed=0 failed=0 replaced=0 cancelled=0 active=0 director=none
[Sandbox19Experiment] schema=1 runId=m2-test event=first-contact simulationMs=4200 matchElapsedMs=4200 state=WAVE wave=1 commanderHp=160 allyAlive=2 allyHp=235,240 enemyAlive=2 enemyHp=95,100 issued=2 completed=0 failed=0 replaced=0 cancelled=0 active=2 director=none
[Sandbox19Order] agent=230 kind=focus result=executing source=bt-condition simulationMs=5542 issuedMs=5500 latencyMs=42
[Sandbox19Order] agent=231 kind=focus result=executing source=bt-condition simulationMs=5550 issuedMs=5500 latencyMs=50
[Sandbox19Experiment] schema=1 runId=m2-test event=phase simulationMs=18000 matchElapsedMs=18000 state=ADVANCE wave=1 commanderHp=160 allyAlive=2 allyHp=210,225 enemyAlive=0 enemyHp=0,0 issued=2 completed=2 failed=0 replaced=0 cancelled=0 active=0 director=none
[Sandbox19Experiment] schema=1 runId=m2-test event=terminal simulationMs=28000 matchElapsedMs=28000 state=VICTORY wave=2 commanderHp=150 allyAlive=2 allyHp=180,200 enemyAlive=0 enemyHp=0,0,0,0 issued=4 completed=4 failed=0 replaced=0 cancelled=0 active=0 director=none
[InputReplay] synthetic=true completed reason=quit
""", expected_focus_count=2)
        self.assertEqual("PASS", result["status"])
        self.assertEqual("VICTORY", result["outcome"])
        self.assertEqual(4200, result["metrics"]["first_contact_ms"])
        self.assertEqual(18000, result["metrics"]["wave1_clear_ms"])
        self.assertEqual(50, result["metrics"]["focus_squad_execution_latency_ms"])
        self.assertEqual(380, result["metrics"]["final_ally_hp"])

    def test_horizon_without_terminal_is_time_limit_not_runner_failure(self):
        result = analyze("""
[ConfigManager] preset= Sandbox19 sample= Sandbox19 seed= 20260710 agents= 7 spawnMode= fixed deterministic= true
[Sandbox19] ready playerId=229 role=commander mission=relay-outpost
[Sandbox19Experiment] schema=1 runId=m2-test event=ready simulationMs=0 matchElapsedMs=0 state=PREPARE wave=0 commanderHp=160 allyAlive=2 allyHp=240,240 enemyAlive=0 enemyHp=- issued=0 completed=0 failed=0 replaced=0 cancelled=0 active=0 director=none
[Sandbox19Experiment] schema=1 runId=m2-test event=horizon simulationMs=30020 matchElapsedMs=30020 state=ADVANCE wave=1 commanderHp=160 allyAlive=1 allyHp=0,145 enemyAlive=0 enemyHp=0,0 issued=0 completed=0 failed=0 replaced=0 cancelled=0 active=0 director=none
[InputReplay] synthetic=true completed reason=quit
""", expected_focus_count=0)
        self.assertEqual("PASS", result["status"])
        self.assertEqual("TIME_LIMIT", result["outcome"])
        self.assertEqual(1, result["metrics"]["final_ally_alive"])

    def test_graphics_device_failure_is_infrastructure_failure(self):
        result = analyze("""
OGRE EXCEPTION(3:RenderingAPIException): Cannot create device! in D3D9Device::createD3D9Device
""", exit_code=1)
        self.assertEqual("FAIL", result["status"])
        self.assertEqual("graphics-device", result["classification"])
        self.assertEqual("UNRESOLVED", result["outcome"])

    def test_declared_runtime_config_mismatch_fails_evidence(self):
        result = analyze("""
[ConfigManager] preset=Sandbox19 sample=Sandbox19 seed=20260710 agents=7 light=3 spawnMode=random aiScheduler=true tickMs=50 maxPerFrame=6 spawnPoints=7
[Sandbox19] ready playerId=229 role=commander mission=relay-outpost
[Sandbox19Experiment] schema=1 runId=m2-test event=ready simulationMs=0 matchElapsedMs=0 state=PREPARE wave=0 commanderHp=160 allyAlive=2 allyHp=240,240 enemyAlive=0 enemyHp=- issued=0 completed=0 failed=0 replaced=0 cancelled=0 active=0 director=none
[Sandbox19Experiment] schema=1 runId=m2-test event=horizon simulationMs=30000 matchElapsedMs=30000 state=WAVE wave=1 commanderHp=160 allyAlive=2 allyHp=220,240 enemyAlive=2 enemyHp=80,100 issued=0 completed=0 failed=0 replaced=0 cancelled=0 active=0 director=none
[InputReplay] synthetic=true completed reason=quit
""")
        self.assertEqual("FAIL", result["status"])
        self.assertIn("config-mismatch", result["reasons"])

    def test_missing_declared_focus_execution_fails_evidence(self):
        result = analyze("""
[ConfigManager] preset= Sandbox19 sample= Sandbox19 seed= 20260710 agents= 7 spawnMode= fixed deterministic= true
[Sandbox19] ready playerId=229 role=commander mission=relay-outpost
[Sandbox19Experiment] schema=1 runId=m2-test event=ready simulationMs=0 matchElapsedMs=0 state=PREPARE wave=0 commanderHp=160 allyAlive=2 allyHp=240,240 enemyAlive=0 enemyHp=- issued=0 completed=0 failed=0 replaced=0 cancelled=0 active=0 director=none
[Sandbox19Experiment] schema=1 runId=m2-test event=horizon simulationMs=30000 matchElapsedMs=30000 state=WAVE wave=1 commanderHp=160 allyAlive=2 allyHp=220,240 enemyAlive=2 enemyHp=80,100 issued=0 completed=0 failed=0 replaced=0 cancelled=0 active=0 director=none
[InputReplay] synthetic=true completed reason=quit
""", expected_focus_count=2)
        self.assertEqual("FAIL", result["status"])
        self.assertIn("focus-execution-count-mismatch", result["reasons"])

    def test_generic_event_contract_extracts_metrics_and_checks_case_fields(self):
        expected_events = {
            "required": ["ready", "visibility", "published", "consumed", "expired", "horizon"],
            "forbidden": [],
            "fields": {
                "ready": {"sharingEnabled": True},
                "visibility": {"aDirect": True, "bDirect": False, "bBlocked": True},
                "expired": {"factPresent": False, "supportActive": False, "bHasMove": False},
            },
        }
        metrics = [
            {"name": "consume_latency_ms", "label": "消费延迟 ms", "event": "consumed", "field": "responseLatencyMs"},
            {"name": "b_displacement_mm", "label": "B 位移 mm", "event": "expired", "field": "bDisplacementMm"},
        ]
        body = """
[ConfigManager] preset=team_sharing_experiment sample=Sandbox12 seed=20260720 agents=3 light=2 spawnMode=fixed aiScheduler=true tickMs=50 maxPerFrame=3 spawnPoints=3
[AIExperiment] schema=1 runId=m3-test event=ready simulationMs=0 matchElapsedMs=0 spawnMode=fixed aiSchedulerEnabled=true aiTickMs=50 aiMaxPerFrame=3 agentCount=3 lightTeamCount=2 sharingEnabled=true
[AIExperiment] schema=1 runId=m3-test event=visibility simulationMs=20 matchElapsedMs=20 aDirect=true bDirect=false bBlocked=true
[AIExperiment] schema=1 runId=m3-test event=published simulationMs=20 matchElapsedMs=20 sourceId=1 targetId=3
[AIExperiment] schema=1 runId=m3-test event=consumed simulationMs=40 matchElapsedMs=40 responseLatencyMs=20 sourceId=1 targetId=3 bDirect=false
[AIExperiment] schema=1 runId=m3-test event=expired simulationMs=3700 matchElapsedMs=3700 factPresent=false supportActive=false bHasMove=false bDisplacementMm=4210
[AIExperiment] schema=1 runId=m3-test event=horizon simulationMs=7000 matchElapsedMs=7000 postExpireDisplacementMm=12
[InputReplay] synthetic=true completed reason=quit
"""
        result = experiment.analyze_run_log(
            WINDOW + body,
            expected_run_id="m3-test",
            expected_seed=20260720,
            expected_sample="Sandbox12",
            expected_preset="team_sharing_experiment",
            horizon_ms=7000,
            exit_code=0,
            require_window_evidence=True,
            expected_conditions={
                "spawn_mode": "fixed", "ai_scheduler_enabled": True, "ai_tick_ms": 50,
                "ai_max_per_frame": 3, "agent_count": 3, "light_team_count": 2,
            },
            expected_events=expected_events,
            metric_definitions=metrics,
        )
        self.assertEqual("PASS", result["status"])
        self.assertEqual("TIME_LIMIT", result["outcome"])
        self.assertEqual(20, result["metrics"]["consume_latency_ms"])
        self.assertEqual(4210, result["metrics"]["b_displacement_mm"])

    def test_generic_event_contract_rejects_forbidden_response(self):
        body = """
[ConfigManager] preset=team_sharing_experiment sample=Sandbox12 seed=20260720 agents=3 light=2 spawnMode=fixed aiScheduler=true tickMs=50 maxPerFrame=3 spawnPoints=3
[AIExperiment] schema=1 runId=m3-test event=ready simulationMs=0 matchElapsedMs=0 sharingEnabled=false
[AIExperiment] schema=1 runId=m3-test event=consumed simulationMs=40 matchElapsedMs=40 responseLatencyMs=20
[AIExperiment] schema=1 runId=m3-test event=horizon simulationMs=7000 matchElapsedMs=7000
[InputReplay] synthetic=true completed reason=quit
"""
        result = experiment.analyze_run_log(
            WINDOW + body,
            expected_run_id="m3-test",
            expected_seed=20260720,
            expected_sample="Sandbox12",
            expected_preset="team_sharing_experiment",
            horizon_ms=7000,
            exit_code=0,
            require_window_evidence=True,
            expected_events={
                "required": ["ready", "horizon"],
                "forbidden": ["published", "consumed", "moved"],
                "fields": {"ready": {"sharingEnabled": False}},
            },
            metric_definitions=[
                {"name": "consume_ms", "label": "消费 ms", "event": "consumed", "field": "simulationMs"},
            ],
        )
        self.assertEqual("FAIL", result["status"])
        self.assertIn("forbidden-event", result["reasons"])

    def test_failed_run_is_excluded_from_metrics_and_pair_delta(self):
        manifest = {
            "experiment_id": "m2-test",
            "variable": {"name": "focus_order_enabled"},
            "cases": [{"id": "control", "value": False}, {"id": "variant", "value": True}],
            "seeds": [1],
            "repetitions": 1,
        }
        results = [
            {"case_id": "control", "seed": 1, "repetition": 1, "status": "PASS",
             "classification": "evidence-complete", "outcome": "TIME_LIMIT",
             "metrics": {name: (10 if name == "first_contact_ms" else None)
                         for name in experiment.NUMERIC_METRICS}},
            {"case_id": "variant", "seed": 1, "repetition": 1, "status": "FAIL",
             "classification": "wall-timeout", "outcome": "UNRESOLVED",
             "metrics": {name: (999 if name == "first_contact_ms" else None)
                         for name in experiment.NUMERIC_METRICS}},
        ]
        comparison = experiment.build_comparison(manifest, results, 2)
        self.assertIsNone(comparison["case_summaries"][1]["metrics"]["first_contact_ms"])
        self.assertEqual({"wall-timeout": 1}, comparison["case_summaries"][1]["failure_classifications"])
        self.assertEqual({}, comparison["paired_runs"][0]["variant_minus_control"])

    def test_markdown_summary_has_no_double_carriage_returns(self):
        manifest = {
            "experiment_id": "m2-test",
            "question": "test",
            "sample": "Sandbox19",
            "preset": "Sandbox19",
            "window": {"width": 1280, "height": 800},
            "horizon_ms": 30000,
            "variable": {"name": "focus_order_enabled"},
        }
        metrics = {name: None for name in experiment.NUMERIC_METRICS}
        comparison = {
            "status": "FAIL",
            "case_summaries": [{"case_id": "control", "value": False, "run_count": 0,
                                "infrastructure_pass": 0, "outcomes": {}, "metrics": metrics}],
        }
        metadata = {"workspace": {"head": "abc", "tracked_clean": False}}
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            experiment.write_markdown_summary(manifest, metadata, comparison, output)
            data = (output / "summary.md").read_bytes()
        self.assertNotIn(bytes([13, 13, 10]), data)
        self.assertIn(b"| `control` |", data)


if __name__ == "__main__":
    unittest.main()
