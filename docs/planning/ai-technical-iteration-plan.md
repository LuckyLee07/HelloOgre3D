# AI 技术迭代规划

> 目标：把 `Sandbox9`-`Sandbox15`、`Sandbox17` 与 `Sandbox18` 已经跑通的 AI 学习 sample，从 Lua-first / sample-first 的实验形态，逐步收口成更高性能、可观测、可扩展的 C++ AI 底座；`Sandbox16` 作为 AI perception pressure / ai_perf 的性能压力入口保留，`Sandbox18` 承接 Chapter 9 C++ 第二版第一切片。
>
> 方向依据：`docs/project-direction.md`、`docs/planning/long-term-iteration-plan.md`、`docs/planning/ai-roadmap.md`、`docs/reference/minigame-ai-production-reference.md`。
>
> 一句话原则：**Lua 讲清楚行为，C++ 扛住规模，sample 验证概念，profiler 验证成本。**

## 1. 当前判断

当前项目已经具备第一轮 AI 学习闭环：

- 感知 / 记忆：`VisionSensor`、`MemoryStore`、lastKnown position、Blackboard metadata。
- 行为：FSM / DecisionTree / BehaviorTree 均可运行，BT 已有 `Parallel`、`Random`、reactive reevaluate、trace 和配置诊断。
- 团队：Lua `TeamBlackboard` 已可展示共享敌人、支援响应和队形协作。
- 战术：Lua `InfluenceMap` 已可展示 danger / support 双层评分与战术移动；C++ `InfluenceMapSystem` 已在 `Sandbox18` 接入 danger / team / objective 三层评分和 best position 查询。
- 调度：`AIScheduler` 已支持 interval、bucket、per-frame budget 和统计摘要。
- Sample：`Sandbox9`-`Sandbox15` 已覆盖 Knowledge、Vision/Memory、TeamBlackboard、InfluenceMap、Hearing/Danger、Formation；`Sandbox16` 承接 AI perception pressure / ai_perf 压力验证；`Sandbox17` 覆盖 Chapter 9 Tactics Lua-first 第一版；`Sandbox18` 覆盖 Chapter 9 Tactics C++ 第二版第一切片。

下一阶段不应继续优先扩充更多教学 sample，而应把已有 sample 背后的高频能力迁入 C++ system。

当前主要技术缺口：

- `AgentSpatialIndexSystem` 已接入 uniform grid 和查询 options；grid / linear fallback 均支持 owner/includeSelf、alive、team include/exclude、objectType 过滤，并输出 filtered / reject / queryMs 统计。`maxResults` 现在保留近邻候选，`perception.maxSpatialResults` 可从 blackboard 控制，pressure preset 默认 16；仍需继续做 Release / scheduler 调参与更完整 AOI 淘汰。
- `TeamBlackboardService` 第一版已接入，可同步 `EnemySighted` 并写回最佳团队敌情；后续仍缺更完整 fact schema 和 Lua 全局状态迁移。
- Lua `InfluenceMap` 仍适合作为教学对照；`Sandbox18` 已把 Chapter 9 DangerousAreas / TeamAreas 的事件输入迁到 C++ `InfluenceMapSystem` + `TacticalQueryService`，debug draw 生命周期也已下沉到 `TacticalDebugDrawService`；仍缺 dirty region / interval 更新、cover / crowd schema、显式 layer debug 配置和 pressure preset。
- `AgentPerceptionSystem` 第一阶段已接入，可每帧批量驱动 `AIController::TickPerception` 并输出 scans / visible / spatial query / memory / vision 统计；`PerceptionResultCache` 已作为显式快照接入 HasEnemy / TickPerception，CanShootEnemy 成功路径同步 cache；后续仍缺 hearing / danger C++ sense 和 Lua 扫描清理。
- BT runtime 已有教学级扩展，但还缺 instance pool、node result cache、blackboard dirty 依赖和距离 LOD。
- 已有 `ai_perf_100` / `ai_perf_500` / `ai_perf_1000` preset 入口；Debug x64 的 spatial on/off、perception system on/off 基线已沉淀到 `docs/perf/ai-perception-baseline-20260602.md`，当前压力入口仍是 `Sandbox16`；后续仍需补 scheduler on/off、Release x64 和必要 Tracy capture。

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

