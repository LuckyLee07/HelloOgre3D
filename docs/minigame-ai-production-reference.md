# MiniGame AI 生产架构参考与落地计划

> 来源：静态阅读 `G:\MiniGame_Dev\Source` 与 `G:\MiniGame_Dev\AssetRuntime\CommonResource\Script\luascript\ai`。
>
> 定位：本文用于沉淀 MiniGame 生产级 AI 架构里可被 `HelloOgre3D` 吸收的做法。它不改变本项目当前 **AI 学习与实验沙盒** 的北极星，只帮助后续在感知、团队、战术、影响力图和 AI 性能观测上用更接近生产系统的方式设计。
>
> 与 `docs/reference-minigame-patterns.md` 的关系：后者是 MiniGame 沙盒体系的通用参考；本文只聚焦 AI 运行时、Lua/C++ 分界和后续落地任务。

## 1. 总体判断

MiniGame 的 AI 不是纯 ECS，也不是简单的 Lua 行为树。它是一个生产级混合架构：

```text
Actor / GameObject / Component / Controller
  -> C++ AI runtime / BT runtime / AIFunctionMgr / FindComponent
    -> AOI / SoA / Job / spatial query / navigation / cache / profiler
      -> Lua BT config / Lua task / UGC task / mob event / debug data
```

它的核心取舍是：

- 对象层保留 Actor、Component、Controller 这类业务表达模型。
- 高频、大规模、可批处理的部分沉到 C++：空间查询、AOI、导航、可见性、BT tick 调度、对象池、缓存、异步 job。
- Lua 侧承担行为创作层：行为树配置、节点脚本、UGC task、事件脚本、调试数据回传。
- 测试工程里另有更激进的 ECS / AOI 性能实验，用来验证大规模 agent 的数据导向设计。

因此对 `HelloOgre3D` 的启发不是“所有 AI 都 ECS 化”，而是：**对象和脚本层保表达力，热路径系统化、数据化、可观测化。**

## 2. 可参考的设计点

### 2.1 GameObject + Component 只做表达层

MiniGame 的 `GameObject` 是组件容器，不是 ECS。组件可注册 tick，宿主只遍历需要 tick 的组件，避免每帧扫所有组件。

参考价值：

- `HelloOgre3D` 可以继续保留 `AgentObject`、`AIController`、组件式入口。
- 不要让每个 AI 能力都变成对象层逐帧 Lua 调度。
- 组件只负责绑定 owner、生命周期、debug 入口和少量状态桥接；批量感知、空间查询、战术评分应进入 C++ system。

### 2.2 行为树 runtime 是系统，不只是节点递归

MiniGame 的 BT runtime 包含：

- `BTManager`：树实例管理、事件 actor 索引、全局事件索引。
- `BehaviorTreeCreator`：节点、黑板、实例对象池与回收池。
- `BTBlackboard`：C++ typed data + Lua 包装。
- `BTNodeBase` / `BTreeIns`：节点结果缓存、blackboard dirty、one-run cache、tick split、distance LOD。
- Lua `BTMain` / `BTTree` / `BTTask`：负责加载配置、创建任务、包装协程、暴露 controller 协议。

参考价值：

- 当前项目的 BT sample 不应只停留在教学递归执行。
- 后续可以补 `BehaviorTreeInstancePool`、`BlackboardDirtySet`、`NodeResultCache`、`BT tick budget`、`distance LOD`。
- Lua 节点仍可保留，但 C++ 节点执行、调度和缓存边界要清楚。

### 2.3 C++ AI query facade 是 Lua 可用性的前提

MiniGame Lua task 大量调用 `AIFunctionMgr`，但重逻辑通常在 C++：

- 找最近玩家、生物、方块。
- 获取可逃离位置。
- 可见性缓存查询。
- 背包、装备、交互、移动、技能动作。
- 周边 item / living 选择。

参考价值：

