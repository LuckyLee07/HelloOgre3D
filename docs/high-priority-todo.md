# 高优先级 TODO

> 文档状态：**执行记录 + 细粒度 backlog**。当前方向与阶段排期先看 `docs/project-direction.md` 和 `docs/long-term-iteration-plan.md`；AI 技术细节看 `docs/ai-technical-iteration-plan.md`。本文保留已完成轨迹和待办细项，不再作为最高优先级入口。
> 当前阶段主线是 **AI 学习与实验沙盒**，长期目标是演进为生产级游戏项目 / 生产级玩法运行时。完整数据驱动 Def、触发器玩法、编辑器、UGC 方向暂缓一次性铺开；只有在它们直接服务 AI 学习 sample、生产级地基收口或清晰垂直切片时再小步引入。

## 当前目标

下一阶段目标从“继续新增 sample”转向“把已跑通的 AI 概念收口为可观测、可承压的 C++ runtime”。近期应优先：

- 补 Release x64 `ai_perf_1000` 基线。
- 建立 `PerceptionResultCache`，让 BT / Lua 只读结果。
- 抽出 `TacticalQueryService`。
- 补 `InfluenceMapSystem` interval / dirty region / layer debug draw。
- 做 BT runtime cache / LOD 第一版。
- 统一 AI RuntimeDiag 输出。

历史上“agent 感知、记忆、决策、行动”的学习切片已通过 `Sandbox9`-`Sandbox15`、`Sandbox17`、`Sandbox18` 覆盖；后续重点是生产化收口和性能基线。

## 迭代记录