### 2.1 规模化性能原则

参考 `docs/reference/minigame-ai-production-reference.md` 对 MiniGame 的代码分析，后续 AI 性能目标不应理解成“1000 个 agent 每帧完整跑同一套逻辑也不卡”，而应拆成以下设计约束：

- 每帧先确定 active set；不在验证范围、AOI 或重要性阈值内的 agent 只做低频或跳过更新。
- 每个批量系统必须支持 interval、bucket、budget 或 distance LOD，避免感知、BT、TeamBlackboard、InfluenceMap 同帧集中重算。
- 所有邻域查询必须走 C++ spatial / AOI facade，并且有 `maxResults`、近邻保留 / 候选访问上限和统计输出。
- Lua 行为节点只读 C++ 产出的 perception / team / tactical result，不新增全局对象遍历。
- 热路径优先优化数据布局和缓存，而不是先引入完整第三方 ECS；必要时再把可批处理系统 job 化。
- `ai_perf_1000` 的验收要同时看 Debug / Release、spatial on/off、scheduler on/off、perception cache on/off 和 Lua callback 数量。

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
  - maxResults 上限，当前语义为保留最近候选而不是首批命中。
  - 候选访问上限与远处采样策略，避免密集场景结果爆炸。
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
- `PerceptionResultCache` 已建立，后续扩展字段时保持：
  - currentTargetId。
  - currentTargetPos。
  - distance。
  - lastKnownPos。
  - confidence。
  - ageMs。
  - source。
- 增加 `HearingSense` / `DangerSense` 的 C++ 结果入口，Lua sample 只负责触发声音或危险源。
- 可见性检测、距离过滤、遮挡预算集中到 system。
- 扫描结果写入 cache 后由 Blackboard / BT 消费，避免每个 Lua action 重复执行感知查询。
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

把 Lua 教学版 InfluenceMap 升级为 C++ 战术评分系统，支持多层、限频、统计和 debug draw。`Sandbox17` 已作为 Lua-first 版本对齐 HelloOgre3DX Chapter 9 的 DangerousAreas / TeamAreas 语义；`Sandbox18` 已承接 C++ 版 `InfluenceMapSystem` 第一切片，`TacticalQueryService` 和 `TacticalDebugDrawService` 已拆出，后续继续补 dirty region / interval 更新、cover/crowd 和显式 layer debug 配置。

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

当前已落地的第一切片：

- 新增 C++ `InfluenceMapSystem`，支持 configurable grid、string layer、radial source、nearest-cell sample、组合评分、best position 查询和 debug summary。
- 新增 C++ `TacticalQueryService`（`sandbox/ai/tactics/TacticalQueryService.h`）：持有 `InfluenceMapSystem`，封装事件订阅/TTL/发布、influence config/layer/source/sample/score 包装、`Rebuild{Danger,Team,Objective}Layer` 与查询 API（`FindBestSupportPosition`/`FindLowThreatPosition`/`ScoreQueryPosition`/`FindBestQueryPosition`）。
- 新增 C++ `TacticalDebugDrawService`（`sandbox/ai/tactics/TacticalDebugDrawService.h`）：承接 `DebugDrawer` 临时绘制与 Ogre `ManualObject` 持久 debug visual 生命周期。
- `ObjectManager` 暴露 Lua facade：clear / configure / clearLayer / addSource / sample / score / findBest / stats / debug draw，当前已薄转发到 `TacticalQueryService` / `TacticalDebugDrawService`。
- `Sandbox18` 复用 Chapter 9 的 BulletShot / BulletImpact / EnemySighted / DeadFriendlySighted 事件语义，Lua 只负责喂事件和读取 C++ 查询结果。

### 验收

- `Sandbox13` 行为不退化，并能显示 C++ InfluenceMap stats。
- `Sandbox17` 的 BulletShot / BulletImpact / EnemySighted / DeadFriendlySighted 事件驱动战术层不退化。
- `Sandbox18` 的 C++ influence map smoke 输出 `[Chapter9TacticsCppSmoke] PASS`。
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
- [x] 基于 `ai_perf_500` / `ai_perf_1000` 复测 grid vs linear 一致性 / 成本对照。

