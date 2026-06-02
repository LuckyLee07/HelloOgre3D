# AI Perception Baseline - 2026-06-02

> 基于 `Sandbox16` / `ai_perf_100` / `ai_perf_500` / `ai_perf_1000` 的 Debug x64 smoke 实测。当前目标不是追求流畅帧率，而是在每帧全量更新下暴露感知、记忆和空间查询热点，为后续 C++ 热路径优化提供基线。

## 测试范围

- 构建：VS2017 Debug x64。
- Sample：`Sandbox16`。
- 调度：默认关闭 AI scheduler，每帧全量更新。
- 随机性：使用 preset 固定 seed。
- 对照模式：
  - `default`：启用 `AgentSpatialIndexSystem` 和 `AgentPerceptionSystem`。
  - `no_spatial`：关闭 spatial index，回退线性候选查询。
  - `no_perception_system`：关闭批量感知系统，回退 `AIController::TickAI` 内 per-agent 感知。
- 原始日志：`tmp/ai_perf_baseline_20260602_144229/`。

## 命令

```powershell
tools\run_sandbox_smoke.ps1 -Preset ai_perf_100 -StopExisting -NoTail
tools\run_sandbox_smoke.ps1 -Preset ai_perf_100 -DisableSpatialIndex -StopExisting -NoTail
tools\run_sandbox_smoke.ps1 -Preset ai_perf_100 -DisablePerceptionSystem -StopExisting -NoTail

tools\run_sandbox_smoke.ps1 -Preset ai_perf_500 -StopExisting -NoTail
tools\run_sandbox_smoke.ps1 -Preset ai_perf_500 -DisableSpatialIndex -StopExisting -NoTail
tools\run_sandbox_smoke.ps1 -Preset ai_perf_500 -DisablePerceptionSystem -StopExisting -NoTail

tools\run_sandbox_smoke.ps1 -Preset ai_perf_1000 -StopExisting -NoTail
tools\run_sandbox_smoke.ps1 -Preset ai_perf_1000 -DisableSpatialIndex -StopExisting -NoTail
tools\run_sandbox_smoke.ps1 -Preset ai_perf_1000 -DisablePerceptionSystem -StopExisting -NoTail
```

`run_sandbox_smoke.ps1` 会为 `ai_perf_500` 自动提升到 90 秒、为 `ai_perf_1000` 自动提升到 120 秒。35 秒对 500 / 1000 agent 只能覆盖启动和少量早期帧，不足以沉淀有效 `FramePerf` 样本。

## 摘要数据

单位为毫秒，取运行后段最多 5 条有效 `FramePerf` 的平均值。`samples` 越低，说明单帧已经足够慢，长时 smoke 中能采到的后段样本数量越少。

| 场景 | 模式 | samples | update | spatial | perceptionSystem | objectLoop | aiTotal | aiMemory | aiVision | aiLua |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 100 | default | 5 | 120.88 | 1.24 | 99.38 | 5.81 | 0.88 | 0.00 | 0.00 | 0.85 |
| 100 | no_spatial | 5 | 124.23 | 0.01 | 104.55 | 5.60 | 0.84 | 0.00 | 0.00 | 0.81 |
| 100 | no_perception_system | 5 | 128.23 | 1.23 | 0.05 | 112.12 | 106.41 | 49.28 | 55.93 | 1.10 |
| 500 | default | 3 | 750.68 | 5.44 | 650.66 | 32.13 | 4.71 | 0.00 | 0.00 | 4.55 |
| 500 | no_spatial | 4 | 958.52 | 0.01 | 880.68 | 27.63 | 4.13 | 0.00 | 0.00 | 4.00 |
| 500 | no_perception_system | 4 | 690.86 | 5.28 | 0.36 | 632.35 | 605.48 | 296.71 | 303.30 | 5.01 |
| 1000 | default | 2 | 1999.94 | 11.88 | 1581.48 | 67.59 | 10.02 | 0.00 | 0.00 | 9.70 |
| 1000 | no_spatial | 2 | 4274.97 | 0.01 | 3899.67 | 71.50 | 15.43 | 0.00 | 0.00 | 15.12 |
| 1000 | no_perception_system | 2 | 1659.14 | 11.39 | 0.74 | 1366.56 | 1311.06 | 641.13 | 658.62 | 10.38 |

