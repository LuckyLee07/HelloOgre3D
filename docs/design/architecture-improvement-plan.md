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
> - 🟢 **已解决**：P6（ComponentKeys.h 常量化）、C5（cpp-roadmap 术语已订正）。
> - 🟠 **部分完成**：P1（SandboxServices 已引入并下发、热点已走服务解析，但 include/`g_*` 未清零）、P5（WeaponComponent 跨组件访问已收口，owner 仍绑 SoldierObject）、P7（感知已抽象走 `AgentPerceptionQuery`，driver 工厂未做）。
> - 🔴 **仍成立**：P2、P3、P4、C1–C4。
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
| P1 | 🔴 | sandbox→game 反向依赖 + 全局单例硬耦合 | 无法单测/多实例，改动易牵连全局 | 14 文件 include `GameManager`，`g_ObjectManager`/`g_SandboxMgr`/`g_GameManager` 散落 |
| P2 | 🔴 | 组件化“只藏不减”，对象层 150+ 转发样板 | 新对象类型仍需复制整套转发 | `AgentObject.cpp:301-341`、`SoldierObject.cpp:203-270` |
| P3 | 🔴 | `GameManager` 是 102 个 FGUI 转发的上帝类 | 编排层混入 UI 适配细节 | `GameManager.h` 123 个 public 方法 |
| P4 | 🟡 | 所有权全靠裸指针 + `SAFE_DELETE` | 悬空/重复删除隐患 | `BaseObject.h:108`、`BlockObject.cpp:78-80`、`AnimComponent.h:55-56` |
| P5 | 🟡 | 组件绑死具体子类，丧失复用性 | 组件无法被非 Soldier 对象复用 | `AnimComponent.cpp:89`、`WeaponComponent.cpp:79` |
| P6 | 🟡 | 组件 key 是散落的魔术字符串 | 易拼写漂移、无编译检查 | `"anim"/"weapon"/"ai"/...` 散落多 cpp |
| P7 | 🟡 | AI 感知无抽象，硬扫全场 + driver 切换硬编码 | agent 增多即 O(n²)，扩展困难 | `AIController.cpp:167`、`AIController.cpp:333-413` |
| C1 | 🟢 | `UIService` 纯空壳转发 | 无意义中间层 | `UIService.cpp:11-22` |
| C2 | 🟢 | `SandboxMgr` 门面转发门面 | 层级冗余 | `SandboxMgr.cpp:107-135` |
| C3 | 🟢 | `ObjectManager` 职责过载 | registry 与调度/更新/输入纠缠 | `ObjectManager.cpp:143-217` |
| C4 | 🟢 | `Update` 顺序硬编码、隐式依赖 | 组件增多后顺序 bug 难查 | `SoldierObject.cpp:139-168` |
| C5 | 🟢 | 文档/代码漂移 | 路线图引用了不存在的类 | 代码无 `GameObject`/`VehicleObject` |

---

## 3. 🔴 第一梯队（优先解决，是其他重构的前提）

### P1：消灭反向依赖与全局单例，引入 SandboxServices 注入

> 🟠 状态（2026-06-11）：**部分完成**。`sandbox/core/SandboxServices.h` 已落地并下发（`ObjectManager` 填充 → `BaseObject::SetSandboxServices` → `IComponent::GetSandboxServices()`），AIController / PhysicsComponent / AgentLocomotion / AgentStateController 的热点已改为 `Resolve*()` 优先走服务、`g_*` 仅兜底（RenderComponent/AnimComponent 已零 `g_*`）。**仍待**：15 个 sandbox 文件仍 `#include GameManager.h`、`g_*` 未清零、静态门禁未加。下方"现状证据"为改造前快照。

> 命名说明：本节的依赖注入聚合体命名为 `SandboxServices`（早期草稿曾叫 `SandboxContext`，但仓库已有 `sandbox/core/event/SandboxContext.h` 作事件 payload，撞名，故改名）。

#### 现状证据

