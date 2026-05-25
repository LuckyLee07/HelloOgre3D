# C++ 对象模型与代码风格重构路线图

> 目标：把 `HelloOgre3D` 当前混合的继承对象、组件对象、Lua 绑定、runtime 适配风格逐步收敛成一套稳定的 C++ 对象模型。
>
> 边界：本文关注 C++ 对象关系、职责边界、所有权、命名和绑定方式。目录结构、FGUI 专项能力、AI 专项能力分别参考 `docs/project-roadmap.md`、`docs/fairygui-final-roadmap.md`、`docs/ai-roadmap.md`。

## 1. 总体判断

当前项目的混乱感主要不是目录问题，而是三套 C++ 风格交错在同一批类里：

- 教学 sample 风格：类似 `code-master` 的 `Game / Actor / Component`，生命周期直接、对象关系直观。
- Ogre sandbox 风格：`Manager / Singleton / raw pointer / engine facade`，方便快速接引擎能力，但容易形成横向耦合。
- Lua/tolua 风格：核心 C++ 类直接承担脚本 API，public 方法越来越多，类边界容易被绑定需求拉宽。

这些风格单独看都能工作，但混在一起会导致：

- 对象到底是继承扩展，还是组件组合扩展，不够明确。
- raw pointer 既可能是拥有关系，也可能是非拥有缓存。
- `GameManager`、`SoldierObject`、`FairyGuiSystem` 等类逐渐变成多职责入口。
- Lua 绑定注释和脚本兼容接口侵入核心对象设计。
- 命名、生命周期和 update 规则缺少统一口径。

## 2. 参考 code-master 的地方

`/Users/lizi/Desktop/code-master` 值得参考的不是扁平目录，而是对象模型的克制。

### 值得借鉴

- `Game` 生命周期清楚：`Initialize / RunLoop / Shutdown / ProcessInput / UpdateGame / GenerateOutput`，读者知道从哪里进、到哪里出。
- `Actor` 是明确的对象根：持有 transform、state、component list，并负责对象级 update/input。
- `Component` 是明确的行为切片：持有 owner，按 update order 更新，析构时从 owner 移除。
- 资源和系统入口集中：Renderer 管渲染资源，Game 管 actor 和 UI stack，不让所有对象随手拿全局系统。
- public API 数量克制：类暴露的是它本身的职责，不把脚本、调试、平台适配都塞进同一个类。

### 不建议照搬

- 不照搬每章独立小项目结构；`HelloOgre3D` 是长期沙盒，需要可演进的 runtime / sandbox / Lua 边界。
- 不照搬“每个 Actor 子类都直接继承扩展”的方式；项目后续有 AI、技能、UI、Lua 扩展，更需要组件化。
- 不照搬旧式 raw pointer 作为默认所有权表达；保留兼容代码可以继续用，新代码要更明确拥有关系。
- 不把 renderer / input / UI 适配直接塞进 gameplay object；这些应收口到 runtime 或 system。

## 3. 目标对象模型

长期目标不是把所有类改成 ECS，也不是一次性推翻现有继承体系，而是收敛成下面这套可解释模型。

```text
Application / Client
  - 负责窗口、主循环、平台启动、关闭。

SandboxWorld
  - 拥有 gameplay systems。
  - 负责 sample 生命周期、对象注册、统一 update。

Entity / BaseObject
  - 表达对象身份、id、team、生命周期、事件通道。
  - 拥有组件容器。
  - 不直接承载大量物理、动画、AI、武器细节。

Component
  - 表达一个可组合能力，例如 Transform、Physics、Render、Locomotion、Health、Weapon、Anim、AiDriver。
  - 明确 owner。
  - 明确 update order。
  - 组件之间通过 owner 查询、事件或系统协调，不互相形成隐式强依赖。

System
  - 处理跨对象逻辑，例如 PhysicsWorld、AIScheduler、ObjectRegistry、NavigationSystem、TimelineSystem。
  - 可以批量访问对象或组件。
  - 不把 per-object 状态藏在 manager 的临时容器里。

Runtime Adapter
  - 处理 Ogre / OIS / FGUI / Tracy / 平台差异。
  - 不承载 gameplay 规则。

Lua Binding Adapter
  - 把 C++ 能力暴露给 Lua。
  - 尽量不让核心类为了 tolua 直接膨胀 public API。
```