候选查询摘要：

| 场景 | 模式 | scans | visible | queries | candidates | results | avgCandidates |
|---|---|---:|---:|---:|---:|---:|---:|
| 100 | default | 100 | 60 | 40 | 1,360 | 712 | 34.00 |
| 100 | no_spatial | 100 | 60 | 40 | 4,000 | 712 | 100.00 |
| 500 | default | 500 | 138 | 408 | 27,224 | 10,404 | 66.35 |
| 500 | no_spatial | 500 | 138 | 396.5 | 198,250 | 10,088.5 | 500.00 |
| 1000 | default | 1000 | 192 | 904 | 66,731 | 24,068 | 73.69 |
| 1000 | no_spatial | 1000 | 192 | 904 | 904,000 | 24,068 | 1000.00 |

## 结论

1. `AgentPerceptionSystem` 的职责切分有效。启用批量感知后，`aiMemory` / `aiVision` 从 `AIController::TickAI` 中归零，热点被集中到 `perceptionSystem`、`perMemory`、`perVision`，后续可以按系统维度做缓存、分桶和统计。
2. 当前卡顿主因仍是感知 / 记忆热路径本身，不是 Lua callback。1000 agent 默认模式下 `perceptionSystem` 约 1581 ms，其中 memory 约 769 ms、vision 约 810 ms，而 `aiLua` 约 10 ms。
3. Spatial index 在中大型规模下已经有收益。1000 agent 默认模式候选数约 66,731，关闭 spatial 后变为 904,000；`perceptionSystem` 从约 1581 ms 上升到约 3900 ms。
4. Spatial index 第一版还不够。默认模式仍然每帧扫描 1000 agent，并对候选执行完整 memory / vision / blackboard 写入；即使候选数被降到平均 73.69，单帧仍明显超预算。
5. `no_perception_system` 的 500 / 1000 agent `update` 数字略低于 default，并不代表 per-agent 模式更好。它只是把耗时分散回 `objectLoop` / `aiTotal`，缺少 system 级缓存与统一治理入口；后续继续优化应留在 `AgentPerceptionSystem` 路径上。

## 下一步

- `AgentSpatialIndexSystem` 二期：在 query 层提前支持 `teamId`、alive、includeSelf、type/tag 过滤，减少进入 vision / memory 的候选数；补 `queryCostMs` 和 grid vs linear 一致性对照。
- `PerceptionResultCache`：缓存 currentTarget、lastKnown、confidence、ageMs、source、候选数和扫描耗时；结果未变化时减少 metadata / blackboard 写入。
- Vision 查询预算：在每帧全量更新前提下，先加候选上限、已知目标优先、距离排序或 early-out，避免每个 agent 都完整跑一遍高成本视线逻辑。
- 后续再补 Release x64、scheduler on/off、Tracy capture 对照；当前 Debug x64 基线已足够证明热点边界。

## 后续实现记录

- 2026-06-02：`AgentSpatialIndexSystem` 二期第一段已实现 owner / includeSelf、teamId include/exclude、alive、objectType 过滤；grid 与 linear fallback 共用过滤语义。
- 2026-06-02：`FramePerf` / RuntimeDiag 新增 `filtered`、`avgFiltered`、`maxFiltered`、`rejectSelf`、`rejectTeam`、`rejectDead`、`rejectType`、`queryMs` 字段。后续需要用同一组 `ai_perf_500` / `ai_perf_1000` 命令重跑二期基线。
