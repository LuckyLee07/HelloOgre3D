# AI Spatial Filter Retest - 2026-06-02

> 基于 `AgentSpatialIndexSystem` 二期第一段过滤实现后的 Debug x64 复测。目标是验证 owner / includeSelf、team、alive、objectType 过滤是否减少进入感知热路径的候选，并确认 grid / linear fallback 的对照数据仍可用。

## 测试范围

- 构建：VS2017 Debug x64。
- Sample：`Sandbox16`。
- Preset：`ai_perf_500`、`ai_perf_1000`。
- 调度：关闭 AI scheduler，每帧全量更新。
- 对照模式：
  - `default`：启用 `AgentSpatialIndexSystem` 和 `AgentPerceptionSystem`。
  - `no_spatial`：关闭 spatial index，走线性 fallback。
  - `no_perception_system`：关闭批量感知系统，回退 per-agent `AIController::TickAI` 感知。
- 原始日志：`tmp/ai_perf_spatial_filter_retest_20260602_161544/`。
- 汇总 CSV：`tmp/ai_perf_spatial_filter_retest_20260602_161544/summary_spatial_filter_retest.csv`。

## 复测结果

单位为毫秒，取运行后段最多 5 条有效 `FramePerf` 的平均值。

| 场景 | 模式 | samples | update | perceptionSystem | memory | vision | queries | rawCandidates | filtered | rejectTeam | queryMs |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 500 | default | 3 | 930.78 | 805.02 | 407.45 | 396.49 | 408 | 27,224 | 268 | 9,728 | 96.24 |
| 500 | no_spatial | 3 | 1060.35 | 957.82 | 319.66 | 632.74 | 408 | 204,000 | 268 | 9,728 | 397.68 |
| 500 | no_perception_system | 4 | 692.14 | 0.30 | 0.00 | 0.00 | 0 | 0 | 0 | 0 | 0.00 |
| 1000 | default | 2 | 1596.57 | 1247.02 | 631.46 | 614.00 | 904 | 66,731 | 564 | 22,600 | 168.12 |
| 1000 | no_spatial | 2 | 3155.93 | 2816.56 | 623.64 | 2191.34 | 904 | 904,000 | 564 | 22,600 | 1748.00 |
| 1000 | no_perception_system | 3 | 1467.83 | 0.63 | 0.00 | 0.00 | 0 | 0 | 0 | 0 | 0.00 |

`no_perception_system` 的感知成本会回到 `aiTotal / aiMemory / aiVision`，不是 `perceptionSystem`：

| 场景 | 模式 | aiTotal | aiMemory | aiVision | aiLua | spatialQueries | rawCandidates | filtered | queryMs |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 500 | no_perception_system | 604.74 | 301.74 | 297.69 | 4.86 | 396.5 | 26,273.5 | 201 | 70.76 |
| 1000 | no_perception_system | 1216.64 | 615.91 | 590.44 | 9.44 | 872 | 64,002 | 376 | 161.09 |

## 与旧基线对照

旧基线见 `docs/perf/ai-perception-baseline-20260602.md`。旧版 spatial `results` 是距离筛选后的候选数；新版 `filtered` 是再经过 self / team / alive / type 过滤后真正进入后续感知校验的候选数。

| 场景 | 模式 | 旧 rawCandidates | 新 rawCandidates | 旧 results | 新 filtered | 旧 perceptionSystem | 新 perceptionSystem |
|---|---|---:|---:|---:|---:|---:|---:|
| 500 | default | 27,224 | 27,224 | 10,404 | 268 | 650.66 | 805.02 |
| 500 | no_spatial | 198,250 | 204,000 | 10,088.5 | 268 | 880.68 | 957.82 |
| 1000 | default | 66,731 | 66,731 | 24,068 | 564 | 1581.48 | 1247.02 |
| 1000 | no_spatial | 904,000 | 904,000 | 24,068 | 564 | 3899.67 | 2816.56 |

## 结论

1. 过滤语义有效。1000 agent default 下，进入后续感知校验的候选从旧版 24,068 降到 564；同队过滤贡献最大，`rejectTeam=22,600`。
2. 1000 agent 下收益明确。default 的 `perceptionSystem` 从约 1581 ms 降到约 1247 ms；linear fallback 从约 3900 ms 降到约 2817 ms。
3. 500 agent 下总耗时没有改善，主要受 Debug 抖动、query 过滤开销和 memory / blackboard 写入成本影响；但候选输入已经从旧版 10,404 降到 268，说明下一步应优化结果写入和缓存，而不是回退过滤方向。
4. `rawCandidates` 没有下降是符合预期的：grid 仍先按 cell / distance 找原始候选，再做 self / team / alive / type 过滤。若要继续减少 raw candidates，需要调整 cell size、空间分布或引入更细的 team grid。
5. `no_perception_system` 路径仍可工作，且 spatial 过滤统计能覆盖 per-agent 感知路径；但该路径缺少 system 级治理入口，不建议作为长期优化方向。

## 下一步

- 优先做 `PerceptionResultCache`：结果未变化时减少 `sense.*` / `memory.*` metadata 和 snapshot 写入。
- 在 cache 前后复测 `memory`、`vision`、`aiMemory`、`aiVision`，确认 blackboard / memory 写入是否是当前 500 / 1000 agent 的主要残余热点。
- 如果 cache 后 `queryMs` 仍高，再考虑 team-aware grid、cell size 参数化基线、候选上限或 preferred target early-out。
