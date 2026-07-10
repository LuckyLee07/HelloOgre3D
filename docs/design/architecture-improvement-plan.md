# 架构优化实施方案（基于代码现状评审）

> 上位：方向定调与跨模块整体优先级（L0–L4）以 `docs/project-direction.md`（北极星）为准；本文的 P1（解耦）/ P2 / P4 / P5（组件化）/ P7（感知）对应北极星 L0–L2 的落地实施。
>
> 来源：这份文档来自一次**不依赖既有路线图、直接通读 C++ / Lua 代码**的架构评审，目标是把发现的问题和可落地的解决方案固化下来，形成可按优先级执行、可验证、可回退的实施清单。
>
> 边界：本文聚焦“当前代码已经存在的结构性问题”，给出具体证据（file:line）、解决方案和验收标准。它与现有路线图互补，不替代：
> - 对象模型分阶段路线、术语定义见 `docs/design/cpp-object-model-refactor-roadmap.md`（本文是它的“证据 + 可执行”补充）。
> - 非 AI 主线优先级见 `docs/archive/project-roadmap.md`。
> - AI 能力路线见 `docs/planning/ai-roadmap.md`。
>
> 执行约束（继承 `AGENTS.md`）：保持 sample 行为稳定；优先小步、可验证、可回退；迁移文件先留 forwarding，确认引用迁完再删；不为重构一次性改 Lua 调用名；C/C++ 源文件保持 Tab 缩进与 CRLF。

> 🔁 复核状态（2026-06-11，逐条对代码核实）：
> - 🟢 **已解决**：P1（SandboxServices 注入已落地；sandbox/runtime 对 `GameManager.h` / `ClientManager.h` 反向 include 与 `g_ObjectManager` / `g_SandboxMgr` / `g_GameManager` 直接引用已清零，并由 `tools/check_sandbox_architecture.ps1` 守住）、P3（`GameManager` 的 FGUI 转发壳已删除，Lua 侧只走 `FairyGuiRuntime`）、P6（ComponentKeys.h 常量化）、C1（`UIService` 空壳已删除，Gorilla UI Lua 入口已从 `SandboxMgr` 下沉到 `SandboxUI`/`UIManager`）、C2（对象创建、UI、相机/profile、导航、raycast、scene/script 与 CppFSM flag 已全部下沉到专属服务，`SandboxMgr` class/global 已删除）、C3（registry/lifecycle/AI update/tactics/navigation/scheduler/team 已拆成服务边界；tactics Lua 主入口已迁到 `SandboxTactics`，`ObjectManager` 不再导出 tactical 旧 facade）、C4（`IComponent::getUpdateOrder()` + `BaseObject::Update()` 统一驱动组件顺序，`SoldierObject::Update` 不再手写组件列表）、C5（cpp-roadmap 术语已订正）。
> - 🟠 **部分完成**：P2（Soldier update 样板已迁为组件 order；BaseObject 已给 Lua 暴露 AI/Weapon/Anim/Attrib/Locomotion typed component getter，agent 入口与 `Sandbox2/3/5/6/10-13/16/17/18`、Chapter2/4 sample、DT/BT 条件、DT/BT action、`AgentUtils.lua`、`MoveHelpers.lua`、`MoveAction/PursueAction`、`ConfigManager`、`parity_trace.lua` 通过 `AgentComponentAccess.lua` 优先走组件直取；RuntimeDiag `ComponentProbeAgent` 已验证非 Soldier `AgentObject` 复用 AI/Attrib/Weapon/Anim/Locomotion 等组件，覆盖 Locomotion maxSpeed/target/targetRadius round-trip、Anim body ASM、Weapon ammo round-trip 与 `ShootBullet()`，且无需 Soldier forwarder；SoldierObject 上 getWeapon/AI/maxHealth/ammo/敌人/移动目标/射击/Enter*Anim 这批 C++ 兼容转发已删除，`AgentObject` 纯 Locomotion facade 已撤出 Lua 导出，ObjectManager 诊断改为组件直读；`AgentObject` target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` C++ pure Locomotion facade 已删除；其它对象级门面仍待继续收敛）、P4（BaseObject 组件容器、ObjectManager 自有 registry/AI/tactics services、NavigationService navmesh map 已迁到 `unique_ptr`，组件 lifecycle state、attach/destroy/update 断言和 debug dump 状态输出已落地；BlockObject/AnimComponent/AgentObject/OpenSteer/LuaScript/Physics/AIController 关键裸指针已标出 owning/non-owning，并补了 Lua env owner 与 AI cached enemy detach 清理；service/runtime 侧 Raycast/Scene/Script/Camera/ObjectFactory/FGUI facade 缓存指针也已标注 owning/non-owning；剩余为更广的缓存裸指针审计）、P5（WeaponComponent 与动画运行链已脱 `SoldierObject` 直接依赖，普通 `AgentObject` 已默认挂载 `AnimComponent` 并验证 body ASM；`AnimComponent` / `SoldierAnimController` 通过 `IAnimContextProvider` 取上下文；DT/BT driver 与 Lua action owner 已泛化到 `AgentObject`，Lua action 生命周期回调已统一传 `u[AgentObject]`；Blackboard、AIController、DT/BT driver/action 已向 Lua 导出 `GetAgentOwner()` 泛化入口，Blackboard 路径由 RuntimeDiag 验证非 Soldier owner round-trip；FSM action context 的敌人/移动/弹药/血量查询与动画表现入口已走组件，DeathState 已走 AnimComponent/IAnimContextProvider/IAnimController；`AgentFactory` 已提供 `default` / `component_probe` / `movement_only` / `animated_probe` 轻量装配 profile，`ObjectFactory` / `SandboxObjects` 已暴露 `CreateAgentWithProfile`，RuntimeDiag 已覆盖非 Soldier animated mesh + body ASM 状态请求）。
> - 🟢 **已解决待回归**：P7（感知已抽象走 `AgentPerceptionQuery`，driver 创建已走 factory/registry；仍需用 FSM/DT/BT sample 回归守住行为）。
> - 🟠 **P2 最新收窄（2026-06-20）**：`SoldierObject` 的 `getWeapon`、`GetAI/GetAIController`、maxHealth、ammo、`HasEnemy/CanShootEnemy/GetEnemy`、移动目标、射击和 Enter*Anim 纯组件转发已从 Lua 导出撤下，对应 C++ 兼容转发实现也已删除；`AgentObject:getBody/GetAnimation/GetObjectASM` 已撤出 Lua 导出，body 动画访问改走 `AnimComponent:GetBodyAnimation/GetBodyAsm()` / `AgentComponentAccess`；`BaseObject:GetLocomotionComponent()` 与 `AgentLocomotion` Lua 导出已补齐，`AgentUtils.lua`、DT/BT `MoveHelpers.lua`、`MoveAction/PursueAction`、Chapter2/4 sample、`Sandbox2/5/6/10-13/16/17/18` 与 `ConfigManager` 的路径/target/maxSpeed/maxForce/steering/height 访问改走 `AgentComponentAccess` 的 Locomotion helper；`AgentObject` 不再向 Lua 导出 `SetTarget/GetTarget/SetPath/GetPath/HasPath/ForceTo*/SetMaxSpeed/GetMaxSpeed/GetHeight/GetRadius` 等纯 Locomotion facade，只保留位置/朝向/速度、物理形体设置、血量和 future-position 等跨组件语义入口；`AgentObject` C++ target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` 纯 Locomotion facade 也已删除，AI/FSM/Soldier/ObjectManager 相关调用改为组件直读。剩余 P2 关注其它对象级门面与跨组件语义入口是否还能继续拆薄。
> - 🟠 **仍成立**：P2 仍有尾部债务（其它对象门面 / 跨组件语义入口审计）未清。
>
> 各条目下方「现状证据」是评审当时（2026-06-01）的快照，部分已被上述进展取代；最新状态以条目下的「状态」标注和 §7 跟踪清单为准。

## 1. 总体判断

当前项目的核心矛盾不是“缺功能”，而是**两套对象范式叠加 + 跨层全局耦合**在持续抬高每一次扩展的成本：

- 组件类（Render / Physics / Anim / Weapon / Locomotion / AIController / Attrib / Script）已经建立，但旧的“上帝对象 + 全局单例 + 裸指针”骨架仍在，组件化只完成了一半。
- 结果是“既付了组件化的复杂度，又没拿到对象瘦身的收益”：`AgentObject` / `SoldierObject` 把实现搬进组件后，又把每个方法原样转发回来。
- `sandbox` 层通过 `#include` 和 `g_*` 全局单例反向依赖 `game` 层，分层名存实亡，组件无法脱离全局环境复用或测试。

因此本方案的最高杠杆动作是**解耦分层（注入替代全局）**和**让组件化兑现减负承诺**，而不是继续往上加新系统。

## 2. 问题总览

