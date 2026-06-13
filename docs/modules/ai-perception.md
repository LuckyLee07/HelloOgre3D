# AI 感知系统（alias: ai-perception）

> 模块参考文档。`/hello-develop-design` 阶段 0 按 `docs/registry.json` 匹配注入；改源码后经 doc-sync 更新。

## 1. 职责

把视觉扫描、空间查询、敌人记忆、感知结果缓存收口为 C++ 批量系统，让 Lua/BT 只读结果、不自己遍历世界。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/perception/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `AgentPerceptionSystem.{h,cpp}` | 系统 | 每帧批量驱动各 agent `AIController::TickPerception`，输出 scans/visible/spatial 统计 |
| `AgentSpatialIndexSystem.{h,cpp}` | 系统 | uniform grid 邻域查询，team/alive/includeSelf/maxResults 过滤 + 命中/拒绝统计 |
| `AgentSpatialQuery.h` | 接口 | `IAgentSpatialQuery::QueryAgentsInRange`；默认实现仍 `getAllAgents()` 线性（下沉一层，未做分区淘汰） |
| `VisionSensor.h` | 传感器 | 单 agent 定时扫描，`scanIntervalMs`/`initialDelayMs`/lastResult |
| `MemoryStore.h` | 记忆 | 敌人 TTL 衰减 + `kMemorySnapshot*` blackboard key 兼容 |
| `AgentPerceptionQuery.h` | 接口 | `AgentPerceptionResult`(target/pos/distance/confidence) + FindNearestEnemy/TryGetEnemy |
| `PerceptionResultCache.h` | 缓存 | 感知结果显式快照：currentTarget/lastKnown/confidence/age/candidate/scanCost |

## 4. 公开能力要点

- 批量感知调度（每帧全量，scheduler 可降频）；spatial grid 范围查询带过滤与统计。
- VisionSensor 已支持 `scanIntervalMs` 门控（ai_perf pressure 故意设 1 压测）。
- `PerceptionResultCache`（4a 落地）：感知结果单一事实来源；`AIController::HasEnemy` 已改读它（4b，等价）。

## 5. 约束与红线

- 空间查询线性扫描只是下沉到 `ObjectManagerAgentSpatialQuery` 一层，**尚未空间分区淘汰**。
- `PerceptionResultCache` 须在 `TickPerception` 内填充（driver 之前），勿被 Lua 旁路重扫。
- MemoryStore 必须保持 `kMemorySnapshot*` key 兼容（BT/Lua 在读）。
- hearing / danger 的 C++ sense 入口**未做**（仍 Lua）。

## 6. 数据流 / 与其他模块关系

`AgentPerceptionSystem.Update → AIController::TickPerception → VisionSensor.Tick(走 AgentSpatialQuery) → WritePerceptionResult(写 Blackboard) + MemoryStore + UpdatePerceptionCache`。结果被 [[ai-controller]]、[[ai-behavior]]、[[ai-team]] 消费。

## 7. 验证策略

- 回归 sample：`Sandbox10`（感知/lastKnown）、`Sandbox16`（压力）。开 `-RuntimeDiag` 看 per-agent。
- 压力/性能：`ai_perf_1000`（看 `[FramePerf] perceptionSystem`，**非 cpuFrame**）。
- 行为对齐：`run_chapter9_parity_gate.ps1`。

## 8. 已知 gap / 相关文档

- 待：PerceptionResultCache 全量集成（HasEnemy 已读，余路径待迁）、hearing/danger C++ sense、空间分区。
- `docs/planning/ai-technical-iteration-plan.md` §3/§4、`docs/perf/ai-perf-release-baseline-20260612.md`、`docs/design/architecture-improvement-plan.md` P7（已解决-感知抽象）。
