# AI Chapter 7-9 迁移后续扩展路线图

> 目标：在迁入 HelloOgre3DX chapter 7-9 的 Knowledge / Perception / Tactics 能力后，把 AI 系统继续推进到可调试、可观测、可复用、可扩展的状态。
>
> 范围：`src/HelloOgre3D/sandbox/ai/`、`src/HelloOgre3D/runtime/` 中与 AI 可视化相关的适配层，以及 `bin/res/scripts/ai/`、`bin/res/scripts/samples/` 下的 Lua AI 脚本与样例。

## 1. 迁移完成后的基线假设

本文档默认 chapter 7-9 迁移完成后，项目已经具备以下能力：

- DecisionTree / BehaviorTree / FSM 三类 AI driver 可运行，并能由 Lua 配置或 Lua action 扩展。
- Blackboard 已可被 AI driver、Lua action、KnowledgeSource 共同使用。
- KnowledgeSource / SoldierKnowledge 已能按置信度、刷新频率产出知识结果。
- AgentSenses / AgentCommunications 已能完成视野感知、事件广播、队伍消息传递。
- InfluenceMap / SoldierTactics 已能表达危险区域、队伍区域，并可在 sample 中可视化。
- 至少有一组 sample 覆盖 Knowledge、Perception、Tactics 的基本行为回归。

如果上述内容尚未完成，应先回到 chapter 7-9 迁移任务，不要提前展开本文档中的中后期扩展。

## 2. 总体方向

迁移后的重点不再是继续堆叠更多 Lua 行为，而是补齐以下系统能力：

- 可调试：能看到当前 AI 为什么做出某个决策。
- 可观测：能通过日志、Tracy、面板看到耗时、事件、感知、地图层变化。
- 可复用：感知、知识、战术、行为节点不绑定单一 SoldierObject。
- 可调参：行为树、知识源、影响力地图、技能参数尽量数据化。
- 可回归：关键 AI 行为可通过 sample、录制、回放或固定随机种子复现。

## 3. 优先级总览

| 优先级 | 方向 | 主要收益 | 前置依赖 |
|---|---|---|---|
| P0 | AI 调试面板 | 迁移后立刻能定位行为问题 | Blackboard / BT trace / FGUI |
| P0 | 事件系统规范化 | 支撑感知、通信、战术层稳定协作 | AgentCommunications |
| P0 | AI 更新调度与性能观测 | 避免感知和战术层引入周期性卡顿 | Tracy / AI tick |
| P1 | 感知系统组件化 | 把视觉、听觉、伤害、队友通报拆成可组合模块 | AgentSenses |
| P1 | TeamBlackboard / 战术指挥层 | 支撑团队共享信息与集火、撤退、包抄 | Blackboard / Communications |
| P1 | InfluenceMap 多层升级 | 让战术决策和寻路使用更多环境信息 | InfluenceMap |
| P1 | 行为树数据化与热重载 | 提升调 AI 的效率 | BehaviorTreeLoader |
| P2 | 技能 timeline / 触发器 | 为技能、命中、受击、AI 事件联动打基础 | 事件系统 |
| P2 | 录制 / 回放 / 可复现测试 | 降低偶现 AI 问题排查成本 | 事件日志 / 随机种子 |
| P2 | AI 工具化与作者体验 | 降低新增行为和调参成本 | 前面各项 |

## 4. P0：AI 调试面板

### 目标

提供一个运行时 AI debug view，用于查看单个 agent 和团队层面的 AI 状态。

### 建议能力

- 当前 agent 的 driver 类型：FSM / DecisionTree / BehaviorTree。
- 当前 FSM state、DecisionTree 命中的 action、BehaviorTree 正在运行或失败的节点。
- Local Blackboard / Team Blackboard / Global Blackboard 的 key-value 快照。
- KnowledgeSource 列表：最近刷新时间、confidence、evaluation 摘要。
- AgentSenses 输出：可见敌人、最后已知位置、记忆剩余时间、遮挡检测结果。
- AgentCommunications 最近事件：事件类型、发送者、接收者、teamOnly 标记。
- InfluenceMap layer 开关：危险区、队伍区、掩体价值、拥挤度等。
- BT trace 开关：只在 debug 模式收集，避免 release 长期额外开销。

### 建议位置

- C++ 数据采集：`src/HelloOgre3D/sandbox/ai/debug/`
- Lua UI：`bin/res/scripts/ui/views/` 或后续 AI debug 专用目录
- Sample 入口：`bin/res/scripts/samples/`

