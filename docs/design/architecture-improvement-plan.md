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
> - 🟢 **已解决**：P1（SandboxServices 注入已落地；sandbox/runtime 对 `GameManager.h` / `ClientManager.h` 反向 include 与 `g_ObjectManager` / `g_SandboxMgr` / `g_GameManager` 直接引用已清零，并由 `tools/check_sandbox_architecture.ps1` 守住）、P3（`GameManager` 的 FGUI 转发壳已删除，Lua 侧只走 `FairyGuiRuntime`）、P6（ComponentKeys.h 常量化）、C1（`UIService` 空壳已删除，Gorilla UI Lua 入口已从 `SandboxMgr` 下沉到 `SandboxUI`/`UIManager`）、C4（`IComponent::getUpdateOrder()` + `BaseObject::Update()` 统一驱动组件顺序，`SoldierObject::Update` 不再手写组件列表）、C5（cpp-roadmap 术语已订正）。
> - 🟠 **部分完成**：P2（Soldier update 样板已迁为组件 order；BaseObject 已给 Lua 暴露 AI/Weapon/Anim/Attrib typed component getter，agent 入口与 `Sandbox10`-`Sandbox13`、`Sandbox17`、`parity_trace.lua` 通过 `AgentComponentAccess.lua` 优先走组件直取；RuntimeDiag `ComponentProbeAgent` 已验证非 Soldier `AgentObject` 复用 AI/Attrib 等 6 个组件且无需 Soldier forwarder；对象层 150+ legacy forwarder 仍待处理）、P4（BaseObject 组件容器已迁到 `unique_ptr`，组件 lifecycle state、attach/destroy/update 断言和 debug dump 状态输出已落地；BlockObject/AnimComponent/AgentObject/OpenSteer/LuaScript/Physics/AIController 关键裸指针已标出 owning/non-owning，并补了 Lua env owner 与 AI cached enemy detach 清理；剩余为更广的缓存裸指针审计）、P5（WeaponComponent 与动画运行链已脱 `SoldierObject` 直接依赖，`AnimComponent` / `SoldierAnimController` 通过 `IAnimContextProvider` 取上下文；DT/BT driver 与 Lua action owner 已泛化到 `AgentObject`；Blackboard 已向 Lua 导出 `GetAgentOwner()` 泛化入口并由 RuntimeDiag 验证非 Soldier owner round-trip；FSM action context 的敌人/移动/弹药/血量查询与动画表现入口已走组件，DeathState 已走 AnimComponent/IAnimContextProvider/IAnimController，旧 SoldierObject owner 仅保留为 Lua 兼容桥）、C2（对象创建 `CreatePlane/CreateAgent/CreateSoldier/...` 已从 `SandboxMgr` 下沉到 `SandboxObjects`/`ObjectFactory`，Gorilla UI 创建/颜色入口已下沉到 `SandboxUI`/`UIManager`，相机/profile 查询已下沉到 `SandboxCamera`/`CameraService`，导航配置/构建/查询已下沉到 `SandboxNav`/`NavigationService`，raycast 已下沉到 `SandboxRaycast`/`RaycastService`，scene/light/material 已下沉到 `SandboxScene`/`SceneService`，CallFile 已下沉到 `SandboxScript`/`ScriptService`，`SandboxMgr` 仅保留 CppFSM flag 与旧 Lua 兼容转发）、C3（`ObjectRegistry` 已拆出对象 map 与 Agent/Block 二级索引，`ObjectLifecycleSystem` 已承接对象 update loop 与 scene cleanup stage，AI 帧调度/感知/Team sync 已收口到 `AIUpdateSystem`，tactics query/debug draw 已下沉到 `TacticalQueryService` / `TacticalDebugDrawService`，navmesh map 与销毁责任已迁到 `NavigationService`，AIScheduler Lua 主入口已迁到 `SandboxAIScheduler`，TeamBlackboard Lua 主入口已迁到 `SandboxTeam`）。
> - 🟢 **已解决待回归**：P7（感知已抽象走 `AgentPerceptionQuery`，driver 创建已走 factory/registry；仍需用 FSM/DT/BT sample 回归守住行为）。
> - 🟠 **P2 最新收窄（2026-06-19）**：`SoldierObject` 的 `getWeapon`、`GetAI/GetAIController`、maxHealth 和 ammo 纯组件转发已从 Lua 导出撤下；DT/BT 条件、射击/换弹 action、Chapter9 legacy agent、`SoldierAgent` 与 `Sandbox3` 改走 `AgentComponentAccess.lua` 或 typed component getter。剩余 P2 主体仍是 AgentObject/其它 Soldier legacy forwarder 继续收敛。
> - 🔴 **仍成立**：P2 仍有主体债务（legacy forwarder）未清；C3 剩余为 tactical/team/scheduler 旧 Lua 兼容 facade 的最终删除；C2 剩余为旧 `SandboxMgr` 兼容转发的最终删除与 CppFSM flag 归属收口。
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
| P2 | 🟠 | 组件化“只藏不减”，对象层 150+ 转发样板；update 样板已改为组件 order，Lua typed component getter 与 clean sample 迁移已落地；Soldier getWeapon/AI/maxHealth/ammo 纯转发已撤出 Lua 导出 | 新对象类型不再复制 Soldier update block；DT/BT/knowledge/team/influence sample 可优先直取组件，但 AgentObject/其它 legacy forwarder 仍待迁 | `BaseObject::GetAIComponent/GetWeaponComponent/GetAnimComponent/GetAttribComponent`、`AgentComponentAccess.lua`、`AgentObject.cpp:301-341`、`SoldierObject.cpp:203-270` |
| P3 | 🟢 | `GameManager` 的 FGUI 转发壳已删除 | FGUI API 不再挂在编排层 | `GameManager.h` 约 79 行，`rg "GameManager::.*FairyGui"` 为 0 |
| P4 | 🟠 | BaseObject 容器已 `unique_ptr` 化，组件 lifecycle 断言/dump 已落地，多处关键裸指针已标注 owning/non-owning 并补 detach 清理 | 悬空/重复删除隐患下降，仍需继续审计其它缓存裸指针 | `IComponent.h` lifecycle state、`BaseObject::DestroyComponent`、`BuildComponentDebugString()`、`AIController::onDetach`、`LuaScriptComponent::onDetach` |
| P5 | 🟠 | WeaponComponent、动画运行链、DT/BT driver、Lua action owner、Blackboard Lua owner、AgentActionContext、DeathState 与 FSM 通用查询已脱 SoldierObject 主路径 | 组件/AI driver 复用性提高，旧 Lua Soldier 签名兼容桥仍需随 sample 迁移继续收窄 | `IAnimContextProvider.h`、`DecisionTreeDriver::GetAgentOwner`、`Blackboard::GetAgentOwner`、`AgentActionContext::GetAnimController` |
| P6 | 🟡 | 组件 key 是散落的魔术字符串 | 易拼写漂移、无编译检查 | `"anim"/"weapon"/"ai"/...` 散落多 cpp |
| P7 | 🟡 | AI 感知无抽象，硬扫全场 + driver 切换硬编码 | agent 增多即 O(n²)，扩展困难 | `AIController.cpp:167`、`AIController.cpp:333-413` |
| C1 | 🟢 | `UIService` 空壳已删除，Gorilla UI Lua 入口已由 `SandboxUI`/`UIManager` 承接 | 少一层无意义中间层，UI API 不再挂在 SandboxMgr | `GameManager::Initialize` 注册 `SandboxUI`、`UIManager.h` tolua 导出、`SandboxMgr` 不再导出 UI 方法 |
| C2 | 🟠 | `SandboxMgr` 对象创建转发已下沉到 `SandboxObjects`/`ObjectFactory`，Gorilla UI 转发已下沉到 `SandboxUI`/`UIManager`，相机/profile 查询已下沉到 `SandboxCamera`/`CameraService`，导航配置/构建/查询已下沉到 `SandboxNav`/`NavigationService`，raycast 已下沉到 `SandboxRaycast`/`RaycastService`，scene/light/material 已下沉到 `SandboxScene`/`SceneService`，CallFile 已下沉到 `SandboxScript`/`ScriptService` | 层级冗余下降，`SandboxMgr` 只剩 CppFSM flag 与旧 Lua 兼容转发 | `GameManager::Initialize` 注册 `SandboxObjects`/`SandboxUI`/`SandboxCamera`/`SandboxNav`/`SandboxRaycast`/`SandboxScene`/`SandboxScript`；对应 service 已 tolua 导出；Lua sample 已迁到新全局 |
| C3 | 🟠 | `ObjectRegistry` 已拆出对象 map 与 Agent/Block 二级索引，`ObjectLifecycleSystem` 已承接对象 update loop 与 scene cleanup stage，AI scheduler/perception/team sync 已收口到 `AIUpdateSystem`，tactics query/debug draw 已下沉到 service，navmesh map 与销毁责任已迁到 `NavigationService`，AIScheduler Lua 主入口已迁到 `SandboxAIScheduler`，TeamBlackboard Lua 主入口已迁到 `SandboxTeam` | registry、lifecycle、AI update、tactical、navigation ownership、scheduler/team Lua API 边界清晰一层，后续可继续拆旧兼容 facade | `ObjectRegistry.h`、`ObjectLifecycleSystem.h`、`AIUpdateSystem.h`、`TacticalDebugDrawService.h`、`NavigationService.h`、`AIScheduler.h`、`TeamBlackboardService.h` |
| C4 | 🟢 | `Update` 顺序已显式化到组件 order | 新组件可声明顺序，Soldier 不再手写组件 update block | `IComponent.h:9-16`、`BaseObject.cpp:76-93`、`SoldierObject.cpp:154-162` |
| C5 | 🟢 | 文档/代码漂移 | 路线图引用了不存在的类 | 代码无 `GameObject`/`VehicleObject` |

