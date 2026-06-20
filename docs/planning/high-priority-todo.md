# 高优先级 TODO

> 文档状态：**执行记录 + 细粒度 backlog**。当前方向与阶段排期先看 `docs/project-direction.md` 和 `docs/planning/long-term-iteration-plan.md`；AI 技术细节看 `docs/planning/ai-technical-iteration-plan.md`。本文保留已完成轨迹和待办细项，不再作为最高优先级入口。
> 当前阶段主线是 **AI 学习与实验沙盒**，长期目标是演进为生产级游戏项目 / 生产级玩法运行时。完整数据驱动 Def、触发器玩法、编辑器、UGC 方向暂缓一次性铺开；只有在它们直接服务 AI 学习 sample、生产级地基收口或清晰垂直切片时再小步引入。

## 当前目标

下一阶段目标从“继续新增 sample”转向“把已跑通的 AI 概念收口为可观测、可承压的 C++ runtime”。近期应优先：

- Release x64 `ai_perf_100/500/1000` 基线已补（见 `docs/perf/ai-perf-release-baseline-20260612.md`）；后续改为补 scheduler on/off 与 perception cache on/off 回测。
- `PerceptionResultCache` 已建立并接入 `HasEnemy` / RuntimeDiag；后续重点是补 perception cache on/off 回测，并继续清理 Lua 侧可能绕过 cache 的重扫路径。
- `TacticalQueryService` / `TacticalService` 已抽出并接入 `SandboxTactics`；后续进入 tactics 二期。
- 补 `InfluenceMapSystem` interval / dirty region、layer debug 显式配置和 pressure preset。
- 做 BT runtime cache / LOD 第一版。
- AI RuntimeDiag 已有 `[AIRuntimeDiag]` 聚合输出；后续补指定 agent / 文本面板筛选。

历史上“agent 感知、记忆、决策、行动”的学习切片已通过 `Sandbox9`-`Sandbox15`、`Sandbox17`、`Sandbox18` 覆盖；后续重点是生产化收口和性能基线。

## 迭代记录