### 近期二：AI Perf Preset

- [x] 新增 100 / 500 / 1000 agent preset。
- [x] 固定 seed 和 grid 出生点分布。
- [x] 输出 spatial / perception / scheduler 统计入口，`run_sandbox_smoke.ps1` 会为 `ai_perf_*` 自动启用 `FramePerf`。
- [x] 提供线性查询与 grid 查询成本对照入口：`-DisableSpatialIndex`。
- [x] 实际记录 100 / 500 / 1000 在 spatial on/off、perception system on/off 下的 Debug x64 基线结果：见 `docs/perf/ai-perception-baseline-20260602.md`。
- [x] pressure preset 接入 `perception.maxSpatialResults=16`，spatial maxResults 保留近邻候选而不是首批命中。
- [ ] 补 scheduler on/off、Release x64、perception cache on/off、Lua callback 数量和必要 Tracy capture 对照。

### 近期三：Perception System

- [x] 将 `VisionSensor` 接入 spatial query。
- [x] 建立 `AgentPerceptionSystem` 第一阶段：保持每帧全量更新，把 per-agent perception tick 集中到 system 级执行和统计。
- [x] 建立独立 `PerceptionResultCache`，`HasEnemy` 读 cache，`CanShootEnemy` 成功路径同步 cache。
- [x] 在感知 query 层提前过滤 teamId / alive / includeSelf，避免无效候选进入 memory / vision 热路径。
- [ ] 将 hearing / danger sample 的结果入口收口到 C++ sense。
- [ ] Lua BT 只读结果，不做对象扫描。

### 近期四：Team / Tactics 迁移准备

- [ ] 梳理 `TeamBlackboard.lua` 的现有 API。
- [ ] 设计 C++ `TeamBlackboardService` fact schema。
- [x] 接入第一版 Lua facade：`TeamBlackboard.lua` 保持旧 API，同时把 `EnemySighted` 同步到 C++ service，并提供 C++ fact count / report count / best fact 写回 blackboard 的兼容入口。
- [ ] 梳理 `InfluenceMap.lua` 的 sample 依赖和 debug 输出。
- [x] 用 `Sandbox17` 固化 Chapter 9 Tactics Lua-first 对照面：DangerousAreas / TeamAreas、事件 TTL、layer 更新统计和 `[Chapter9TacticsSmoke] PASS`。
- [x] 设计 C++ `InfluenceMapSystem` layer schema，并在第一切片中落地 danger / team / objective 三层。
- [x] 用 `Sandbox18` 固化 Chapter 9 Tactics C++ 第二版第一切片：Lua 喂事件，C++ 写入 influence layer、执行 tactical score / best position query，并输出 `[Chapter9TacticsCppSmoke] PASS`。
- [x] 抽出独立 `TacticalQueryService`：`sandbox/ai/tactics/TacticalQueryService.h` 已落地，持有 `InfluenceMapSystem`、跑事件订阅/TTL、`Rebuild{Danger,Team,Objective}Layer` 和查询 API（`FindBestSupportPosition`/`FindLowThreatPosition`/`ScoreQueryPosition`/`FindBestQueryPosition`）。
- [x] `TacticalQueryService` / `TacticalDebugDrawService` 收口：service 已是查询/层实现事实来源，`ObjectManager` 的 clear/configure/layer/source/sample/score/findBest/stats 入口已薄转发到 service，`configureTacticalInfluenceFromNavMesh` 的三角提取与建图编排已下沉；`rebuildTacticalInfluenceLayerDebugVisual` / Ogre ManualObject 生命周期已下沉到 `TacticalDebugDrawService`。
- [ ] 补 `InfluenceMapSystem` dirty region / interval 更新、cover / crowd / support schema、layer debug 显式配置和 pressure preset。

## 11. 成功标准

下一阶段成功的标志不是新增更多章节 sample，而是：

- 1000 agent 级别的感知查询有清晰性能数据。
- Lua AI 不再承担世界扫描和战术批量评分。
- TeamBlackboard 和 InfluenceMap 有明确 C++ 迁移路径。
- BT runtime 有预算、LOD、cache 的设计和验收面。
- 每个 AI 概念 sample 都能继续运行，并能输出更清楚的 debug / perf 摘要。