### 完成标准

- [ ] 能在运行时选择一个 agent，并看到它当前的 AI driver 和状态。
- [ ] Blackboard、KnowledgeSource、Perception、最近事件至少能显示摘要。
- [ ] BehaviorTree trace 能显示最近一帧成功、失败、运行中的节点。
- [ ] 面板关闭后不继续保留无效 Lua callback 或 C++ 观察者。

## 5. P0：事件系统规范化

### 目标

把感知、通信、战术层依赖的事件链路收口，避免 Lua 脚本之间靠隐式 table 字段互相耦合。

### 事件层级

- Local event：单个 agent 内部事件，例如生命值变化、状态切换、技能 notify。
- Team event：同队共享事件，例如发现敌人、请求支援、集火目标变化。
- Global event：Sandbox 级事件，例如子弹命中、区域触发、全局调试指令。

### 建议能力

- 统一事件类型常量，避免散落字符串。
- 明确事件 payload 字段，例如 `senderId`、`teamId`、`position`、`targetId`、`timeMs`。
- 支持订阅生命周期管理，agent 销毁后自动解绑。
- 支持延迟事件或下一帧派发，避免事件回调里直接修改遍历中的对象列表。
- 支持 debug dump：最近 N 条事件、订阅者数量、每类事件派发次数。

### 完成标准

- [ ] `AgentCommunications` 不再直接依赖临时 table 约定，核心字段有明确规范。
- [x] Local / Team / Global 三类事件的派发路径清晰。
- [ ] agent 销毁、sample 重载、UI 关闭后不会留下悬空 callback。
- [x] 事件派发耗时接入 Tracy 或至少有统计 dump。

当前进展（2026-05-31）：

- [x] `SandboxEventPayload` 第一版已统一 `eventType/scope/senderId/targetId/teamId/timeMs/x/y/z` 字段。
- [x] Local 事件已覆盖 `HEALTH_CHANGE`、`ASM_STATE_CHANGE`、`ASM_NOTIFY`，并由 dispatcher 记录最近事件、订阅者数量和派发次数。
- [x] Dispatcher 已返回订阅 token，`AgentObject` / `SoldierObject` 在销毁链路中显式解绑 Local event callback。
- [x] `SandboxEventTypes` 已补 `EnemySighted` / `BulletShot` / `BulletImpact` / `SupportRequested` / `SupportResponded`，作为后续 AI 事件命名收口点。
- [x] `SandboxEventDispatcherManager` 已支持 Local / Team / Global scope 路由、eventName query params 过滤、`QueueEmit` / `FlushQueuedEvents` 延迟派发，并可用 `HELLO_AI_EVENT_SELF_TEST=1` 跑通 scope selftest。
- [ ] Spatial 过滤、Lua 侧事件 facade、`AgentCommunications` 临时 table 约定收口仍待后续处理。

## 6. P0：AI 更新调度与性能观测

### 目标

感知、知识源、影响力地图、战术层都不应每帧全量更新。需要建立统一调度和观测，避免 agent 数量增长后出现周期性 spike。

### 建议能力

- AI tick 与渲染 / 动画 update 解耦，默认低频运行。
- Sensor / KnowledgeSource / Tactics 各自支持刷新频率。
- 按 agentId 或 bucket 错峰更新，避免同一帧全部扫描。
- 按距离、可见性、重要性做 AI LOD。
- Navigation / RayCast / InfluenceMap 更新加入每帧预算。
- Tracy zone 覆盖：
  - `AIScheduler::Tick`
  - `Sensor::Scan`
  - `KnowledgeSource::Evaluate`
  - `InfluenceMap::Spread`
  - `BehaviorTreeDriver::Tick`

### 完成标准

- [ ] 20+ agent 场景下，AI 更新不会集中在固定帧产生明显 spike。
- [ ] 感知和知识源刷新频率可配置。
- [ ] InfluenceMap 更新频率和 layer 开关可配置。
- [ ] 有一份固定 sample 用于性能回归。

当前进展（2026-05-23）：

- [x] `AIScheduler` 第一版已支持 Soldier AI tick 与动画/渲染 update 解耦、按 agentId 错峰、每帧 tick budget 和 Lua 可配置开关。
- [x] `RuntimeProfileCounters` 已接入 `AIScheduler*` Tracy plots，`ObjectManager:buildAiSchedulerDebugSummary()` 可 dump 最近一帧调度数据。
- [x] `ai_perf_smoke` sample preset 已固定 seed、20 个 agent、固定出生点和 AI scheduler 参数，可由 smoke 脚本复现性能样例。
- [ ] Sensor / KnowledgeSource / InfluenceMap 独立刷新频率仍待补齐。