## 4. 重构到什么程度

本轮重构目标是“建立稳定对象模型并逐步迁移热点类”，不是追求完全现代 C++ 化。

### 必须达到

- 新增玩法对象时，不需要复制 `SoldierObject` 的大块逻辑。
- 新增能力时，优先判断它是 component、system、runtime adapter 还是 binding adapter。
- 核心对象的拥有关系能通过构造、成员类型或注释看出来。
- Lua 兼容接口保留，但新增 Lua API 优先进入绑定适配层。
- `GameManager` 不继续承担 FGUI、Lua、输入、资源诊断的无限转发。
- `FairyGuiSystem` public facade 保持稳定，但内部按资源、渲染、输入、事件、diagnostics 拆分。

### 暂不追求

- 不一次性替换所有 raw pointer。
- 不一次性移除所有 manager / singleton。
- 不强制把现有 tolua 生成链全部重做。
- 不大规模重命名 Lua 已调用的接口。
- 不在没有回归样例前移动大量 gameplay 行为。

## 5. 当前主要重构对象

### BaseObject / GameObject / IComponent

当前 `BaseObject` 继承 `SandboxObject`，内部又持有 `GameObject` 作为组件容器。这让“对象根”和“组件容器”分成两层，但外部通常仍把 `BaseObject` 当对象根使用。

目标：

- 让 `BaseObject` 成为清晰的 entity root。
- 组件容器可以先继续由 `GameObject` 承担，但所有权、生命周期、访问方式要明确。
- 中期考虑把组件容器能力直接收进 `BaseObject`，`GameObject` 退化或删除。

### VehicleObject / AgentObject / SoldierObject

当前继承链承担了太多能力：

- `VehicleObject` 转发 physics、locomotion、steering、path、health、mass 等接口。
- `AgentObject` 加 body、Lua env、事件、死亡、输入。
- `SoldierObject` 加 weapon、animation、AI driver、目标选择、弹药、状态请求。

目标：

- 保留继承链短期兼容 Lua 和 sample。
- 把可组合能力逐步搬到组件：`HealthComponent`、`WeaponComponent`、`AnimComponent`、`AiDriverComponent`。
- `SoldierObject` 逐步变成 facade 和默认装配器，而不是所有逻辑的最终归宿。

### GameManager

当前 `GameManager` 同时承担 gameplay 编排、Lua VM、FGUI API 转发、输入处理、资源诊断等职责。

目标：

- `GameManager` 只保留应用级 gameplay 编排和旧接口兼容。
- FGUI 转发拆到 `FairyGuiLuaApi` 或类似 binding adapter。
- 资源诊断、输入注入、Lua API 注册分别拆出清晰入口。

### FairyGuiSystem

当前 `FairyGuiSystem` 是可运行的 facade，但内部已经混合对象 handle、资源、渲染、输入、IME、事件、diagnostics。

目标：

- public API 尽量不破坏 Lua 调用。
- 内部拆成多个实现类或文件：
  - `FairyGuiObjectRegistry`
  - `FairyGuiPackageManager`
  - `FairyGuiRenderer`
  - `FairyGuiInputBridge`
  - `FairyGuiEventBridge`
  - `FairyGuiDiagnostics`

## 6. 分阶段路线

### Phase 0：规则固化与基线保护

目标：先统一判断标准，避免边重构边扩大范围。

任务：

- 明确对象模型术语：entity、component、system、runtime adapter、binding adapter。
- 标注关键 raw pointer 的拥有关系，至少覆盖 `BaseObject`、`GameObject`、`VehicleObject`、`SoldierObject`、`GameManager`。
- 梳理 Lua 当前调用到的核心 C++ API，形成兼容边界。
- 为 `BaseObject / VehicleObject / SoldierObject / GameManager / FairyGuiSystem` 建立最小行为回归清单。

验收：

- 新增功能时能在本文档模型里找到归属。
- 能说清楚哪些 public API 是 C++ 内部接口，哪些是 Lua 兼容接口。
- 不发生大规模代码移动。

### Phase 1：组件模型收口

