# 组件系统（alias: components）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

可组合能力单元：把对象职责拆成 agent/anim/combat/physics/render/script/ai 组件，支持复用。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/components/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ComponentKeys.h` | 常量 | 8 个 key 常量化（P6 已完成） |
| `agent/AgentLocomotion/AgentAttrib/AgentBody.{h,cpp}` | agent | 运动力/属性/身体参数 |
| `physics/PhysicsComponent.{h,cpp}` | 物理 | btRigidBody 容器，见 [[systems-physics]] |
| `render/RenderComponent.{h,cpp}` | 渲染 | Ogre entity/node + 位置真源同步逻辑 |
| `anim/AnimComponent.{h,cpp}` | 动画 | ASM 容器，见 [[objects-anim]] |
| `combat/WeaponComponent.{h,cpp}` | 战斗 | 弹药/射击/挂接 |
| `ai/AIController.{h,cpp}` | AI | 见 [[ai-controller]] |
| `script/LuaScriptComponent.{h,cpp}` | 脚本 | Lua 绑定 |

## 4. 公开能力要点

- `IComponent`：onAttach/onDetach/update；`getUpdateOrder()` 显式声明组件更新顺序；`getOwner`/`FindComponent<T>()`/`GetSandboxServices()`。
- 各组件 public API（ShootBullet/ApplyForce/GetEntity/...）。

## 5. 约束与红线

- **P6 完成**：ComponentKeys 常量化，C++ 内无散落字符串 key。
- **C4 完成**：AI/Render/Anim/Weapon 通过 `ComponentUpdateOrder` 声明 100/200/300/400 顺序，`BaseObject::Update` 稳定排序后驱动。
- **P4（核心完成）**：`BaseObject` 组件容器已迁到 `std::map<string,unique_ptr<IComponent>>`；组件查询接口仍返回 non-owning 裸指针以保持兼容；`IComponent` lifecycle state 与 `BaseObject` attach/destroy/update 断言已落地，组件 debug dump 会输出状态；AnimComponent 的 Entity 缓存、LuaScriptComponent VM/env owner、PhysicsComponent added world、AIController cached enemy 等关键裸指针已标注 owning/non-owning，Lua env owner 与 AI cached enemy 会在 detach 清空。
- **P5**：AnimComponent/WeaponComponent 已不持有 `SoldierObject* owner` 成员；WeaponComponent 已移除 `SoldierObject` include/dynamic_cast，射击链依赖 BaseObject/RenderComponent/SandboxServices；AnimComponent / SoldierAnimController 已走 `IAnimContextProvider`，不再直接 include/call `SoldierObject`；DT/BT driver 与 Lua action 的真实 owner 已泛化到 `AgentObject`，旧 `SoldierObject` owner 仅作 Lua 兼容桥；FSM 通用查询经 AgentActionContext 走 AIController/Weapon/Attrib，Move/Shoot/Pursue/Reload state 与 evaluator 不再直接依赖 Soldier；Blackboard Lua owner 已泛化，DeathState 与 AgentActionContext 动画表现入口已走 AnimComponent/IAnimContextProvider/IAnimController；WeaponComponent / AgentLocomotion / AIController 服务访问不再回退 `g_*`；RenderComponent / AnimComponent 已移除 `GameManager.h`。
- **P2 前置能力**：`BaseObject` 已向 Lua 暴露 `GetAIComponent()` / `GetWeaponComponent()` / `GetAnimComponent()` / `GetAttribComponent()`；`AIController`、`WeaponComponent`、`AnimComponent`、`AgentAttrib` 常用接口已可由 Lua 直取，agent 入口、`Sandbox10`-`Sandbox13`、`Sandbox17` 与 `parity_trace.lua` 通过 `AgentComponentAccess.lua` 统一走组件优先、旧对象方法兜底；RuntimeDiag `ComponentProbeAgent` 会创建非 Soldier `AgentObject` 并验证 AI/Attrib 组件复用。
- **P2 Lua 主路径收窄（2026-06-19）**：`AgentComponentAccess.lua` 的 Weapon/AI/MaxHealth/Ammo helper 已改为纯 typed component getter 路径；DT/BT 条件、射击/换弹 action、Chapter9 legacy agent、`SoldierAgent` 与 `Sandbox3` 不再调用 `SoldierObject` 的 getWeapon/ammo/maxHealth/AI 纯转发，`SoldierObject` 侧对应 Lua 导出已移除。
- **P5 Blackboard owner**：`Blackboard` 内部 owner 为 `AgentObject*`，Lua 已可通过 `GetAgentOwner()` 获取泛化 owner；旧 `GetOwner()` 保留为 `SoldierObject*` 兼容桥。
- 跨组件依赖走 `FindComponent<T>()`，勿调具体对象方法；服务走 `GetSandboxServices()`。
- 位置真源同步在 `RenderComponent::Update`（见 [[objects]]）。

## 6. 数据流 / 与其他模块关系

工厂装配组件（[[systems-service]]）；对象 Update 链驱动各组件；服务注入来自 [[core-object]] SandboxServices。

## 7. 验证策略

- 回归 sample：`Sandbox6/7/8/9/10/11/12/13/16/17` 与 `chapter9_tactics_legacy_parity`；RuntimeDiag `Sandbox2 -RuntimeDiag` 的 `[ComponentAccessSelfTest] result=true`；Lua 侧优先 `agent:GetAIComponent()` / `agent:GetAttribComponent()` 或 `AgentComponentAccess.lua`，C++ 侧优先 `owner->FindComponent<WeaponComponent>()->ShootBullet()`。

## 8. 已知 gap / 相关文档

- 待：P5 旧 Lua Soldier 签名兼容桥按 sample 节奏继续收窄、P4 其它缓存裸指针继续审计、P2 继续迁移 AgentObject/其它 Soldier legacy forwarder 与更多 Lua/sample 调用。`docs/design/architecture-improvement-plan.md` P2/P4/P5/P6、`docs/design/cpp-object-model-refactor-roadmap.md`。