- `HelloOgre3D` 不应让 Lua task 自己遍历 agent 列表或世界对象。
- 应提供更小、更清晰的 C++ facade，例如：
  - `AISenseQueryService`
  - `AITargetQueryService`
  - `AITacticalQueryService`
  - `AINavigationQueryService`
- Lua 只传查询意图和参数，C++ 返回有限数量的结果。

### 2.4 AOI / SoA / Job 是中大型 AI 的性能底座

MiniGame 的 AOI 不是普通组件遍历，而是：

- `SoaObjectAccessor` 管理连续数组和 handle。
- `AOIEntity` 用 handle 指向 SoA 存储，owner 用 `InstanceID` 保持线程安全。
- `AOISystem` 可同步或异步更新。
- `PhysicsAOISystem` / `ActorAOIComponent` 展示了“组件是桥，系统做批处理”的模式。
- enter / leave 通过差分计算，再切回主线程通知。

参考价值：

- `HelloOgre3D` 的感知、团队通信、战术层如果要支撑中大型 AI，应先补 C++ 空间索引和 AOI。
- 组件负责把 agent 注册进 system；system 负责批量查询、缓存和统计。
- 查询结果要有数量上限，避免密集场景退化成 O(N²)。

### 2.5 Lua AI 层值得学的是协议，不是热路径

MiniGame 的 Lua AI 目录很重，约数百个文件，包含：

- `BTMain.lua`：C++ 调 Lua 的行为树入口函数。
- `BTTree.lua`：解析配置并创建 C++ node userdata。
- `BTTask.lua`：`will/onbeg/run/onwait/onend/release/send_debug_data` 任务协议。
- `BTBlackboard.lua`：Lua 黑板包装与缓存。
- `BTEventManager.lua`：事件注册、事件参数、树级事件队列。
- `task/` 与 `ugctask/`：大量业务节点。
- `mobevent/`：mob 事件脚本。

参考价值：

- 可以借鉴 Lua task 生命周期协议，让 sample 的行为脚本更统一。
- 可以借鉴 `controller.bb`、`controller.target`、`controller.wait`、`controller.subparam` 这种简洁接口。
- 可以借鉴 `send_debug_data` 把节点运行参数回传到 AI debug view。

但需要警惕：

- Lua 里循环候选对象、持续 `while + wait`、每 tick 多次跨语言调用，都会成为性能风险。
- Lua task 可以编排行为，不应承担大规模感知扫描、战术评分、影响力传播和队伍分配。

### 2.6 异步查询模式可直接借鉴

MiniGame 的一些 query executor 使用了成熟模式：

```text
主线程采集安全快照
  -> 后台线程过滤 / 排序 / 格式化
    -> 主线程 completion
      -> jobId 校验 / cancel flag / fence 等待
```

参考价值：

- AI debug 查询、trace 整理、战术候选点筛选、影响力图重建都可以使用该模式。
- 后台线程不要直接访问活动 Actor 指针，只处理快照数据。
- completion 回主线程前校验 job 状态，避免过期结果写回。

### 2.7 `MiniGameTest/TestAOI` 是性能方向的参考样板

测试工程里有 ECS / AOI 性能验证代码，虽然不是默认生产路径，但很有设计价值：

- flecs 多线程 pipeline。
- Component mode 与 ECS mode 对照。
- SparseGrid 与 Octree 对照。
- AOI staging buffer，主线程再写回 ECS component。
- 缓存 positions / velocities / species / AABB，避免热查询频繁 `ecs_get`。
- 设置 `MAX_AOI_RESULTS` 避免密集场景结果爆炸。
- 支持 100 到 10000 fish 的 UI 压测。

参考价值：

- `HelloOgre3D` 后续可以做自己的 `AI perf benchmark sample`。
- 目标不是马上引入 flecs，而是先建立可对比的基准：对象 tick、C++ spatial index、AOI batch、job 化、Lua 编排各自的成本。

## 3. Lua / C++ 分界建议

### 3.1 Lua 适合承担