目标：让组件真正降低对象复杂度，而不是只把实现藏到组件后再由对象全量转发。

任务：

- 为组件增加统一约定：owner、update order、attach/detach、destroy、debug name。
- 增加 typed component 访问方式，例如按类型或稳定 key 获取组件，减少字符串散落。
- 让 `BaseObject` 明确拥有组件容器；短期可以继续委托 `GameObject`。
- 将 `VehicleObject` 中纯转发接口分组，标记哪些是 Lua 兼容 facade，哪些后续迁移到组件。

验收：

- 新组件接入方式固定，不再每个对象手写不同生命周期。
- `PhysicsComponent`、`AgentLocomotion`、`RenderComponent` 的所有权和 owner 关系清楚。
- 旧 sample 行为不变。

### Phase 2：绑定层拆薄 GameManager

目标：减少 Lua/tolua 对核心 manager 的侵入。

任务：

- 新增 `FairyGuiLuaApi`，承接 `GameManager` 中 FGUI 相关 tolua 导出实现。
- 新增或整理 `SandboxLuaApi`，承接对象、sample、debug dump 相关 Lua API。
- `GameManager` 保留旧函数签名，内部只转发到 binding adapter。
- 后续新增 Lua API 不再直接加到 `GameManager`。

验收：

- Lua 调用不需要改名即可运行。
- `GameManager` 的新增代码明显减少。
- FGUI / Sandbox / diagnostics 的绑定实现有独立文件和职责边界。

### Phase 3：FairyGuiSystem 内部拆分

目标：保持 FGUI facade 稳定，把内部职责拆开。

任务：

- 先按文件拆分，不急于改变 public API。
- 优先拆出 diagnostics、event bridge、input bridge，因为这些边界较清晰。
- 再拆 resource/package 和 renderer，因为它们对生命周期和渲染状态影响更大。

验收：

- `HELLO_FGUI_SELF_TEST_ALL=1` 通过。
- FGUI 输入、生命周期、资源回零、debug panel 仍可运行。
- `FairyGuiSystem.cpp` 行数和职责显著下降。

### Phase 4：Soldier 能力组件化

目标：避免后续 Monster、NPC、机关、载具继续复制 `SoldierObject`。

任务：

- `HealthComponent`：生命、最大生命、死亡事件。
- `WeaponComponent`：弹药、换弹、射击请求、武器挂点。
- `AnimComponent` 或 `AgentAnimComponent`：动画意图、状态机、notify。
- `AiDriverComponent`：FSM / DecisionTree / BehaviorTree driver 持有与切换。
- `TargetingComponent`：敌人选择、最近目标、射击条件。

验收：

- `SoldierObject` 仍能作为默认士兵对象运行。
- 至少有一个非 Soldier 对象复用其中两个以上组件。
- 新增对象类型不需要复制 Soldier 的 AI、武器、生命值整套逻辑。

### Phase 5：System 与 Manager 边界收敛

目标：让跨对象逻辑进入 system，而不是继续堆在 object 或 manager。

任务：

- `ObjectManager` 逐步收敛为 object registry 和生命周期管理。
- AI tick 继续交给 `AIScheduler`，不要回流到 `ObjectManager`。
- Navigation、Physics、Timeline、Trigger 等跨对象逻辑独立 system 化。
- 减少 sandbox object 直接 include `ClientManager` / `GameManager`。

验收：

- 对象层不直接依赖应用层 manager。
- manager 之间横向调用减少。
- 新系统有明确 owner、update 时机和 debug dump。

### Phase 6：所有权与现代化清理

目标：在行为稳定后再做所有权表达升级。

任务：

- 新代码默认使用 `std::unique_ptr` 表达拥有关系。
- raw pointer 默认视为非拥有，除非明确注释或旧代码约定。
- 逐步移除不必要的 `SAFE_DELETE`。
- 析构链路补充断言或 dump，确保组件和 Lua callback 清理完整。

验收：

- 核心对象销毁顺序可解释。
- sample 切换、对象清理、UI 关闭后无明显悬空 callback。
- 没有为了现代化而破坏 Lua 兼容。

## 7. 后续写代码的偏航检查

新增 C++ 能力前先问：

