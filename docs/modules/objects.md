# 游戏对象（alias: objects）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

游戏对象层：Agent/Soldier/Block 对象 + OpenSteer 转向/路径适配，承载运动、朝向、转向力、路径跟踪。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/objects/`（动画子系统单列 [[objects-anim]]）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `AgentObject.{h,cpp}` | 基础对象 | cpp ~596 行；pure Locomotion C++ facade 已删除，保留位置/朝向/速度、血量等跨组件语义入口 |
| `SoldierObject.{h,cpp}` | 战斗特化 | cpp ~533 行，绑武器/动画/AI；组件顺序已交给 BaseObject/IComponent order，纯组件 C++ 转发已继续瘦身 |
| `BlockObject.{h,cpp}` | 静态块 | 刚体+渲染双组件，双重持有 |
| `steer/OpenSteerAdapter.{h,cpp}` | 转向 | OpenSteer AbstractVehicle 适配；部分基向量方法是空桩 |
| `steer/AgentPath.{h,cpp}` | 路径 | PolylinePathway 包装 |

## 4. 公开能力要点

- 对象层保留位置/朝向/速度、物理形体设置、血量与命令语义；转向力（ForceToPosition/AvoidAgents/Wander…）、路径设置与跟踪由 `AgentLocomotion` 承接。

## 5. 约束与红线

- **位置真源**（AGENTS.md）：有有效刚体 → PhysicsComponent 权威、RenderComponent 同步；无刚体 → RenderComponent 权威。对象层只触发同步，不手写 Bullet→SceneNode；视觉偏移用 `SetVisualOffset`。
- **P2/P5**：新增对象类型勿复制单组件 forwarder，新代码勿加对象层样板。RuntimeDiag `ComponentProbeAgent` 已通过 `SandboxObjects:CreateAgentWithProfile(..., "component_probe", ...)` 验证非 Soldier `AgentObject` 能复用 AI/Attrib/Weapon/Anim/Locomotion 等组件并通过 typed getter 访问，含 Locomotion maxSpeed/target/targetRadius round-trip、Anim body ASM、Weapon ammo round-trip 与 `ShootBullet()`；`NonSoldierAnimProbeAgent.lua` 通过 `animated_probe` 验证普通 `AgentObject` 可挂 animated mesh、配置 body ASM 与请求状态切换；agent 入口、`Sandbox2/3/5/6/10-13/16/17/18`、Chapter2/4 sample、DT/BT 条件/动作与 `parity_trace.lua` 已通过 `AgentComponentAccess.lua` 走组件优先。
- **P2 收窄（2026-06-20）**：`SoldierObject` Lua 导出已撤下 `getWeapon`、`GetAI/GetAIController`、`SetMaxHealth/GetMaxHealth`、`SetAmmo/GetAmmo/SetMaxAmmo/GetMaxAmmo/HasAmmo/ConsumeAmmo/RestoreAmmo`、`HasEnemy/CanShootEnemy/GetEnemy`、`ShootBullet`、`HasMovePosition/SetMovePosition/ClearMovePosition/IsTargetReached` 与 `Enter*Anim` 这批纯组件转发，对应 C++ 兼容转发也已删除；`AgentObject:getBody/GetAnimation/GetObjectASM` 也已撤出 Lua 导出；Lua 的 AI/Attrib/Weapon/Locomotion/敌人查询/移动目标/动画意图/body animation/body ASM 访问走 `BaseObject` typed component getter 或 `AgentComponentAccess.lua`；`AgentUtils.lua`、DT/BT `MoveHelpers.lua`、`MoveAction/PursueAction`、`ConfigManager`、Chapter2/4 sample 和 `Sandbox2/5/6/10-13/16/17/18` 的公共 movement/steering/target/path 主路径已转向 `AgentLocomotion` helper；`AgentObject` 的 pure Locomotion facade 已撤出 Lua 导出，target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` C++ facade 也已删除，AI/FSM/Soldier/ObjectManager 改为组件直读。`GetHealth/SetHealth`、位置/朝向/物理速度、物理形体设置等跨组件语义入口仍保留。
- **C4 完成 / P2 部分缓解**：SoldierObject 已不再手写 AI/Render/Anim/Weapon update block，组件顺序由 `IComponent::getUpdateOrder()` + `BaseObject::Update()` 驱动。
- **P1**：AgentObject / BlockObject 的战术事件、碰撞粒子清理和 FSM flag 读取只走 `SandboxServices`；SoldierObject 通过 `SandboxServices.input` 获取输入；这些对象文件不再直接 include `GameManager.h` 或回退 `g_*`。
- **P4（核心完成）**：BaseObject 组件容器已 `unique_ptr` 化，组件 lifecycle 断言/dump 已落地；BlockObject 与 AgentObject 的缓存组件裸成员、OpenSteerAdapter owner 已标注 non-owning，并在生命周期尾部清空以避免误判为拥有关系；SoldierObject input 指针标注为自身 owning。
- OpenSteerAdapter 多个转向辅助方法是空桩（疑未用）。

## 6. 数据流 / 与其他模块关系

对象持 [[components]]；组件更新顺序由 `BaseObject::Update` + `IComponent::getUpdateOrder` 驱动（[[objects-anim]] [[systems-physics]]）；服务经 `GetSandboxServices`（[[core-object]]）。

## 7. 验证策略

- 回归 sample：`Sandbox6/7/8`（战斗）；对象创建销毁看 `buildObjectDebugSummary`。

## 8. 已知 gap / 相关文档

- 待：P2 继续审计其它对象门面和跨组件语义入口、P4 其它缓存裸指针继续审计。`docs/design/architecture-improvement-plan.md` P2/P4、`docs/design/cpp-object-model-refactor-roadmap.md`、`docs/archive/SoldierObjectRefactor.md`。