- 2026-05-29：新增 `SandboxServices`，并从 `GameManager -> ObjectManager -> BaseObject` 注入；组件现在能从所属对象读取服务上下文。
- 2026-05-29：`PhysicsComponent` 优先通过 `SandboxServices` 找物理世界；2026-06-18 已移除旧 `g_GameManager` 兜底。
- 2026-05-29：`AIController` 的对象查询和寻路入口优先通过 `SandboxServices`；2026-06-18 已移除旧全局变量兜底。
- 2026-05-29：继续扩大服务上下文覆盖面：运动避障、FSM 寻路/随机点/分离力、武器发射、FSM 开关、子弹碰撞粒子清理、导航网格对象访问。
- 2026-05-29：事件总线新增异步队列入口 `QueueEmit`、队列上限、丢弃计数和每帧 flush；作为通用基础保留，但不再作为近期触发器主线推进。
- 2026-05-30：行为树 runtime 增加 `Parallel` / `Random` 节点，Lua `BehaviorTreeLoader` 支持从配置创建，并补上 blackboard 参数运行时取值。
- 2026-05-30：`Sequence` / `Selector` 增加可配置重评估能力，配置 `reactive` 或 `reevaluateMs` 后可从第一个 child 重新检查条件并中断旧 RUNNING 分支。
- 2026-05-30：Blackboard 补齐 `object-id`、整型/浮点/字符串数组、object-id 数组，用于行为树参数化和后续 AI 感知/记忆表达。
- 2026-05-30：给对象层组件转发方法加上 legacy 护栏，并把 `WeaponComponent` 对宿主渲染组件的访问改成直接 typed component 查询，避免继续沿对象层新增转发口。
- 2026-05-30：组件侧 owner 访问统一收敛为 `BaseObject*` + 局部类型转换；常用组件 key 集中到 `ComponentKeys`，对象/工厂侧优先走 typed component 查询；AI 敌人感知查询抽成 `IAgentPerceptionQuery` / `AgentPerceptionQuery` 小接口。
- 2026-06-18：`GameManager` 的 FGUI public/C++ 兼容转发壳已删除，Lua native 后端只走 `FairyGuiRuntime`；`tools/run_fgui_selftest.ps1 -Mode All` 已修复空跑问题，必须看到 suite 全通过。
- 2026-06-18：P2 前置能力落地：`BaseObject` 向 Lua 暴露 AI/Weapon/Anim/Attrib typed component getter，agent 入口通过 `AgentComponentAccess.lua` 优先走组件直取并保留旧接口兜底；`Sandbox10`-`Sandbox13` 与 `parity_trace.lua` 的 Blackboard 入口已迁到组件优先；RuntimeDiag `ComponentProbeAgent` 验证非 Soldier `AgentObject` 可复用 AI/Attrib/Weapon 等组件且不复制 Soldier forwarder，含 Weapon ammo round-trip 与 `ShootBullet()`。
- 2026-06-19：P2 继续收窄：`SoldierObject` Lua 导出撤下 getWeapon/AI/maxHealth/ammo/HasEnemy/CanShootEnemy/GetEnemy/移动目标/射击/Enter*Anim 纯组件转发，`AgentObject:getBody/GetAnimation/GetObjectASM` 也撤下 Lua 导出；DT/BT 条件、移动/调查/编队/等待/射击/换弹 action、Chapter9 legacy agent、`SoldierAgent`、`Sandbox3` 与 `parity_trace.lua` 改走 `AgentComponentAccess.lua` 或 typed component getter；DT/BT Lua action 回调签名已统一为 `u[AgentObject]`；剩余迁移面聚焦 AgentObject legacy forwarder 与其它对象门面。
- 2026-06-20：P2 继续瘦身：`SoldierObject` 上述 getWeapon/AI/maxHealth/ammo/敌人/移动目标/射击/Enter*Anim 这批 C++ 兼容转发实现已删除；`SoldierObject` 内部逻辑和 `ObjectManager` AI/Object 诊断改为组件直读。
- 2026-06-20：P2 Locomotion 主路径收窄：`BaseObject` 向 Lua 暴露 `GetLocomotionComponent()`，`AgentLocomotion` 常用移动/路径/steering API 已进入 tolua 导出；`AgentComponentAccess.lua` 新增 Locomotion helper，`AgentUtils.lua`、DT/BT `MoveHelpers.lua` 与 Chapter9 legacy 运动热点改走组件优先路径；RuntimeDiag `ComponentAccessSelfTest` 验证非 Soldier `locomotion`、maxSpeed/target/targetRadius round-trip。
- 2026-06-20：P2 AgentObject Lua 导出继续收口：`AgentObject` 不再向 Lua 导出 `SetTarget/GetTarget/SetTargetRadius/GetTargetRadius`、`SetMaxForce/GetMaxForce/SetMaxSpeed/GetMaxSpeed`、`SetPath/GetPath/HasPath`、路径采样、`ForceTo*`、`ApplyForce`、`GetMass/GetHeight/GetRadius` 等纯 Locomotion facade；`Sandbox2/5/6/10-13/16/17/18`、Chapter2/4 sample、DT/BT `MoveAction/PursueAction` 与 `ConfigManager` 已改用 `AgentComponentAccess.lua` / `AgentLocomotion`。`AgentObject` Lua 侧仅保留位置/朝向/速度、物理形体设置、血量和 future-position 等跨组件语义入口。
- 2026-06-20：P2 AgentObject C++ 纯 Locomotion facade 继续删除：`AgentObject` 上 target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` 这批 C++ 兼容转发实现已移除；`AIController`、FSM `AgentStateController` / `AgentActionContext` / `PursueState`、`SoldierObject` 命令路径、`AgentPerceptionQuery` sight origin 与 `ObjectManager` 诊断改为直接读写 `AgentLocomotion` / `PhysicsComponent`。
- 2026-06-19：C2 继续给 `SandboxMgr` 减肥：新增 `NavigationService` 并导出 Lua 全局 `SandboxNav`，导航 config/build/query API 从 `SandboxMgr` 迁出；AI 感知/FSM 通过 `SandboxServices.navigation` 查询路径和随机点。
- 2026-06-19：C2 继续给 `SandboxMgr` 减肥：新增 `RaycastService` 并导出 Lua 全局 `SandboxRaycast`，raycast 实现从 `SandboxMgr` 迁出；旧 `SandboxMgr::RayCastObjectId` 导出后续已删除。
- 2026-06-19：C2 继续给 `SandboxMgr` 减肥：新增 `SceneService` / `ScriptService` 并导出 Lua 全局 `SandboxScene` / `SandboxScript`，skybox/light/material/scene graph 与 CallFile 从 `SandboxMgr` 迁出；Lua sample 已迁到新全局，旧 `SandboxMgr` scene/script 方法导出后续已删除。
- 2026-06-19：C3/Tactics query 第一阶段：`TacticalQueryService` 包住 influence config/layer/source/sample/score/stats 与 navmesh 建图编排，为后续独立 tactics Lua 主入口留出边界。
- 2026-06-19：C3/Tactics debug draw 第一阶段：新增 `TacticalDebugDrawService`，承接 `DebugDrawer` 临时绘制与 Ogre `ManualObject` 持久 debug visual 生命周期。
- 2026-06-19：C3/Navigation 继续收口：navmesh map 与销毁责任从 `ObjectManager` 迁到 `NavigationService`；`ObjectManager::getNavigationMesh/addNavigationMesh` 只保留 C++ 兼容转发，debug summary 的 navmesh 计数来自 `SandboxServices.navigation`。
- 2026-06-19：C3/TeamBlackboard 继续收口：`TeamBlackboardService` 直接 tolua 导出为 Lua 全局 `SandboxTeam`；Lua `TeamBlackboard.lua` 优先走新入口，`ObjectManager` 只保留旧 API 兼容转发。
- 2026-06-19：C3/AIScheduler 继续收口：`AIScheduler` 直接 tolua 导出为 Lua 全局 `SandboxAIScheduler`；`ConfigManager` 与 `runtime_diagnostics.lua` 优先走新入口，`ObjectManager` 只保留旧 API 兼容转发。
- 2026-06-19：C3/AI update 继续收口：新增 `AIUpdateSystem`，承接 scheduler begin/tick、spatial rebuild、`AgentPerceptionSystem` 批量 update、`TeamBlackboardService` 每帧 sync 和 AI perf stats 写回；`ObjectManager` 只保留对象生命周期与阶段编排。
- 2026-06-19：C3/Lifecycle 继续收口：新增 `ObjectLifecycleSystem`，承接对象 update loop、待删对象移除、对象 event flush 与延迟 scene node 清理；`ObjectManager::Update` 只保留阶段总编排。
- 2026-06-19：P4 裸指针审计继续收口：`AgentObject::m_renderComp`、`OpenSteerAdapter::m_owner`、`LuaScriptComponent` VM/env owner、`PhysicsComponent::m_addedWorld`、`AIController::m_enemy` 等关键裸指针已标注 owning/non-owning；Lua env owner 与 AI cached enemy 在 detach 清空。
- 2026-06-20：P4 所有权继续收敛：`NavigationService::m_navMeshes` 从 raw pointer map 迁为 `std::unique_ptr<NavigationMesh>` owner map，`AddNavigationMesh` 接收历史 raw pointer 后立即接管所有权，避免手动 `SAFE_DELETE` 替换/析构路径。
- 2026-06-20：P4 所有权继续收敛：`ObjectManager` 自有 `ObjectRegistry`、AI scheduler/spatial/perception/team、tactics query/debug/service 指针已迁为 `std::unique_ptr`，getter 保持返回 non-owning 裸指针观察值。
- 2026-06-20：P5 非 Soldier 组件复用面继续扩大：普通 `AgentObject` 现在由 `AgentFactory` 默认挂载 `AnimComponent` 并初始化 body ASM（不启用 Soldier 动画事件），RuntimeDiag `ComponentProbeAgent` 验证 `anim` / `bodyAsm` 与 AI/Attrib/Weapon 同时可用。
- 2026-06-20：P5 Agent 装配 profile 第一段落地：`AgentFactory` 新增 `default` / `component_probe` / `movement_only` / `animated_probe` 轻量 profile 表，`CreateAgent` 仍委派 `default`，`ObjectFactory` 向 Lua 暴露 `SandboxObjects:CreateAgentWithProfile(...)`；RuntimeDiag `ComponentProbeAgent` 改走 `component_probe` profile 验证非 Soldier 组件装配链路，并用 `animated_probe` + `NonSoldierAnimProbeAgent.lua` 验证普通 `AgentObject` 可挂 animated mesh、配置 body ASM 并请求状态切换。
- 2026-06-20：AI RuntimeDiag 统一输出第一段落地：`ObjectManager::buildAiRuntimeDebugSummary(maxAgents)` 作为 Lua 诊断主入口，`runtime_diagnostics.lua` 优先打印 `[AIRuntimeDiag]` 聚合块，一次覆盖 perception/spatial、memory/blackboard、team facts、tactical influence bestScore、BT trace 与 scheduler stats，旧 `buildAiDebugSummary` + `buildAiSchedulerDebugSummary` 路径保留兜底。
- 2026-06-20：BT runtime 性能化第一段落地：`BehaviorTreeDriver` 支持 `HELLO_BT_TRACE_SAMPLE_INTERVAL` / `SetDebugTraceSampleInterval(...)` 控制 trace 采样，并输出 `[BTStats] ticks/traceSamples/traceSkipped/sampleEvery/cacheHits/invalidated`；`[AIRuntimeDiag]` 的 BT agent 行会带上该统计。真正 node result cache、dirty key 依赖、bucket/LOD 仍待后续。
- 2026-06-19：C2/C3 兼容 facade 继续收口：新增 `AgentConfigService` 并导出 Lua 全局 `SandboxAgentConfig`，CppFSM flag 状态从 `SandboxMgr` 迁出；sample 改走新入口，`AgentObject` 通过 `SandboxServices.agentConfig` 读取，不再依赖 `SandboxServices.sandbox`。`TeamBlackboard.lua` / `ConfigManager.lua` / Chapter9 legacy raycast 去掉 `ObjectManager`/`Sandbox` 兜底，只走 `SandboxTeam` / `SandboxAIScheduler` / `SandboxRaycast`。
- 2026-06-19：C2 兼容 facade 收口完成：`SandboxMgr` 旧 Lua 方法导出清空后，空壳 class、Lua 全局 `Sandbox`、tolua pkg 引用和本地 VS 工程条目已删除；`Sandbox3` 陈旧块注释中的旧入口也已删除。
- 2026-06-19：C3 scheduler/team 兼容 facade 继续收口：删除 `ObjectManager` 上的 AIScheduler 与 TeamBlackboard 旧 Lua 导出，Lua 侧只保留 `SandboxAIScheduler` / `SandboxTeam` 主入口。
- 2026-06-19：C3 tactical 兼容 facade 收口完成：新增 `TacticalService` 并导出 Lua 全局 `SandboxTactics`，`Sandbox17` / `Sandbox18` 改走新入口；`ObjectManager` 上的 tactical 旧 Lua 导出已删除。
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
- [x] 增加 baseline 静态门禁 `tools/check_sandbox_architecture.ps1`，防止新增 sandbox/runtime 反向依赖和裸 `g_*` 回流。
- [x] `PhysicsComponent` 通过 `onSandboxServicesChanged` 延迟接入 `SandboxServices.physics`，移除 `g_GameManager` 兜底。
- [x] `WeaponComponent` 依赖 `SandboxServices` 和 `FindComponent<RenderComponent>`，移除 `g_SandboxMgr` / `g_ObjectManager` 兜底。
- [x] `AIController` / `AgentLocomotion` / `AgentStateController` 热点只走 `SandboxServices`，移除 `g_ObjectManager` / `g_SandboxMgr` 兜底。
- [x] `AgentObject` / `BlockObject` 战术事件、碰撞粒子清理和 FSM flag 读取只走 `SandboxServices`，并移除对 `GameManager.h` 的直接 include。
- [x] `RenderComponent` 通过 `SceneFactory` / SceneNode creator 创建和销毁 Ogre 对象，移除 `GameManager.h`；`AnimComponent` 用本地累计时间更新 ASM，移除 `GameManager.h`。
- [x] `SandboxServices` 增加 `input` 服务，`SoldierObject` 不再通过 `GameManager` 获取 `InputManager`。
- [x] `SandboxMgr::RayCastObjectId` 通过 `ObjectManager::GetSandboxServices().physics` 获取物理世界，移除 `g_GameManager` 访问和 `GameManager.h` include；2026-06-19 已进一步迁到 `SandboxRaycast`/`RaycastService`，旧 `SandboxMgr` raycast 导出已删除。
- [x] `SceneFactory` 通过 `SetRootSceneNode` 接收应用层 root scene node，不再直接 include `GameManager.h`。
- [x] `UIManager` 通过构造注入 `Ogre::Camera*`，`UIService` 移除无用 `GameManager.h` include；UI 层不再为 Gorilla 初始化反向依赖 `GameManager`。
- [x] `CameraService` 通过构造注入 camera / scene manager / profile time getter，不再持有 `ClientManager*`。
- [x] `NavigationMesh` debug visual 和 `ObjectManager` 场景节点/scene manager 访问改走 `SceneFactory`，移除 `ClientManager.h` include；`ObjectManager` 当前时间由 `GameManager` 每帧写入，移除 `GameManager.h` include。
- [x] `InputManager` 通过构造注入 `RenderWindow` / `OgreCameraController` / 状态回调，移除 `ClientManager.h` include。
- [x] 删除 manager 层历史 `g_ObjectManager` / `g_SandboxMgr` 与对应 `GetInstance()`；sandbox/runtime 反向 include 和裸 `g_*` 扫描为 0。
- [x] `BaseObject` 组件容器迁向 `std::unique_ptr`，保持 `GetComponent`/`FindComponent` 返回 non-owning 裸指针兼容旧调用。
- [x] 标注并清空 BlockObject/AnimComponent 关键缓存裸指针，排除这些点被误判为拥有关系或二次释放点。
- [x] 标注 AgentObject/OpenSteer/LuaScript/Physics/AIController 关键裸指针 owning/non-owning 语义，并清理 Lua env owner / AI cached enemy detach 状态。
- [x] 补组件生命周期状态、attach/destroy/update 断言与 debug dump 状态输出。
- [~] 继续审计其它缓存裸指针是否需要 non-owning 标注；`ObjectManager` 自有 system/service 指针与 `NavigationService` navmesh map 已迁 `unique_ptr` owner，剩余关注 service/runtime 侧缓存指针。
- [x] `IComponent::getUpdateOrder` + `BaseObject::Update` 统一组件更新顺序，`SoldierObject::Update` 不再手写 AI/Render/Anim/Weapon update block。
- [~] 继续迁移 AgentObject/SoldierObject legacy forwarder，让 Lua/sample 逐步通过组件直取；已完成 AI/Weapon/Anim/Attrib/Locomotion typed getter、agent 入口、`Sandbox2/3/5/6/10-13/16/17/18`、Chapter2/4 sample、DT/BT 条件、DT/BT action、Chapter9 legacy agent、`AgentUtils.lua`、`MoveHelpers.lua`、`MoveAction/PursueAction`、`ConfigManager` 与 `parity_trace.lua` helper 迁移和非 Soldier ComponentProbeAgent 诊断验证（含 Locomotion maxSpeed/target/targetRadius round-trip、Anim body ASM、Weapon ammo round-trip 与 `ShootBullet()`），`SoldierObject` getWeapon/AI/maxHealth/ammo/敌人查询/移动目标/射击/Enter*Anim、`AgentObject:getBody/GetAnimation/GetObjectASM` 以及 `AgentObject` 纯 Locomotion facade 均已撤出 Lua 导出，Soldier 这批 C++ 兼容转发也已删除；`AgentObject` target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` 这批 C++ pure Locomotion facade 已删除，AI/FSM/Soldier/ObjectManager 相关调用改为组件直读；剩余关注其它对象门面和真正跨组件语义入口是否还需要继续拆薄。
- [x] `SandboxMgr` 不再持有/导出对象创建 `Create*` 纯转发；Lua sample 统一通过 `SandboxObjects`/`ObjectFactory` 创建对象，`WeaponComponent` 创建 bullet 改走 `SandboxServices.objectFactory`。
- [x] `SandboxMgr` 不再持有/导出 Gorilla UI `CreateUIFrame` / `SetMarkupColor` 纯转发；Lua sample/base UI 统一通过 `SandboxUI`/`UIManager` 创建面板和设置 markup 颜色。
- [x] `SandboxMgr` 不再导出相机/profile 查询纯转发；Lua sample/base UI 统一通过 `SandboxCamera`/`CameraService` 获取相机、朝向和帧耗时信息。
- [x] `SandboxMgr` 不再导出导航 config/build/query 纯转发；Lua sample/AI helper 统一通过 `SandboxNav`，AI/FSM C++ 侧通过 `SandboxServices.navigation`。
- [x] `SandboxMgr` 不再实现/导出 raycast；Lua Chapter9 legacy 视线遮挡通过 `SandboxRaycast`。
- [x] `SandboxMgr` 不再作为 scene/light/material/CallFile 主入口，也不再导出这些旧方法；Lua sample 统一通过 `SandboxScene` / `SandboxScript`。
- [x] `SandboxMgr` 不再持有/导出 CppFSM flag 状态；`AgentConfigService` / `SandboxAgentConfig` 承接配置，`SandboxServices` 也不再持 `SandboxMgr*`。
- [x] 删除 `SandboxMgr` 空壳 class / Lua 全局 `Sandbox` / tolua pkg 引用 / 本地 VS 工程条目，C2 收口完成。
- [x] `ObjectRegistry` 第一切片：从 `ObjectManager` 拆出对象 id 分配、对象 map、Agent/Block 二级索引和对象查找，`ObjectManager` 保留生命周期、update/AI/感知/战术编排。
- [x] `ObjectManager::Update` 第一切片：对象生命周期/update loop 与延迟 scene node 清理从主 Update 拆出，主 Update 保持阶段编排。
- [x] `ObjectLifecycleSystem` 第一切片：对象 map 遍历、待删对象移除、对象 update/event flush 与延迟 scene node 清理从 `ObjectManager` 私有 helper 提升为独立 system。
- [x] `AIUpdateSystem` 第一切片：scheduler begin/tick、spatial rebuild、批量 perception、TeamBlackboard sync 与 AI perf stats 从 `ObjectManager` 主体中拆出。
- [x] `ObjectManager` 不再持有 navmesh map；`NavigationService` 按 name 持有并销毁 navmesh，ObjectManager 只提供 fixed blocks 与兼容转发。
- [x] `WeaponComponent` 移除 `SoldierObject` 依赖，射击链改为依赖 BaseObject/RenderComponent/SandboxServices。
- [x] `AnimComponent` / `SoldierAnimController` 通过 `IAnimContextProvider` 解除对 `SoldierObject` 的直接 include / dynamic_cast / owner 持有。
- [x] DT/BT driver 与 Lua action 的真实 owner 从 `SoldierObject*` 泛化为 `AgentObject*`，AIController/Blackboard/DT/BT driver/action 已向 Lua 提供 `GetAgentOwner()`，DT/BT Lua action 回调签名已统一为 `u[AgentObject]`。
- [x] FSM 通用查询从 Soldier forwarder 迁到 `AgentActionContext -> AIController / WeaponComponent / AgentAttrib`，Move/Shoot/Pursue/Reload state 与 evaluator 不再直接 cast Soldier。
- [x] `Blackboard` 向 Lua 导出 `GetAgentOwner()` 泛化入口，RuntimeDiag `ComponentAccessSelfTest` 验证非 Soldier `AgentObject` owner round-trip。
- [x] `DeathState` 死亡动画入口改走 `AnimComponent` / `IAnimContextProvider` / `IAnimController`，不再直接 include/cast `SoldierObject`。
- [x] `AgentActionContext` 动画表现桥改走 `AnimComponent` / `IAnimController`，不再直接 include/cast `SoldierObject`。
- [x] 普通 `AgentObject` 默认挂载 `AnimComponent` 并初始化 body ASM，RuntimeDiag `ComponentAccessSelfTest` 验证非 Soldier `anim`/`bodyAsm` 运行面。
- [x] `AgentFactory` 提供可命名装配 profile，`ObjectFactory` / `SandboxObjects` 暴露 `CreateAgentWithProfile`，RuntimeDiag 使用 `component_probe` profile 验证默认可复用组件组合，并使用 `animated_probe` 验证非 Soldier animated mesh + body ASM 状态请求。
- [x] 用 uniform grid 或等价空间分区替换线性 agent 查询第一版。
- [x] 补齐 spatial query 的过滤、统计和近邻上限，避免只停留在“能查附近 agent”的最小实现。
- [ ] 继续补 spatial query 的 Release / scheduler 调参与更完整 AOI 淘汰对照。

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

