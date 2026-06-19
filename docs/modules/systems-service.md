# 系统服务（alias: systems-service）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

对象/资源工厂集群 + 相机/导航/raycast/场景/脚本服务：构造与装配 Agent/Soldier/Block/Bullet、刚体、场景节点、相机，并承接导航配置/构建/查询、物理 raycast、场景光照与脚本加载门面。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/service/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ObjectFactory.{h,cpp}` | 工厂 | CreatePlane/Block/Bullet/Agent/Soldier |
| `AgentFactory.{h,cpp}` | 装配 | `AttachAgentComponents` 组件装配（硬编码） |
| `SoldierFactory.{h,cpp}` | 装配 | Soldier 专化（含刚体+脚本） |
| `PhysicsFactory.{h,cpp}` | 工厂 | 刚体/形状，见 [[systems-physics]] |
| `SceneFactory.{h,cpp}` | 工厂 | Ogre SceneNode/ManualObject；root scene node 由 `GameManager::Initialize` 注入 |
| `CameraService.{h,cpp}` | 服务 | 相机/profile 查询 facade；camera / scene manager / profile time getter 由应用层注入，Lua 全局 `SandboxCamera` 访问 |
| `NavigationService.{h,cpp}` | 服务 | Recast config 默认值/agent 设置、navmesh 构建、按 name 持有 navmesh map 与 `RandomPoint`/`FindClosestPoint`/`FindPath` 查询；ObjectManager 由应用层注入用于读取 fixed blocks，Lua 全局 `SandboxNav` 访问 |
| `RaycastService.{h,cpp}` | 服务 | 物理 raycast facade；PhysicsWorld 由应用层注入，Lua 全局 `SandboxRaycast` 访问；`SandboxMgr::RayCastObjectId` 仅保留兼容转发 |
| `SceneService.{h,cpp}` | 服务 | skybox、ambient light、directional light、material 与 scene graph 更新 facade；SceneManager / CameraService 由应用层注入，Lua 全局 `SandboxScene` 访问 |
| `ScriptService.{h,cpp}` | 服务 | 脚本文件加载 facade；ScriptLuaVM 由应用层注入，Lua 全局 `SandboxScript` 访问 |

## 4. 公开能力要点

- 对象工厂链 ObjectFactory→AgentFactory/SoldierFactory 分层装配组件。
- `ObjectFactory` 已导出给 Lua 全局 `SandboxObjects`，对象创建不再通过 `SandboxMgr` 纯转发；`SandboxServices.objectFactory` 供组件侧创建 bullet 等运行时对象。
- `NavigationService` 已导出给 Lua 全局 `SandboxNav`，导航配置/构建/查询和 navmesh 所有权不再通过 `SandboxMgr` / `ObjectManager` 主路径；`SandboxServices.navigation` 供 AI/FSM/感知侧查询路径和随机点。
- `RaycastService` 已导出给 Lua 全局 `SandboxRaycast`，raycast 不再由 `SandboxMgr` 直接访问 `ObjectManager`/`PhysicsWorld`；`SandboxServices.raycast` 供后续 C++ 侧查询。
- `SceneService` 已导出给 Lua 全局 `SandboxScene`，skybox/light/material/scene graph 更新不再通过 `SandboxMgr` 纯转发；`SandboxServices.scene` 供后续 C++ 场景门面收口。
- `ScriptService` 已导出给 Lua 全局 `SandboxScript`，旧 sample 的 `CallFile` 不再通过 `SandboxMgr` 纯转发；`SandboxServices.scriptService` 供后续 C++ 脚本门面收口。

## 5. 约束与红线

- **P5**：AgentFactory 组件装配硬编码（新组件改代码，无装配表）；SoldierFactory 绑死 SoldierObject（新 NPC 需复制）。
- **C1/C2 进展**：UIService 空壳已删除；UIManager 由应用层构造并导出为 Lua 全局 `SandboxUI`，Gorilla UI frame/color API 不再通过 SandboxMgr 转发；CameraService 由 GameManager 持有并导出为 Lua 全局 `SandboxCamera`，相机/profile 查询不再通过 SandboxMgr 转发；NavigationService 由 GameManager 持有并导出为 Lua 全局 `SandboxNav`，导航配置/构建/查询不再通过 SandboxMgr 转发；RaycastService 由 GameManager 持有并导出为 Lua 全局 `SandboxRaycast`，raycast 不再由 SandboxMgr 实现；SceneService 由 GameManager 持有并导出为 Lua 全局 `SandboxScene`，场景/light/material API 不再通过 SandboxMgr 纯转发；ScriptService 由 GameManager 持有并导出为 Lua 全局 `SandboxScript`，CallFile 不再通过 SandboxMgr 纯转发。SandboxMgr 只保留 CppFSM flag 与旧 Lua 兼容转发。
- SceneFactory 不应 include `GameManager.h`；root scene node 通过 `SetRootSceneNode` 注入。
- UIManager 不应 include `GameManager.h`；camera 通过构造注入。
- CameraService 不应 include `ClientManager.h`；profile time 通过构造注入的 getter 查询。
- NavigationService 不应 include `GameManager.h` / `ClientManager.h`；通过构造注入 `ObjectManager*` 读取 fixed blocks，navmesh 所有权由 NavigationService 按 name 管理。
- RaycastService 不应 include `GameManager.h` / `ClientManager.h` / `ObjectManager.h`；通过构造注入 `PhysicsWorld*`。
- SceneService 不应 include `GameManager.h` / `ClientManager.h`；通过构造注入 `SceneManager*` 和 `CameraService*`。
- ScriptService 不应 include `GameManager.h` / `ClientManager.h`；通过构造注入 `ScriptLuaVM*`。

## 6. 数据流 / 与其他模块关系

`Lua SandboxObjects → ObjectFactory.CreateSoldier → SoldierFactory 装配 [[components]] → ObjectManager 注册`；组件接收 SandboxServices。`Lua SandboxNav → NavigationService.CreateNavigationMesh → ObjectManager.getFixedObjects → NavigationMesh`，后续查询直接走 `NavigationService.GetNavigationMesh(name)`。`Lua SandboxRaycast → RaycastService → PhysicsWorld.rayCastToRigidBody → objectId`。`Lua SandboxScene → SceneService → Ogre SceneManager/SceneNode`。`Lua SandboxScript → ScriptService → ScriptLuaVM.callFile`。

## 7. 验证策略

- 回归 sample：`Sandbox1`(场景/创建)、`Sandbox3`(CallFile/动画)、`Sandbox6`(导航/FSM)、`Sandbox13`(战术导航查询)、`Sandbox16`(批量压力)。

## 8. 已知 gap / 相关文档

- 待：装配表驱动（P5）。`docs/design/architecture-improvement-plan.md` P5。