- 2026-05-29：新增 `SandboxServices`，并从 `GameManager -> ObjectManager -> BaseObject` 注入；组件现在能从所属对象读取服务上下文。
- 2026-05-29：`PhysicsComponent` 优先通过 `SandboxServices` 找物理世界，保留旧 `g_GameManager` 路径作为兼容兜底。
- 2026-05-29：`AIController` 的对象查询和寻路入口优先通过 `SandboxServices`，保留旧全局变量作为兼容兜底。
- 2026-05-29：继续扩大服务上下文覆盖面：运动避障、FSM 寻路/随机点/分离力、武器发射、FSM 开关、子弹碰撞粒子清理、导航网格对象访问。
- 2026-05-29：事件总线新增异步队列入口 `QueueEmit`、队列上限、丢弃计数和每帧 flush；作为通用基础保留，但不再作为近期触发器主线推进。
- 2026-05-30：行为树 runtime 增加 `Parallel` / `Random` 节点，Lua `BehaviorTreeLoader` 支持从配置创建，并补上 blackboard 参数运行时取值。
- 2026-05-30：`Sequence` / `Selector` 增加可配置重评估能力，配置 `reactive` 或 `reevaluateMs` 后可从第一个 child 重新检查条件并中断旧 RUNNING 分支。
- 2026-05-30：Blackboard 补齐 `object-id`、整型/浮点/字符串数组、object-id 数组，用于行为树参数化和后续 AI 感知/记忆表达。
- 2026-05-30：给对象层组件转发方法加上 legacy 护栏，并把 `WeaponComponent` 对宿主渲染组件的访问改成直接 typed component 查询，避免继续沿对象层新增转发口。
- 2026-05-30：组件侧 owner 访问统一收敛为 `BaseObject*` + 局部类型转换；常用组件 key 集中到 `ComponentKeys`，对象/工厂侧优先走 typed component 查询；AI 敌人感知查询抽成 `IAgentPerceptionQuery` / `AgentPerceptionQuery` 小接口。
- 2026-05-30：AI 感知查询从“返回敌人指针”推进为 `AgentPerceptionResult`，可按 blackboard 配置视野范围/寻路要求，并把目标 id、位置、距离、最后已知位置写回 blackboard；新增 `IAgentSpatialQuery` 作为后续空间查询替换点。
- 2026-05-30：阶段方向回到 AI 学习与实验沙盒；清理 Lua 生物 Def、CreatureAssembler、TriggerRuntime、TriggerVolume、BehaviorEventRuntime、数据驱动触发器到 BT 的 Sandbox9 切片。
- 2026-05-30：进入 `AIArchitectureBeyondBook.md` 的 Ch7/Ch8 Stage 3：`Blackboard` 增加 metadata entry 最小通道（typed value + confidence + timestamp + ttl + source），当前感知结果同步写入 `sense.*` / `memory.*` metadata，作为后续 VisionSensor / MemoryComponent 的地基。
- 2026-05-30：完成 K-02 最小闭环：`Blackboard` 增加 TTL 自动过期和 confidence 衰减策略，`AIController` 为 `sense.*` / `memory.*` metadata 设置有效期，RuntimeDiag 可触发 Blackboard 自测。
- 2026-05-30：完成 P-01 最小闭环：新增 `VisionSensor`，由 `AIController` 定时驱动视觉扫描并写入 `sense.*` / `memory.*` metadata，RuntimeDiag 可看到 vision sensor 摘要。
- 2026-05-30：完成 P-02 最小闭环：新增 `MemoryStore`，把 lastKnown 敌人位置、时间戳、有效期和 confidence 衰减收敛为领域接口，底层继续复用 Blackboard metadata。
- 2026-05-30：完成 P-03 最小闭环：`MemoryStore` 同步 Lua 可读 snapshot，BT 增加 lastKnown 记忆条件和移动到最后已知位置的 action。
- 2026-05-30：新增 `Sandbox9` 作为 Chapter 7 Knowledge sample：Lua `KnowledgeSource` 定时评估最近可达敌人与最佳逃跑点，写入 C++ `Blackboard`，再由 BT 消费。
- 2026-05-30：正式收口 `Sandbox10` 为 Chapter 8 Vision/Memory 垂直切片：基于 `Sandbox8` 的 BT 场景拷贝，用左上角阶段面板、蓝色视野圈、黄色记忆点和 RuntimeDiag 展示 `VisionSensor -> MemoryStore -> BT`。
- 2026-05-30：新增 `Sandbox11` 作为 Chapter 8 Sensory Perception 原味复刻 sample：6 个 agent 随机分散出生、交替分队、随机移动/追击/射击，绿色视线与红色路径线展示 `EnemySighted -> TeamMemory -> shared enemy`。
- 2026-05-31：新增 `Sandbox12` 作为 TeamBlackboard 第一版 sample：复用 Chapter 8 动态战场，把队伍共享敌人位置收敛到 `TeamBlackboard`，用于后续 Chapter 9 战术层。
- 2026-05-31：强化 `Sandbox12` 团队协作验收：固定开场 `EnemySighted`，队友写入 `SupportResponded` 并移动到支援点，smoke 会检查 `[TeamBlackboardSmoke] PASS`。
- 2026-05-31：新增 `Sandbox13` 作为 InfluenceMap 第一版 sample：在 danger/support 双层影响图上选择低危险支援点，smoke 会检查 `[InfluenceMapSmoke] PASS`。
- 2026-05-31：补齐 `Sandbox13` 的更新节奏与观测：preset 默认启用 `AIScheduler`，InfluenceMap 通过 `updateIntervalMs` 降频刷新，面板和 smoke 输出 updates/skips/cellWrites。
- 2026-05-31：补齐 TeamBlackboard 调度验收：`Sandbox12` / `Sandbox13` 的视线 pair 扫描、共享记忆应用和记忆清理按 interval + budget 分批执行，smoke 输出 scan/apply stats。
- 2026-05-31：补齐 `SandboxEventDispatcherManager` 的 Team / Global scope 验证：C++ selftest 覆盖 Local / Team / Global 路由、teamId 过滤、QueueEmit 延迟派发和 token unsubscribe，smoke 可用 `-AiEventSelfTest` 检查。
- 2026-05-30：补齐 `BehaviorTreeLoader` 配置诊断：错误和警告会输出节点路径、节点类型、字段名、错误说明和 fallback 后果，便于定位声明式 BT 配置问题。
- 2026-05-31：新增 `Sandbox14` 作为 Hearing / Danger 最小 sample：关闭直接视线扫描，用脚本化枪声和危险线写入 blackboard，展示“没看见敌人但听到/感到威胁后调查或规避”，smoke 检查 `[HearingDangerSmoke] PASS`。
- 2026-05-31：新增 `Sandbox15` 作为 Chapter 9 Formation / 协作 BT 最小 sample：队友分配 formation slot，BT 读取 `CallForBackup` / `WaitForSquadMate` / `MoveToFormationSlot`，smoke 检查 `[FormationSmoke] PASS`。
- 2026-06-01：新增 `AgentSpatialIndexSystem` 第一版，`ObjectManagerAgentSpatialQuery` 默认优先走 uniform grid，并可通过 `HELLO_AI_SPATIAL_INDEX_ENABLE=0` 回退线性查询。
- 2026-06-01：新增 `TeamBlackboardService` 第一版，Lua `TeamBlackboard` 可把 `EnemySighted` 同步到 C++ facts，并把最佳团队敌情写回 agent blackboard。
- 2026-06-01：新增 `Sandbox16` 作为 AI perception pressure sample，默认 120 agent、关闭 AI scheduler、每帧全量感知更新，用于暴露感知/记忆/空间查询性能问题。
- 2026-06-01：新增 `FramePerf` 卡顿诊断日志，支持 `HELLO_PERF_STALL_LOG`、`HELLO_PERF_STALL_THRESHOLD_MS`、`HELLO_PERF_SUMMARY_INTERVAL_MS`，可分段输出 frame / simulate / game / object / AI / spatial / teamBB 耗时。
- 2026-06-02：新增 `AgentPerceptionSystem` 第一阶段：`ObjectManager` 每帧批量驱动 `AIController::TickPerception`，`AIController::TickAI` 默认跳过重复 perception；FramePerf / RuntimeDiag 可单独看到 perception system 的 scans、visible、spatial query 和 memory/vision 耗时。
- 2026-06-02：沉淀 `Sandbox16` / `ai_perf_100` / `ai_perf_500` / `ai_perf_1000` Debug x64 基线报告：见 `docs/perf/ai-perception-baseline-20260602.md`；当前结论是热点集中在 C++ 感知 / 记忆写入，spatial index 已显著降低候选数但仍不足以支撑 500 / 1000 agent 每帧全量感知。
- 2026-06-02：`AgentSpatialIndexSystem` 二期第一段完成：spatial query options 支持 owner/includeSelf、alive、team include/exclude、objectType 过滤；grid 和 linear fallback 走同一套过滤统计，FramePerf / RuntimeDiag 输出 filtered、rejectSelf、rejectTeam、rejectDead、rejectType、queryMs。
- 2026-06-02：完成 spatial filter 二期复测：见 `docs/perf/ai-spatial-filter-retest-20260602.md`；1000 agent 下 filtered 从旧版 24,068 降到 564，`perceptionSystem` 从约 1581 ms 降到约 1247 ms，下一步优先做 `PerceptionResultCache` 减少重复 blackboard / memory 写入。
- 2026-06-02：新增 `Sandbox17` 作为 Chapter 9 Tactics Lua-first 第一版 sample：复刻 HelloOgre3DX 的 DangerousAreas / TeamAreas 战术层，基于 BulletShot / BulletImpact / EnemySighted / DeadFriendlySighted 事件驱动 influence layer；`Sandbox16` 继续作为 AI perception pressure / `ai_perf_*` 入口保留。
- 2026-06-02：新增 `Sandbox18` 作为 Chapter 9 Tactics C++ 第二版第一切片：Lua 继续负责事件编排，C++ `InfluenceMapSystem` 负责 danger / team / objective 三层网格、radial source 写入、战术评分查询和统计输出；smoke 检查 `[Chapter9TacticsCppSmoke] PASS`。

