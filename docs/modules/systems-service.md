# 系统服务（alias: systems-service）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

对象/资源工厂集群 + 相机/UI 服务：构造与装配 Agent/Soldier/Block/Bullet、刚体、场景节点、相机。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/service/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ObjectFactory.{h,cpp}` | 工厂 | CreatePlane/Block/Bullet/Agent/Soldier |
| `AgentFactory.{h,cpp}` | 装配 | `AttachAgentComponents` 组件装配（硬编码） |
| `SoldierFactory.{h,cpp}` | 装配 | Soldier 专化（含刚体+脚本） |
| `PhysicsFactory.{h,cpp}` | 工厂 | 刚体/形状，见 [[systems-physics]] |
| `SceneFactory.{h,cpp}` | 工厂 | Ogre SceneNode/ManualObject |
| `CameraService.{h,cpp}` | 服务 | 相机创建/控制 |
| `UIService.{h,cpp}` | 服务 | 纯空壳转发 UIManager（C1） |

## 4. 公开能力要点

- 对象工厂链 ObjectFactory→AgentFactory/SoldierFactory 分层装配组件。

## 5. 约束与红线

- **P5**：AgentFactory 组件装配硬编码（新组件改代码，无装配表）；SoldierFactory 绑死 SoldierObject（新 NPC 需复制）。
- **C1**：UIService 纯空壳转发，应直连 UIManager。
- CameraService 与 [[runtime]] OgreCameraController 耦合。

## 6. 数据流 / 与其他模块关系

`SandboxMgr/Lua → ObjectFactory.CreateSoldier → SoldierFactory 装配 [[components]] → ObjectManager 注册`；组件接收 SandboxServices。

## 7. 验证策略

- 回归 sample：`Sandbox1`(创建)、`Sandbox6`、`Sandbox16`(批量压力)。

## 8. 已知 gap / 相关文档

- 待：装配表驱动（P5）、删 UIService 空壳（C1）。`docs/design/architecture-improvement-plan.md` P5/C1。