- 这是单个对象的能力吗？如果是，优先做 component。
- 这是跨对象调度或查询吗？如果是，优先做 system。
- 这是 Ogre / OIS / FGUI / Tracy / 平台差异吗？如果是，优先进 runtime adapter。
- 这是给 Lua 调用的门面吗？如果是，优先进 binding adapter。
- 这个 public 方法是否只是为了脚本方便？如果是，核心类不要继续膨胀。
- 这个 raw pointer 谁删除？如果一句话说不清，先不要合入新所有权关系。
- 这个对象是否 include 了更上层 manager？如果是，优先考虑依赖倒置或传入服务接口。

硬性约束：

- 不再往 `SoldierObject` 里堆新大能力，除非它只是调用已有组件或系统。
- 不再往 `GameManager` 里直接新增大批 FGUI / AI / diagnostics API。
- 不在 gameplay object 里直接写平台分支。
- 不为了重构一次性改 Lua 调用名。
- 不做没有 sample 或自测覆盖的大搬迁。

## 8. 建议近期执行顺序

第一批任务应小、可回退、能验证：

1. 给 `BaseObject / GameObject / IComponent` 补齐所有权和生命周期注释，明确当前组件容器规则。
2. 增加组件 debug name / typed access helper，减少字符串 key 的隐式约定。
3. 把 `GameManager` 中 FGUI tolua 实现拆到 `FairyGuiLuaApi`，保留旧函数签名转发。
4. 把 `FairyGuiSystem` 的 diagnostics 函数先拆出文件。
5. 新增 `HealthComponent`，让 `VehicleObject / SoldierObject` 先通过它读写生命值，但保留旧 `GetHealth / SetHealth`。
6. 新增 `AiDriverComponent`，把 `SoldierObject` 的 `m_driver` 挪进去，旧接口转发。

每完成一项，都要更新本文档对应阶段的状态，并记录验证方式。

## 9. 当前进展

### 2026-05-24 首批落地

- [x] 新增本文档，并在 `docs/project-roadmap.md` 的对象系统组件化条目中指向本文档。
- [x] `IComponent` 记录所属 `GameObject` 和稳定 component key，`getOwner()` 增加空指针保护，detach 时清理 key。
- [x] `GameObject` 明确拥有通过 `addComponent` 加入的组件，拒绝已挂接组件重复加入，并统一组件销毁顺序为 `onDestroy -> onDetach -> delete`。
- [x] `BaseObject / GameObject` 增加 keyed lookup、typed lookup 和按类型查找 helper，为后续拆 `HealthComponent / AiDriverComponent` 做准备。
- [x] macOS arm64 Debug Xcode 构建通过，确认首批对象模型改动不破坏当前工程编译。

### 2026-05-24 FGUI Lua adapter 首批落地

- [x] 新增 `FairyGuiLuaApi`，作为 `GameManager` 与 `FairyGuiSystem` 之间的 Lua binding adapter。
- [x] `GameManager` 保留原有 tolua 导出函数名，首批将 FGUI availability、package、基础对象创建和渲染 diagnostics 接口转发到 `FairyGuiLuaApi`。
- [x] package 名、stencil/material/texture/frame render 等临时字符串缓存从 `GameManager` 移到 adapter，减少 manager 状态膨胀。
- [x] 输入注入、IME debug、事件监听接口迁移到 `FairyGuiLuaApi`，形成 Lua / 自测桥接层。
- [x] 对象属性、controller、列表、focus、transition、删除清理等 FGUI Lua 接口迁移到 `FairyGuiLuaApi`，`GameManager` 不再直接承接 `FairyGuiSystem` 细节实现。
- [x] 2026-05-25：`FairyGuiLuaApi` 实现按 package/create、diagnostics、object、input、event/lifetime 拆分到独立 cpp，并新增 private internal include 收口 `FairyGuiSystem` 依赖。
- [x] 2026-05-25：`FairyGuiLuaApi*` 从 `game` 迁移到 `runtime/ui/fairygui/lua_bridge`，物理位置贴近 `FairyGuiSystem`，`GameManager` 仅保留旧 tolua API 转发兼容层。
- [x] 2026-05-25：新增 `runtime/RuntimeToLua.pkg` 作为 runtime 侧 tolua 绑定入口，后续 runtime-owned API 不再继续堆到 `game/GameToLua.pkg`。
- [x] 2026-05-25：`FairyGuiLuaApi` 构造依赖从 `ClientManager*` 收缩为 `FairyGuiSystem*`，runtime Lua bridge 不再反向认识 game manager 汇聚对象。
- [x] 2026-05-25：真实键鼠输入链路改为 `GameManager -> FairyGuiSystem`，Lua 注入、自测和兼容 tolua API 继续走 `FairyGuiLuaApi`，避免 runtime 输入被 Lua bridge 概念污染。
- [x] 2026-05-25：Lua 侧新增 `FairyGuiNativeApi` 作为唯一 native facade，`manager/fairygui` 子模块不再直接依赖 `GameManager:*FairyGui*`，后续切换到 `RuntimeToLua` 只需替换 facade 后端。
- [x] 2026-05-25：`RuntimeToLua.pkg` 生成 `RuntimeToLua.cpp`，`GameManager::InitLuaEnv` 注册 `FairyGuiRuntime` 全局对象；Lua facade 默认优先走 runtime 后端，`GameManager` 仅作为旧接口 fallback。
- [ ] 后续新增 FGUI Lua API 继续按分组落到 adapter 对应文件；Lua 导出名和 generated binding 边界保持兼容。