## P0 - 方向回正

- [x] 把当前阶段北极星收敛为 AI 学习与实验沙盒，并保留长期生产级项目目标。
- [x] 删除偏数据驱动玩法切片的 Lua runtime 和 sample。
- [x] 清理 `game_init.lua`、smoke 参数和 `BehaviorTreeLoader` 中只服务触发器切片的注册点。
- [x] 重新选择一个 AI 学习 sample 作为下一阶段主验证面：`Sandbox9` / Chapter 7 Knowledge。

## P0 - AI 感知与记忆切片

- [x] 完成 K-01 最小闭环：`Blackboard` 支持 metadata entry / safe tagged value，并能输出 debug 摘要。
- [x] 完成 K-02 最小闭环：`Blackboard` 支持 metadata TTL 自动过期和 confidence 衰减，避免后续 Sensor / Memory 各自实现过期机制。
- [x] 完成 P-01 最小闭环：`VisionSensor` 接管当前敌人扫描，按间隔输出 `sense.*` / `memory.*` 感知结果。
- [x] 完成 P-02 最小闭环：`MemoryStore` 提供 lastKnown 敌人记忆查询，RuntimeDiag 可看到 memory 摘要。
- [x] 在继续做多生物行为前，把 AI 感知查询先抽成一个小接口。
- [x] 把 AI 感知查询扩展为可配置感知结果，并写回 blackboard。
- [x] 为空间范围查询预留 `IAgentSpatialQuery` 接口，当前用 `ObjectManager` 线性实现兜底。
- [x] 增加记忆层：目标离开视野后保留 lastKnown 位置、时间戳和有效期。
- [x] 增加行为树条件 / action：有目标、失去目标但有记忆、移动到最后已知位置、搜索失败后回待机。
- [x] 在 sample 中展示“发现敌人 -> 追击 -> 失去视野 -> 搜索最后已知位置”的完整过程。
- [x] 日志或 debug 摘要能看出当前感知目标、最后已知位置和 BT 当前节点。

