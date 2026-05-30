# 高优先级 TODO

> 方向依据：`docs/project-direction.md` 与 `docs/ai-roadmap.md`。
> 当前主线是 **AI 学习与实验沙盒**。数据驱动 Def、触发器玩法、编辑器、UGC 方向暂缓；只有在它们直接服务 AI 学习 sample 时再重新引入。

## 当前目标

先跑通一个更贴近 AI 学习的切片：

> agent 通过感知发现敌人，把目标 id、位置、距离写入 blackboard；目标离开视野后保留最后已知位置；行为树能根据这些信息追击、搜索或回到待机，并能通过 trace / 日志看懂它为什么这么做。

换句话说：先把“AI 如何感知、记忆、决策、行动”讲清楚，而不是先做玩法数据管线或触发器编辑器地基。

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
- 2026-05-30：方向回正为 AI 学习与实验沙盒；清理 Lua 生物 Def、CreatureAssembler、TriggerRuntime、TriggerVolume、BehaviorEventRuntime、数据驱动触发器到 BT 的 Sandbox9 切片。
- 2026-05-30：进入 `AIArchitectureBeyondBook.md` 的 Ch7/Ch8 Stage 3：`Blackboard` 增加 metadata entry 最小通道（typed value + confidence + timestamp + ttl + source），当前感知结果同步写入 `sense.*` / `memory.*` metadata，作为后续 VisionSensor / MemoryComponent 的地基。
- 2026-05-30：完成 K-02 最小闭环：`Blackboard` 增加 TTL 自动过期和 confidence 衰减策略，`AIController` 为 `sense.*` / `memory.*` metadata 设置有效期，RuntimeDiag 可触发 Blackboard 自测。
- 2026-05-30：完成 P-01 最小闭环：新增 `VisionSensor`，由 `AIController` 定时驱动视觉扫描并写入 `sense.*` / `memory.*` metadata，RuntimeDiag 可看到 vision sensor 摘要。
- 2026-05-30：完成 P-02 最小闭环：新增 `MemoryStore`，把 lastKnown 敌人位置、时间戳、有效期和 confidence 衰减收敛为领域接口，底层继续复用 Blackboard metadata。
- 2026-05-30：完成 P-03 最小闭环：`MemoryStore` 同步 Lua 可读 snapshot，BT 增加 lastKnown 记忆条件和移动到最后已知位置的 action。
- 2026-05-30：新增 `Sandbox9` 作为 Chapter 7 Knowledge sample：Lua `KnowledgeSource` 定时评估最近可达敌人与最佳逃跑点，写入 C++ `Blackboard`，再由 DT 消费。

## P0 - 方向回正

- [x] 把项目北极星改回 AI 学习与实验沙盒。
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
- [ ] 在 sample 中展示“发现敌人 -> 追击 -> 失去视野 -> 搜索最后已知位置”的完整过程。
- [ ] 日志或 debug 摘要能看出当前感知目标、最后已知位置和 BT 当前节点。

## P0 - 行为树与 Blackboard 可观测性

- [x] 增加 `Parallel` 和 `Random` 行为树节点。
- [x] 增加行为树节点参数的运行时取值能力，先支持常量和 blackboard key。
- [x] 增加 Selector / Sequence 的中断或条件重评估能力。
- [x] 增加 object-id 和数组类型的 blackboard 值支持。
- [ ] 给 AI sample 增加一份稳定的 BT trace / blackboard 摘要输出。
- [ ] 配置错误能明确报出节点类型、字段和 fallback 行为。

## P1 - AI 架构债

- [x] 不再继续给对象层新增“转发到某个组件”的方法；碰到旧方法时标为 legacy 或逐步迁移。
- [x] 统一组件访问 owner 的方式，全部收敛到 `BaseObject*`。
- [x] 优先使用类型化组件查询，减少散落的字符串 key。
- [ ] 继续把新增 sandbox/runtime 代码改成优先走 `SandboxServices`，减少直接访问 `g_*` 全局单例。
- [ ] 组件所有权逐步迁向 `std::unique_ptr`。
- [ ] 用 uniform grid 或等价空间分区替换线性 agent 查询实现。

## P1 - 后续 AI 学习主题

- [ ] TeamBlackboard 第一版：同队 agent 可以共享发现的敌人位置。
- [ ] 团队协作 sample：一个 agent 发现敌人后，队友改变行为。
- [ ] InfluenceMap 多层结果与 debug 显示。
- [ ] 战术移动 sample：AI 避开危险区域或偏好掩体区域。
- [ ] AI 更新调度和性能观测：感知、知识源、InfluenceMap 频率可配置。
- [ ] 固定随机种子 sample preset，支撑可复现 AI 行为验证。

## 暂缓任务

- [ ] Lua 生物 Def / CreatureAssembler。
- [ ] 完整 CSV 管线。
- [ ] TriggerRuntime / TriggerVolume / ECA 触发器玩法切片。
- [ ] 触发器对象库和编辑器摆放流程。
- [ ] 完整可视化积木编辑器。
- [ ] UGC / Mod 打包和沙箱安全。
- [ ] 存档 / 读档序列化。
- [ ] 世界 streaming。