> 详细规划见 `docs/planning/ai-technical-iteration-plan.md`。当前重点从“继续新增 sample”转向“把已跑通的 AI 概念收口为可扩展 C++ 底座”。

- [x] `AgentSpatialIndexSystem` 第一版：`ObjectManager` 每帧 rebuild agent grid，`IAgentSpatialQuery` 默认优先走 grid，支持 `maxResults` 和基础 candidates/results stats。
- [x] `AgentSpatialIndexSystem` 二期第一段：补 teamId / alive / includeSelf / objectType 过滤和 queryMs / filtered / reject 统计，grid 与 linear fallback 共用过滤语义。
- [x] `AgentSpatialIndexSystem` 二期复测：基于 `ai_perf_500` / `ai_perf_1000` 重新记录 grid vs linear 结果一致性和成本对照。
- [x] `AgentSpatialIndexSystem` 二期第二段：`maxResults` 改为保留近邻候选，`AIController` 通过 blackboard `perception.maxSpatialResults` 透传，`ai_perf_*` 默认限制 16 个候选。
- [x] `ai_perf` preset：固定 seed，支持 100 / 500 / 1000 agent，输出 spatial / perception / scheduler 统计。
- [x] `ai_perf` baseline 第一版：基于 `Sandbox16` 固化 100 / 500 / 1000 agent Debug x64 场景，记录 spatial on/off、perception system on/off 的 FramePerf 摘要。
- [ ] `ai_perf` baseline 二期：补 scheduler on/off、Release x64、必要 Tracy capture 对照。
- [x] `AIScheduler` Lua 主入口收口：`AIScheduler` 已导出为 `SandboxAIScheduler`，sample 配置和 RuntimeDiag scheduler summary 走新入口；`ConfigManager.lua` 不再兜底 `ObjectManager`，`ObjectManager` 旧入口已删除。
- [x] `AIScheduler` 帧执行收口：每帧 begin/tick/统计发布由 `AIUpdateSystem` 编排，对象循环只通过 `ObjectLifecycleSystem` 调用 helper tick。
- [ ] `AgentPerceptionSystem`：把视觉、听觉、危险等感知收口到 C++ 批量系统，Lua 只读结果。
- [x] `AgentPerceptionSystem` 第一阶段保持每帧全量更新，不启用 scheduler 降频；先把 `AIController` 内的 per-agent vision/memory 热点集中到 system 级统计和缓存。
- [x] `PerceptionResultCache`：保存 currentTarget、lastKnown、confidence、ageMs、source、候选数和扫描耗时，保持现有 blackboard key 兼容；`HasEnemy` 已读 cache，`CanShootEnemy` 成功路径同步 cache。
- [ ] `TeamBlackboardService`：把 Lua `TeamBlackboard` 迁移为 C++ service，支持 fact TTL、priority、统计和 Lua facade。
  - [x] 第一版 C++ service + Lua facade：Lua `EnemySighted` 会同步写入 C++ facts，Lua 可把最佳团队敌情写回 agent blackboard，并在 `Sandbox12` smoke 中验收 `cppFacts/cppReports/cppApplies`。
  - [x] Lua 主入口收口：`TeamBlackboardService` 已导出为 `SandboxTeam`，`TeamBlackboard.lua` 只走新入口，不再兜底 `ObjectManager`；`ObjectManager` 旧入口已删除。
  - [x] 每帧 sync 收口：`TeamBlackboardService::SyncFromAgents` 已由 `AIUpdateSystem` 调用并写回 perf stats。