- AGENTS.md 声明依赖方向为 `runtime → sandbox → game`，但实际有 **14 个 sandbox 文件 `#include` `GameManager`、4 个 include `ClientManager`**（NavigationMesh、AnimComponent、PhysicsComponent、RenderComponent、AgentObject、BlockObject、SoldierObject、InputManager、ObjectManager、SandboxMgr、CameraService、SceneFactory、UIManager 等）。
- 组件/对象直接用全局单例抓依赖：
  - `g_ObjectManager->getAllAgents()`：[AIController.cpp:162-205](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L162-L205)、[AgentLocomotion.cpp:268](src/HelloOgre3D/sandbox/components/agent/AgentLocomotion.cpp#L268)、[AgentStateController.cpp:215](src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp#L215)
  - `g_SandboxMgr->FindPath(...)`：[AIController.cpp:157](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L157)
  - `g_GameManager->getPhysicsWorld()`：[PhysicsComponent.cpp:59](src/HelloOgre3D/sandbox/components/physics/PhysicsComponent.cpp#L59)

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
       INavQuery*      nav       = nullptr;  // 寻路查询（FindPath 抽成接口）
       ScriptLuaVM*    script    = nullptr;  // 脚本调用
   };
   ```

2. `GameManager` / `ClientManager` 在初始化时填好 `SandboxServices` 并下发给 `ObjectManager`，由对象创建链路（`ObjectFactory` / `AgentFactory` / `SoldierFactory`）把 context 透传给新建对象，对象再传给组件。
3. 把热点访问点改为走 context：AIController 找敌人、Locomotion 分离力、PhysicsComponent 取世界、AIController 寻路。
4. 全局单例**保留但降级**：`g_ObjectManager` 等暂时不删，仅作为 context 填充来源；新代码禁止再直接引用 `g_*`（可在 `tools/check_fgui_static.ps1` 风格的静态检查里加一条 grep 门禁）。

> 不要求一次替换所有 `g_*`。先迁移“被组件/对象直接引用”的点；纯应用层（game 内部）继续用全局无妨。

#### 验收

- [ ] sandbox 层组件 `.cpp` 不再 `#include GameManager.h` / `ClientManager.h`（至少 AIController、PhysicsComponent、AnimComponent、RenderComponent、Locomotion 先达标）。
- [ ] 上述热点不再出现 `g_ObjectManager` / `g_SandboxMgr` / `g_GameManager` 直接引用。
- [ ] `Sandbox6/7/8` 移动、射击、寻路、状态切换行为不变。
- [ ] 静态检查新增“sandbox 组件禁止引用 game manager / g_* 全局”门禁。

#### 风险与回退

- 透传链路较长，分对象类型增量迁移；每迁一类跑对应 sample。
- `INavQuery` 抽接口前可先直接放 `SandboxMgr*`，把“去全局”和“抽接口”分两步走，降低单次 diff。

---

### P2：停止对象层转发样板，让组件化真正减负

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

- [ ] 关键组件（Weapon/Attrib/Anim/AIController）常用方法已 tolua 导出，Lua 可直接通过 `GetComponent(key)` 使用。
- [ ] 新增一个示例对象（如最小 `DummyAgent`）复用 ≥2 个组件，且**不复制** Soldier 的转发代码。
- [ ] 现存 sample 行为不变。
- [ ] 对象层不再新增“单组件单接口”转发（评审约定 + 标注）。

---

### P3：拆薄 GameManager 的 FGUI 转发

#### 现状证据

- [GameManager.cpp](src/HelloOgre3D/game/GameManager.cpp) 866 行、**123 个 public 方法，其中约 102 个是 FairyGui 转发**（`GameManager.h:44-150`）。
- 同时兼任 Lua 环境初始化、`IInputHandler` 输入处理、服务访问入口。

#### 问题

Lua 侧 FGUI 已通过 `FairyGuiRuntime`（`RuntimeToLua.pkg`）走 runtime 后端，GameManager 里这批 C++ 转发壳多数已无 Lua 调用方，仍留着只增加“上帝类”体量。

#### 解决方案

1. 用 `rg` 确认每个 `*FairyGui*` 转发方法是否仍有 **C++ 内部调用方**。
2. 无调用方的：直接删除（Lua 已不依赖）。
3. 仍被 C++ 内部使用的：迁到 `FairyGuiLuaApi` / runtime 侧，GameManager 仅保留必要的薄封装或彻底改为持有 `FairyGuiSystem*` 直调。
4. GameManager 收敛为“应用编排 + 服务持有 + Lua 环境注册”，输入处理可进一步拆到独立 input adapter（与 P1 的 context 配合）。

#### 验收

- [ ] `GameManager` public 方法数显著下降（目标砍掉大部分 FGUI 转发）。
- [ ] `HELLO_FGUI_SELF_TEST_ALL=1` 通过。
- [ ] `tools/run_fgui_production_gate.ps1`（按改动面选 Fast/RuntimeOnly/Full）通过。

---

## 4. 🟡 第二梯队（局部耦合与所有权）

### P4：所有权用 unique_ptr 表达，消除 SAFE_DELETE 隐患

#### 现状证据

- 组件容器是裸指针 map：`std::map<std::string, IComponent*> m_components`（[BaseObject.h:108](src/HelloOgre3D/sandbox/core/object/BaseObject.h#L108)），析构走 `SAFE_DELETE` 宏（[SandboxMacros.h:4](src/HelloOgre3D/sandbox/core/SandboxMacros.h#L4)）。
- **双重持有**：`BlockObject` 同一 `RenderComponent` 既 `AddComponent("render",...)`（由 BaseObject 析构删）又存裸成员 `m_renderComp`（[BlockObject.cpp:78-80](src/HelloOgre3D/sandbox/objects/BlockObject.cpp#L78-L80)）。
- **持有不拥有的指针**：`AnimComponent` 存 `Ogre::Entity* m_bodyEntity/m_weaponEntity`（[AnimComponent.h:55-56](src/HelloOgre3D/sandbox/components/anim/AnimComponent.cpp#L55)），重 init 后可能成野指针。

#### 解决方案

1. 组件容器改为 `std::map<std::string, std::unique_ptr<IComponent>>`；`AddComponent` 接管所有权，`GetComponent`/`FindComponent` 返回裸指针（非拥有观察者）。
2. 对象/组件里**缓存性裸指针**统一视为“非拥有引用”，加注释 `// non-owning`，并确保它指向的对象由 map 拥有，删除重复 `SAFE_DELETE`。
3. `AnimComponent` 的 Entity 指针明确标注由 `RenderComponent` 拥有；重 init 时先置空再取新值。
4. 析构链补最小断言/dump：组件全部 detach、Lua callback 全部解绑。

#### 验收

- [ ] 组件容器迁移到 `unique_ptr`，无对“map 已拥有对象”的二次手动 delete。
- [ ] `BlockObject` 不再既 AddComponent 又重复删除同一组件。
- [ ] 对象创建/销毁 + `ObjectManager:buildObjectDebugSummary` 正常，无崩溃/泄漏。

---

### P5：组件 owner 统一为 BaseObject，解除对具体子类的依赖

> 🟠 状态（2026-06-11）：**部分完成**。`WeaponComponent` 的跨组件访问已收口为 `owner->FindComponent<RenderComponent>()`（不再 `owner->GetRenderComponent()`）。**仍待**：`AnimComponent`/`WeaponComponent` 仍持 `SoldierObject* owner` 并调 `GetUseCppFSM()`/`getStanceType()` 等具体方法。

#### 现状证据

- `AnimComponent` owner 直接是 `SoldierObject*`，调 `m_owner->GetUseCppFSM()`、`getStanceType()`（[AnimComponent.cpp:89](src/HelloOgre3D/sandbox/components/anim/AnimComponent.cpp#L89)）。
- `WeaponComponent` 调 `m_owner->GetRenderComponent()`（[WeaponComponent.cpp:79](src/HelloOgre3D/sandbox/components/combat/WeaponComponent.cpp#L79)），隐含假设 owner 一定是 Soldier。

#### 解决方案

1. 组件 owner 类型统一为 `BaseObject*`（`IComponent` 已是此约定，收口子类里偷用的具体类型）。
2. 跨组件依赖一律走 `owner->FindComponent<T>()`，不调具体对象方法（如用 `FindComponent<RenderComponent>()->GetSceneNode()` 替代 `owner->GetRenderComponent()`）。
3. Soldier 专有数据（stance、useCppFSM）下沉为组件状态或通过 Blackboard/Attrib 暴露，组件不读具体对象类型字段。

#### 验收

- [ ] `AnimComponent`、`WeaponComponent` 不再出现对 `SoldierObject` 具体方法的直接调用。
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

> 🟠 状态（2026-06-11）：**部分完成**。感知已抽象——`AIController` 找敌人改走 `AgentPerceptionQuery`（接口 `IAgentPerceptionQuery` + 可插拔 `IAgentSpatialQuery` + FOV/LOS/path 过滤），不再直接 `g_ObjectManager->getAllAgents()`（验收第 1 条达标）；注意线性扫描只是下沉到默认 `ObjectManagerAgentSpatialQuery` 一层，尚未做空间分区。**仍待**：driver 工厂未做，`SetDriverByType` 仍是 `"fsm"/"dt"/"bt"` string if-else + 各 `Set*Driver` 内 inline `new`。

#### 现状证据

- 找敌人直接 `g_ObjectManager->getAllAgents()` 线性遍历（[AIController.cpp:167](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L167)），无感知/查询层，agent 一多即 O(n²)。
- driver 切换是字符串 if-else `"fsm"/"dt"/"bt"`，每个 `SetXxxDriver` 内部直接 `new`，无工厂（[AIController.cpp:333-413](src/HelloOgre3D/sandbox/components/ai/AIController.cpp#L333-L413)）。

#### 解决方案

1. 抽一个最小 `PerceptionQuery`（按范围/队伍过滤的敌人查询），AI 节点只读查询结果，不再自行扫全场；查询走 P1 的 `SandboxServices.objects`。后续可在此挂空间分区优化。
2. driver 用枚举 + 工厂/注册表替代字符串 `new`：`AIDriverFactory::Create(DriverType)`；Lua 侧 `SetDriverByType("bt")` 仅在边界做一次 string→enum 转换。
3. 与 `ai-roadmap.md` 的“感知系统组件化”衔接：`PerceptionQuery` 是 VisionSense/MemoryStore 的前置最小件。

#### 验收

- [ ] AI 找敌人改为通过 `PerceptionQuery`，`AIController` 不再直接 `g_ObjectManager->getAllAgents()`。
- [ ] driver 创建走工厂，新增 driver 类型只需注册，不改 `SetDriverByType` 的 if-else 主体。
- [ ] FSM / DecisionTree / BehaviorTree 各一条 sample 行为不变。

---

## 5. 🟢 第三梯队（清理项，可穿插）

> 这些是低风险局部清理，建议在第一/二梯队对应区域被改动时顺手做，不单独立大改。每条执行前先 `rg` 复核调用点。

- **C1 删 `UIService` 空壳**：[UIService.cpp:11-22](src/HelloOgre3D/sandbox/systems/service/UIService.cpp#L11-L22) 纯转发 UIManager；删除后调用点直连 UIManager。
  - 验收：无 `UIService` 引用残留，UI 相关 sample 正常。
- **C2 `SandboxMgr` 减肥**：`CreatePlane/CreateBlock/...` 8 个方法纯转发 `ObjectFactory`，Camera/UI 方法纯转发 service（[SandboxMgr.cpp:107-135](src/HelloOgre3D/sandbox/systems/manager/SandboxMgr.cpp#L107-L135)）。压扁“门面转发门面”层级，保留真有逻辑的部分（如 `CreateNavigationMesh`）。
- **C3 `ObjectManager` 职责拆分**：registry + update 循环 + AI 调度 + 输入转发 + navmesh 存储 + 场景节点延迟删全揉在一起（[ObjectManager.cpp:143-217](src/HelloOgre3D/sandbox/systems/manager/ObjectManager.cpp#L143-L217)）。`AIScheduler` 已是独立类，下一步把“update 驱动循环”与“对象 registry”分离（呼应 `cpp-object-model-refactor-roadmap.md` Phase 5）。
- **C4 `Update` 顺序显式化**：[SoldierObject::Update](src/HelloOgre3D/sandbox/objects/SoldierObject.cpp#L139-L168) 手写 5 个组件更新次序，隐式依赖（anim 依赖 render 先更新）。给 `IComponent` 加 update order，由对象/容器按 order 统一驱动，替代硬编码。
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

1. 组件容器迁 `unique_ptr`，清理重复 `SAFE_DELETE` 与双重持有。
2. 删/迁 `GameManager` 的 FGUI 转发壳，收敛 GameManager 职责。

验收：所有权可解释、销毁无悬空；`HELLO_FGUI_SELF_TEST_ALL=1` 与 production gate 通过。

### Milestone 4：AI 抽象与结构清理（对应 P7、C1–C5）

1. `PerceptionQuery` + driver 工厂。
2. 穿插完成 C1–C5 清理。

验收：AI 扩展不再硬扫全场 / 硬编码切换；冗余层级清理后 sample 行为不变。

---

## 7. 跟踪清单

> 状态符号：[x] 已解决 · [~] 部分完成 · [ ] 仍成立（待办）。2026-06-11 逐条对代码核实。

- [~] P1 SandboxServices 注入，去 sandbox→game 反向依赖与 `g_*` 热点 —— 服务已引入并下发、热点已走解析；**待**：include/`g_*` 清零 + 静态门禁
- [ ] P2 停止对象层转发样板，Lua/sample 走组件直取 —— 仍成立：AgentObject 788 行/SoldierObject 651 行，转发块原样保留，无 `[legacy-forward]` 标注
- [ ] P3 拆薄 GameManager FGUI 转发 —— 仍成立：GameManager.cpp 896 行，`.h` 内 FairyGui 方法 111 处，未缩减
- [ ] P4 组件容器 `unique_ptr` 化，清理 `SAFE_DELETE`/双重持有 —— 仍成立：`m_components` 仍 `IComponent*` 裸指针 map，BlockObject 仍双重持有
- [~] P5 组件 owner 统一 `BaseObject`，解除子类依赖 —— WeaponComponent 跨组件访问已收口；**待**：owner 仍绑 `SoldierObject`、仍调具体方法
- [x] P6 字符串 key → 类型化访问 + 常量头 —— `ComponentKeys.h` 已建，全量常量化，无散落字面量
- [~] P7 `PerceptionQuery` + driver 工厂 —— 感知已抽象（走 `AgentPerceptionQuery`，不再直扫 getAllAgents）；**待**：driver 工厂（仍 string if-else + inline `new`）、空间分区
- [ ] C1 删 `UIService` 空壳 —— 仍成立：`UIService.cpp:11-23` 纯转发 UIManager
- [ ] C2 `SandboxMgr` 减肥 —— 仍成立：`Create*` 仍一行转发 `ObjectFactory`
- [ ] C3 `ObjectManager` registry / update 拆分 —— 仍成立：registry+update+spawn+navmesh 仍揉在一起（且新增了 SandboxServices owner 职责）
- [ ] C4 `IComponent` update order 显式化 —— 仍成立：`SoldierObject::Update` 仍手写次序，`IComponent` 无 update order
- [x] C5 修正 `cpp-object-model-refactor-roadmap.md` 术语漂移 —— 5/29 订正 banner 已加

## 8. 验证策略（按改动面取最小集）

- 对象 / 组件改动：对象创建销毁 + `ObjectManager:buildObjectDebugSummary`。
- `AgentObject` / `SoldierObject` / AI driver 改动：`Sandbox6 / Sandbox7 / Sandbox8` 移动、射击、状态切换、寻路。
- FGUI / GameManager 转发改动：对应 `HELLO_FGUI_*_SELF_TEST`，复杂改动跑 `HELLO_FGUI_SELF_TEST_ALL=1` 与 `tools/run_fgui_production_gate.ps1`。
- Lua 绑定改动：同步检查 `.pkg`、生成 cpp、Lua 调用点。
- 平台兼容：检查 Windows / macOS 分支条件编译。