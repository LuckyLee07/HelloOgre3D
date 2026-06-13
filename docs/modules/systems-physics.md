# 物理系统（alias: systems-physics）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

Bullet 物理世界、刚体生命周期、碰撞检测、raycast；为对象提供刚体物理与碰撞响应。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/physics/`、`src/HelloOgre3D/sandbox/components/physics/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `PhysicsWorld.{h,cpp}` | 世界 | btDiscreteDynamicsWorld 包装；stepWorld/碰撞/raycast |
| `Collision.h` | 数据 | 碰撞点/法线 |
| `components/physics/PhysicsComponent.{h,cpp}` | 组件 | 持 btRigidBody；注册/反注册到 PhysicsWorld；ApplyForce/SetPosition/RebuildCapsule |
| `systems/service/PhysicsFactory.{h,cpp}` | 工厂 | 刚体/形状创建 |

## 4. 公开能力要点

- stepWorld 仿真、addRigidBody/removeRigidBody、checkCollision、rayCast；力经 Locomotion→ApplyForce→stepWorld。

## 5. 约束与红线

- **位置真源**：有刚体则 Bullet 权威、RenderComponent 从刚体同步（勿直改 scenenode）。
- PhysicsComponent 持的 btRigidBody 由 PhysicsFactory 建、PhysicsWorld 拥有（非组件拥有）。
- **PhysicsFactory 复合形状当前只支持单个子形状**（多形状代码注释掉）。
- 碰撞经 `BaseObject::CollideWithObject` 虚回调分发。

## 6. 数据流 / 与其他模块关系

`SoldierFactory → PhysicsFactory.CreateCapsule → PhysicsComponent → PhysicsWorld.addRigidBody`；每帧 `Locomotion.ApplyForce → stepWorld → RenderComponent 同步`。关联 [[components]] [[objects]] [[systems-service]]。

## 7. 验证策略

- 回归 sample：`Sandbox1`（射击/受力）、`Sandbox8`（碰撞特效）；无穿透/浮起。

## 8. 已知 gap / 相关文档

- 待：PhysicsFactory 多形状复合、位置真源代码落实校验。`docs/design/architecture-improvement-plan.md` P1。
