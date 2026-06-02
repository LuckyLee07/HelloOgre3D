# AI 技术迭代规划

> 目标：把 `Sandbox9`-`Sandbox15` 已经跑通的 AI 学习 sample，从 Lua-first / sample-first 的实验形态，逐步收口成更高性能、可观测、可扩展的 C++ AI 底座。
>
> 方向依据：`docs/project-direction.md`、`docs/ai-roadmap.md`、`docs/minigame-ai-production-reference.md`。
>
> 一句话原则：**Lua 讲清楚行为，C++ 扛住规模，sample 验证概念，profiler 验证成本。**

## 1. 当前判断

当前项目已经具备第一轮 AI 学习闭环：

- 感知 / 记忆：`VisionSensor`、`MemoryStore`、lastKnown position、Blackboard metadata。
- 行为：FSM / DecisionTree / BehaviorTree 均可运行，BT 已有 `Parallel`、`Random`、reactive reevaluate、trace 和配置诊断。
- 团队：Lua `TeamBlackboard` 已可展示共享敌人、支援响应和队形协作。
- 战术：Lua `InfluenceMap` 已可展示 danger / support 双层评分与战术移动。
- 调度：`AIScheduler` 已支持 interval、bucket、per-frame budget 和统计摘要。
- Sample：`Sandbox9`-`Sandbox15` 已覆盖 Knowledge、Vision/Memory、TeamBlackboard、InfluenceMap、Hearing/Danger、Formation。

下一阶段不应继续优先扩充更多教学 sample，而应把已有 sample 背后的高频能力迁入 C++ system。

当前主要技术缺口：

- `AgentSpatialIndexSystem` 已接入 uniform grid 和查询 options；grid / linear fallback 均支持 owner/includeSelf、alive、team include/exclude、objectType 过滤，并输出 filtered / reject / queryMs 统计。后续仍需基于 500 / 1000 agent 复测 grid vs linear 成本对照。
- `TeamBlackboardService` 第一版已接入，可同步 `EnemySighted` 并写回最佳团队敌情；后续仍缺更完整 fact schema 和 Lua 全局状态迁移。
- InfluenceMap 仍是 Lua 网格，适合教学，不适合大规模战术评分或路径代价。
- `AgentPerceptionSystem` 第一阶段已接入，可每帧批量驱动 `AIController::TickPerception` 并输出 scans / visible / spatial query / memory / vision 统计；后续仍缺独立 `PerceptionResultCache`、hearing / danger C++ sense 和 Lua 扫描清理。
- BT runtime 已有教学级扩展，但还缺 instance pool、node result cache、blackboard dirty 依赖和距离 LOD。
- 已有 `ai_perf_100` / `ai_perf_500` / `ai_perf_1000` preset 入口；Debug x64 的 spatial on/off、perception system on/off 基线已沉淀到 `docs/perf/ai-perception-baseline-20260602.md`，后续仍需补 scheduler on/off、Release x64 和必要 Tracy capture。

## 2. 迭代主线

```text
AgentSpatialIndexSystem
  -> AgentPerceptionSystem
    -> TeamBlackboardService
      -> InfluenceMapSystem / TacticalQueryService
        -> BehaviorTree runtime budget / cache / LOD
          -> AI debug panel / perf benchmark / regression
```

关键顺序：

1. 先做空间索引，因为感知、队伍、战术、影响力图都依赖“附近有哪些 agent”。
2. 再做批量感知，让 Lua 行为树读取结果，而不是自己扫描世界。
3. 然后把团队和战术层从 Lua table 迁到 C++ service。
4. 最后优化 BT runtime 和 debug / benchmark，确保规模上来后可观测。

## 3. Phase 1：Agent Spatial / AOI 底座

### 目标

把当前线性 agent 查询替换为 C++ 空间索引，为中大型 AI 场景提供基础查询能力。

### 建议实现

- 新增 `AgentSpatialIndexSystem`。
- 第一版使用 uniform grid，不急于引入第三方 ECS 或复杂树结构。
- 新增 `AgentSpatialHandle`，由 agent 注册 / 注销 / 移动时维护。
- `IAgentSpatialQuery` 保持为外部接口，默认实现替换为 grid query。
- 查询支持：
  - center + radius。
  - teamId 过滤。
  - alive / dead 过滤。
  - includeSelf 开关。
  - maxResults 上限。
  - 可选 tag / type 过滤。
- 统计支持：
  - registeredAgents。
  - queryCount。
  - candidatesVisited。
  - resultCount。
  - maxCandidatesPerQuery。
  - queryCostMs。

### 验收