- sample 编排。
- BT / FSM / DecisionTree 的行为配置。
- 低频条件判断和动作编排。
- 节点参数转换、debug 数据回传。
- 行为可读性较强、规模较小的实验逻辑。
- 对 C++ facade 的薄封装。

### 3.2 C++ 应承担

- agent 空间索引与 AOI。
- 视野、距离、遮挡、可见性缓存。
- 感知扫描和结果缓存。
- TeamBlackboard 生命周期与批量更新。
- 战术评分、角色分配、目标分配。
- InfluenceMap 多层传播和组合评分。
- 导航预算、路径请求队列、移动可达性批量检查。
- BT tick 调度、LOD、预算、对象池、黑板 dirty cache。
- 性能统计、trace 采样、debug snapshot。

### 3.3 判断规则

如果一个逻辑满足任一条件，应优先放 C++：

- 会被大量 agent 高频调用。
- 会遍历对象、网格、路径点、候选目标。
- 需要做 pairwise 判断或空间邻域查询。
- 需要跨线程、分片或预算控制。
- 结果需要被多个 agent 复用。
- 需要进入 Tracy / perf counter 做稳定观测。

如果一个逻辑满足以下条件，可以放 Lua：

- 主要是行为组合和调用顺序。
- 单次执行只处理少量对象。
- 主要服务 sample 可读性或调试展示。
- 需要快速改配置、热调试、验证 AI 概念。

## 4. 对 HelloOgre3D 的落地路线

### Phase 0：先立边界

目标：避免后续把团队和战术继续堆到 Lua。

任务：

- 明确 `Lua action / condition` 只能通过 C++ query facade 获取世界信息。
- 给 Lua 返回列表设置上限和统计。
- 所有 AI 热点入口接入最小 perf counter。
- 文档中明确：Lua-first sample 可以存在，但生产化收口目标是 C++ system。

完成标准：

- 新增 AI task 时能判断它属于 Lua 编排还是 C++ 热点。
- Lua 脚本不再新增全局对象扫描。

### Phase 1：C++ Agent Spatial / AOI 底座

目标：先把“附近有哪些 agent / 目标 / 队友”变成 C++ 快路径。

建议模块：

- `AgentSpatialIndexSystem`
- `AgentAOIEntity`
- `AgentSpatialHandle`
- `AISenseQueryService`

能力：

- 注册 / 注销 agent。
- 按位置、半径、team、tag、alive 状态查询。
- 查询结果有上限。
- 支持 debug draw 和统计：注册数量、查询次数、候选数量、耗时。

适配 sample：

- 感知 sample：敌人进入视野后写 blackboard。
- 团队 sample：一个 agent 发现敌人，同队共享目标。

### Phase 2：感知与可见性缓存

目标：让 Lua 行为树读取感知结果，而不是自己做判断。

建议模块：

- `AgentPerceptionSystem`
- `VisionSense`
- `HearingSense`
- `DamageSense`
- `AgentMemoryStore`

能力：

- 按 tick interval / bucket 分片。
- 支持 lastKnownPosition、confidence、ageMs。
- 支持可见性缓存和遮挡检测预算。
- 输出 `AgentPerceptionResult` 给 BT / Blackboard。

适配 sample：

- last known position 行为。
- 听觉 / 伤害事件改变行为。

### Phase 3：TeamBlackboard 与战术系统 C++ 化

目标：团队层不再只是 Lua table 共享，而是有生命周期、查询和调度边界。

建议模块：

- `TeamBlackboardService`
- `TeamEventChannel`
- `TacticalAssignmentSystem`
- `FormationSystem`

能力：

- teamId -> shared facts。
- focus target、support request、retreat point、formation slot。
- 按优先级和过期时间管理共享信息。
- 支持 commander 选择或简单角色分配。

Lua 侧职责：

- 配置战术偏好。
- 行为树读取 team fact。
- sample 展示队伍响应。

### Phase 4：InfluenceMap 和战术评分

目标：把战术移动从“找最近点”提升为“多层评分”。

建议模块：

- `InfluenceMapSystem`
- `InfluenceLayer`
- `TacticalQueryService`

