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
- **C4 已解决**：AnimComponent 通过 `getUpdateOrder()` 排在 Render 后、Weapon 前；`SoldierObject::Update` 不再手写 anim/render/weapon 次序。
- AnimComponent 持非拥有 `Ogre::Entity*`（由 RenderComponent 拥有），重 init 先置空。
- **SMG 无 reload 动画资源**，非 sniper 武器兜底 sniper_idle。

## 6. 数据流 / 与其他模块关系

`Locomotion/Action Intent → SoldierAnimController → AnimStateMachine.RequestState → notify → checkpoints`。关联 [[components]]（AnimComponent）[[objects]]。

## 7. 验证策略

- 回归 sample：`Sandbox3`（动画切换/blend/reload notify）、`Sandbox6`。

## 8. 已知 gap / 相关文档

- 待：补 SMG reload 动画；AI driver / Blackboard / Lua action 的 Soldier 专属接口继续收口。`docs/design/architecture-improvement-plan.md` P5、`docs/design/cpp-object-model-refactor-roadmap.md`。