- `VisionSensor` 可以通过新 `IAgentSpatialQuery` 查询候选 agent。
- `Sandbox10` / `Sandbox11` 行为不退化。
- `Sandbox12` / `Sandbox13` 共享记忆和支援点查询能复用 spatial query。
- 新增或扩展 `ai_perf` preset，至少支持 100 / 500 / 1000 agent 查询压力。
- RuntimeDiag 或日志能输出 spatial query stats。

## 4. Phase 2：Agent Perception System

### 目标

将视觉、听觉、危险、伤害等感知统一为 C++ 批量系统，让 Lua 只消费结果。

### 建议实现

- 新增 `AgentPerceptionSystem`。
- `VisionSensor` 从单 agent 定时扫描，升级为 system 分片调度。
- 增加 `PerceptionResultCache`：
  - currentTargetId。
  - currentTargetPos。
  - distance。
  - lastKnownPos。
  - confidence。
  - ageMs。
  - source。
- 增加 `HearingSense` / `DangerSense` 的 C++ 结果入口，Lua sample 只负责触发声音或危险源。
- 可见性检测、距离过滤、遮挡预算集中到 system。
- 感知写 Blackboard 的路径保持稳定，避免破坏现有 BT action / condition。

### 验收

- `Sandbox10` 的 `VisionSensor -> MemoryStore -> BT` 仍可观察。
- `Sandbox14` 的 hearing / danger 行为由 C++ sense result 驱动，而不是 Lua 临时逻辑。
- 感知扫描支持 interval、bucket、budget。
- RuntimeDiag 能输出每帧 scanCount / skippedCount / candidates / visibleCount。
- Lua 行为节点不新增对象遍历。

## 5. Phase 3：TeamBlackboardService

### 目标

把团队共享信息从 Lua 全局表迁到 C++ service，提供生命周期、TTL、priority 和统计。

### 建议实现

- 新增 `TeamBlackboardService`。
- 数据模型：
  - `teamId`
  - `factType`
  - `sourceAgentId`
  - `targetAgentId`
  - `position`
  - `confidence`
  - `priority`
  - `timeMs`
  - `ttlMs`
- 支持 fact 类型：
  - `EnemySighted`
  - `SupportRequested`
  - `SupportResponded`
  - `FocusTarget`
  - `RetreatPoint`
  - `FormationSlot`
- Lua 保留薄 facade，兼容现有 `TeamBlackboard.lua` 的主要调用方式。
- 队伍状态跟随 sandbox/service 生命周期清理。

### 验收

- `Sandbox12` 继续展示“一个 agent 发现敌人，队友响应”。
- `Sandbox15` 继续展示 formation slot、等待队友、移动到队形点。
- Team fact 可 TTL 过期，不依赖 Lua 全局 reset。
- 日志或 RuntimeDiag 可输出 team count、fact count、expired count、active focus target。

## 6. Phase 4：InfluenceMapSystem / TacticalQueryService

### 目标

把 Lua 教学版 InfluenceMap 升级为 C++ 战术评分系统，支持多层、限频、统计和 debug draw。

### 建议实现

- 新增 `InfluenceMapSystem`。
- 新增 `InfluenceLayer`：
  - `EnemyThreat`
  - `TeamSupport`
  - `Objective`
  - `Cover`
  - `Crowd`
- 新增 `TacticalQueryService`：
  - `FindBestSupportPosition`
  - `FindLowThreatPosition`
  - `ScorePosition`
  - `SampleLayer`
- 支持固定 interval 更新和脏区更新。
- 支持组合评分：

```text
score = objective + support + cover - threat - crowd
```

- Lua 侧只选择 query 类型和权重，不直接遍历网格。

### 验收

- `Sandbox13` 行为不退化，并能显示 C++ InfluenceMap stats。
- 至少支持 3 个 layer 的单独 debug draw。
- 输出 updates / skips / cellWrites / queryCount / bestScore。
- 候选点数量有上限，战术查询不会随 agent 数量失控增长。

## 7. Phase 5：BehaviorTree Runtime 生产化补强

### 目标

让 BT 支撑更多 agent，而不是每棵树每帧完整递归执行。

### 建议实现

- `BehaviorTreeInstancePool`。
- `BehaviorNodeResultCache`。
- Blackboard dirty key 与节点依赖表。
- `tickStep` / `bucket` / `distanceLOD`。
- event-driven reevaluate。
- trace 采样开关。
- 每帧 BT budget。

### 验收

- 100+ agent 场景中，BT tick 可按 bucket / LOD 分散。
- RuntimeDiag 输出 BT ticked / skipped / cached / invalidated。
- Debug trace 关闭时不保留明显额外开销。
- `Sandbox8` / `Sandbox10` / `Sandbox15` 行为不退化。

## 8. Phase 6：AI Benchmark 与 Debug