能力：

- threat / ally control / objective / cover / crowding 多层。
- 固定频率或脏区更新。
- 组合评分：`score = objective + cover - threat - crowding`。
- debug 显示单层和组合层。

Lua 侧职责：

- 选择使用哪种 tactical query。
- 配置 sample 参数。
- 展示选择理由。

### Phase 5：行为树 runtime 升级

目标：让 BT 能支撑更大规模 agent，而不是每棵树每帧完整跑。

任务：

- BT instance pool。
- Blackboard dirty key 与节点依赖。
- Node result cache。
- tick step / bucket / distance LOD。
- event-driven reevaluate。
- debug trace 采样开关。

Lua 侧仍可提供：

- `LuaAction` / `LuaCondition`。
- `send_debug_data`。
- BT config 和 sample 行为。

### Phase 6：AI benchmark sample

目标：用数据校验设计，而不是只凭架构感觉。

建议场景：

- 100 / 500 / 1000 / 5000 agent。
- 对比：
  - 对象逐个 tick。
  - C++ spatial index 查询。
  - AOI batch。
  - Lua task 编排。
  - BT tick LOD。
  - InfluenceMap 开关。

指标：

- 每帧 AI 总耗时。
- 感知查询次数。
- 候选数量均值 / 最大值。
- Lua callback 次数。
- BT tick 次数 / skip 次数。
- influence cell writes。

## 5. 不建议照搬

不要照搬以下内容：

- 巨型 `AIController`：生产项目里有历史耦合，但本项目不应再堆一个全能 AI manager。
- 巨型 `AIFunctionMgr`：可以借鉴 facade 思路，但要拆成小服务，避免所有 AI API 都进一个类。
- 完整 UGC / AI 编辑器链路：当前北极星不是生产级 UGC。
- Lua 大量业务节点直接进入主线：Lua 节点适合 sample 和行为编排，不适合作为团队 / 战术 / 感知热路径。
- 先引入第三方 ECS 再改架构：应先做 spatial / AOI / benchmark，等瓶颈清楚后再评估是否需要 ECS 库。

## 6. 文档联动

后续使用本文时，应与以下文档一起看：

| 文档 | 关系 |
|---|---|
| `docs/project-direction.md` | 北极星。本文所有建议必须服务 AI 学习与实验沙盒。 |
| `docs/ai-roadmap.md` | AI 长期路线。本文补充“生产级参考如何落地”。 |
| `docs/behavior-tree-gap-analysis.md` | BT runtime 补强时参考。 |
| `docs/high-priority-todo.md` | 如果把本文任务纳入当前迭代，需要同步拆到 TODO。 |
| `docs/reference-minigame-patterns.md` | MiniGame 通用沙盒参考，本文是 AI 专项补充。 |

## 7. 后续建议任务清单

| 优先级 | 任务 | 目标 |
|---|---|---|
| P0 | 明确 Lua / C++ AI 分界规则 | 防止战术、感知继续堆 Lua。 |
| P0 | 设计 `AgentSpatialIndexSystem` | 给感知和团队查询提供 C++ 底座。 |
| P0 | 设计 `AISenseQueryService` | Lua task 只调用受控 query facade。 |
| P1 | TeamBlackboard C++ 化 | 支撑团队共享目标、支援、撤退和队形。 |
| P1 | InfluenceMapSystem 批量更新 | 支撑战术层和 debug 可视化。 |
| P1 | BT runtime pool / cache / LOD | 支撑更多 agent 的行为树执行。 |
| P1 | AI perf benchmark sample | 用数据验证对象 tick、Lua、AOI、BT LOD 的成本。 |
| P2 | 异步 AI debug query | 支撑复杂调试查询，不阻塞主线程。 |

## 8. 一句话原则

`HelloOgre3D` 可以保留 AI 学习沙盒的清晰性，但在系统边界上要按生产级思路设计：**Lua 讲清楚行为，C++ 扛住规模，sample 验证概念，profiler 验证成本。**