---

## 3. 🔴 第一梯队（优先解决，是其他重构的前提）

### P1：消灭反向依赖与全局单例，引入 SandboxServices 注入

> 🟢 状态（2026-06-18）：**已解决，持续由门禁守住**。`sandbox/core/SandboxServices.h` 已落地并下发（`ObjectManager` 填充 → `BaseObject::SetSandboxServices` → `IComponent::onSandboxServicesChanged()` / `GetSandboxServices()`），AIController / PhysicsComponent / WeaponComponent / AgentLocomotion / AgentStateController 的热点已改为 `Resolve*()` 只走服务；AgentObject / BlockObject 的死亡/碰撞事件发布、粒子清理和 FSM flag 读取也只走 `SandboxServices`；SoldierObject 通过 `SandboxServices.input` 获取输入；RenderComponent / AnimComponent 已移除 `GameManager.h`；InputManager 通过构造注入 `RenderWindow` / `OgreCameraController` / 状态回调，不再 include `ClientManager.h`；`SandboxMgr::RayCastObjectId` 已进一步薄转发到 `RaycastService`，由应用层注入 `PhysicsWorld` 并导出 Lua 全局 `SandboxRaycast`；scene/light/material 已下沉到 `SceneService`，由应用层注入 `SceneManager` / `CameraService` 并导出 Lua 全局 `SandboxScene`；CallFile 已下沉到 `ScriptService`，由应用层注入 `ScriptLuaVM` 并导出 Lua 全局 `SandboxScript`；`SceneFactory` 通过 `SetRootSceneNode` 接收应用层 root scene node，移除 `GameManager.h` include；`UIManager` 通过构造注入 `Ogre::Camera*`，`UIService` 移除无用 `GameManager.h` include；`CameraService` 通过构造注入 camera / scene manager / profile time getter，不再持 `ClientManager*`；`NavigationMesh` debug visual 与 `ObjectManager` 场景访问改走 `SceneFactory`，`ObjectManager` 当前时间由 `GameManager` 每帧写入；`ObjectManager::GetInstance` / `SandboxMgr::GetInstance` 及 `g_ObjectManager` / `g_SandboxMgr` 已删除。找不到服务时显式返回空并走原调用点的降级分支，不再回退 `g_*`。`tools/check_sandbox_architecture.ps1` 已作为静态门禁，防止新增 sandbox/runtime 反向依赖和裸 `g_*`。下方"现状证据"为改造前快照。

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