| 编号 | 梯队 | 问题 | 主要影响 | 关键证据 |
|---|---|---|---|---|
| P1 | 🟢 | sandbox→game 反向依赖 + 全局单例硬耦合 | 无法单测/多实例，改动易牵连全局 | sandbox/runtime 反向 include 与裸 `g_*` 已清零，架构门禁覆盖 |
| P2 | 🟠 | 组件化“只藏不减”的主体债务已明显收窄；update 样板已改为组件 order，Lua typed component getter 与 sample 迁移已落地；Soldier getWeapon/AI/maxHealth/ammo/敌人/移动/射击/Enter*Anim 纯转发已撤出 Lua 导出，且对应 C++ 兼容转发已删除；AgentObject pure Locomotion facade 已撤出 Lua 导出并删除 C++ 实现 | 新对象类型不再复制 Soldier update block；DT/BT/knowledge/team/influence sample 可优先直取组件；剩余关注其它对象门面与跨组件语义入口审计 | `BaseObject::GetAIComponent/GetWeaponComponent/GetAnimComponent/GetAttribComponent/GetLocomotionComponent`、`AgentComponentAccess.lua`、`AgentObject.cpp`、`SoldierObject.cpp` |
| P3 | 🟢 | `GameManager` 的 FGUI 转发壳已删除 | FGUI API 不再挂在编排层 | `GameManager.h` 约 79 行，`rg "GameManager::.*FairyGui"` 为 0 |
| P4 | 🟠 | BaseObject 容器、ObjectManager 自有 system/service 指针和 NavigationService navmesh map 已 `unique_ptr` 化，组件 lifecycle 断言/dump 已落地，多处关键裸指针已标注 owning/non-owning 并补 detach 清理；service/runtime facade 缓存指针已继续标注 | 悬空/重复删除隐患下降，仍需继续审计其它缓存裸指针 | `IComponent.h` lifecycle state、`BaseObject::DestroyComponent`、`BuildComponentDebugString()`、`ObjectManager::m_registry/m_aiScheduler`、`NavigationService::m_navMeshes`、`RaycastService::m_physicsWorld`、`FairyGuiSystem::m_impl` |
| P5 | 🟠 | WeaponComponent、动画运行链、DT/BT driver、Lua action owner、Blackboard/AIController/driver/action Lua owner、DT/BT Lua action 回调、AgentActionContext、DeathState 与 FSM 通用查询已脱 SoldierObject 主路径；普通 AgentObject 已覆盖非 Soldier AnimComponent/body ASM 运行面；AgentFactory 轻量装配 profile、Lua 创建入口与 animated probe RuntimeDiag 已落地 | 组件/AI driver 复用性提高；剩余关注点是更完整新对象行为样例、profile 外部数据化与 legacy forwarder 收敛 | `IAnimContextProvider.h`、`AgentFactory::CreateAgentWithProfile`、`ObjectFactory::CreateAgentWithProfile`、`NonSoldierAnimProbeAgent.lua`、`DecisionTreeDriver::GetAgentOwner`、`LuaDecisionAction::GetAgentOwner`、`Blackboard::GetAgentOwner`、`AgentActionContext::GetAnimController` |
| P6 | 🟡 | 组件 key 是散落的魔术字符串 | 易拼写漂移、无编译检查 | `"anim"/"weapon"/"ai"/...` 散落多 cpp |
| P7 | 🟡 | AI 感知无抽象，硬扫全场 + driver 切换硬编码 | agent 增多即 O(n²)，扩展困难 | `AIController.cpp:167`、`AIController.cpp:333-413` |
| C1 | 🟢 | `UIService` 空壳已删除，Gorilla UI Lua 入口已由 `SandboxUI`/`UIManager` 承接 | 少一层无意义中间层，UI API 不再挂在 SandboxMgr | `GameManager::Initialize` 注册 `SandboxUI`、`UIManager.h` tolua 导出、`SandboxMgr` 不再导出 UI 方法 |
| C2 | 🟢 | `SandboxMgr` 对象创建、Gorilla UI、相机/profile、导航、raycast、scene/light/material、CallFile 与 CppFSM flag 转发已全部下沉到专属 service/global，`SandboxMgr` class/global 已删除 | 层级冗余已清，Lua 入口只保留语义化服务全局 | `GameManager::Initialize` 注册 `SandboxObjects`/`SandboxUI`/`SandboxCamera`/`SandboxNav`/`SandboxRaycast`/`SandboxScene`/`SandboxScript`/`SandboxAgentConfig`；`SandboxToLua.pkg` 不再包含 `SandboxMgr.h` |
| C3 | 🟢 | `ObjectRegistry` 已拆出对象 map 与 Agent/Block 二级索引，`ObjectLifecycleSystem` 已承接对象 update loop 与 scene cleanup stage，AI scheduler/perception/team sync 已收口到 `AIUpdateSystem`，tactics query/debug draw 已下沉到 `TacticalQueryService` / `TacticalDebugDrawService` / `TacticalService`，navmesh map 与销毁责任已迁到 `NavigationService`，AIScheduler Lua 主入口已迁到 `SandboxAIScheduler`，TeamBlackboard Lua 主入口已迁到 `SandboxTeam`，tactics Lua 主入口已迁到 `SandboxTactics`；ObjectManager 上 scheduler/team/tactical 旧导出已删除 | registry、lifecycle、AI update、tactical、navigation ownership、scheduler/team/tactics Lua API 边界清晰一层 | `ObjectRegistry.h`、`ObjectLifecycleSystem.h`、`AIUpdateSystem.h`、`TacticalService.h`、`TacticalDebugDrawService.h`、`NavigationService.h`、`AIScheduler.h`、`TeamBlackboardService.h` |
| C4 | 🟢 | `Update` 顺序已显式化到组件 order | 新组件可声明顺序，Soldier 不再手写组件 update block | `IComponent.h:9-16`、`BaseObject.cpp:76-93`、`SoldierObject.cpp:154-162` |
| C5 | 🟢 | 文档/代码漂移 | 路线图引用了不存在的类 | 代码无 `GameObject`/`VehicleObject` |

---

## 3. 🔴 第一梯队（优先解决，是其他重构的前提）

### P1：消灭反向依赖与全局单例，引入 SandboxServices 注入

> 🟢 状态（2026-06-18）：**已解决，持续由门禁守住**。`sandbox/core/SandboxServices.h` 已落地并下发（`ObjectManager` 填充 → `BaseObject::SetSandboxServices` → `IComponent::onSandboxServicesChanged()` / `GetSandboxServices()`），AIController / PhysicsComponent / WeaponComponent / AgentLocomotion / AgentStateController 的热点已改为 `Resolve*()` 只走服务；AgentObject / BlockObject 的死亡/碰撞事件发布、粒子清理和 FSM flag 读取也只走 `SandboxServices`；SoldierObject 通过 `SandboxServices.input` 获取输入；RenderComponent / AnimComponent 已移除 `GameManager.h`；InputManager 通过构造注入 `RenderWindow` / `OgreCameraController` / 状态回调，不再 include `ClientManager.h`；raycast 已下沉到 `RaycastService`，由应用层注入 `PhysicsWorld` 并导出 Lua 全局 `SandboxRaycast`，旧 `SandboxMgr` raycast 导出已删除；scene/light/material 已下沉到 `SceneService`，由应用层注入 `SceneManager` / `CameraService` 并导出 Lua 全局 `SandboxScene`；CallFile 已下沉到 `ScriptService`，由应用层注入 `ScriptLuaVM` 并导出 Lua 全局 `SandboxScript`；`SceneFactory` 通过 `SetRootSceneNode` 接收应用层 root scene node，移除 `GameManager.h` include；`UIManager` 通过构造注入 `Ogre::Camera*`，`UIService` 移除无用 `GameManager.h` include；`CameraService` 通过构造注入 camera / scene manager / profile time getter，不再持 `ClientManager*`；`NavigationMesh` debug visual 与 `ObjectManager` 场景访问改走 `SceneFactory`，`ObjectManager` 当前时间由 `GameManager` 每帧写入；`ObjectManager::GetInstance` / `SandboxMgr::GetInstance` 及 `g_ObjectManager` / `g_SandboxMgr` 已删除。找不到服务时显式返回空并走原调用点的降级分支，不再回退 `g_*`。`tools/check_sandbox_architecture.ps1` 已作为静态门禁，防止新增 sandbox/runtime 反向依赖和裸 `g_*`。下方"现状证据"为改造前快照。

> 命名说明：本节的依赖注入聚合体命名为 `SandboxServices`（早期草稿曾叫 `SandboxContext`，但仓库已有 `sandbox/core/event/SandboxContext.h` 作事件 payload，撞名，故改名）。

#### 现状证据