### 2026-05-25 FairyGuiSystem 内部实现拆分

- [x] `FairyGuiSystem.cpp` 收缩为 facade / lifecycle / update / render / resize 主流程。
- [x] 新增 `FairyGuiSystemObjects.cpp`，承接 package、object handle、控件属性、controller、transition、list、对象 alias 和配置化 package object 创建。
- [x] 新增 `FairyGuiSystemEvents.cpp`，承接事件监听注册、解绑、事件目标查找和 Lua event dispatch。
- [x] 新增 `FairyGuiSystemInput.cpp`，承接鼠标、键盘、TextInput caret、IME hook、候选窗定位和输入坐标转换。
- [x] 新增 `FairyGuiSystemRender.cpp`，承接 Ogre manual object 渲染、scissor/stencil、CPU clip、frame render stats、material/texture 管理和资源销毁。
- [x] 新增 `FairyGuiSystemInternal.h` 收口拆分后共享的 FairyGUI/Ogre include 与内部 helper，避免 public facade 继续膨胀。
- [x] 重新生成 VS2017 工程，确认新增 cpp 已进入 `HelloOgre3D.vcxproj`。
- [x] VS2017 Debug|x64 编译通过。

## 10. 验证策略

按重构影响面选择最小验证：

- `BaseObject / Component` 改动：运行对象创建、销毁、组件 debug dump，检查 `ObjectManager:buildObjectDebugSummary`。
- `VehicleObject / SoldierObject` 改动：验证 `Sandbox6 / Sandbox7 / Sandbox8` 中移动、射击、状态切换。
- AI driver 改动：验证 FSM / DecisionTree / BehaviorTree 至少各一条 sample。
- FGUI binding 或 facade 改动：运行对应 `HELLO_FGUI_*_SELF_TEST`，复杂改动跑 `HELLO_FGUI_SELF_TEST_ALL=1`。
- runtime/input 改动：手动跑 macOS / Windows 分支对应 sample，确认 UI 和相机输入都正常。
- Lua 绑定改动：同步检查 `.pkg`、生成代码、Lua 调用点。

## 11. 跟踪清单

- [ ] Phase 0：关键对象所有权注释和 Lua 兼容边界梳理。
- [ ] Phase 1：组件生命周期和 typed access 规则落地。
- [x] Phase 2：`GameManager` FGUI binding adapter 拆分。
- [x] Phase 3：`FairyGuiSystem` diagnostics / input / event / renderer 分拆。
- [ ] Phase 4：`HealthComponent`、`WeaponComponent`、`AnimComponent`、`AiDriverComponent` 分阶段接入。
- [ ] Phase 5：`ObjectManager` registry 化，跨对象逻辑 system 化。
- [ ] Phase 6：核心所有权逐步迁移到 `std::unique_ptr` 或明确非拥有指针。
