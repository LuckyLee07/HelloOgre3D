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

- `IComponent`：onAttach/onDetach/update；`getOwner`/`FindComponent<T>()`/`GetSandboxServices()`。
- 各组件 public API（ShootBullet/ApplyForce/GetEntity/...）。

## 5. 约束与红线

- **P6 完成**：ComponentKeys 常量化，C++ 内无散落字符串 key。
- **P4**：容器仍 `std::map<string,IComponent*>` 裸指针 + SAFE_DELETE。
- **P5**：AnimComponent/WeaponComponent 仍持 `SoldierObject* owner`（WeaponComponent 跨组件访问已改 `FindComponent<RenderComponent>`）。
- 跨组件依赖走 `FindComponent<T>()`，勿调具体对象方法；服务走 `GetSandboxServices()`。
- 位置真源同步在 `RenderComponent::Update`（见 [[objects]]）。

## 6. 数据流 / 与其他模块关系

工厂装配组件（[[systems-service]]）；对象 Update 链驱动各组件；服务注入来自 [[core-object]] SandboxServices。

## 7. 验证策略

- 回归 sample：`Sandbox6/7/8`；`soldier:FindComponent<WeaponComponent>():ShootBullet()`。

## 8. 已知 gap / 相关文档

- 待：P5 owner 泛化、P4 unique_ptr、P2 减负。`docs/design/architecture-improvement-plan.md` P2/P4/P5/P6、`docs/design/cpp-object-model-refactor-roadmap.md`。