## P0 - 行为树与 Blackboard 可观测性

- [x] 增加 `Parallel` 和 `Random` 行为树节点。
- [x] 增加行为树节点参数的运行时取值能力，先支持常量和 blackboard key。
- [x] 增加 Selector / Sequence 的中断或条件重评估能力。
- [x] 增加 object-id 和数组类型的 blackboard 值支持。
- [x] 给 AI sample 增加一份稳定的 BT trace / blackboard 摘要输出。
- [x] 配置错误能明确报出节点类型、字段和 fallback 行为。

## P1 - AI 架构债

- [x] 不再继续给对象层新增“转发到某个组件”的方法；碰到旧方法时标为 legacy 或逐步迁移。
- [x] 统一组件访问 owner 的方式，全部收敛到 `BaseObject*`。
- [x] 优先使用类型化组件查询，减少散落的字符串 key。
- [ ] 继续把新增 sandbox/runtime 代码改成优先走 `SandboxServices`，减少直接访问 `g_*` 全局单例。
- [ ] 组件所有权逐步迁向 `std::unique_ptr`。
- [x] 用 uniform grid 或等价空间分区替换线性 agent 查询第一版。
- [ ] 继续补齐 spatial query 的过滤、统计和对照基准，避免只停留在“能查附近 agent”的最小实现。

## P1 - 后续 AI 学习主题

- [x] TeamBlackboard 第一版：同队 agent 可以共享发现的敌人位置。
- [x] 团队协作 sample：一个 agent 发现敌人后，队友改变行为。
- [x] InfluenceMap 多层结果与 debug 显示。
- [x] 战术移动 sample：AI 避开危险区域或偏好掩体区域。
- [x] AI 更新调度和性能观测：感知、知识源、InfluenceMap 频率可配置。
- [x] HearingSensor / DangerSensor 最小 sample：没看见敌人时也能根据声音和危险区域改变行为。
- [x] Chapter 9 Formation / 协作 BT 第一版：队形 slot、请求支援、等待队友和移动到队形点。
- [x] Chapter 9 Tactics Lua-first 第一版：`Sandbox17` 展示 DangerousAreas / TeamAreas 双层战术评分，并用 `[Chapter9TacticsSmoke] PASS` 验收事件、layer cell 和更新统计。
- [x] Chapter 9 Tactics C++ 第二版第一切片：`Sandbox18` 使用 C++ `InfluenceMapSystem` 承接 danger / team / objective 三层影响力、位置评分和 best position 查询，并用 `[Chapter9TacticsCppSmoke] PASS` 验收。
- [x] 固定随机种子 sample preset，支撑可复现 AI 行为验证。

## P0/P1 - 下一阶段技术迭代

> 详细规划见 `docs/ai-technical-iteration-plan.md`。当前重点从“继续新增 sample”转向“把已跑通的 AI 概念收口为可扩展 C++ 底座”。