## 7. P1：感知系统组件化

### 目标

把 chapter 8 的 AgentSenses 拆成可组合模块，避免未来潜行、听觉、伤害仇恨、队友通报都继续堆在单个 Lua 文件里。

### 建议模块

- VisionSense：视野角、距离、遮挡、目标类型过滤。
- HearingSense：声音事件、距离衰减、可疑位置。
- DamageSense：受击来源、最近伤害者、仇恨增量。
- TeamSense：队友通报、共享目标、请求支援。
- MemoryStore：最后已知位置、感知置信度衰减、过期清理。

### 完成标准

- [ ] 视觉感知和记忆衰减不再强绑定单个 Soldier 脚本。
- [x] 至少一个 sample 展示“敌人离开视野后，AI 移动到最后已知位置”。
- [ ] 视野角、视野距离、记忆保留时间可通过配置调整。
- [ ] Lua 行为节点只读感知结果，不再在节点内临时扫描全场对象。

## 8. P1：TeamBlackboard 与战术指挥层

### 目标

把单兵智能扩展到团队协作，让队伍能共享目标、危险区域、撤退点和集火意图。

### 建议能力

- TeamBlackboard：按 teamId 管理共享数据。
- Team target：集火目标、优先级、锁定时间。
- Retreat point：队伍撤退点或安全区域。
- Commander / Role assignment：
  - Assault：推进和压制。
  - Support：保持距离和掩护。
  - Flank：寻找侧翼路径。
  - Retreat：低血量或危险区域撤退。

### 完成标准

- [ ] 同队 agent 可以共享发现的敌人位置。
- [ ] 队伍可以选择一个共享集火目标。
- [x] 至少一个 sample 展示“一个 agent 发现敌人后，队友改变行为”。
- [ ] TeamBlackboard 生命周期跟随 Sandbox 或队伍，不依赖单个 agent。

## 9. P1：InfluenceMap 多层升级

### 目标

把 chapter 9 的影响力地图从“危险区可视化”扩展为战术决策和寻路权重的一部分。

### 建议 layer

- EnemyThreat：敌方威胁。
- TeamControl：我方控制。
- CoverValue：掩体价值。
- SightLine：射击视野价值。
- CrowdDensity：拥挤度。
- ObjectiveAttraction：目标吸引力。

### 建议能力

- 每个 layer 独立 falloff、inertia、刷新频率。
- 支持 layer 组合评分，例如 `score = cover - threat - crowd + objective`。
- 支持 debug 选择单层或组合层显示。
- 寻路阶段可选使用 influence cost，而不是只走最短路。

### 完成标准

- [ ] 至少支持 3 个以上可独立显示的 influence layer。
- [ ] 战术行为能读取 influence 结果选择移动目标。
- [ ] 至少一个 sample 展示“AI 避开危险区域或偏好掩体区域”。
- [ ] InfluenceMap 更新成本在 Tracy 中可见。

## 10. P1：行为树数据化与热重载

### 目标

让行为树调试从“改代码、重启、复现”变成“改配置、热重载、观察结果”。

### 建议能力

- 行为树 config 支持 subtree。
- 标准节点库：
  - Sequence / Selector / Parallel
  - Inverter / ForceSuccess / ForceFailure
  - Wait / Timeout / Repeat / Random
  - LuaAction / LuaCondition
- 节点配置支持 debug name。
- 热重载时保留或重建运行时状态的策略明确。
- 配置错误能输出具体节点路径。

### 完成标准

- [ ] 修改 Lua BT config 后可运行时重载。
- [x] 配置错误能定位到具体节点和字段。
- [ ] 至少一个 sample 使用 subtree 复用通用战斗子树。
- [ ] 热重载不会泄漏旧节点、Lua callback 或 C++ 对象。

## 11. P2：技能 timeline 与触发器

### 目标

把攻击、换弹、命中、受击、技能释放从零散 action 整理成 timeline + event notify，作为未来技能系统和 AI 感知联动的基础。

### 建议能力

- SkillTimeline：按时间点触发 notify。
- HitWindow：命中窗口和判定范围。
- Cast / Windup / Active / Recovery 四段式技能状态。
- TriggerVolume：区域触发器、技能范围、任务区域。
- timeline notify 进入统一事件系统，让感知和战术层可订阅。