> 🟠 状态（2026-06-18）：**前置能力已落地，主体迁移继续**。`BaseObject` 已向 Lua 暴露 `GetAIComponent()` / `GetWeaponComponent()` / `GetAnimComponent()` / `GetAttribComponent()`，`AIController`、`WeaponComponent`、`AnimComponent`、`AgentAttrib` 的常用接口已补 tolua 导出；`bin/res/scripts/agent/AgentComponentAccess.lua` 统一封装 AI/Attrib/Health 组件优先访问，agent 目录内旧 `agent:GetAI()` / `agent:GetMaxHealth()` 直接调用已收敛到该 helper 的兼容兜底，`Sandbox10`-`Sandbox13`、`Sandbox17` 与 `parity_trace.lua` 的 Blackboard / tactics 入口也已迁为组件优先。RuntimeDiag 新增 `ComponentProbeAgent.lua` 自测，创建非 Soldier `AgentObject` 并验证 `ai/attrib/locomotion/physics/render/script` 6 个组件、AI/Attrib typed getter 和 Health round-trip。对象层大批单组件 legacy forwarder 仍保留。

> 2026-06-19 增量：`AgentComponentAccess.lua` 的 Weapon/AI/maxHealth/ammo helper 已不再回退 `SoldierObject` 方法；`SoldierObject` Lua 绑定撤下 `getWeapon`、`GetAI/GetAIController`、`SetMaxHealth/GetMaxHealth`、`SetAmmo/GetAmmo/SetMaxAmmo/GetMaxAmmo/HasAmmo/ConsumeAmmo/RestoreAmmo`；DT/BT 条件、`ShootAction`、`ReloadAction`、Chapter9 legacy agent、`SoldierAgent` 和 `Sandbox3` 均改走 typed component getter。