- [ ] `TeamBlackboardService` 二期：扩展 `SupportRequested`、`SupportResponded`、`FocusTarget`、`RetreatPoint`、`FormationSlot` 等 fact 类型，减少 Lua 全局表承担团队状态。
- [ ] `InfluenceMapSystem` / `TacticalQueryService`：把 Lua 教学版 InfluenceMap 迁移为 C++ 多层战术评分系统。
- [x] `InfluenceMapSystem` 迁移准备：先用 `Sandbox17` Lua-first 版本固化 Chapter 9 tactics 的事件输入、danger/team layer、统计和 smoke 验收，作为 `Sandbox18` C++ 第二版对照。
- [x] `InfluenceMapSystem` 第一切片：支持 danger / team / objective 3 层，提供 `cellWrites`、`queryCount`、`bestScore`、active cells 和 debug summary 统计，并通过 `SandboxTactics` / `TacticalService` 接入 `Sandbox18`。
- [ ] `InfluenceMapSystem` 第二阶段：补 dirty region / interval 更新、cover / crowd / support schema、layer debug 显式配置和更多 pressure preset。
- [x] `TacticalQueryService` 第一阶段：`sandbox/ai/tactics/TacticalQueryService.h` 已落地——持有 `InfluenceMapSystem`，提供事件订阅/TTL/发布、`RebuildDanger/Team/ObjectiveLayer`，以及查询 API `FindBestSupportPosition`、`FindLowThreatPosition`、`ScoreQueryPosition`、`FindBestQueryPosition` 和 stats。
- [x] `TacticalQueryService` / `TacticalDebugDrawService` / `TacticalService` 收口：influence config/layer/source/sample/score/stats、事件与 `configureTacticalInfluenceFromNavMesh` 建图编排已走 `SandboxTactics` / `TacticalService`；`rebuildTacticalInfluenceLayerDebugVisual` 影响图可视化构建已下沉到独立 `TacticalDebugDrawService`；`ObjectManager` tactical 旧 Lua 导出已删除。
- [ ] Tactics 二期：补 interval/dirty region、候选点上限、cover/crowd/support schema、layer debug 显式配置和 pressure preset。
- [~] BehaviorTree runtime 补强：trace sampling 与 RuntimeDiag `[BTStats]` 第一段已完成；instance pool、node result cache、blackboard dirty 依赖、tick bucket、distance LOD 和每帧预算仍待做。
- [~] BT runtime 性能化分步落地：trace sampling / stats 已落地，cacheHits/invalidated 先以 0 统计占位；下一步做 dirty key 依赖和 LOD，避免在感知热点未稳定前扩大改动面。
- [x] AI debug / RuntimeDiag：统一输出 perception、memory、team facts、influence score、BT trace 和 scheduler stats。
- [ ] AI Debug 第一阶段：不急于完整 UI，先扩展 RuntimeDiag / 文本面板，支持选中或指定 agent 输出 perception snapshot、memory、team fact、BT trace、spatial stats；当前已有 `[AIRuntimeDiag]` 聚合 dump，剩余是指定 agent / 文本面板筛选。
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
