# 高优先级 TODO

> 方向依据：`docs/project-direction.md` 和 `docs/foundation-modules-inventory.md`。
> 这个文件只记录“当前最该做什么”。如果它和北极星方向冲突，以北极星方向为准，并更新这里。

## 当前目标

先跑通第一个最小垂直切片：

> 一个“由数据定义的生物”进入世界；一个区域触发器检测到玩家进入；生物通过数据驱动的行为树发起攻击；修改数据后可以低成本热重载。

换句话说：先证明“内容从代码变成数据”这条主线能真的跑起来，再去扩展编辑器、UGC 和大世界规模。

## 迭代记录

- 2026-05-29：新增 `SandboxServices`，并从 `GameManager -> ObjectManager -> BaseObject` 注入；组件现在能从所属对象读取服务上下文。
- 2026-05-29：`PhysicsComponent` 优先通过 `SandboxServices` 找物理世界，保留旧 `g_GameManager` 路径作为兼容兜底。
- 2026-05-29：`AIController` 的对象查询和寻路入口优先通过 `SandboxServices`，保留旧全局变量作为兼容兜底。
- 2026-05-29：继续扩大服务上下文覆盖面：运动避障、FSM 寻路/随机点/分离力、武器发射、FSM 开关、子弹碰撞粒子清理、导航网格对象访问。
- 2026-05-29：事件总线新增异步队列入口 `QueueEmit`、队列上限、丢弃计数和每帧 flush；flush 时带递归保护，回调中再次排队的事件会留到下一轮分发。
- 2026-05-30：补齐事件总线整组能力：每帧分发节流、过滤器订阅、参数化事件名、Local / Team / Global scope 路由，供后续触发器和行为树事件复用。
- 2026-05-30：定义第一版 Lua table 生物 `Def`，并增加最小 `CreatureAssembler`，可从 Def 创建 soldier / agent，并应用队伍、位置、运动、属性和武器初始值。
- 2026-05-30：跑通第一个垂直切片：`Sandbox9` 使用 Lua `TriggerVolume` 检测玩家进入区域，经 `TriggerRuntime` 唤醒守卫 BT 的 `Event` 节点；F8 可低成本重载 Def / Trigger / BT。

## P0 - 地基任务

- [x] 引入 `SandboxServices`，作为 sandbox 系统和组件之间的依赖注入载体。
- [ ] 继续把新增 sandbox/runtime 代码改成优先走 `SandboxServices`，减少直接访问 `g_*` 全局单例。
- [x] 扩展 `SandboxEventDispatcherManager`，让事件总线能支撑触发器和行为树事件。
  - [x] 增加跨帧异步事件队列：先排队，后续统一分发。
  - [x] 增加队列长度上限和丢弃计数，避免事件无限堆积。
  - [x] 增加 flush 递归保护，避免事件回调里继续触发事件导致连环递归。
  - [x] 增加每帧分发节流，避免单帧事件过多卡住。
  - [x] 支持“过滤器即订阅”，订阅时能只接收符合条件的事件。
  - [x] 支持参数化事件名，例如 `PLAYER_ENTER?regionId=1`。
  - [x] 实现 Local / Team / Global 三种事件作用域路由。
- [x] 定义第一版 Lua table 生物 `Def` 格式。
- [x] 增加一个最小生物装配器：读取 Lua table `Def`，创建对象并挂组件。

## P0 - 第一个垂直切片

- [x] 增加 `TriggerVolume`，作为第一个区域事件源。
- [x] 增加 `TriggerRuntime.lua`，先用手写 Lua 完成“事件 -> 条件 -> 动作”的触发器注册和分发。
- [x] 增加 `BehaviorEventNode`，或等价的行为树事件入口，让事件能喂给 BT。
- [x] 增加第一个切片 sample：数据定义生物 + 玩家进入区域 + 生物攻击反应。
- [x] 增加低成本热重载：重新加载 Def / BT / trigger Lua 文件即可验证改动。

## P1 - 行为树补强

- [ ] 增加 `Parallel` 和 `Random` 行为树节点。
- [ ] 增加行为树节点参数的运行时取值能力，先支持常量和黑板 key。
- [ ] 增加 Selector / Sequence 的中断或条件重评估能力。
- [ ] 增加 object-id 和数组类型的黑板值支持。

## P1 - 顺手偿还的架构债

- [ ] 不再继续给对象层新增“转发到某个组件”的方法；碰到旧方法时标为 legacy 或逐步迁移。
- [ ] 组件所有权逐步迁向 `std::unique_ptr`。
- [ ] 统一组件访问 owner 的方式，全部收敛到 `BaseObject*`。
- [ ] 优先使用类型化组件查询，减少散落的字符串 key。
- [ ] 在继续做多生物行为前，把 AI 感知查询先抽成一个小接口。

## 暂缓任务

- [ ] 完整 CSV 管线。
- [ ] 存档 / 读档序列化。
- [ ] 完整可视化积木编辑器。
- [ ] 触发器对象库和编辑器摆放流程。
- [ ] UGC / Mod 打包和沙箱安全。
- [ ] 世界 streaming。
