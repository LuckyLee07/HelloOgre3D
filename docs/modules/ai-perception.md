# AI 感知系统（alias: ai-perception）

> 模块参考文档。`/hello-develop-design` 阶段 0 按 `docs/registry.json` 匹配注入；改源码后经 doc-sync 更新。

## 1. 职责

把视觉扫描、空间查询、敌人记忆、感知结果缓存收口为 C++ 批量系统，让 Lua/BT 只读结果、不自己遍历世界。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/perception/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `AgentPerceptionSystem.{h,cpp}` | 系统 | 每帧批量驱动各 agent `AIController::TickPerception`，输出 scans/visible/spatial 统计；作为 `SandboxPerception` facade 汇总 Hearing/Danger 统计 |
| `HearingDangerSense.{h,cpp}` | 传感器 | 枪声/危险事件队列、interval + agentsPerTick 预算扫描、cooldown、blackboard 写回和 `RetreatPoint` typed fact 写回 |
| `AgentSpatialIndexSystem.{h,cpp}` | 系统 | uniform grid 邻域查询，team/alive/includeSelf/maxResults 过滤 + 命中/拒绝统计；maxResults 保留近邻候选而非首批命中 |
| `AgentSpatialQuery.h` | 接口 | `IAgentSpatialQuery::QueryAgentsInRange`；默认实现仍 `getAllAgents()` 线性（下沉一层，未做分区淘汰） |
| `VisionSensor.h` | 传感器 | 单 agent 定时扫描，`scanIntervalMs`/`initialDelayMs`/lastResult |
| `MemoryStore.h` | 记忆 | 敌人 TTL 衰减 + `kMemorySnapshot*` blackboard key 兼容 |
| `AgentPerceptionQuery.h` | 接口 | `AgentPerceptionResult`(target/pos/distance/confidence) + FindNearestEnemy/TryGetEnemy |
| `PerceptionResultCache.h` | 缓存 | 感知结果显式快照：currentTarget/lastKnown/confidence/age/candidate/scanCost |

## 4. 公开能力要点

- 批量感知调度（每帧全量，scheduler 可降频）；spatial grid 范围查询带过滤与统计。
- VisionSensor 已支持 `scanIntervalMs` 门控（ai_perf pressure 故意设 1 压测）。
- AIController 会从 blackboard 读取 `perception.maxSpatialResults`，并透传到 spatial query；`ai_perception_pressure` / `ai_perf_*` 默认限制为 16 个近邻候选，用于避免 500/1000 agent pressure 下继续把所有过滤后候选交给每个 agent 二次筛选。
- `PerceptionResultCache`（4a 落地）：感知结果单一事实来源；`AIController::HasEnemy` 已改读它（4b，等价），`CanShootEnemy` 成功路径会同步 cache 但保留短距离/无寻路重查语义；`HELLO_AI_PERCEPTION_CACHE_ENABLE=0` / smoke `-DisablePerceptionCache` 可禁用 `HasEnemy` cache 读路径做 A/B。
- Hearing/Danger 第一段：Lua sample 只发布枪声/危险源并负责 debug 绘制；`HearingDangerSense` 负责 interval + agentsPerTick 预算扫描、cooldown、`chapter8.sensoryIntent` / `sense.heard*` / `sense.danger*` 写回，并把危险响应写为 `RetreatPoint` typed team fact；`RuntimeProfileCounters` 已提供 `HearingDangerSense*` 专用图表。

## 5. 约束与红线

- 空间查询线性扫描只是下沉到 `ObjectManagerAgentSpatialQuery` 一层；maxResults 现在会保留近邻候选，但仍不是完整 AOI / visibility set 淘汰。
- `PerceptionResultCache` 须在 `TickPerception` 内填充（driver 之前），勿被 Lua 旁路重扫；需要回测时只通过 `HELLO_AI_PERCEPTION_CACHE_ENABLE=0` 切到直接 vision query 路径。
- MemoryStore 必须保持 `kMemorySnapshot*` key 兼容（BT/Lua 在读）。
- Hearing/Danger C++ sense 已从 `AgentPerceptionSystem` 拆出为独立 `HearingDangerSense`；后续新增非视觉输入时继续按 sense 类隔离，不回填到 system facade。

## 6. 数据流 / 与其他模块关系

`AgentPerceptionSystem.Update → AIController::TickPerception → VisionSensor.Tick(走 AgentSpatialQuery) → WritePerceptionResult(写 Blackboard) + MemoryStore + UpdatePerceptionCache`；`Sandbox14/Sandbox13 Lua → SandboxPerception.publishHearingDangerEvent → HearingDangerSense.Update → Blackboard + TeamBlackboardService.RetreatPoint`。结果被 [[ai-controller]]、[[ai-behavior]]、[[ai-team]] 消费。

## 7. 验证策略

- 回归 sample：`Sandbox10`（感知/lastKnown）、`Sandbox16`（压力）。开 `-RuntimeDiag` 看 per-agent。
- Hearing/Danger：`run_sandbox_smoke.ps1 -Preset hearing_danger` 必须看到 `[HearingDangerSmoke] PASS cppSense=true ... cppRetreatApplies>0`。
- 压力/性能：`ai_perf_1000`（看 `[FramePerf] perceptionSystem`，**非 cpuFrame**）。
- 行为对齐：`run_chapter9_parity_gate.ps1`。

## 8. 已知 gap / 相关文档

- 待：更多非视觉 sense 输入、空间查询上限调参 / Release + scheduler 对照，以及更完整的 AOI / visibility set 淘汰。
- `docs/planning/ai-technical-iteration-plan.md` §3/§4、`docs/perf/ai-perf-release-baseline-20260612.md`、`docs/design/architecture-improvement-plan.md` P7（已解决-感知抽象）。
