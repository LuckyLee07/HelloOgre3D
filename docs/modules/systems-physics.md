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
| `components/physics/PhysicsComponent.{h,cpp}` | 组件 | 持 btRigidBody；通过 SandboxServices 注册/反注册到 PhysicsWorld；ApplyForce/SetPosition/RebuildCapsule |
| `systems/service/PhysicsFactory.{h,cpp}` | 工厂 | 刚体/形状创建 |

## 4. 公开能力要点

- stepWorld 仿真、addRigidBody/removeRigidBody、checkCollision、rayCast；力经 Locomotion→ApplyForce→stepWorld。

## 5. 约束与红线

- **位置真源**：有刚体则 Bullet 权威、RenderComponent 从刚体同步（勿直改 scenenode）。
- PhysicsComponent 通过 `IComponent::onSandboxServicesChanged` 延迟接入 PhysicsWorld，不再直接回退到 `g_GameManager`。
- PhysicsFactory 创建刚体；PhysicsComponent 析构先从 PhysicsWorld 注销，再删除 motion state、顶层 collision shape 与 body。PhysicsWorld 只注册/模拟，不接管组件的刚体。mesh 工厂的局部 MeshHullCompoundShape 用 unique_ptr 独占所创建的 hull，顶层虚析构随之回收；Bullet 通用 child 仍是借用关系，不递归删除其他 shared/borrowed child。
- **PhysicsFactory 的 mesh 复合形状当前只装一个 identity 子形状**，不做质心偏移。
- 碰撞经 `BaseObject::CollideWithObject` 虚回调分发。

## 6. 数据流 / 与其他模块关系

`SoldierFactory → PhysicsFactory.CreateCapsule → PhysicsComponent → SandboxServices.physics → PhysicsWorld.addRigidBody`；每帧 `Locomotion.ApplyForce → stepWorld → RenderComponent 同步`。关联 [[components]] [[objects]] [[systems-service]]。

## 7. 验证策略

- 回归 sample：`Sandbox1`（射击/受力）、`Sandbox8`（碰撞特效）；无穿透/浮起。

## 8. 已知 gap / 相关文档

- 待：PhysicsFactory 多形状复合、位置真源代码落实校验。`docs/design/architecture-improvement-plan.md` P1。

## 2026-09-12 跟随镜头遮挡

`PhysicsWorld::sweepCamera` 为 C++ CameraService 提供球体扫掠，只接受有碰撞响应的静态/运动学对象；角色和动态子弹不缩短镜头。由 RaycastService 转换 Ogre/Bullet 向量，镜头立即缩距、平滑复位，查询不改变刚体或导航。RenderComponent 在仿真后记录前后物理姿态，只向场景节点输出插值；刚体仍为真源。

## 2026-09-19 凸包创建与切关耗时

`PhysicsFactory::CreateSimplifiedConvexHull` 将 Ogre 顶点转换到对齐的 Bullet 数组，源凸包与简化凸包均使用批量构造器。本仓 Bullet 的 `addPoint` 每次完整重算 AABB，逐点构造随点数平方增长；批量构造消除这段重复扫描，不缓存/共享可变碰撞形状，不改顶点、简化算法和刚体所有权。

源壳继续先 `setMargin(.01)`、`setSafeMargin(.01)`（有效 .001）再计算非空 AABB；最终壳继续先构造默认 margin 的 AABB，再 `setMargin(.01)`，不可顺手重算导致现有边界改变。性能及行为证据见[切关响应优化](../dev-design/plans/2026-09-19-crossfire-transition-latency.md)。
