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

- FGUI stencil backend observability:
  - C++ frame render stats now expose CPU clip source/output triangles, output fragments, stencil clip scope/polygon counts, hardware stencil support, and the active stencil backend.
  - Lua `GetRenderStats`, `DumpRenderStats`, Pressure logs, and DebugPanel lines show `shapeCpu` backend details.
  - Current Ogre `Pass` has no per-material stencil state; true GPU stencil parity should be implemented through a custom `Renderable` path or a ManualObject section replacement that can set/restore `RenderSystem` stencil state per renderable.

- FGUI P0 finishing pass:
  - Resource fallback diagnostics now record package/component/child/event-policy misses through `FairyGuiManager:RecordResourceFallback()`, `DumpResourceFallbacks()`, and DebugPanel fallback summaries.
  - DebugPanel now exposes target-oriented refresh/dump/close actions and `DumpDebugTarget()` / `CloseDebugTarget()` helpers for UI or package inspection.
  - TextInput policy is available through Manager/View/Ctrl APIs, with Lua-side `inputType`, `restrict`, and `maxLength` normalization while native GTextInput binding remains minimal.
  - `BusinessBenchmark`, `ResourceFallback`, and `TextInputPolicy` selftest modes were added to `tools/run_fgui_selftest.ps1` and the all-in-one suite.
  - `tools/run_fgui_tracy_sample.ps1` now invokes the selftest script in-process instead of nesting another PowerShell process.

## 2026-05-23 P0 Verification

- Passed: `tools/run_fgui_selftest.ps1 -Mode TextInputPolicy -Seconds 32 -Tail 140 -StopExisting`.
- Passed: `tools/run_fgui_selftest.ps1 -Mode ResourceFallback -Seconds 36 -Tail 100 -StopExisting`.
- Passed: `tools/run_fgui_selftest.ps1 -Mode DebugPanelSelfTest -Seconds 36 -Tail 140 -StopExisting`.
- Passed: `tools/run_fgui_selftest.ps1 -Mode BusinessBenchmark -Count 2 -Seconds 38 -Tail 160 -StopExisting`.
- Passed once in visible mode: `tools/run_fgui_selftest.ps1 -Mode Pressure -Count 2 -PressurePopupCount 2 -PressureListCount 20 -Seconds 45 -Tail 80 -StopExisting -Visible`.
- Recorded visible pressure extract: `docs/perf/fgui-tracy-sample-20260523-172234-visible.md`.
- Blocked in this desktop session: hidden or nested PowerShell launches later hit Ogre D3D9 `Cannot create device`, so the Tracy wrapper could not complete a fresh hidden-mode report after the visible pass.
- Static script note: `tools/check_fgui_static.ps1` could not run here because `luac` is not available on PATH.

## Remaining High Priority

- True GPU stencil pixel-level parity remains; shape-aware CPU stencil clipping now covers GraphMaskProbe and NestedGraphMaskProbe, with backend and CPU load visible in Dump/DebugPanel logs.
- AI event payload normalization.
- AI update scheduling, refresh budget, and Tracy-visible AI tick counters.