- [x] `AgentSpatialIndexSystem` 第一版：`ObjectManager` 每帧 rebuild agent grid，`IAgentSpatialQuery` 默认优先走 grid，支持 `maxResults` 和基础 candidates/results stats。
- [x] `AgentSpatialIndexSystem` 二期第一段：补 teamId / alive / includeSelf / objectType 过滤和 queryMs / filtered / reject 统计，grid 与 linear fallback 共用过滤语义。
- [x] `AgentSpatialIndexSystem` 二期复测：基于 `ai_perf_500` / `ai_perf_1000` 重新记录 grid vs linear 结果一致性和成本对照。
- [x] `ai_perf` preset：固定 seed，支持 100 / 500 / 1000 agent，输出 spatial / perception / scheduler 统计。
- [x] `ai_perf` baseline 第一版：基于 `Sandbox16` 固化 100 / 500 / 1000 agent Debug x64 场景，记录 spatial on/off、perception system on/off 的 FramePerf 摘要。
- [ ] `ai_perf` baseline 二期：补 scheduler on/off、Release x64、必要 Tracy capture 对照。
- [ ] `AgentPerceptionSystem`：把视觉、听觉、危险等感知收口到 C++ 批量系统，Lua 只读结果。
- [x] `AgentPerceptionSystem` 第一阶段保持每帧全量更新，不启用 scheduler 降频；先把 `AIController` 内的 per-agent vision/memory 热点集中到 system 级统计和缓存。
- [ ] `PerceptionResultCache`：保存 currentTarget、lastKnown、confidence、ageMs、source、候选数和扫描耗时，保持现有 blackboard key 兼容。
- [ ] `TeamBlackboardService`：把 Lua `TeamBlackboard` 迁移为 C++ service，支持 fact TTL、priority、统计和 Lua facade。
  - [x] 第一版 C++ service + Lua facade：Lua `EnemySighted` 会同步写入 C++ facts，Lua 可把最佳团队敌情写回 agent blackboard，并在 `Sandbox12` smoke 中验收 `cppFacts/cppReports/cppApplies`。
- [ ] `TeamBlackboardService` 二期：扩展 `SupportRequested`、`SupportResponded`、`FocusTarget`、`RetreatPoint`、`FormationSlot` 等 fact 类型，减少 Lua 全局表承担团队状态。
- [ ] `InfluenceMapSystem` / `TacticalQueryService`：把 Lua 教学版 InfluenceMap 迁移为 C++ 多层战术评分系统。
- [x] `InfluenceMapSystem` 迁移准备：先用 `Sandbox17` Lua-first 版本固化 Chapter 9 tactics 的事件输入、danger/team layer、统计和 smoke 验收，作为 `Sandbox18` C++ 第二版对照。
- [x] `InfluenceMapSystem` 第一切片：支持 danger / team / objective 3 层，提供 `cellWrites`、`queryCount`、`bestScore`、active cells 和 debug summary 统计，并通过 `ObjectManager` Lua facade 接入 `Sandbox18`。
- [ ] `InfluenceMapSystem` 第二阶段：补 dirty region / interval 更新、cover / crowd / support schema、layer debug draw 和更多 pressure preset。
- [x] `TacticalQueryService` 第一阶段：`sandbox/ai/tactics/TacticalQueryService.h` 已落地——持有 `InfluenceMapSystem`，提供事件订阅/TTL/发布、`RebuildDanger/Team/ObjectiveLayer`，以及查询 API `FindBestSupportPosition`、`FindLowThreatPosition`、`ScoreQueryPosition`、`FindBestQueryPosition` 和 stats。
- [ ] `TacticalQueryService` 收口+二期：把 `ObjectManager` 里仍承担的 tactical 逻辑（`rebuildTacticalInfluenceLayerDebugVisual` 影响图可视化构建、`configureTacticalInfluenceFromNavMesh` 建图编排）下沉到 service / 独立 `TacticalDebugDrawService`，`ObjectManager` 只剩薄转发（service 成唯一实现事实来源，不强迫 Lua 直连）；并补 interval/dirty region、候选点上限、cover/crowd/support schema、layer debug 显式配置。
- [ ] BehaviorTree runtime 补强：instance pool、node result cache、blackboard dirty 依赖、tick bucket、distance LOD 和每帧预算。
- [ ] BT runtime 性能化分步落地：先做 trace sampling / cache 统计，再做 dirty key 依赖和 LOD，避免在感知热点未稳定前扩大改动面。
- [ ] AI debug / RuntimeDiag：统一输出 perception、memory、team facts、influence score、BT trace 和 scheduler stats。
- [ ] AI Debug 第一阶段：不急于完整 UI，先扩展 RuntimeDiag / 文本面板，支持选中或指定 agent 输出 perception snapshot、memory、team fact、BT trace、spatial stats。
- [ ] AI event / communication 清理：补 Lua event facade，清理 `AgentCommunications` 临时 table 约定，并验证 sample reload / agent destroy 后不会留下悬空 callback。

## 暂缓任务

- [ ] Lua 生物 Def / CreatureAssembler。
- [ ] 完整 CSV 管线。
- [ ] TriggerRuntime / TriggerVolume / ECA 触发器玩法切片。
- [ ] 触发器对象库和编辑器摆放流程。
- [ ] 完整可视化积木编辑器。
- [ ] UGC / Mod 打包和沙箱安全。
- [ ] 存档 / 读档序列化。
- [ ] 世界 streaming。