- AGENTS.md 声明依赖方向为 `runtime → sandbox → game`，但实际有 **14 个 sandbox 文件 `#include` `GameManager`、4 个 include `ClientManager`**（NavigationMesh、AnimComponent、PhysicsComponent、RenderComponent、AgentObject、BlockObject、SoldierObject、InputManager、ObjectManager、SandboxMgr、CameraService、SceneFactory、UIManager 等）。
- 组件/对象直接用全局单例抓依赖：
  - `g_ObjectManager->getAllAgents()`：[AIController.cpp:162-205](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L162-L205)、[AgentLocomotion.cpp:268](src/HelloOgre3D/sandbox/components/agent/AgentLocomotion.cpp#L268)、[AgentStateController.cpp:215](src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp#L215)
  - `g_SandboxMgr->FindPath(...)`：[AIController.cpp:157](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L157)
  - `g_GameManager->getPhysicsWorld()`：原在 `PhysicsComponent` 与 `SandboxMgr::RayCastObjectId`，已通过 `SandboxServices.physics` 清理。

#### 问题

组件无法脱离全局环境构造或单测；无法在两个 world 实例下并存；manager 初始化顺序的任何调整都可能炸链路。这是所有其他重构的“地基病”——不先解开，拆什么都会被全局单例拽住。

#### 解决方案

引入一个**按值注入的运行时上下文**，集中暴露 sandbox 层真正需要的服务接口，替换 `g_*` 全局访问。

1. 新增 `sandbox/core/SandboxServices.h`，聚合接口（先用现有具体类型的指针，后续再抽接口）：

   ```cpp
   // sandbox/core/SandboxServices.h（草图）
   struct SandboxServices {
       ObjectManager*  objects   = nullptr;  // 对象查询/注册
       PhysicsWorld*   physics   = nullptr;  // 物理世界
       InputManager*    input     = nullptr;  // 输入查询
       INavQuery*      nav       = nullptr;  // 寻路查询（FindPath 抽成接口）
       ScriptLuaVM*    script    = nullptr;  // 脚本调用
   };
   ```

2. `GameManager` / `ClientManager` 在初始化时填好 `SandboxServices` 并下发给 `ObjectManager`，由对象创建链路（`ObjectFactory` / `AgentFactory` / `SoldierFactory`）把 context 透传给新建对象，对象再传给组件。
3. 把热点访问点改为走 context：AIController 找敌人、Locomotion 分离力、PhysicsComponent 取世界、AIController 寻路。
4. 全局入口清零：`g_ObjectManager` / `g_SandboxMgr` 与对应 `GetInstance()` 已删除；新代码通过 `SandboxServices`、构造注入或显式 owner 链路拿依赖。

> 不要求一次替换所有 `g_*`。先迁移“被组件/对象直接引用”的点；纯应用层（game 内部）继续用全局无妨。

#### 验收

- [x] sandbox/runtime 层不再 `#include GameManager.h` / `ClientManager.h`（AIController、PhysicsComponent、AnimComponent、RenderComponent、Locomotion、InputManager、SandboxMgr、SceneFactory、UIManager、CameraService、NavigationMesh、ObjectManager 已达标）。
- [x] sandbox/runtime 不再出现 `g_ObjectManager` / `g_SandboxMgr` / `g_GameManager` 直接引用（AIController / PhysicsComponent / WeaponComponent / AgentLocomotion / AgentStateController / AgentObject / BlockObject / SandboxMgr RayCast / manager 历史入口均已清）。
- [x] `Sandbox6/7/8` 移动、射击、寻路、状态切换行为不变（2026-06-18 smoke PASS）。
- [x] 静态检查新增 baseline 门禁：`tools/check_sandbox_architecture.ps1` 禁止新增 sandbox/runtime 对 `GameManager` / `ClientManager` include 与裸 `g_GameManager` / `g_ObjectManager` / `g_SandboxMgr` 访问。

#### 风险与回退

- 透传链路较长，分对象类型增量迁移；每迁一类跑对应 sample。
- `NavigationService` 已先承接导航配置/构建/查询，后续若要抽 `INavQuery` 可在该 service 上收口，不再回退到 `SandboxMgr`。`RaycastService` 已承接物理 raycast，后续遮挡/命中查询也应从该服务扩展。

---

### P2：停止对象层转发样板，让组件化真正减负

> 🟠 状态（2026-06-18）：**前置能力已落地，主体迁移继续**。`BaseObject` 已向 Lua 暴露 `GetAIComponent()` / `GetWeaponComponent()` / `GetAnimComponent()` / `GetAttribComponent()`，`AIController`、`WeaponComponent`、`AnimComponent`、`AgentAttrib` 的常用接口已补 tolua 导出；`bin/res/scripts/agent/AgentComponentAccess.lua` 统一封装 AI/Attrib/Weapon/Health 组件优先访问，agent 目录内旧 `agent:GetAI()` / `agent:GetMaxHealth()` 直接调用已收敛到该 helper 的兼容兜底，`Sandbox10`-`Sandbox13`、`Sandbox17` 与 `parity_trace.lua` 的 Blackboard / tactics 入口也已迁为组件优先。RuntimeDiag 新增 `ComponentProbeAgent.lua` 自测，创建非 Soldier `AgentObject` 并验证 `ai/attrib/weapon/locomotion/physics/render/script` 等组件、AI/Attrib/Weapon typed getter、Health round-trip、Weapon ammo round-trip 与 `ShootBullet()`。对象层大批单组件 legacy forwarder 仍保留。

> 2026-06-19 增量：`AgentComponentAccess.lua` 的 Weapon/AI/maxHealth/ammo/enemy/move/animation/body-ASM helper 已不再依赖对象层纯转发主路径；`SoldierObject` Lua 绑定撤下 `getWeapon`、`GetAI/GetAIController`、`SetMaxHealth/GetMaxHealth`、`SetAmmo/GetAmmo/SetMaxAmmo/GetMaxAmmo/HasAmmo/ConsumeAmmo/RestoreAmmo`、`HasEnemy/CanShootEnemy/GetEnemy`、`ShootBullet`、`HasMovePosition/SetMovePosition/ClearMovePosition/IsTargetReached` 与 `Enter*Anim`，`AgentObject:getBody/GetAnimation/GetObjectASM` 也已撤下；DT/BT 条件、`ShootAction`、`ReloadAction`、移动/调查/编队/等待类 action、Chapter9 legacy agent、`SoldierAgent`、`Sandbox3` 与 `parity_trace.lua` 均改走 typed component getter 或 `AgentComponentAccess.lua`。

> 2026-06-20 增量：上述 `SoldierObject` 纯组件 C++ 兼容转发也已删除；`SoldierObject` 内部逻辑与 `ObjectManager` 的 AI/Object 诊断改为直接 `FindComponent<AIController/WeaponComponent/AgentAttrib>()`，避免 C++ 新代码继续以旧对象 facade 为主路径。

#### 现状证据

- 评审快照里，`AgentObject` + `SoldierObject` 曾合计存在大量“查组件 → 判空 → 转发”的样板；当前 `SoldierObject` 上 getWeapon/AI/maxHealth/ammo/敌人/移动/射击/Enter*Anim 这批纯组件转发已删除。
- 剩余主体仍集中在 `AgentObject` 以及少量对象级门面：例如 [AgentObject.cpp:301-341](src/HelloOgre3D/sandbox/objects/AgentObject.cpp#L301-L341)（`SetMass/SetHeight/SetRadius` 同时转发 Locomotion 和 Physics）。
- 组件把实现搬走后，对象层不应再把每个单组件 API 原样转发回来；后续新增对象类型必须沿组件直取/装配器路径走。

#### 问题

新增一个 Monster/NPC 仍要复制整套对象级转发；对象类持续膨胀；“组件化”没有降低复制粘贴成本。

#### 解决方案

明确**两类调用方**，停止为每个组件方法在对象上开转发：

1. **Lua / sample**：默认通过组件直取使用能力，例如
   ```lua
   soldier:GetComponent("weapon"):Shoot()
   soldier:GetComponent("attrib"):SetMaxHealth(100)
   ```
   为此需要把关键组件（Weapon/Attrib/Anim/AIController）的常用方法挂上 tolua 导出（多数已导出，补齐缺口即可）。
2. **对象级门面只保留少数真正高频/跨组件聚合的接口**：位置、朝向、速度等会被相机/物理/渲染同时使用的，保留在对象上。其余“单组件单接口”的转发逐步标注 `// [legacy-forward] 待迁移`，新代码不再新增。
3. 新增对象类型时，用 `AgentFactory` 风格的**装配器**组合组件，而不是继承一个塞满转发的基类。

> 兼容优先：现存 Lua 调用名先不动，旧转发方法保留为 legacy facade；只“停止增量 + 标注 + 新对象走组件直取”。

#### 验收

- [~] 关键组件（Weapon/Attrib/Anim/AIController/Locomotion）常用方法已 tolua 导出，Lua 可通过 `BaseObject` typed getter 直取；agent 入口、`Sandbox2/3/5/6/10-13/16/17/18`、Chapter2/4 sample、DT/BT 条件、DT/BT action、`AgentUtils.lua`、`MoveHelpers.lua`、`MoveAction/PursueAction`、`ConfigManager` 和 `parity_trace.lua` 已通过 typed component getter / `AgentComponentAccess.lua` 迁到组件优先，Soldier getWeapon/AI/maxHealth/ammo/敌人查询/移动目标/射击/Enter*Anim、AgentObject getBody/GetAnimation/GetObjectASM 与 AgentObject 纯 Locomotion facade 均已撤出 Lua 导出，Soldier 这批 C++ 兼容转发也已删除，AgentObject target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` C++ pure Locomotion facade 也已删除，DT/BT Lua action 回调已统一为 `AgentObject`；仍待审计其它对象门面。
- [x] 新增最小 `ComponentProbeAgent` 运行时诊断样例：复用非 Soldier `AgentObject` 的 ≥2 个组件（已覆盖 AI/Attrib/Weapon/Anim/Locomotion 等组件，含 Locomotion maxSpeed/target/targetRadius round-trip、Weapon ammo round-trip 与 `ShootBullet()`），且不复制 Soldier 的转发代码。
- [x] 已覆盖 `Sandbox6/7/8/9/10/11/12/13/16` 与 `chapter9_tactics_legacy_parity` smoke，现存 sample 行为未见回归。
- [ ] 对象层不再新增“单组件单接口”转发（评审约定 + 标注）。

---

### P3：拆薄 GameManager 的 FGUI 转发

> 🟢 状态（2026-06-18）：**已解决**。`GameManager.h/.cpp` 中约百个 `*FairyGui*` public 转发方法已删除，`GameManager` 只保留 `FairyGuiLuaApi*` 的持有与 `InitLuaEnv()` 中 `FairyGuiRuntime` 全局对象注册；Lua 侧 FGUI 入口继续通过 `FairyGuiNativeApi -> FairyGuiRuntime -> RuntimeToLua.pkg / FairyGuiLuaApi`。同时修复 `tools/run_fgui_selftest.ps1 -Mode All` 的空跑问题：自动化模式会实际调用 `FGUI_Init()`，脚本必须看到 `self test suite end: N / N` 且全通过才算成功。

#### 现状证据

- 改造前：[GameManager.cpp](src/HelloOgre3D/game/GameManager.cpp) 866 行、**123 个 public 方法，其中约 102 个是 FairyGui 转发**（`GameManager.h:44-150`）。
- 改造后：`GameManager.h` 约 79 行、`GameManager.cpp` 约 370 行；`rg "GameManager::.*FairyGui"` 为 0，`GameManager.cpp` 内 `FairyGui` 只剩 `m_pFairyGuiLuaApi` 持有/注册与真实输入直达 runtime，不再有 FGUI API 转发函数。
- 仍兼任 Lua 环境初始化、`IInputHandler` 输入处理、应用服务持有；这些属于后续更细拆分，不再包含 FGUI API 适配细节。

#### 问题

改造前，Lua 侧 FGUI 已通过 `FairyGuiRuntime`（`RuntimeToLua.pkg`）走 runtime 后端，GameManager 里这批 C++ 转发壳多数已无 Lua 调用方，继续保留只会增加“上帝类”体量。

#### 解决方案

1. 用 `rg` 确认每个 `*FairyGui*` 转发方法是否仍有 **C++ 内部调用方**。
2. 无调用方的：直接删除（Lua 已不依赖）。
3. 仍被 C++ 内部使用的：迁到 `FairyGuiLuaApi` / runtime 侧，GameManager 仅保留必要的薄封装或彻底改为持有 `FairyGuiSystem*` 直调。
4. GameManager 收敛为“应用编排 + 服务持有 + Lua 环境注册”，输入处理可进一步拆到独立 input adapter（与 P1 的 context 配合）。

#### 验收

- [x] `GameManager` public 方法数显著下降（目标砍掉大部分 FGUI 转发）。
- [x] `HELLO_FGUI_SELF_TEST_ALL=1` 通过（`tools/run_fgui_selftest.ps1 -Mode All -Count 3 -StopExisting`，suite `30 / 30`）。
- [ ] `tools/run_fgui_production_gate.ps1`（按改动面选 Fast/RuntimeOnly/Full）通过。

---

## 4. 🟡 第二梯队（局部耦合与所有权）

### P4：所有权用 unique_ptr 表达，消除 SAFE_DELETE 隐患

> 🟠 状态（2026-06-20）：**核心完成，审计继续**。`BaseObject` 内部组件容器已从 `std::map<std::string, IComponent*>` 迁到 `std::map<std::string, std::unique_ptr<IComponent>>`，`AddComponent` 成功后由 `BaseObject` 接管所有权，`GetComponent` / `FindComponent` 仍返回非拥有裸指针以保持调用面兼容；销毁链集中走 `BaseObject::DestroyComponent(ComponentPtr&)` 并保持 `onDestroy()` → `onDetach()` 顺序。`ObjectManager` 自有的 `ObjectRegistry`、AI scheduler/spatial/perception/team、tactics query/debug/service 已迁为 `std::unique_ptr`，getter 仍返回非拥有观察指针。`NavigationService::m_navMeshes` 已迁为 `std::unordered_map<Ogre::String, std::unique_ptr<NavigationMesh>>`，`AddNavigationMesh` 接收历史 raw pointer 后立即接管所有权。`IComponent` 已记录 lifecycle state，`BaseObject` 在 attach/destroy/update 断言组件状态，`BuildComponentDebugString()` 会输出 `key:state`。`BlockObject` 缓存的组件/owner 指针、`AnimComponent` 缓存的 `Ogre::Entity*`、`AgentObject::m_renderComp`、`OpenSteerAdapter::m_owner`、`LuaScriptComponent` 的 VM/env owner、`PhysicsComponent::m_addedWorld` 与 `AIController::m_enemy` 已标注 owning/non-owning 语义；`LuaScriptComponent::onDetach` 会清掉 local env owner，`AIController::onDetach` 会清掉 cached enemy。service/runtime 侧的 `RaycastService::m_physicsWorld`、`SceneService::m_sceneManager/m_cameraService`、`ScriptService::m_scriptVM`、`CameraService::m_camera/m_sceneManager`、`ObjectFactory::m_objectManager`、`FairyGuiSystem::m_impl` 与 `FairyGuiLuaApi::m_system` 也已显式标注 owning/non-owning。**仍待**：继续审计其它缓存裸指针是否需要同样语义标注。

#### 现状证据

- 组件容器已迁为 `std::map<std::string, std::unique_ptr<IComponent>> m_components`（[BaseObject.h:19](src/HelloOgre3D/sandbox/core/object/BaseObject.h#L19)、[BaseObject.h:116](src/HelloOgre3D/sandbox/core/object/BaseObject.h#L116)），`SAFE_DELETE` 不再用于组件容器析构。
- `IComponent` 已有 `LifecycleState`，`BaseObject::AddComponent` / `DestroyComponent` / `Update` 会检查 attach/destroy/update 状态；对象诊断处调用的 `BuildComponentDebugString()` 现在输出组件 lifecycle state。
- **非拥有缓存指针**：`BlockObject` 同一 `RenderComponent` 既 `AddComponent("render",...)`（由 BaseObject 容器拥有）又缓存裸成员 `m_renderComp`；现已标注为 non-owning（[BlockObject.h:74-77](src/HelloOgre3D/sandbox/objects/BlockObject.h#L74-L77)）。`AgentObject::m_renderComp` 同样标注为 BaseObject component map 拥有。
- **持有不拥有的指针**：`AnimComponent` 存 `Ogre::Entity* m_bodyEntity/m_weaponEntity`（[AnimComponent.h:55-56](src/HelloOgre3D/sandbox/components/anim/AnimComponent.h#L55)），现已标注为 non-owning 并在 `onDetach()` 清空；`OpenSteerAdapter::m_owner`、`LuaScriptComponent::m_pScriptVM/m_localEnvOwner`、`PhysicsComponent::m_addedWorld`、`AIController::m_enemy` 也已标注为 non-owning，`AIController::m_driver`、`AgentLocomotion::m_adapter`、`SoldierObject::m_inputInfo` 标注为 owning。

#### 解决方案

1. 组件容器改为 `std::map<std::string, std::unique_ptr<IComponent>>`；`AddComponent` 接管所有权，`GetComponent`/`FindComponent` 返回裸指针（非拥有观察者）。
2. 对象/组件里**缓存性裸指针**统一视为“非拥有引用”，加注释 `// non-owning`，并确保它指向的对象由 map 拥有，删除重复 `SAFE_DELETE`。
3. `AnimComponent` 的 Entity 指针明确标注由 `RenderComponent` 拥有；重 init 时先置空再取新值。
4. 析构链补最小断言/dump：组件记录 lifecycle state，attach/destroy/update 断言状态，debug summary 输出 `key:state`；Lua callback 解绑按脚本生命周期另行审计。

#### 验收

- [x] 组件容器迁移到 `unique_ptr`，`BaseObject` 内部不再手动 `SAFE_DELETE` 组件。
- [x] `BaseObject` 组件 map、`ObjectManager` 自有 system/service 指针与 `NavigationService` navmesh map 已由 `unique_ptr` 表达拥有关系；`BlockObject` / `AnimComponent` / `AgentObject` / `LuaScriptComponent` / `PhysicsComponent` / `AIController` / `OpenSteerAdapter` 关键裸指针明确 owning 或 non-owning 语义，且无对“map 已拥有对象”的二次手动 delete。
- [x] 组件生命周期 debug dump / 断言补齐，避免后续新组件重新引入双重持有。
- [ ] 对象创建/销毁 + `ObjectManager:buildObjectDebugSummary` 正常，无崩溃/泄漏。

---

### P5：组件 owner 统一为 BaseObject，解除对具体子类的依赖

> 🟠 状态（2026-06-20）：**部分完成**。`AnimComponent` / `WeaponComponent` 都不再持有 `SoldierObject* owner` 成员，owner 通过 `IComponent::getOwner()` 观察；`WeaponComponent` 的跨组件访问已收口为 `owner->FindComponent<RenderComponent>()`，服务访问也不再回退 `g_*`，并已移除对 `SoldierObject` 的 include / dynamic_cast，射击链只依赖 `BaseObject` id/team、`RenderComponent` 朝向和 `SandboxServices`。普通 `AgentObject` 已通过 `AgentFactory` 默认挂载 `WeaponComponent` 与 `AnimComponent`，RuntimeDiag 会验证 ammo round-trip、`ShootBullet()`、`anim` 组件与 body ASM。动画链新增 `IAnimContextProvider`，`AnimComponent` 通过该接口读取 stance / cpp-fsm flag / shoot hook / FSM controller，`SoldierAnimController` 只持接口指针，不再 include 或调用 `SoldierObject`；非 Soldier `AnimComponent` 会初始化 body ASM 但不启用 Soldier 动画事件。DT/BT driver 已以 `AgentObject*` 作为真实 owner，Lua action 直接持 `AgentObject* + Blackboard*`，生命周期回调统一按 `u[AgentObject]` 调用；`Blackboard`、`AIController`、`DecisionTreeDriver` / `BehaviorTreeDriver` 与 `LuaDecisionAction` / `LuaBehaviorAction` 的 Lua owner 入口已提供 `GetAgentOwner()`，这些类的 `GetOwner()` 不再导出给 Lua。FSM `AgentActionContext` 已把敌人、移动目标、血量、弹药查询下沉到 AIController / WeaponComponent / AgentAttrib，并把 idle/move/shoot/reload/presentation 动画入口改为 `AnimComponent` / `IAnimController`；Move/Shoot/Pursue/Reload state 与 transition evaluator 不再直接 include/cast `SoldierObject`；`DeathState` 死亡动画入口已走 `AnimComponent` / `IAnimContextProvider` / `IAnimController`，不再直接 include/cast `SoldierObject`。`AgentFactory` 已提供 `default` / `component_probe` / `movement_only` / `animated_probe` 轻量装配 profile，`CreateAgent` 委派 `default`，`ObjectFactory` / `SandboxObjects` 已暴露 `CreateAgentWithProfile`，RuntimeDiag 使用 `component_probe` profile 验证非 Soldier 组件组合，并使用 `animated_probe` + `NonSoldierAnimProbeAgent.lua` 验证普通 `AgentObject` 可挂 animated mesh、配置 body ASM 并请求状态切换。**仍待**：更完整非 Soldier 行为场景、profile 外部数据化与 legacy forwarder 收敛。

#### 现状证据

- `AnimComponent` 已改为 `GetAnimContext()`，只依赖 `IAnimContextProvider`；`SoldierObject` 负责把自身适配为动画上下文。
- `SoldierAnimController` 已改为持有 `IAnimContextProvider*`，通过接口获取 body/weapon ASM 与 stance，不再持 `SoldierObject* owner`。
- `WeaponComponent` 跨组件访问已改 `FindComponent<RenderComponent>()`，`DoShootBullet` 以 `BaseObject` owner id/team 作为 bullet owner / tactical event 来源。
- `DecisionTreeDriver` / `BehaviorTreeDriver` 真实 owner 已改为 `AgentObject*`；driver/action Lua 查询入口已改为 `GetAgentOwner()`；Lua action 优先使用 driver 注入的 `Blackboard*`，生命周期回调已统一按 `u[AgentObject]` 调用。
- `AgentActionContext` 的敌人/移动/弹药/血量查询已走 AIController / WeaponComponent / AgentAttrib，动画表现入口已走 AnimComponent / IAnimController；FSM evaluator 和 Move/Shoot/Pursue/Reload state 不再直接依赖 `SoldierObject`。
- 普通 `AgentObject` 已经由 `AgentFactory` 挂载 `AnimComponent`，并在 RuntimeDiag 中验证 body ASM 可用。

#### 解决方案

1. 组件 owner 类型统一为 `BaseObject*`（`IComponent` 已是此约定，收口子类里偷用的具体类型）。
2. 跨组件依赖一律走 `owner->FindComponent<T>()`，不调具体对象方法（如用 `FindComponent<RenderComponent>()->GetSceneNode()` 替代 `owner->GetRenderComponent()`）。
3. Soldier 专有数据（stance、useCppFSM）先由 `IAnimContextProvider` 适配，后续可继续下沉为组件状态或通过 Blackboard/Attrib 暴露，组件不读具体对象类型字段。

#### 验收

- [x] `AnimComponent`、`WeaponComponent` 不再出现对 `SoldierObject` 具体方法的直接调用；`SoldierAnimController` 不再持 `SoldierObject* owner`。
- [x] `DecisionTreeDriver` / `BehaviorTreeDriver` / `LuaDecisionAction` / `LuaBehaviorAction` 的真实 owner、Lua 查询入口和 Lua 生命周期回调均不再是 `SoldierObject*`，非 Soldier agent 可创建 DT/BT driver。
- [x] `AgentStateEvaluators` 与 Move/Shoot/Pursue/Reload state 不再直接 include/cast `SoldierObject`；通用查询经 `AgentActionContext` 转到组件。
- [x] `AgentActionContext` 的 idle/move/shoot/reload/presentation 动画入口不再直接 include/cast `SoldierObject`，改走 `AnimComponent` / `IAnimController`。
- [x] 至少一个非 Soldier 对象成功挂载 `WeaponComponent` 与 `AnimComponent` 并运行（普通 `AgentObject` + RuntimeDiag 覆盖 Anim body ASM、WeaponComponent ammo round-trip 与 `ShootBullet()`）。

---

### P6：用类型化访问替代散落的字符串 key

> 🟢 状态（2026-06-11）：**已解决**。`sandbox/components/ComponentKeys.h` 已建，8 个组件 key 全量常量化（`ComponentKeys::Render/Physics/...`），`FindComponent<T>()` 类型查找已普遍使用；sandbox `.cpp` 内已无散落的 `"render"/"physics"/...` 字面量或 `GetComponent("...")`/`AddComponent("...")` 字符串调用。

#### 现状证据

- `"render"/"physics"/"anim"/"weapon"/"ai"/"attrib"/"locomotion"/"script"` 散布在 `AgentObject.cpp`、`SoldierObject.cpp`、`BlockObject.cpp`、`WeaponComponent.cpp` 等多处，无常量、无编译检查。

#### 解决方案

1. 代码里已有 `FindComponent<T>()` 类型查找，优先全面用它替代字符串版 `GetComponentAs("key")`。
2. 确需稳定 key 的场景（Lua 侧按名取），集中到一处常量定义（如 `sandbox/core/ComponentKeys.h`），C++ 内部不再裸写字符串。

#### 验收

- [ ] C++ 内部组件获取以类型查找为主，魔术字符串集中到常量头。
- [ ] grep 不到散落的组件 key 字面量（除常量定义处）。

---

### P7：AI 感知抽象化，driver 切换工厂化

> 🟢 状态（2026-06-18）：**已解决待回归**。感知已抽象——`AIController` 找敌人改走 `AgentPerceptionQuery`（接口 `IAgentPerceptionQuery` + 可插拔 `IAgentSpatialQuery` + FOV/LOS/path 过滤），不再直接 `g_ObjectManager->getAllAgents()`；driver 创建已改为 factory/registry，`SetDriverByType` 只保留字符串归一化边界。

#### 现状证据

- 找敌人直接 `g_ObjectManager->getAllAgents()` 线性遍历（[AIController.cpp:167](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L167)），无感知/查询层，agent 一多即 O(n²)。
- driver 切换是字符串 if-else `"fsm"/"dt"/"bt"`，每个 `SetXxxDriver` 内部直接 `new`，无工厂（[AIController.cpp:333-413](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L333-L413)）。

#### 解决方案

1. 抽一个最小 `PerceptionQuery`（按范围/队伍过滤的敌人查询），AI 节点只读查询结果，不再自行扫全场；查询走 P1 的 `SandboxServices.objects`。后续可在此挂空间分区优化。
2. driver 用枚举 + 工厂/注册表替代字符串 `new`：`AIDriverFactory::Create(DriverType)`；Lua 侧 `SetDriverByType("bt")` 仅在边界做一次 string→enum 转换。
3. 与 `ai-roadmap.md` 的“感知系统组件化”衔接：`PerceptionQuery` 是 VisionSense/MemoryStore 的前置最小件。

#### 验收

- [x] AI 找敌人改为通过 `PerceptionQuery`，`AIController` 不再直接 `g_ObjectManager->getAllAgents()`。
- [x] driver 创建走工厂，新增 driver 类型只需注册，不改 `SetDriverByType` 的 if-else 主体。
- [ ] FSM / DecisionTree / BehaviorTree 各一条 sample 行为不变。

---

## 5. 🟢 第三梯队（清理项，可穿插）

> 这些是低风险局部清理，建议在第一/二梯队对应区域被改动时顺手做，不单独立大改。每条执行前先 `rg` 复核调用点。

- **C1 删 `UIService` 空壳**（🟢 已解决 2026-06-18）：`UIService.{h,cpp}` 已删除并从 VS 工程移除；Gorilla UI 的 `CreateUIFrame` / `SetMarkupColor` 已从 `SandboxMgr` 移到 `UIManager` tolua 导出，并由 Lua 全局 `SandboxUI` 调用；`SandboxMgr` 不再持有 `UIManager*`，`rg "UIService"` 仅剩 FGUI profiling 计数名。
- **C2 `SandboxMgr` 减肥**（🟢 已解决 2026-06-19）：`CreatePlane/CreateBlock/CreateBullet/CreateAgent/CreateSoldier` 等对象创建 API 已从 `SandboxMgr` 移到 `ObjectFactory` tolua 导出，并由 Lua 全局 `SandboxObjects` 调用；`WeaponComponent` 创建 bullet 改走 `SandboxServices.objectFactory`；Gorilla UI frame/color API 已从 `SandboxMgr` 移到 `SandboxUI`/`UIManager`；相机与 profile 查询已从 `SandboxMgr` 移到 `SandboxCamera`/`CameraService`；导航 `DefaultConfig/ApplySettingConfig/CreateNavigationMesh/RandomPoint/FindClosestPoint/FindPath` 已移到 `SandboxNav`/`NavigationService`；raycast 已移到 `SandboxRaycast`/`RaycastService`；scene/light/material/scene graph 更新已移到 `SandboxScene`/`SceneService`；CallFile 已移到 `SandboxScript`/`ScriptService`；CppFSM flag 已移到 `SandboxAgentConfig`/`AgentConfigService`，`AgentObject` 通过 `SandboxServices.agentConfig` 读取。`SandboxMgr` 旧方法导出清空后，class/global 已删除。
- **C3 `ObjectManager` 职责拆分**（🟢 已解决 2026-06-19）：`ObjectRegistry` 已承接对象 map、对象 id 分配、Agent/Block 二级索引和 `getObjectById` 查找；`ObjectLifecycleSystem` 已承接对象生命周期/update loop、待删对象移除、对象 event flush 与场景节点延迟清理；`AIUpdateSystem` 已承接 scheduler begin/tick、spatial rebuild、`AgentPerceptionSystem` 批量 update、`TeamBlackboardService` 每帧 sync 与 AI perf stats 写回；`TacticalQueryService` 已包住 influence config/layer/source/sample/score/stats 与 navmesh 建图编排，`TacticalDebugDrawService` 已承接 tactical debug visual 的 Ogre 绘制生命周期，`TacticalService` / `SandboxTactics` 已承接 Lua tactics 主入口；navmesh map 与销毁责任已迁到 `NavigationService`，ObjectManager 只保留 fixed blocks 来源与兼容 C++ 转发；AIScheduler Lua 配置/诊断入口已迁到 `SandboxAIScheduler`，TeamBlackboard Lua 主入口已迁到 `SandboxTeam`，对应 ObjectManager 旧导出已删除。
- **C4 `Update` 顺序显式化**：已加 `IComponent::getUpdateOrder()` / `ComponentUpdateOrder`，`BaseObject::Update()` 按 order 稳定驱动组件；AI/Render/Anim/Weapon 分别声明 100/200/300/400，`SoldierObject::Update` 只保留 transform 预同步、统一组件驱动和 stance 收尾。
- **C5 修正文档/代码漂移**（🟢 已解决 2026-06-11）：`cpp-object-model-refactor-roadmap.md` 已加 2026-05-29 订正 banner，声明 `GameObject` / `VehicleObject` 不存在、实际继承链为 `SandboxObject → BaseObject → AgentObject → SoldierObject`，文内残留引用已就地订正或标历史。

---

## 6. 分阶段执行计划

按“先打地基，再兑现减负，最后清理”的顺序。每个里程碑完成后更新本文档勾选状态并记录验证方式。

### Milestone 1：解耦地基（对应 P1）

1. 新增 `SandboxServices` 并下发到 `ObjectManager` / 对象创建链路。
2. 迁移热点访问点（AIController 找敌人/寻路、Locomotion 分离力、PhysicsComponent 取世界）到 context。
3. sandbox 组件 `.cpp` 去掉对 `GameManager`/`ClientManager` 的 include 与 `g_*` 引用。
4. 加静态检查门禁。

验收：组件不再反向依赖 game 层；`Sandbox6/7/8` 行为不变。

### Milestone 2：组件化兑现减负（对应 P2、P5、P6）

1. 补齐关键组件 tolua 导出，Lua/sample 改走 `GetComponent` 直取。
2. 组件 owner 统一 `BaseObject*`，跨组件依赖走 `FindComponent<T>()`。
3. 字符串 key 收口为类型查找 + 常量头。
4. 新增最小 `DummyAgent` 验证“复用组件、零复制”。

验收：新增对象类型不复制 Soldier 转发；组件可被非 Soldier 复用。

### Milestone 3：所有权与上帝类收敛（对应 P3、P4）

1. 收口 P4 剩余所有权语义：组件容器已迁 `unique_ptr`，生命周期断言/dump 已落地，关键缓存裸指针已标注 non-owning，继续做其它缓存裸指针审计。
2. 删/迁 `GameManager` 的 FGUI 转发壳，收敛 GameManager 职责。

验收：所有权可解释、销毁无悬空；`HELLO_FGUI_SELF_TEST_ALL=1` 与 production gate 通过。

### Milestone 4：AI 抽象与结构清理（对应 P7、C1–C5）

1. `PerceptionQuery` + driver 工厂。
2. 穿插完成 C1–C5 清理。

验收：AI 扩展不再硬扫全场 / 硬编码切换；冗余层级清理后 sample 行为不变。

---

## 7. 跟踪清单

> 状态符号：[x] 已解决 · [~] 部分完成 · [ ] 仍成立（待办）。2026-06-11 逐条对代码核实。

- [x] P1 SandboxServices 注入，去 sandbox→game 反向依赖与 `g_*` 热点 —— 服务已引入并下发，AI/Locomotion/FSM/Weapon/Physics/AgentObject/BlockObject/SoldierObject 热点已清，Render/Anim/Input include 已清，raycast 已下沉到 RaycastService 且 SandboxMgr 已删除，SceneFactory root node、UIManager camera、CameraService、NavigationMesh debug visual 与 ObjectManager 场景/时间访问已从 GameManager/ClientManager 解耦，manager `g_*` 已清零，静态门禁守住回归
- [~] P2 停止对象层转发样板，Lua/sample 走组件直取 —— `SoldierObject::Update` 已不再手写 AI/Render/Anim/Weapon update block，改由 `BaseObject::Update` + `IComponent::getUpdateOrder` 驱动；BaseObject typed component getter 已暴露给 Lua，agent 入口、`Sandbox2/3/5/6/10-13/16/17/18`、Chapter2/4 sample、DT/BT 条件、DT/BT action、`AgentUtils.lua`、`MoveHelpers.lua`、`MoveAction/PursueAction`、`ConfigManager` 和 `parity_trace.lua` 已通过 typed component getter / `AgentComponentAccess.lua` 优先走组件直取；`SoldierObject` getWeapon/AI/maxHealth/ammo/敌人查询/移动目标/射击/Enter*Anim、`AgentObject:getBody/GetAnimation/GetObjectASM` 与 `AgentObject` 纯 Locomotion facade 已撤出 Lua 导出，Soldier 这批 C++ 兼容转发也已删除；`AgentObject` target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` C++ pure Locomotion facade 已删除，AI/FSM/Soldier/ObjectManager 相关调用已改为 `FindComponent<AgentLocomotion>()` / `PhysicsComponent`；RuntimeDiag `ComponentProbeAgent` 已验证非 Soldier AgentObject 组件复用，并覆盖 Locomotion maxSpeed/target/targetRadius round-trip、Anim body ASM、Weapon ammo round-trip / `ShootBullet()`；**待**：其它对象门面和跨组件语义入口继续审计
- [x] P3 拆薄 GameManager FGUI 转发 —— `GameManager.h/.cpp` 已删除 FGUI public 转发壳，只保留 `FairyGuiRuntime` 注册；FGUI All selftest suite `30 / 30`
- [~] P4 组件容器 / manager service / navmesh map `unique_ptr` 化，清理 `SAFE_DELETE`/双重持有 —— `BaseObject::m_components`、`ObjectManager` 自有 registry/AI/tactics services 与 `NavigationService::m_navMeshes` 已迁 `unique_ptr` 并集中销毁，组件 lifecycle 断言/dump 已落地，BlockObject/AnimComponent/AgentObject/LuaScriptComponent/PhysicsComponent/AIController/OpenSteerAdapter 关键裸指针已标注 owning/non-owning，Lua env owner 与 AI cached enemy 在 detach 清空；Raycast/Scene/Script/Camera/ObjectFactory/FGUI facade 缓存指针也已标注 owning/non-owning；**待**：继续审计其它缓存裸指针
- [~] P5 组件 owner 统一 `BaseObject`，解除子类依赖 —— 组件已不持 `SoldierObject* owner` 成员，WeaponComponent 已移除 SoldierObject 依赖并以 BaseObject/RenderComponent/SandboxServices 工作，普通 AgentObject 已挂载并由 RuntimeDiag 运行验证 WeaponComponent；普通 AgentObject 已默认挂载 AnimComponent 并验证 body ASM；AnimComponent / SoldierAnimController 已走 `IAnimContextProvider`；DT/BT driver 与 Lua action 真实 owner 已泛化到 AgentObject，Lua 查询入口已改为 `GetAgentOwner()`，Lua action 回调签名已统一为 `u[AgentObject]`；Blackboard 与 AIController 已向 Lua 导出 `GetAgentOwner()`，RuntimeDiag 验证 Blackboard 非 Soldier owner round-trip；FSM 通用查询与 AgentActionContext 动画表现入口已走 AIController/Weapon/Attrib/AnimComponent/IAnimController；DeathState 已走 AnimComponent/IAnimContextProvider/IAnimController；AgentFactory 已提供轻量装配 profile 并经 `CreateAgentWithProfile` 暴露给 Lua；RuntimeDiag 已覆盖非 Soldier animated mesh + body ASM 状态请求；**待**：更完整非 Soldier 行为场景、profile 外部数据化与 legacy forwarder 收敛
- [x] P6 字符串 key → 类型化访问 + 常量头 —— `ComponentKeys.h` 已建，全量常量化，无散落字面量
- [x] P7 `PerceptionQuery` + driver 工厂 —— 感知已抽象（走 `AgentPerceptionQuery`，不再直扫 getAllAgents）；driver 创建已走 factory/registry；行为由 `Sandbox6/7/8` 回归守住
- [x] C1 删 `UIService` 空壳 —— `UIService.{h,cpp}` 已删除并从工程移除，Gorilla UI `CreateUIFrame`/`SetMarkupColor` 已由 `SandboxUI`/`UIManager` 承接
- [x] C2 `SandboxMgr` 减肥 —— 对象创建 `Create*` 已从 `SandboxMgr` 移到 `SandboxObjects`/`ObjectFactory`，`WeaponComponent` 创建 bullet 走 `SandboxServices.objectFactory`，Gorilla UI `CreateUIFrame`/`SetMarkupColor` 已从 `SandboxMgr` 移到 `SandboxUI`/`UIManager`，相机/profile 查询已从 `SandboxMgr` 移到 `SandboxCamera`/`CameraService`，导航配置/构建/查询已从 `SandboxMgr` 移到 `SandboxNav`/`NavigationService`，raycast 已从 `SandboxMgr` 移到 `SandboxRaycast`/`RaycastService`，scene/light/material 已从 `SandboxMgr` 移到 `SandboxScene`/`SceneService`，CallFile 已从 `SandboxMgr` 移到 `SandboxScript`/`ScriptService`，CppFSM flag 已迁到 `SandboxAgentConfig`/`AgentConfigService`，`SandboxMgr` class/global 已删除
- [x] C3 `ObjectManager` registry / lifecycle / AI update / tactics / navigation / scheduler / team 拆分 —— `ObjectRegistry` 已拆出对象 map、对象 id、Agent/Block 二级索引和查找；`ObjectLifecycleSystem` 已承接对象 update loop、待删对象移除、对象 event flush 与 scene cleanup；`AIUpdateSystem` 已承接 scheduler begin/tick、spatial rebuild、批量感知、TeamBlackboard sync 与 AI perf stats；`TacticalQueryService` 已包住 influence config/layer/source/sample/score/stats 与 navmesh 建图编排，`TacticalDebugDrawService` 已承接 tactical debug visual，`TacticalService` / `SandboxTactics` 已承接 Lua tactics 主入口；navmesh map 与销毁责任已迁到 `NavigationService`；AIScheduler Lua 配置/诊断入口已迁到 `SandboxAIScheduler` 且 Lua 不再兜底 ObjectManager；TeamBlackboard Lua 主入口已迁到 `SandboxTeam` 且 Lua 不再兜底 ObjectManager；ObjectManager scheduler/team/tactical 旧导出已删除
- [x] C4 `IComponent` update order 显式化 —— `ComponentUpdateOrder` 已落地，AI/Render/Anim/Weapon 声明顺序，`SoldierObject::Update` 不再手写组件更新列表
- [x] C5 修正 `cpp-object-model-refactor-roadmap.md` 术语漂移 —— 5/29 订正 banner 已加

## 8. 验证策略（按改动面取最小集）

- 对象 / 组件改动：对象创建销毁 + `ObjectManager:buildObjectDebugSummary`。
- `AgentObject` / `SoldierObject` / AI driver 改动：`Sandbox6 / Sandbox7 / Sandbox8` 移动、射击、状态切换、寻路。
- FGUI / GameManager 转发改动：对应 `HELLO_FGUI_*_SELF_TEST`，复杂改动跑 `HELLO_FGUI_SELF_TEST_ALL=1` 与 `tools/run_fgui_production_gate.ps1`。
- Lua 绑定改动：同步检查 `.pkg`、生成 cpp、Lua 调用点。
- 平台兼容：检查 Windows / macOS 分支条件编译。

---

## 9. 2026-07 代码复审补充（P8–P11）

> 来源：2026-07-02 一次直接通读 `src/HelloOgre3D` C++ 代码的复审，记录 §2–§7 尚未覆盖或轻描淡写的结构性问题。方法同 §5 前言：给 file:line 证据、方案和验收，小步可回退。
>
> **方向前提（2026-07-02，经讨论修订）**：长期品类暂不锁定；核心方向是把这套 AI / 寻路 / 多 agent 技术接到一个“能上手玩”的最小应用上（先在现有战斗 sample 加“玩家操控一个单位”的纵切片探路），再定品类。详见 `docs/project-direction.md` 方向说明。**本节 P8–P11 属于“无悔动作”——不管最终做成中间件还是游戏都需要的地基清理，优先级不受品类决定影响**；尤其 P9 应在采集性能基线之前完成。
>
> 复审同时确认的**健康项**（无需再动）：sandbox/runtime 反向 include 与裸 `g_*` 实测均为 0（P1 门禁生效）；旧 `SandboxMgr` 类、全局实例与 Lua 导出已删除；`ComponentKeys` 常量化到位；`SandboxToLua.cpp` 的 14961 行是 tolua 生成物（源头是 46 行 pkg 清单 + 各头文件导出标记），非手工肥大化。

### P8：分层破口——component 反向依赖 ObjectManager / 具体对象（🔴 新增）

> 状态（2026-07-02）：**新发现，未处理**。P1 只堵住了对 `GameManager` / `ClientManager` 的反向依赖，没堵对 `ObjectManager`（上帝对象）和具体 `*Object` 的反向依赖。

#### 现状证据

- 多个组件 `.cpp` 直接 `#include "systems/manager/ObjectManager.h"`：`AIController.cpp`、`WeaponComponent.cpp`、`AgentLocomotion.cpp`。
- [AIController.cpp:18-28](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L18-L28) 还额外 include 了 `objects/AgentObject.h` + `objects/SoldierObject.h` + behavior/decision/fsm/perception/tactics 全家桶——一个“组件”认识 systems 层的 `ObjectManager` 和具体对象类。
- `AnimComponent.cpp` 反向 include AI 层 `ai/fsm/AgentStateController.h`、`ai/fsm/states/AgentState.h`。

#### 问题

运行时虽走 `SandboxServices`（非裸 `g_*`，故门禁不报），但编译期把 components 与 systems/objects/ai 三层焊死，形成 `component → ObjectManager → 各子系统` 的向上/环状依赖，违反 `runtime → sandbox → game` 的分层预期，也阻碍组件被单测/复用。

#### 解决方案

1. 组件只依赖 core / 接口；对 `ObjectManager` 的需求收口为 `SandboxServices` 上的**窄接口**（如 `IAgentQuery`），头文件前向声明 + 接口指针，不 include `ObjectManager.h`。
2. `AIController` 对具体 `SoldierObject` 的依赖并入 P5 的 owner 泛化收尾。
3. 门禁扩展：`tools/check_sandbox_architecture.ps1` 增加规则，禁止 `sandbox/components/**` 反向 include `systems/manager/ObjectManager.h` 与 `objects/*Object.h`。

#### 验收

- [ ] `components/**` 不再 include `ObjectManager.h` 与具体 `*Object.h`。
- [ ] 门禁覆盖该规则，防回流。
- [ ] `Sandbox6/7/8` 行为不变。

### P9：死代码节流 + 热点组件访问（🟡 部分完成，性能基线前必清）

> 状态（2026-07-10）：**P9 完成**——伪节流、`USE_CPP_FSM` 死宏与热点组件全表扫描已清理，Release 基线已刷新（`docs/perf/ai-perf-release-baseline-20260710.md`）。`Agent_Update` 明确保持每次对象/AI tick 调用；降频由 AIScheduler、BT tick interval 等显式策略负责，不恢复跨 agent 共享的静态计时器。热点 getter 已改读 `BaseObject` 侧缓存指针。

#### 现状证据

- ~~帧节流被 `|| true` / `forceUpdate=true` 短路~~（2026-07-10 已清理）：两处 `Agent_Update` 现在直接表达“随对象/AI tick 调用”的真实行为；原静态计时器跨所有 agent 共享，不能作为合法节流恢复。
- ~~`USE_CPP_FSM` 死宏与注释 `#ifdef`~~（2026-07-10 已删除）：实际逻辑只走 `SandboxServices.agentConfig → AgentConfigService` flag。
- ~~`FindComponent<T>()` 热点全表扫描~~（2026-07-10 已清理）：`BaseObject` 现持 6 个 non-owning 缓存指针（ai/weapon/anim/attrib/locomotion/physics），在 `AddComponent`/`RemoveComponent` 时 `RefreshComponentCache()` 重建一次（语义等价 `FindComponent<T>()` 的“map 顺序首个匹配”）；另加 C++-only `GetPhysicsComponent()` 与 `const GetLocomotionComponent()` 供跨组件 / const 热点读缓存。已把每帧 / 每 tick 热点全部从 `FindComponent<T>()` 改走缓存 getter：对象层 transform getter（AgentObject/SoldierObject）、对象更新循环 AIController tick（ObjectLifecycleSystem）、感知（AgentPerceptionSystem、AgentPerceptionQuery sight origin）、团队（TeamBlackboardService）、FSM（AgentStateController / PursueState / DeathState）、动作上下文（AgentActionContext）、DT/BT action（LuaDecisionAction / LuaBehaviorAction）、locomotion 邻居避让与 ApplyForce（AgentLocomotion）、render 同步（RenderComponent）、player（PlayerController）。仅保留非热点 `FindComponent`：`GetLuaScript`(LuaScript) 与未缓存类型（PlayerController / RenderComponent），以及 `ObjectManager` 的诊断 / 计数汇总（RuntimeDiag-only，非每帧）。

#### 解决方案

1. ~~明确节流意图并修正~~：已选择行为保持型清理，删除无用计时器、强制分支和 `USE_CPP_FSM` 死宏；降频只走显式 scheduler/driver policy。
2. 热点组件（physics/locomotion/ai）在对象 attach 时解析一次并缓存 non-owning 指针，getter 不再每帧 `FindComponent`。
3. 明文化方针：**不引入完整 ECS，hot component 由对象侧缓存指针**（见 §暂缓清单，避免将来揺り戻し）。

#### 验收

- [x] 两处伪节流与 `USE_CPP_FSM` 死宏清理，行为与注释一致；Release x64、`Sandbox2`、`Sandbox8`、`ai_perf_100` smoke 已通过。
- [x] 热点 getter 不再每帧全表扫描 + dynamic_cast（`BaseObject` 缓存指针 + `AddComponent`/`RemoveComponent` 刷新），覆盖对象层与 AI tick 路径（感知 / 团队 / FSM / DT-BT action / locomotion / lifecycle / render 同步）；Release x64 rebuild、`Sandbox6/7/8/10/12/19`、`ai_perf_100` smoke 与 `check_sandbox_architecture` 已通过（2026-07-10）。
- [x] 清理后再采集 `ai_perf_100/500/1000` 基线（`docs/perf/ai-perf-release-baseline-20260710.md`，2026-07-10）：candidates 与 20260612 逐值相同（确定性一致），AI 分项同量级或略优（perceptionSystem 1000 agent 23.67→21.8），无回归；P9 组件缓存的直接收益在聚合帧指标里低于噪声底，属累积刷新非受控 A/B。

### P10：BaseObject 被 typed getter 领域污染（🟡 新增）

> 状态（2026-07-02）：**新发现**。是 P2“只藏不减”的另一种表现；长期品类尚未锁定时，更应保持 entity root 干净，避免把具体玩法组件固化到通用基类。

#### 现状证据

- [BaseObject.h:53-56](src/HelloOgre3D/sandbox/core/object/BaseObject.h#L53-L56) 把 `GetAIComponent()/GetWeaponComponent()/GetAnimComponent()/GetAttribComponent()` 直接 tolua 导出在通用实体根上，基类前向声明并返回 `AIController/WeaponComponent/AnimComponent/AgentAttrib` 这些具体玩法组件。

#### 解决方案

- Lua 侧 typed 访问改走 `GetComponent(key)` + 组件自身导出（`ComponentKeys` 已就绪），逐步把这 4 个 getter 标为 legacy 并随 sample 迁移撤下，让 `core/object` 不认识具体玩法组件。

#### 验收

- [ ] `BaseObject` 不再前向声明/返回具体玩法组件类型（保留兼容期标注）。
- [ ] Lua/sample 通过组件直取，行为不变。

### P11：event dispatcher 过度设计（🟡 新增）

> 状态（2026-07-02）：**新发现**。呼应 `docs/memory/no-greenfield-event-infra.md`——不新建事件系统，但现有 `SandboxEventDispatcherManager` 值得内部收敛。Context/Dispatcher/Payload 三件套本身干净，问题集中在 Manager。

#### 现状证据

- [SandboxEventDispatcherManager.h](src/HelloOgre3D/sandbox/core/event/SandboxEventDispatcherManager.h)（491 行，全内联单例）把队列/限流/路由/字符串 DSL 事件名/数值匹配五种关注点聚在一处：
  - 字符串 DSL 事件名 `name?key=val&k2=v2`，运行时手写解析 `?`/`&`/`=`（`:377-401`）。
  - 数值匹配 string↔double 往复，用 `numeric_limits::max()` 当哨兵（`:429-444`）。

#### 解决方案

- 内部按“订阅路由 / 队列限流 / 参数匹配 / 调试统计”拆到独立实现文件；去掉字符串 DSL 事件名，改用显式 typed filter（保持现有订阅 API 兼容）。

#### 验收

- [ ] Manager 头文件不再承担全部实现；字符串 DSL 事件名下线。
- [ ] `-AiEventSelfTest` 通过，事件路由行为不变。

### 对既存条目的状态注记（2026-07-02 复审）

- **P2（对象层转发本体）**：仍是主体债务，`AgentObject.cpp` 过半、`SoldierObject.cpp` 大量方法仍是“找组件→判空→转发”。建议配套决策：把 sample 分成「冻结兼容」与「跟随主线」两类，只对后者设一个真正删除 legacy forwarder 的截止点，终止“两份成本”的中途状态。
- **C3（ObjectManager 减负）**：旧 tactical/team Lua facade 已删除，当前 `ObjectManager.h` 剩余公开面主要是对象/导航兼容 API、调度/AI 服务 getter 与少量 debug 入口（[ObjectManager.h:63-104](src/HelloOgre3D/sandbox/systems/manager/ObjectManager.h#L63-L104)）。后续重点不是再按“约 60 个 tactical/team 转发”批量删除，而是继续缩小对象/导航兼容面，并在需要时把 debug summary 收到 diagnostics service。
