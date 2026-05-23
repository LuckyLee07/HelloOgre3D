# FairyGUI and AI Debug Progress 2026-05-23

## Completed

- Windows IME observability first pass:
  - C++ records composition active state, candidate open state, candidate count/selection, commit/end counters, and candidate window position.
  - Lua can inspect the state through `FairyGuiManager:GetImeDebugString()`.
  - `IME` self test now covers composition text, commit text, and clear/end state.

- AI DebugPanel first pass:
  - `ObjectManager:buildAiDebugSummary()` summarizes agent/soldier count, state, driver type, Blackboard highlights, and BehaviorTree trace text.
  - `FairyGuiManager:ShowAiDebugPanel()` displays the summary using the existing service-container panel foundation.
  - `HELLO_FGUI_AI_DEBUG_PANEL_SELF_TEST=1` and `HELLO_FGUI_SELF_TEST_ALL=1` cover the panel entry.

- Tracy pressure sampling entry:
  - `tools/run_fgui_tracy_sample.ps1` wraps the existing Pressure self test and writes a compact report under `docs/perf/`.
  - The report extracts pressure stats, render detail, health, and perf lines for later Tracy viewer comparison.

## Remaining High Priority

- True GPU stencil pixel-level parity remains; shape-aware CPU stencil clipping now covers GraphMaskProbe and NestedGraphMaskProbe.
- AI event payload normalization.
- AI update scheduling, refresh budget, and Tracy-visible AI tick counters.