### 目标

用数据验证架构选择，并降低 AI 异常行为排查成本。

### Benchmark

新增或扩展 `ai_perf` preset：

- agent 数：100 / 500 / 1000 / 5000。
- 场景：
  - idle only。
  - perception only。
  - perception + team。
  - perception + team + influence。
  - BT full。
- 指标：
  - AI total ms。
  - spatial query count / candidates / results。
  - perception scan / visible / skipped。
  - team facts active / expired。
  - influence updates / cellWrites / queryCount。
  - BT ticked / skipped / cached。
  - Lua callback count。

### Debug

逐步建设 AI debug panel 或 RuntimeDiag 输出：

- 选中 agent 的 perception snapshot。
- MemoryStore lastKnown。
- Team facts。
- Influence score。
- BT 当前节点和最近 trace。
- Scheduler / spatial / perception / influence 统计。

### 验收

- 每个阶段都有可重复 smoke 或 preset。
- 性能退化能从日志或 Tracy counter 定位到具体系统。
- 固定 seed 下，关键 sample 行为可复现。

## 9. 暂缓项

下一段时间不优先做：

- 完整 ECS 框架引入。
- UGC / Mod / Def 管线恢复。
- 完整触发器编辑器。
- 完整 AI 可视化编辑器。
- 大世界 streaming。
- 大量新增 Lua AI task。
- 巨型 `AIFunctionMgr` 式全能 facade。

这些能力不是永久否定，但必须等 AI 热点边界、性能基准和 C++ service 先稳定。

## 10. 近期建议拆解

### 近期一：Spatial Index

- [x] 设计 `AgentSpatialIndexSystem` 数据结构。
- [x] 接入 agent 注册、注销、位置更新。
- [x] 替换 `ObjectManagerAgentSpatialQuery` 线性查询。
- [x] 增加 maxResults 和基础 query stats。
- [x] 增加 spatial query smoke / RuntimeDiag 输出。
- [x] 补齐 owner / includeSelf、teamId include/exclude、alive、objectType 过滤。
- [x] 补齐 filtered、rejectSelf、rejectTeam、rejectDead、rejectType、queryMs 统计。
- [ ] 基于 `ai_perf_500` / `ai_perf_1000` 复测 grid vs linear 一致性 / 成本对照。

### 近期二：AI Perf Preset

- [x] 新增 100 / 500 / 1000 agent preset。
- [x] 固定 seed 和 grid 出生点分布。
- [x] 输出 spatial / perception / scheduler 统计入口，`run_sandbox_smoke.ps1` 会为 `ai_perf_*` 自动启用 `FramePerf`。
- [x] 提供线性查询与 grid 查询成本对照入口：`-DisableSpatialIndex`。
- [x] 实际记录 100 / 500 / 1000 在 spatial on/off、perception system on/off 下的 Debug x64 基线结果：见 `docs/perf/ai-perception-baseline-20260602.md`。
- [ ] 补 scheduler on/off、Release x64 和必要 Tracy capture 对照。

### 近期三：Perception System

- [x] 将 `VisionSensor` 接入 spatial query。
- [x] 建立 `AgentPerceptionSystem` 第一阶段：保持每帧全量更新，把 per-agent perception tick 集中到 system 级执行和统计。
- [ ] 建立独立 `PerceptionResultCache`。
- [x] 在感知 query 层提前过滤 teamId / alive / includeSelf，避免无效候选进入 memory / vision 热路径。
- [ ] 将 hearing / danger sample 的结果入口收口到 C++ sense。
- [ ] Lua BT 只读结果，不做对象扫描。

### 近期四：Team / Tactics 迁移准备

- [ ] 梳理 `TeamBlackboard.lua` 的现有 API。
- [ ] 设计 C++ `TeamBlackboardService` fact schema。
- [x] 接入第一版 Lua facade：`TeamBlackboard.lua` 保持旧 API，同时把 `EnemySighted` 同步到 C++ service，并提供 C++ fact count / report count / best fact 写回 blackboard 的兼容入口。
- [ ] 梳理 `InfluenceMap.lua` 的 sample 依赖和 debug 输出。
- [ ] 设计 C++ `InfluenceMapSystem` layer schema。

## 11. 成功标准

下一阶段成功的标志不是新增更多章节 sample，而是：

- 1000 agent 级别的感知查询有清晰性能数据。
- Lua AI 不再承担世界扫描和战术批量评分。
- TeamBlackboard 和 InfluenceMap 有明确 C++ 迁移路径。
- BT runtime 有预算、LOD、cache 的设计和验收面。
- 每个 AI 概念 sample 都能继续运行，并能输出更清楚的 debug / perf 摘要。
