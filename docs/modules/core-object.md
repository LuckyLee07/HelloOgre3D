# 对象核心基础（alias: core-object）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

对象/组件运行时基础：对象根、组件容器、事件总线、服务注入聚合体。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/core/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `object/BaseObject.{h,cpp}` | 对象根 | 组件容器 `std::map<string,unique_ptr<IComponent>>` + 组件生命周期断言/dump + objid/teamid + `SetSandboxServices` |
| `object/SandboxObject.{h,cpp}` | 基类 | 虚 Update |
| `component/IComponent.{h,cpp}` | 组件接口 | onAttach/onDetach/update + `GetSandboxServices()` + `onSandboxServicesChanged()` |
| `SandboxServices.h` | 注入聚合 | `{objects, physics, input, script, objectFactory, agentConfig, navigation, raycast, scene, scriptService}`（P1 核心） |
| `event/SandboxEventDispatcher*.h` | 事件 | name→dispatcher / Subscribe/Emit/Token |
| `event/SandboxContext.h` | payload | 事件上下文（勿与 SandboxServices 撞名） |
| `SandboxMacros.h` | 宏 | SAFE_DELETE |

## 4. 公开能力要点

- 对象生命周期 + 组件容器（AddComponent/FindComponent<T>/GetComponent(key)）+ `BaseObject::Update` 按 `IComponent::getUpdateOrder()` 驱动组件 + 服务注入/变更通知 + 事件总线。

## 5. 约束与红线

- **P4（核心完成）**：组件容器已迁到 `std::unique_ptr<IComponent>` 并由 `BaseObject` 统一销毁；`IComponent` 记录 lifecycle state，`BaseObject` 在 attach/destroy/update 做断言，`BuildComponentDebugString()` 会输出组件状态；BlockObject/AnimComponent/AgentObject/LuaScriptComponent/PhysicsComponent/AIController/OpenSteerAdapter 关键裸指针已标注 owning/non-owning，Lua env owner 与 AI cached enemy 会在 detach 清空。
- **P1（已解决）**：SandboxServices 已落地下发并通知组件，`tools/check_sandbox_architecture.ps1` 已加静态门禁；Physics/Weapon/AI/Locomotion/FSM/AgentObject/BlockObject/SoldierObject 热点已移除 `g_*`/`GameManager` 兜底，Render/Anim/Input include 已清，AgentConfig/Raycast/Scene/Script 等门面已由应用层注入为 `AgentConfigService` / `RaycastService` / `SceneService` / `ScriptService`，SceneFactory root node / UIManager camera / CameraService / InputManager 依赖已由应用层注入，NavigationMesh/ObjectManager 场景访问已走 SceneFactory，manager 层 `g_ObjectManager`/`g_SandboxMgr` 已删除。
- 事件总线目前同步直发，无缓冲队列/节流。

## 6. 数据流 / 与其他模块关系

`ObjectManager 填 SandboxServices → BaseObject.SetSandboxServices → IComponent.onSandboxServicesChanged / GetSandboxServices`；对象创建链（[[systems-service]]）透传。被所有对象/组件依赖。

## 7. 验证策略

- 回归 sample：`Sandbox6`；对象创建销毁无泄漏/野指针（`buildObjectDebugSummary`）。

## 8. 已知 gap / 相关文档

- 待：P4 其它缓存裸指针继续审计。`docs/design/architecture-improvement-plan.md` P4、`docs/design/cpp-object-model-refactor-roadmap.md`。