### 完成标准

- [ ] 普通射击或换弹至少有一个 timeline 化样例。
- [ ] 技能 notify 能触发事件，例如 BulletShot、BulletImpact、SkillHit。
- [ ] AI 能根据技能冷却和目标状态选择是否释放技能。
- [ ] timeline 配置与表现层动画状态机边界清晰。

## 12. P2：录制、回放与可复现测试

### 目标

降低 AI 偶现问题排查成本。

### 建议记录内容

- 随机种子。
- sample 名称和初始参数。
- 输入事件。
- AI 关键决策：driver、节点、action、target。
- Blackboard 关键 key 变化。
- 感知事件和通信事件。
- InfluenceMap 关键 layer 的摘要数据。

### 完成标准

- [ ] 固定随机种子后，同一 sample 的 AI 初始行为可复现。
- [ ] 能导出最近 N 秒 AI 决策日志。
- [ ] 出现异常行为时，日志能回答“为什么选择这个目标 / 为什么切这个 action”。
- [ ] 至少有一个轻量 replay 或半自动回归入口。

## 13. P2：工具化与作者体验

### 目标

降低后续新增 AI 行为、配置战术参数、调试 sample 的成本。

### 建议能力

- AI debug console 命令：
  - 选择 agent。
  - 切换 BT trace。
  - dump blackboard。
  - dump team events。
  - 切换 influence layer。
- Sample preset：
  - 固定双方人数。
  - 固定出生点。
  - 固定随机种子。
  - 固定战术配置。
- 配置校验：
  - 行为树节点字段校验。
  - KnowledgeSource 字段校验。
  - InfluenceMap layer 配置校验。

### 完成标准

- [ ] 新增一个行为树节点时，有明确的注册、配置、调试流程。
- [ ] 新增一个 sample preset 不需要改 C++。
- [ ] 配置错误在启动或 reload 时能被明确报出。

## 14. 建议迭代顺序

### Milestone A：可看见 AI 为什么这么做

1. AI 调试面板第一版。
2. 事件系统规范化。
3. AI 更新调度与 Tracy 观测。

验收：迁移后的 Knowledge / Perception / Tactics 行为问题可以通过面板和日志定位。

### Milestone B：从单兵 AI 到团队 AI

1. 感知系统组件化。
2. TeamBlackboard。
3. 战术指挥层第一版。
4. InfluenceMap 多层显示。

验收：至少一个 sample 展示队伍共享敌情、选择集火目标、避开危险区域。

### Milestone C：提升调参效率

1. 行为树数据化补强。
2. 行为树热重载。
3. 配置校验。
4. Sample preset。

验收：常见战斗行为可以通过 Lua config 调整，不需要重启完整客户端。

### Milestone D：玩法扩展基础

1. 技能 timeline。
2. TriggerVolume。
3. 录制 / 回放。
4. 更完整的 AI 工具命令。

验收：技能事件能驱动 AI 感知和战术反应，偶现问题有复现路径。

## 15. 不建议短期投入的方向

- 不急着引入 GOAP / HTN / Utility AI。当前 DT / BT / FSM 加上知识、感知、战术层已经足够支撑沙盒阶段。
- 不急着做完整 ECS。除非对象类型和组合复杂度已经明显压垮当前继承体系。
- 不把所有 AI 逻辑都搬进 Lua。感知扫描、影响力传播、寻路预算这类热点逻辑应优先放 C++。
- 不把调试面板做成正式业务 UI。它应是开发工具，优先稳定和信息密度。
- 不在没有回归 sample 的情况下重构事件、感知、战术核心链路。

## 16. 跟踪清单

- [ ] AI debug view 第一版。
- [ ] Local / Team / Global 事件层级落地。
- [ ] AI 更新调度器和 Tracy 埋点落地。
- [ ] VisionSense / MemoryStore 拆分。
- [ ] TeamBlackboard 第一版。
- [ ] InfluenceMap 多 layer 与 debug 显示。
- [ ] InfluenceMap 结果接入战术移动选择。
- [ ] BehaviorTree config subtree。
- [ ] BehaviorTree hot reload。
- [ ] 技能 timeline 第一版。
- [ ] TriggerVolume 第一版。
- [ ] AI 决策日志导出。
- [ ] 固定随机种子 sample preset。
- [ ] 至少一个团队协作 sample。
- [ ] 至少一个影响力地图寻路 sample。

