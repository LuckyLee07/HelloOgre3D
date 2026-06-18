# 系统服务（alias: systems-service）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

对象/资源工厂集群 + 相机服务：构造与装配 Agent/Soldier/Block/Bullet、刚体、场景节点、相机。

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

## 4. 公开能力要点

- 对象工厂链 ObjectFactory→AgentFactory/SoldierFactory 分层装配组件。
- `ObjectFactory` 已导出给 Lua 全局 `SandboxObjects`，对象创建不再通过 `SandboxMgr` 纯转发；`SandboxServices.objectFactory` 供组件侧创建 bullet 等运行时对象。

## 5. 约束与红线

- **P5**：AgentFactory 组件装配硬编码（新组件改代码，无装配表）；SoldierFactory 绑死 SoldierObject（新 NPC 需复制）。
- **C1/C2 进展**：UIService 空壳已删除；UIManager 由应用层构造并导出为 Lua 全局 `SandboxUI`，Gorilla UI frame/color API 不再通过 SandboxMgr 转发；CameraService 由 GameManager 持有并导出为 Lua 全局 `SandboxCamera`，相机/profile 查询不再通过 SandboxMgr 转发。
- SceneFactory 不应 include `GameManager.h`；root scene node 通过 `SetRootSceneNode` 注入。
- UIManager 不应 include `GameManager.h`；camera 通过构造注入。
- CameraService 不应 include `ClientManager.h`；profile time 通过构造注入的 getter 查询。

## 6. 数据流 / 与其他模块关系

`Lua SandboxObjects → ObjectFactory.CreateSoldier → SoldierFactory 装配 [[components]] → ObjectManager 注册`；组件接收 SandboxServices。

## 7. 验证策略

- 回归 sample：`Sandbox1`(创建)、`Sandbox6`、`Sandbox16`(批量压力)。

## 8. 已知 gap / 相关文档

- 待：装配表驱动（P5）。`docs/design/architecture-improvement-plan.md` P5。
