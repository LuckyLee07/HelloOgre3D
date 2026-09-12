# 系统服务（alias: systems-service）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

对象/资源工厂集群 + agent 配置/相机/导航/raycast/场景/脚本服务：构造与装配 Agent/Soldier/Block/Bullet、刚体、场景节点、相机，并承接 AI 样例配置、导航配置/构建/查询、物理 raycast、场景光照与脚本加载门面。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/service/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ObjectFactory.{h,cpp}` | 工厂 | CreatePlane/VisualPlane/Block/Bullet/Agent/AgentWithProfile/Soldier/SoldierWithProfile |
| `AgentFactory.{h,cpp}` | 装配 | 通过 `default` / `component_probe` / `movement_only` / `animated_probe` 轻量 profile 装配普通 Agent 的 locomotion/physics/script/render + AI/Attrib/Weapon/Anim |
| `SoldierFactory.{h,cpp}` | 装配 | Soldier 专化；`ai_soldier` / `player_soldier` / `commander_soldier` profile 互斥装配 AI/玩家 controller，并允许武器成为可选组件 |
| `PhysicsFactory.{h,cpp}` | 工厂 | 刚体/形状，见 [[systems-physics]] |
| `SceneFactory.{h,cpp}` | 工厂 | Ogre SceneNode/ManualObject；root scene node 由 `GameManager::Initialize` 注入 |
| `AgentConfigService.{h,cpp}` | 服务 | CppFSM flag；Lua 全局 `SandboxAgentConfig` 访问，`AgentObject` 通过 `SandboxServices.agentConfig` 读取 |
| `CameraService.{h,cpp}` | 服务 | 相机/profile 查询 facade；提供 C++ 侧世界位移平移入口，并新增第三人称 FOLLOW 门面 `EnterFollowMode`/`ExitFollowMode`/`UpdateFollow`（**非 tolua**，转发到注入的 `OgreCameraController`，供 `PlayerController` 驱动 Sandbox19 第三人称跟随）；camera / scene manager / **OgreCameraController** / profile time getter 由 GameManager 注入（均 non-owning），Lua 全局 `SandboxCamera` 访问 |
| `NavigationService.{h,cpp}` | 服务 | Recast config 默认值/agent 设置、navmesh 构建、按 name 以 `unique_ptr` 持有 navmesh map 与 `RandomPoint`/`FindClosestPoint`/`FindPath` 查询；ObjectManager 由应用层注入用于读取 fixed blocks，Lua 全局 `SandboxNav` 访问 |
| `RaycastService.{h,cpp}` | 服务 | 物理 raycast facade；PhysicsWorld 由应用层注入，Lua 全局 `SandboxRaycast` 访问 |
| `SceneService.{h,cpp}` | 服务 | skybox、ambient light、directional light、material、scene compositor 与 scene graph 更新 facade；SceneManager / CameraService 由应用层注入，Lua 全局 `SandboxScene` 访问 |
| `ScriptService.{h,cpp}` | 服务 | 脚本文件加载 facade；ScriptLuaVM 由应用层注入，Lua 全局 `SandboxScript` 访问 |

## 4. 公开能力要点

- `SandboxObjects:RequestDestroyAgent(objId)` 对有效 agent 标记延迟清理，由 ObjectLifecycleSystem 删除；缺失、非 agent、非正 id 返回 false。返回 true 只表示已接受请求，Lua 不获得所有权，也不可据此继续保存裸 userdata；跨帧清理回调只保存 id。RuntimeDiag 用该入口回收临时 probe。

- 对象工厂链 ObjectFactory→AgentFactory/SoldierFactory 分层装配组件。
- `ObjectFactory` 已导出给 Lua 全局 `SandboxObjects`，对象创建不再通过 `SandboxMgr` 纯转发；`CreateAgentWithProfile` 可按命名 profile 创建普通 `AgentObject`，`CreateSoldierWithProfile` 通过 `ai_soldier` / `player_soldier` / `commander_soldier` 选择互斥 controller 与可选武器；`SandboxServices.objectFactory` 供组件侧创建 bullet 等运行时对象。
- `CreateVisualPlane(width, height)` 创建 ObjectManager 持有的 `BlockObject` 与 Ogre plane/entity，但不创建 Bullet 刚体；`OBJ_TYPE_PLANE` 使其不进入 navmesh 固定几何，无刚体使其不进入物理射线和 AI 视线阻挡。Lua 返回值是 non-owning userdata，Sandbox19 用它承载地表旧化、接触阴影与 crossed-card 植被。
- `AgentConfigService` 已导出给 Lua 全局 `SandboxAgentConfig`，CppFSM flag 不再由 `SandboxMgr` 持有；`SandboxServices.agentConfig` 供 `AgentObject` 读取。
- `NavigationService` 已导出给 Lua 全局 `SandboxNav`，导航配置/构建/查询和 navmesh 所有权不再通过 `SandboxMgr` / `ObjectManager` 主路径；`SandboxServices.navigation` 供 AI/FSM/感知侧查询路径和随机点。
- `RaycastService` 已导出给 Lua 全局 `SandboxRaycast`，raycast 不再由 `SandboxMgr` 直接访问 `ObjectManager`/`PhysicsWorld`；`SandboxServices.raycast` 供后续 C++ 侧查询。
- `SceneService` 已导出给 Lua 全局 `SandboxScene`，skybox/light/material/scene graph 更新不再通过 `SandboxMgr` 纯转发；`SetCompositorEnabled(name, enabled)` 按当前 camera viewport 幂等查找/添加并切换 compositor，缺资源返回 false 并写日志；`SandboxServices.scene` 供后续 C++ 场景门面收口。
- `ScriptService` 已导出给 Lua 全局 `SandboxScript`，旧 sample 的 `CallFile` 不再通过 `SandboxMgr` 纯转发；`SandboxServices.scriptService` 供后续 C++ 脚本门面收口。

- `CreateBlockBox` 按 PlaneGenerator 实际面轴分配尺寸，避免旧 BoxGenerator 在 ±X 面交换高度/深度；Bullet 继续使用原输入尺寸。`HELLO_SANDBOX_SMOKE_TEST=1` 检查实际 mesh bounds（扣除 Ogre padding），smoke 要求至少一个非等边盒体通过。
- `SandboxScene:ConfigureDirectionalShadows(light, enabled)` 转发 runtime 阴影适配，返回配置成功布尔值，Light 与渲染目标均不向 Lua 转移所有权；无 callback/ref 或持久裸指针。头文件、`.pkg` 的 `$cfile` 引用与局部 tolua 绑定同步；不全量运行生成器。资源缺失/配置异常返回 false，首次渲染仍需真实窗口验证。

## 5. 约束与红线

- **P5**：AgentFactory 已给普通 Agent 默认装配 AI/Attrib/Weapon/Anim 这组可复用运行组件，并新增轻量 profile 表；RuntimeDiag 通过 `component_probe` profile 覆盖非 Soldier `anim`/`bodyAsm` 与武器组件，通过 `animated_probe` 覆盖普通 `AgentObject` 挂 animated mesh、配置 body ASM 并请求状态切换；SoldierFactory 已支持 `ai_soldier` / `player_soldier` / `commander_soldier` 控制 profile，后者证明武器可从 Soldier 装配中拿掉；profile 仍是 C++ 内置表且对象类型仍绑死 `SoldierObject`，新 NPC 泛化仍待推进。
- **C1/C2 进展**：UIService 空壳已删除；UIManager 由应用层构造并导出为 Lua 全局 `SandboxUI`，Gorilla UI frame/color API 不再通过 SandboxMgr 转发（2026-07-11 另加 `CreatePolygon`→`UIPolygon` 封装 `Gorilla::Polygon` 矢量多边形，供 Sandbox19 圆盘雷达画浅蓝圆盘/圆点 blip/三角箭头，手术式补 tolua 绑定、C++ 持所有权）；AgentConfigService 由 GameManager 持有并导出为 Lua 全局 `SandboxAgentConfig`，CppFSM flag 不再由 SandboxMgr 持有；CameraService 由 GameManager 持有并导出为 Lua 全局 `SandboxCamera`，相机/profile 查询不再通过 SandboxMgr 转发；NavigationService 由 GameManager 持有并导出为 Lua 全局 `SandboxNav`，导航配置/构建/查询不再通过 SandboxMgr 转发；RaycastService 由 GameManager 持有并导出为 Lua 全局 `SandboxRaycast`，raycast 不再由 SandboxMgr 实现；SceneService 由 GameManager 持有并导出为 Lua 全局 `SandboxScene`，场景/light/material API 不再通过 SandboxMgr 纯转发；ScriptService 由 GameManager 持有并导出为 Lua 全局 `SandboxScript`，CallFile 不再通过 SandboxMgr 纯转发。SandboxMgr class/global 已删除。
- SceneFactory 不应 include `GameManager.h`；root scene node 通过 `SetRootSceneNode` 注入。
- UIManager 不应 include `GameManager.h`；camera 通过构造注入。
- CameraService 不应 include `ClientManager.h`；profile time 通过构造注入的 getter 查询。
- CameraService 屏幕坐标互转（2026-08-04 加，供 Sandbox19 指挥切片做单位点选 / 框选 / 地面点选）：
  `WorldToScreen(world)` 返回屏幕像素，**相机后方或无 viewport 时返回 (-1,-1)**，调用方必须据此丢弃；
  `ScreenToGroundPoint(x, y, groundY)` 与水平面求交，**射线与平面平行/背离时返回相机位置**兜底、不返回 NaN。
  两者均按手术式补 tolua 绑定，未跑 `tolua.bat` 全量重生成。
- NavigationService 不应 include `GameManager.h` / `ClientManager.h`；通过构造注入 `ObjectManager*` 读取 fixed blocks，navmesh 所有权由 NavigationService 的 `unique_ptr` map 按 name 管理；`AddNavigationMesh` 接收历史 raw pointer 后立即接管所有权。
- RaycastService 不应 include `GameManager.h` / `ClientManager.h` / `ObjectManager.h`；通过构造注入 `PhysicsWorld*`。
- SceneService 不应 include `GameManager.h` / `ClientManager.h`；通过构造注入 `SceneManager*` 和 `CameraService*`。
- ScriptService 不应 include `GameManager.h` / `ClientManager.h`；通过构造注入 `ScriptLuaVM*`。
- P4 owning/non-owning 审计：`ObjectFactory::m_objectManager`、`CameraService::m_camera/m_sceneManager`、`RaycastService::m_physicsWorld`、`SceneService::m_sceneManager/m_cameraService`、`ScriptService::m_scriptVM` 都是注入的 non-owning 缓存指针。

## 6. 数据流 / 与其他模块关系

`Lua SandboxObjects → ObjectFactory.CreateAgentWithProfile/CreateSoldierWithProfile → AgentFactory / SoldierFactory profile 装配 [[components]] → ObjectManager 注册`；组件接收 SandboxServices。`Lua SandboxAgentConfig → AgentConfigService.SetUseCppFsmFlag → AgentObject.GetUseCppFSM`。`Lua SandboxNav → NavigationService.CreateNavigationMesh → ObjectManager.getFixedObjects → NavigationMesh`，后续查询直接走 `NavigationService.GetNavigationMesh(name)`。`Lua SandboxRaycast → RaycastService → PhysicsWorld.rayCastToRigidBody → objectId`。`Lua SandboxScene → SceneService → Ogre SceneManager/SceneNode`。`Lua SandboxScript → ScriptService → ScriptLuaVM.callFile`。

## 7. 验证策略

- 回归 sample：`Sandbox1`(场景/创建)、`Sandbox3`(CallFile/动画)、`Sandbox6`(导航/FSM)、`Sandbox13`(战术导航查询)、`Sandbox16`(批量压力)、`Sandbox19`(Soldier controller profile)。

## 8. 已知 gap / 相关文档

- 待：Agent profile 外部数据化、更完整非 Soldier 行为场景与 SoldierFactory 泛化（P5）。`docs/design/architecture-improvement-plan.md` P5。

## 2026-09-12 镜头与发弹边界

CameraService 新增 C++ 专用 `BeginFollowOrbit` / `EndFollowOrbit` / `DragFollowOrbit` / `GetFollowForward` / `SetFollowTurnInput` / `RenderFollow`，通过注入的 RaycastService 做静态球扫掠，不增加 Lua 指针所有权。RenderFollow 保留原视线方向，只改变距离；不再对主动偏航施加位置弹簧。

SceneFactory 创建粒子仍交给 SceneManager/现有延迟节点清理管理；`HELLO_RENDER_PARTICLES=0` 只跳过效果创建，调用方必须判空，物理弹、命中和伤害继续。BlockObject 用真实接触点/法线创建反馈：Relay 的 Ground/MainRoute/Concrete/ConcreteShade/Cover 用 Dust，其余表面含装甲用 Spark；这是有限材质覆盖，未实现通用表面标签系统。

玩家发弹沿发射当帧的 `b_muzzle` 三维朝向，不再用独立水平向量覆盖枪口；查询前求值当前骨骼并同步手部挂点，显示插值后也同步武器。48m/s 物理弹保持重力与碰撞；AI 的 `ShootBulletAt` 仍按目标点瞄准。验证见[体验修复](../dev-design/plans/2026-09-12-sandbox19-experience-fixes.md)。

FOLLOW 的镜头视线方向以 OgreCameraController 为唯一真源；活跃玩法的鼠标相对位移直接经 `CameraService::RotateFollowView` 转镜，Q/E 仍按渲染帧时长推进，中键拖动只在临时指针模式使用。PlayerController 持续从 `GetFollowForward` 取得水平瞄准方向并让身体追随，W/S 读取本帧刚体朝向、A/D 相对身体侧移。UpdateFollow 不把身体方向回写镜头，SnapFollowTarget 仍可显式设置初始方向。ResetFollowCamera/禁用相对移动清理转向速率，暂停由 PlayerController::ResetTransientInput 清理。上述增量接口均为 C++ 专用，无 Lua 绑定变更。
