# 角色动画子系统（alias: objects-anim）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

骨骼动画状态机驱动：连接动画表现与游戏状态（idle/move/shoot/reload/death），意图驱动选状态。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/objects/animation/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `AgentAnimStateMachine.{h,cpp}` | 通用 FSM | RequestState/GetCurrState/混合/Notify@time |
| `AgentAnim/AgentAnimState/AgentAnimTransition.{h,cpp}` | 框架 | 片段/状态/转移 |
| `SoldierAnimController.{h,cpp}` | 驱动 | 意图驱动：`SetLocomotionIntent`/`RequestAction`/`ConsumeShootExecution` |
| `SoldierAnimProfile.{h,cpp}` | 配置 | 动作/混合参数 |
| `SoldierAnimTypes.h` | 枚举 | SSTATE_* |

## 4. 公开能力要点

- 状态机 + blend + 事件 notify；locomotion intent(IDLE/MOVE) 与 action intent(SHOOT/RELOAD/DEATH)。

## 5. 约束与红线

- `SoldierAnimController` 通过 `IAnimContextProvider` 读取 stance、body/weapon ASM 与 presentation hooks，不再持 `SoldierObject* owner` 或直接调用 `SoldierObject`（P5 动画侧已收口）。
- 普通 `AgentObject` 也会由 `AgentFactory` 默认挂载 `AnimComponent` 并初始化 body ASM；非 Soldier 路径不启用 Soldier 动画事件，由 RuntimeDiag 覆盖 `anim`/`bodyAsm` 可用性。
- **C4 已解决**：AnimComponent 通过 `getUpdateOrder()` 排在 Render 后、Weapon 前；`SoldierObject::Update` 不再手写 anim/render/weapon 次序。
- AnimComponent 持非拥有 `Ogre::Entity*`（由 RenderComponent 拥有），重 init 先置空。
- **SMG 无 reload 动画资源**，非 sniper 武器兜底 sniper_idle。

## 6. 数据流 / 与其他模块关系

`Locomotion/Action Intent → SoldierAnimController → AnimStateMachine.BlendToState → notify → checkpoints`；通用 Lua 动画演示继续使用显式 `RequestState`/transition 图。关联 [[components]]（AnimComponent）[[objects]]。

## 7. 验证策略

- 回归 sample：`Sandbox3`（动画切换/blend/reload notify）、`Sandbox6`。

## 8. 已知 gap / 相关文档

- 待：补 SMG reload 动画；更复杂非 Soldier 动画行为样例；AI driver / Blackboard / Lua action 的 Soldier 专属接口继续收口。`docs/design/architecture-improvement-plan.md` P5、`docs/design/cpp-object-model-refactor-roadmap.md`。

## 2026-09-12 移动射击与通知合同

AnimComponent 在每次仿真更新前恢复原 ASM 状态，更新动作后由 `SoldierLocomotionLayer` 组合上身动作与下身步态。分层只读真实水平速度/朝向，以现有脊柱后代骨骼为上身遮罩，按速度累计步相，并在 spine 接缝补偿髋部旋转、避免上身枪向随侧步转走；不推进动作通知、不写回 Bullet。Ogre 使用 CUMULATIVE 混合，避免 AVERAGE 按整条动画权重归一而削弱上下身。

玩家与 AI 的发弹都由 `shoot_fire` 通知触发。Lua DT/BT 动作通过 `AnimComponent:ConsumeShootExecution()` 单次消费；`weapon.actionOwnsFire` 在动作期间抑制通用 callback 发弹，退出恢复旧值。取消、死亡和重复通知不能额外扣弹；AI 消费仍比通知晚一个 AI tick。待机/射击停止水平速度并保留竖直分量，碰撞之后仍可推移对象。

分层持借用 Entity/AnimationState；替换 body 或 detach 时先恢复状态、销毁自建 mask，再销毁 Render。body 重建同时清掉 Soldier 控制器通知注册和临时动作状态，下一次更新向新 ASM 注册。站立横移暂复用资源内 crouch 侧步，步频采用参考步长，尚无足底 IK 或滑移量标定。结果与运行边界见[体验修复](../dev-design/plans/2026-09-12-sandbox19-experience-fixes.md)。

## 2026-09-12 可中断动作与独立时钟

Soldier 控制器使用 C++ 内部 `BlendToState`：保持同一目标不重启；替换/取消从当前有效轨道权重开始，默认140ms smoothstep混合，死亡80ms且拒绝普通动作覆盖。逻辑状态共用同一 AgentAnim 时只计算一次轨道。退出轨道继续贡献姿态，但不再触发 gameplay notify；通知含 PlaybackId，替换后的旧通知不被消费。通用 `RequestState` 的配置过渡仍用于 Sandbox3 等显式动画图。

`AgentAnim` 自有逻辑时间、权重和启用状态；下身遮罩与渲染采样临时修改 Ogre 状态时，`GetTime/GetWeight` 仍返回仿真值。`AnimComponent` 在物理后按实际水平位移推进下身步相，100ms响应平滑起停、前后左右权重和站蹲；runtime 只插值两帧完整骨骼姿态。高显示帧率不会额外生成射击通知。暂停冻结插值历史；body/weapon 替换及 detach 先恢复并释放显示缓存。

验证含取消换弹、动作替换、取消发弹、死亡覆盖、身体/武器重建、蹲起、暂停与30/60/120Hz显示对照，详见[实现与证据](../dev-design/plans/2026-09-12-agent-animation-smooth.md)。站立侧步仍复用 crouch 资源，没有新动画、足底IK或上身观察偏移。