#### 现状证据

- `AgentObject`（≈898 行）+ `SoldierObject`（≈703 行）合计转发 **150+ 个方法**，全是同一种“查组件 → 判空 → 转发”样板：
  - [AgentObject.cpp:301-341](src/HelloOgre3D/sandbox/objects/AgentObject.cpp#L301-L341)（`SetMass/SetHeight/SetRadius` 同时转发 Locomotion 和 Physics）
  - [SoldierObject.cpp:203-270](src/HelloOgre3D/sandbox/objects/SoldierObject.cpp#L203-L270)（`SetMaxHealth/SetAmmo/ConsumeAmmo` 转发 Attrib/Weapon）
- 组件把实现搬走了，对象又原样转发回来，等于既付复杂度又没瘦身。

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

- [~] 关键组件（Weapon/Attrib/Anim/AIController）常用方法已 tolua 导出，Lua 可通过 `BaseObject` typed getter 直取；agent 入口、`Sandbox3`、`Sandbox10`-`Sandbox13`、`Sandbox17`、DT/BT 条件、射击/换弹 action 和 `parity_trace.lua` 已通过 typed component getter / `AgentComponentAccess.lua` 迁到组件优先，Soldier getWeapon/AI/maxHealth/ammo 纯转发已撤出 Lua 导出；仍待迁移 AgentObject/其它 Soldier forwarder。
- [x] 新增最小 `ComponentProbeAgent` 运行时诊断样例：复用非 Soldier `AgentObject` 的 ≥2 个组件（实际 6 个），且不复制 Soldier 的转发代码。
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

> 🟠 状态（2026-06-19）：**核心完成，审计继续**。`BaseObject` 内部组件容器已从 `std::map<std::string, IComponent*>` 迁到 `std::map<std::string, std::unique_ptr<IComponent>>`，`AddComponent` 成功后由 `BaseObject` 接管所有权，`GetComponent` / `FindComponent` 仍返回非拥有裸指针以保持调用面兼容；销毁链集中走 `BaseObject::DestroyComponent(ComponentPtr&)` 并保持 `onDestroy()` → `onDetach()` 顺序。`IComponent` 已记录 lifecycle state，`BaseObject` 在 attach/destroy/update 断言组件状态，`BuildComponentDebugString()` 会输出 `key:state`。`BlockObject` 缓存的组件/owner 指针、`AnimComponent` 缓存的 `Ogre::Entity*`、`AgentObject::m_renderComp`、`OpenSteerAdapter::m_owner`、`LuaScriptComponent` 的 VM/env owner、`PhysicsComponent::m_addedWorld` 与 `AIController::m_enemy` 已标注 owning/non-owning 语义；`LuaScriptComponent::onDetach` 会清掉 local env owner，`AIController::onDetach` 会清掉 cached enemy。**仍待**：继续审计其它缓存裸指针是否需要同样语义标注。

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
- [x] `BlockObject` / `AnimComponent` / `AgentObject` / `LuaScriptComponent` / `PhysicsComponent` / `AIController` / `OpenSteerAdapter` 关键裸指针明确 owning 或 non-owning 语义，且无对“map 已拥有对象”的二次手动 delete。
- [x] 组件生命周期 debug dump / 断言补齐，避免后续新组件重新引入双重持有。
- [ ] 对象创建/销毁 + `ObjectManager:buildObjectDebugSummary` 正常，无崩溃/泄漏。

---

### P5：组件 owner 统一为 BaseObject，解除对具体子类的依赖

> 🟠 状态（2026-06-18）：**部分完成**。`AnimComponent` / `WeaponComponent` 都不再持有 `SoldierObject* owner` 成员，owner 通过 `IComponent::getOwner()` 观察；`WeaponComponent` 的跨组件访问已收口为 `owner->FindComponent<RenderComponent>()`，服务访问也不再回退 `g_*`，并已移除对 `SoldierObject` 的 include / dynamic_cast，射击链只依赖 `BaseObject` id/team、`RenderComponent` 朝向和 `SandboxServices`。动画链新增 `IAnimContextProvider`，`AnimComponent` 通过该接口读取 stance / cpp-fsm flag / shoot hook / FSM controller，`SoldierAnimController` 只持接口指针，不再 include 或调用 `SoldierObject`。DT/BT driver 已以 `AgentObject*` 作为真实 owner，Lua action 直接持 `AgentObject* + Blackboard*`，旧 `GetOwner()` / `u[SoldierObject]` 回调仅作为现有脚本兼容桥保留。FSM `AgentActionContext` 已把敌人、移动目标、血量、弹药查询下沉到 AIController / WeaponComponent / AgentAttrib，并把 idle/move/shoot/reload/presentation 动画入口改为 `AnimComponent` / `IAnimController`；Move/Shoot/Pursue/Reload state 与 transition evaluator 不再直接 include/cast `SoldierObject`；`DeathState` 死亡动画入口已走 `AnimComponent` / `IAnimContextProvider` / `IAnimController`，不再直接 include/cast `SoldierObject`。**仍待**：旧 Lua `u[SoldierObject]` 签名兼容桥随 sample 迁移继续收窄。

#### 现状证据

- `AnimComponent` 已改为 `GetAnimContext()`，只依赖 `IAnimContextProvider`；`SoldierObject` 负责把自身适配为动画上下文。
- `SoldierAnimController` 已改为持有 `IAnimContextProvider*`，通过接口获取 body/weapon ASM 与 stance，不再持 `SoldierObject* owner`。
- `WeaponComponent` 跨组件访问已改 `FindComponent<RenderComponent>()`，`DoShootBullet` 以 `BaseObject` owner id/team 作为 bullet owner / tactical event 来源。
- `DecisionTreeDriver` / `BehaviorTreeDriver` 真实 owner 已改为 `AgentObject*`；Lua action 优先使用 driver 注入的 `Blackboard*`，并在 owner 不是 Soldier 时按 `u[AgentObject]` 调 Lua 回调。
- `AgentActionContext` 的敌人/移动/弹药/血量查询已走 AIController / WeaponComponent / AgentAttrib，动画表现入口已走 AnimComponent / IAnimController；FSM evaluator 和 Move/Shoot/Pursue/Reload state 不再直接依赖 `SoldierObject`。

#### 解决方案

1. 组件 owner 类型统一为 `BaseObject*`（`IComponent` 已是此约定，收口子类里偷用的具体类型）。
2. 跨组件依赖一律走 `owner->FindComponent<T>()`，不调具体对象方法（如用 `FindComponent<RenderComponent>()->GetSceneNode()` 替代 `owner->GetRenderComponent()`）。
3. Soldier 专有数据（stance、useCppFSM）先由 `IAnimContextProvider` 适配，后续可继续下沉为组件状态或通过 Blackboard/Attrib 暴露，组件不读具体对象类型字段。

#### 验收

- [x] `AnimComponent`、`WeaponComponent` 不再出现对 `SoldierObject` 具体方法的直接调用；`SoldierAnimController` 不再持 `SoldierObject* owner`。
- [x] `DecisionTreeDriver` / `BehaviorTreeDriver` / `LuaDecisionAction` / `LuaBehaviorAction` 的真实 owner 不再是 `SoldierObject*`，非 Soldier agent 可创建 DT/BT driver；旧 Lua 签名保持兼容。
- [x] `AgentStateEvaluators` 与 Move/Shoot/Pursue/Reload state 不再直接 include/cast `SoldierObject`；通用查询经 `AgentActionContext` 转到组件。
- [x] `AgentActionContext` 的 idle/move/shoot/reload/presentation 动画入口不再直接 include/cast `SoldierObject`，改走 `AnimComponent` / `IAnimController`。
- [ ] 至少一个非 Soldier 对象成功挂载 `WeaponComponent` 或 `AnimComponent` 并运行。

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
- **C2 `SandboxMgr` 减肥**（🟠 主体转发已解决 2026-06-19）：`CreatePlane/CreateBlock/CreateBullet/CreateAgent/CreateSoldier` 等对象创建 API 已从 `SandboxMgr` 移到 `ObjectFactory` tolua 导出，并由 Lua 全局 `SandboxObjects` 调用；`WeaponComponent` 创建 bullet 改走 `SandboxServices.objectFactory`；Gorilla UI frame/color API 已从 `SandboxMgr` 移到 `SandboxUI`/`UIManager`；相机与 profile 查询已从 `SandboxMgr` 移到 `SandboxCamera`/`CameraService`；导航 `DefaultConfig/ApplySettingConfig/CreateNavigationMesh/RandomPoint/FindClosestPoint/FindPath` 已移到 `SandboxNav`/`NavigationService`；raycast 已移到 `SandboxRaycast`/`RaycastService`；scene/light/material/scene graph 更新已移到 `SandboxScene`/`SceneService`；CallFile 已移到 `SandboxScript`/`ScriptService`。`SandboxMgr` 仅保留 CppFSM flag 与旧 Lua 兼容转发，后续等旧入口彻底删除时收口。
- **C3 `ObjectManager` 职责拆分**（🟠 registry/lifecycle/AI update/tactics/navigation/scheduler/team 第一批切片已解决 2026-06-19）：`ObjectRegistry` 已承接对象 map、对象 id 分配、Agent/Block 二级索引和 `getObjectById` 查找；`ObjectLifecycleSystem` 已承接对象生命周期/update loop、待删对象移除、对象 event flush 与场景节点延迟清理；`AIUpdateSystem` 已承接 scheduler begin/tick、spatial rebuild、`AgentPerceptionSystem` 批量 update、`TeamBlackboardService` 每帧 sync 与 AI perf stats 写回；`TacticalQueryService` 已包住 influence config/layer/source/sample/score/stats 与 navmesh 建图编排，`TacticalDebugDrawService` 已承接 tactical debug visual 的 Ogre 绘制生命周期，`ObjectManager` 的 Lua 入口只做薄转发；navmesh map 与销毁责任已迁到 `NavigationService`，ObjectManager 只保留 fixed blocks 来源与兼容转发；AIScheduler Lua 配置/诊断入口已迁到 `SandboxAIScheduler`，TeamBlackboard Lua 主入口已迁到 `SandboxTeam`，ObjectManager 只保留旧入口兼容转发。下一步按 sample 节奏删除旧兼容 facade。
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

- [x] P1 SandboxServices 注入，去 sandbox→game 反向依赖与 `g_*` 热点 —— 服务已引入并下发，AI/Locomotion/FSM/Weapon/Physics/AgentObject/BlockObject/SoldierObject 热点已清，Render/Anim/Input include 已清，SandboxMgr raycast 已薄转发到 RaycastService，SceneFactory root node、UIManager camera、CameraService、NavigationMesh debug visual 与 ObjectManager 场景/时间访问已从 GameManager/ClientManager 解耦，manager `g_*` 已清零，静态门禁守住回归
- [~] P2 停止对象层转发样板，Lua/sample 走组件直取 —— `SoldierObject::Update` 已不再手写 AI/Render/Anim/Weapon update block，改由 `BaseObject::Update` + `IComponent::getUpdateOrder` 驱动；BaseObject typed component getter 已暴露给 Lua，agent 入口、`Sandbox3`、`Sandbox10`-`Sandbox13`、`Sandbox17`、DT/BT 条件、射击/换弹 action 和 `parity_trace.lua` 已通过 typed component getter / `AgentComponentAccess.lua` 优先走组件直取；`SoldierObject` getWeapon/AI/maxHealth/ammo 纯转发已撤出 Lua 导出；RuntimeDiag `ComponentProbeAgent` 已验证非 Soldier AgentObject 组件复用；**待**：AgentObject/其它 Soldier legacy forwarder 继续收敛
- [x] P3 拆薄 GameManager FGUI 转发 —— `GameManager.h/.cpp` 已删除 FGUI public 转发壳，只保留 `FairyGuiRuntime` 注册；FGUI All selftest suite `30 / 30`
- [~] P4 组件容器 `unique_ptr` 化，清理 `SAFE_DELETE`/双重持有 —— `BaseObject::m_components` 已迁 `unique_ptr` 并集中销毁，组件 lifecycle 断言/dump 已落地，BlockObject/AnimComponent/AgentObject/LuaScriptComponent/PhysicsComponent/AIController/OpenSteerAdapter 关键裸指针已标注 owning/non-owning，Lua env owner 与 AI cached enemy 在 detach 清空；**待**：继续审计其它缓存裸指针
- [~] P5 组件 owner 统一 `BaseObject`，解除子类依赖 —— 组件已不持 `SoldierObject* owner` 成员，WeaponComponent 已移除 SoldierObject 依赖并以 BaseObject/RenderComponent/SandboxServices 工作，AnimComponent / SoldierAnimController 已走 `IAnimContextProvider`；DT/BT driver 与 Lua action 真实 owner 已泛化到 AgentObject；Blackboard 已向 Lua 导出 `GetAgentOwner()`，RuntimeDiag 验证非 Soldier owner round-trip；FSM 通用查询与 AgentActionContext 动画表现入口已走 AIController/Weapon/Attrib/AnimComponent/IAnimController；DeathState 已走 AnimComponent/IAnimContextProvider/IAnimController；**待**：旧 Lua Soldier 签名兼容桥随 sample 迁移继续收窄
- [x] P6 字符串 key → 类型化访问 + 常量头 —— `ComponentKeys.h` 已建，全量常量化，无散落字面量
- [x] P7 `PerceptionQuery` + driver 工厂 —— 感知已抽象（走 `AgentPerceptionQuery`，不再直扫 getAllAgents）；driver 创建已走 factory/registry；行为由 `Sandbox6/7/8` 回归守住
- [x] C1 删 `UIService` 空壳 —— `UIService.{h,cpp}` 已删除并从工程移除，Gorilla UI `CreateUIFrame`/`SetMarkupColor` 已由 `SandboxUI`/`UIManager` 承接
- [~] C2 `SandboxMgr` 减肥 —— 对象创建 `Create*` 已从 `SandboxMgr` 移到 `SandboxObjects`/`ObjectFactory`，`WeaponComponent` 创建 bullet 走 `SandboxServices.objectFactory`，Gorilla UI `CreateUIFrame`/`SetMarkupColor` 已从 `SandboxMgr` 移到 `SandboxUI`/`UIManager`，相机/profile 查询已从 `SandboxMgr` 移到 `SandboxCamera`/`CameraService`，导航配置/构建/查询已从 `SandboxMgr` 移到 `SandboxNav`/`NavigationService`，raycast 已从 `SandboxMgr` 移到 `SandboxRaycast`/`RaycastService`，scene/light/material 已从 `SandboxMgr` 移到 `SandboxScene`/`SceneService`，CallFile 已从 `SandboxMgr` 移到 `SandboxScript`/`ScriptService`；**待**：删除旧兼容转发并迁出/定位 CppFSM flag
- [~] C3 `ObjectManager` registry / lifecycle / AI update / tactics / navigation / scheduler / team 拆分 —— `ObjectRegistry` 已拆出对象 map、对象 id、Agent/Block 二级索引和查找；`ObjectLifecycleSystem` 已承接对象 update loop、待删对象移除、对象 event flush 与 scene cleanup；`AIUpdateSystem` 已承接 scheduler begin/tick、spatial rebuild、批量感知、TeamBlackboard sync 与 AI perf stats；`TacticalQueryService` 已包住 influence config/layer/source/sample/score/stats 与 navmesh 建图编排，`TacticalDebugDrawService` 已承接 tactical debug visual，ObjectManager tactical Lua 入口为薄转发；navmesh map 与销毁责任已迁到 `NavigationService`；AIScheduler Lua 配置/诊断入口已迁到 `SandboxAIScheduler`；TeamBlackboard Lua 主入口已迁到 `SandboxTeam`，ObjectManager 旧入口为薄转发；**待**：旧 tactical/team/scheduler Lua 兼容 facade 最终删除
- [x] C4 `IComponent` update order 显式化 —— `ComponentUpdateOrder` 已落地，AI/Render/Anim/Weapon 声明顺序，`SoldierObject::Update` 不再手写组件更新列表
- [x] C5 修正 `cpp-object-model-refactor-roadmap.md` 术语漂移 —— 5/29 订正 banner 已加

## 8. 验证策略（按改动面取最小集）

- 对象 / 组件改动：对象创建销毁 + `ObjectManager:buildObjectDebugSummary`。
- `AgentObject` / `SoldierObject` / AI driver 改动：`Sandbox6 / Sandbox7 / Sandbox8` 移动、射击、状态切换、寻路。
- FGUI / GameManager 转发改动：对应 `HELLO_FGUI_*_SELF_TEST`，复杂改动跑 `HELLO_FGUI_SELF_TEST_ALL=1` 与 `tools/run_fgui_production_gate.ps1`。
- Lua 绑定改动：同步检查 `.pkg`、生成 cpp、Lua 调用点。
- 平台兼容：检查 Windows / macOS 分支条件编译。
