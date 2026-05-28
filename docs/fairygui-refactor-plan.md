# FGUI 代码结构重构落地方案

> 状态：已核实 / 已按静态门禁与真实状态归属重新调整（2026-05-28）
> 适用范围：`src/HelloOgre3D/runtime/ui/fairygui/`、`bin/res/scripts/manager/fairygui/`、`bin/res/scripts/ui/` 及相关 autogen / 验收工具
> 相关文档：[fairygui-final-roadmap.md](fairygui-final-roadmap.md)、[fairygui-production-convergence-todo.md](fairygui-production-convergence-todo.md)、[fairygui-business-framework-todo.md](fairygui-business-framework-todo.md)

---

## 背景与诊断

FGUI 子系统已经完成过两轮拆分：C++ 侧 `FairyGuiLuaApi*` 拆入 `lua_bridge/`，Lua 侧 `FairyGuiManager` 的功能拆到多个子模块。当前问题不是功能缺失，而是**状态所有权、生命周期收口、文件边界和静态门禁还没有完全对齐**。

| # | 问题 | 位置 | 评级 |
|---|---|---|---|
| 1 | `FairyGuiManager.lua` 仍是过宽 facade，公开 API、生命周期编排、调试转发、部分状态写入仍集中在一个文件 | [FairyGuiManager.lua](../bin/res/scripts/manager/fairygui/FairyGuiManager.lua) | P0 |
| 2 | 核心对象/层级状态存在引用别名：Lifecycle / Layers 创建或接管状态表，再挂回 Manager 供其它模块通过 `owner.X` 读写 | [FairyGuiLifecycle.lua](../bin/res/scripts/manager/fairygui/FairyGuiLifecycle.lua)、[FairyGuiLayers.lua](../bin/res/scripts/manager/fairygui/FairyGuiLayers.lua) | P0 |
| 3 | 共享状态不只 25 个核心字段，Events / Lists / Package / Controls / Services / Profiler 也在把领域状态挂到 `owner` | `bin/res/scripts/manager/fairygui/*.lua` | P0 |
| 4 | 两个同名 `FairyGuiManager.lua` 有命名歧义，但 `bin/res/scripts/manager/FairyGuiManager.lua` 当前是静态门禁要求保留的 compatibility facade，不能先删 | [shim](../bin/res/scripts/manager/FairyGuiManager.lua)、[check_fgui_static.ps1](../tools/check_fgui_static.ps1) | P0 |
| 5 | `FairyGuiServices.lua` 多职责混杂，Toast / Tip / Loading / Mask / MessageBox / Dialog / PopupMenu / Service 统计堆在一个文件 | [FairyGuiServices.lua](../bin/res/scripts/manager/fairygui/FairyGuiServices.lua) | P1 |
| 6 | C++ 内部头 `FairyGuiSystemInternal.h` 过重，平台、Ogre、cocos2d、FairyGUI、IME、渲染/输入/对象状态集中在一个内部声明头 | [FairyGuiSystemInternal.h](../src/HelloOgre3D/runtime/ui/fairygui/FairyGuiSystemInternal.h) | P1 |
| 7 | 命名风格混用：`FairyGui` / `fairygui` / `fgui` 的使用场景没有写成约束 | 全仓库 | P2 |

### 根因

`FairyGuiManager:Init()` 当前主要负责 new 子模块，本身不是唯一状态创建点。真实结构是：

- **核心状态**：Lifecycle 与 Layers 分别维护对象索引、view/controller 索引、父子 UI、scene/layer/stack 等状态，并通过 alias 挂到 Manager。
- **领域状态**：Events、Lists、Package、Controls、Services、Profiler 也各自维护状态，同时把部分状态挂回 Manager。
- **调用习惯**：大量子模块内部会 `local self = self.owner`，随后直接读写 `self.objects`、`self.uiStack`、`self.bindings`、`self.packages` 等字段。

因此真正的问题不是“Manager 单独持有全部状态”，而是：**状态被多个子模块创建、别名共享，并通过 Manager 形成星型读写面；没有单一真源，也没有统一的 mutation 入口。**

重构目标不是简单把所有表塞进一个新的 God Store，而是：

1. 先建立 Store 作为唯一状态根。
2. Store 内部按领域分 state bag，不做一个巨型扁平表。
3. 每一步只迁移一个可验证领域。
4. 过渡期允许 alias，但必须指向 Store 内同一批表。
5. 最终通过 accessor / domain API 收口直接写表。

---

## 状态清单

### R01-R02 先迁移的核心状态

**Object/Lifecycle 域**：
`objects` / `objectsByHandle` / `uiRegistry` / `uiNameToKey` / `hiddenObjects` / `views` / `viewsByHandle` / `controllers` / `controllersByHandle` / `childKeysByParentKey` / `parentKeyByChildKey`

**Layer/Scene/Stack 域**：
`currentSceneName` / `designWidth` / `designHeight` / `scaleMode` / `layers` / `layerPolicies` / `layerNextOrder` / `layerObjects` / `layerRoots` / `safeArea` / `uiStack` / `popupStack` / `stackEntriesByKey` / `nextStackSerial`

这 25 个字段是第一轮 Store 落地范围。它们覆盖打开、关闭、隐藏、层级、scene 切换、stack 顶层查询等最核心流程。

### R05 后续迁移的领域状态

这些状态也存在 owner alias 或跨模块读取，但不放进第一轮，避免一次改动过大：

| 领域 | 状态例子 | 当前主要位置 |
|---|---|---|
| Events / Timers | `callbacks`、`bindings`、`bindingsByHandle`、`transitionCallbacks`、`transitionCallbacksByHandle`、`timers`、`timersByKey`、`eventStats`、`eventDispatchTotal`、`lastEvent`、`nextCallbackId` | [FairyGuiEvents.lua](../bin/res/scripts/manager/fairygui/FairyGuiEvents.lua) |
| Lists / Tree | `childrenByHandle`、`listItemHandlesByHandle`、`listItemIndexByHandle`、`listDataByHandle`、`listRenderersByHandle`、`listVirtualByHandle`、`listVirtualOptionsByHandle`、`listVirtualStatsByHandle`、`treeDataByHandle`、`treeStateByHandle`、`treeRenderersByHandle`、`treeChildPathByHandle` | [FairyGuiLists.lua](../bin/res/scripts/manager/fairygui/FairyGuiLists.lua) |
| Package / Resource | `packageRoot`、`packages`、`packagesByName`、`resourceFallbacks`、`resourceFallbackKeys`、`resourceFallbackMaxCount`、`resourceFallbackPolicy`、`cachePolicy` | [FairyGuiPackage.lua](../bin/res/scripts/manager/fairygui/FairyGuiPackage.lua) |
| Controls / TextInput | `textInputPoliciesByHandle`、`textInputPolicyBindingsByHandle` | [FairyGuiControls.lua](../bin/res/scripts/manager/fairygui/FairyGuiControls.lua) |
| Services | `serviceSkins`、`serviceStats`、`toastQueue`、`toastActive`、`toastSerial`、`toastDedupe`、`loadingRefs`、`loadingRefTotal` | [FairyGuiServices.lua](../bin/res/scripts/manager/fairygui/FairyGuiServices.lua) |
| Profiler | `perfStats` | [FairyGuiProfiler.lua](../bin/res/scripts/manager/fairygui/FairyGuiProfiler.lua) |

---

## 总体原则

1. **不动外部调用方式**：`FairyGuiManager:Method(...)` 仍是 Lua 业务入口，Ctrl/View/Model 调用面保持稳定。
2. **保留 compatibility facade**：`bin/res/scripts/manager/FairyGuiManager.lua` 当前由 `tools/check_fgui_static.ps1` 强制要求存在，不能在第一步删除。
3. **Store 分阶段落地**：R01 只迁移核心 object/layer 状态；领域状态在 R05 迁移。
4. **过渡期 alias 可以存在**：R01 验收只要求 alias 指向 Store 表；R02/R05 再删除对应 alias。
5. **mutation 入口要有语义**：不鼓励到处 `store.objects[key] = value`。对象注册、隐藏、关闭、view/controller 绑定应逐步变成 `RegisterObject`、`HideObject`、`AttachView`、`RemoveView` 这类 API。
6. **静态门禁跟随架构演进**：如果方法体迁出原文件，要么保留薄 wrapper 让现有 guard 通过，要么在同一步更新 `check_fgui_static.ps1`。
7. **每步独立可验收**：优先 `check_fgui_static.ps1`，涉及运行行为时追加 `run_fgui_selftest.ps1` 对应模式。

---

## 阶段总览

| 阶段 | 目标 | 工作量 | 风险 |
|---|---|---|---|
| R00 | 对齐兼容入口和静态门禁：保留 shim，不再把删除 shim 作为第一步 | 0.5d | 低 |
| R01 | 引入 `FairyGuiStore`，只迁移 object/lifecycle + layer/scene/stack 核心状态创建权 | 1-1.5d | 中 |
| R02 | 核心状态访问改走 Store/domain accessor，删除核心 alias | 1-2d | 中 |
| R03 | 收口对象生命周期 mutation：注册、隐藏、销毁、view/controller attach/detach 走统一 API | 1-2d | 中 |
| R04 | 拆 `FairyGuiServices.lua`，保留 `FairyGuiServices` facade | 1-2d | 中 |
| R05 | 迁移 Events / Lists / Package / Controls / Services / Profiler 领域状态 | 2-3d | 中高 |
| R06 | 拆 C++ `FairyGuiSystemInternal.h`，降低内部头耦合 | 1-2d | 中 |
| R07 | 固化命名/路径规范，决定 shim 长期保留还是改门禁后移除 | 0.5d | 低 |

---

## 静态守卫影响矩阵（执行前必读）

`tools/check_fgui_static.ps1` 大量使用**文本匹配**断言，会和「把实现移出原文件」直接冲突。每一步开工前先对照下表，否则架构更对但 gate 不让过。

| 守卫断言（check_fgui_static.ps1） | 约束内容 | 受影响步骤 | 应对 |
|---|---|---|---|
| `Test-FairyGuiLuaFrameworkGuard` L156-166 | shim 必须存在且内容恰为 `return require("...fairygui.FairyGuiManager")` | R00 / R07 | 保留 shim；删除须同步改此断言 |
| 同上 L168-186 | `FairyGuiManager.lua` 必须 `require` `FairyGuiStore` 和全部 9 个子模块 | 全程 | 中央 facade 不得移除 Store 或任一子模块 require |
| 同上 L217-236 | `FairyGuiProfiler.lua` 必须含 snapshot/panel 字段 | R05(Profiler) | 迁状态不动这些方法/字段文本 |
| `Test-FairyGuiProductionFeatureGuard` L360-390 | `FairyGuiManager.lua` 必须含约 28 个 `function FairyGuiManager:XXX` **定义文本** | R02 / R03 | 收口后这些方法**必须以薄 wrapper 形式留在本文件**，否则同步改断言 |
| 同上 L392-405 | `FairyGuiServices.lua` 必须含 `DEFAULT_SERVICE_SKINS` / `RegisterServiceSkin` / `ResolveServiceSkin` / `ShowHoverTip` / `HideTip` 及 `serviceSkinName` / `popupMenuAnchorRect` / `tipAnchorRect` 等标记 | R04 | 拆分后这些方法/常量须留在 facade，或在同一步更新断言 |
| 同上 L407-413 | `FairyGuiLayers.lua` 必须含 `GetGuideMaskRects` / `NormalizeGuideMaskRect` / `mergeIntervals` | R02 / R03 | 状态迁移不动这些方法文本 |
| 同上 L415-465 | Controls / Events / Lists / Package / Profiler 各自必须含若干方法定义文本 | R05 | 只迁**状态**到 Store，方法定义留在原文件（改为读 store） |
| 同上 L467-473 | `FairyGuiSystemRender.cpp` 必须含 `RecordCpuClipWork` 等 4 个标记 | R06 | 拆头不动 render 实现文本 |
| `Test-FairyGuiPublicHeaderFacadeGuard` L120-150 | `FairyGuiSystem.h` 不得出现 `Ogre::`/`cocos2d`/`fairygui::`/`GObject` 等，且必须保留 `FairyGuiSystemStartupContext`、`Initialize(const FairyGuiSystemStartupContext&)`、`FairyGuiSystemImpl* m_impl` | R06 | 拆 internal 头时**实现类名必须保持 `FairyGuiSystemImpl`**；第三方头不得回流到 public facade |
| `Test-FairyGuiRuntimeOwnershipGuard` L99-108 | `runtime/ui/fairygui/` 下不得引用 `GameManager`/`ClientManager` | R06 | 新增/拆出的内部头同样受限 |
| `Test-FairyGuiNativeEntryGuard` L44-55 | `manager/fairygui/` 下每个 `*.lua` 不得出现 `GameManager` | R01 | **新建 `FairyGuiStore.lua` 不得含 `GameManager` 字样**（含注释） |

**总结性约束**：R02/R03/R04 阶段，「收口」= 把**实现**搬到 Store/domain API，但**公共方法名必须以薄 wrapper 形式留在 `FairyGuiManager.lua` / `FairyGuiServices.lua`**，直到决定一并更新守卫为止。因此 R02 验收里「`FairyGuiManager.lua` 行数下降」是软目标——薄 wrapper 仍占行数，不要为压行数而删 wrapper 撞 gate。

---

## R00：兼容入口与门禁对齐

### 目标

修正原方案“先删 shim”的错误前提。当前 `tools/check_fgui_static.ps1` 要求 `bin/res/scripts/manager/FairyGuiManager.lua` 必须存在，且内容必须是：

```lua
return require("res.scripts.manager.fairygui.FairyGuiManager")
```

### 动作

1. 保留 `bin/res/scripts/manager/FairyGuiManager.lua`，把它定义为 compatibility facade。
2. `script_init.lua` 使用真实路径 `res.scripts.manager.fairygui.FairyGuiManager`；shim 仍然保留给历史脚本和门禁。
3. 文档和命名规范改成“新代码不新增短路径 require”，而不是“立即删除 shim”。

### 验收

- `tools/check_fgui_static.ps1` 通过。
- `bin/res/scripts/manager/FairyGuiManager.lua` 存在且内容保持 compatibility facade。
- 新增代码不再引入 `require("res.scripts.manager.FairyGuiManager")`。

---

## R01：引入 `FairyGuiStore`，迁移核心状态创建权

### 目标

建立 Store 作为唯一状态根，但第一步只接管 25 个核心字段。过渡期仍保留 `owner.X` / `self.X` alias，确保行为不变。

### 建议结构

```lua
local FairyGuiStore = Class("FairyGuiStore")

function FairyGuiStore:Init()
	self.objectState = {
		objects = {},
		objectsByHandle = {},
		uiRegistry = {},
		uiNameToKey = {},
		hiddenObjects = {},
		views = {},
		viewsByHandle = {},
		controllers = {},
		controllersByHandle = {},
		childKeysByParentKey = {},
		parentKeyByChildKey = {},
	}

	self.layerState = {
		currentSceneName = "Default",
		designWidth = nil,
		designHeight = nil,
		scaleMode = "stretch",
		layers = nil,
		layerPolicies = {},
		layerNextOrder = {},
		layerObjects = {},
		layerRoots = {},
		safeArea = nil,
		uiStack = {},
		popupStack = {},
		stackEntriesByKey = {},
		nextStackSerial = 0,
	}
end

function FairyGuiStore:GetObjectState()
	return self.objectState
end

function FairyGuiStore:GetLayerState()
	return self.layerState
end
```

### 动作

1. 新增 `bin/res/scripts/manager/fairygui/FairyGuiStore.lua`。
2. `FairyGuiManager.lua` 顶部 require `FairyGuiStore`。
3. `FairyGuiManager:Init()` 第一行创建 `self.store`，并增加 `GetStore()`。
4. Lifecycle / Layers 初始化时从 `owner:GetStore()` 取 state，而不是各自创建状态表。
5. 如需拆成多次合入，Store 可以临时提供 alias 兼容；进入 R02 后必须删除，不作为长期 API。
6. 只迁移表创建权，不在本步大规模替换所有读写。

### 验收

- 核心 25 个字段只由 `FairyGuiStore` 创建。
- 过渡期 `owner.X` / `self.X` 如暂留，必须指向 Store 内部表；R02 完成后删除。
- `tools/check_fgui_static.ps1` 通过。
- `tools/run_fgui_selftest.ps1 -Mode All -StopExisting` 通过；如涉及 close/open 生命周期，追加 `-Mode LongLoop`。

---

## R02：核心状态访问收口，删除核心 alias

### 目标

把核心 object/layer 状态的直接读写改成 Store accessor 或 domain API，然后删除 R01 保留的核心 alias。

### 动作

按小步迁移，避免一次改动过大：

1. `RegisterUI` / `GetUIConfig` 改走 `store:RegisterUI` / `store:GetUIConfig`。
2. `GetView` / `GetController` / `PutView` / `RemoveView` / `PutController` / `RemoveController` 改走 Store API。查找顺序必须保持旧逻辑：先 handle，再 key，再原始值。
3. `GetObjectInfo` / `GetObjectKey` / object index 读写改走 Store API。
4. scene / stack / popup stack 读写改走 Store 或 Layers domain API。
5. 删除 Lifecycle 的 `bindOwnerState` 和 Layers 的 owner alias 块。

### 验收

- 核心字段 `owner.objects`、`owner.views`、`owner.controllers`、`owner.uiStack`、`owner.currentSceneName` 等直接读写在 `manager/fairygui` 内清零，Store 兼容方法除外。
- `FairyGuiLayers` 的标量状态只读写 `store:GetLayerState()`：`currentSceneName`、`designWidth`、`designHeight`、`scaleMode`、`nextStackSerial` 不再保留同名 `self.X` 影子缓存。
- `FairyGuiManager.lua` 行数下降，但不把固定行数作为硬门禁。
- `tools/check_fgui_static.ps1` 通过。
- `tools/run_fgui_selftest.ps1 -Mode All -StopExisting` 和 `-Mode LongLoop -StopExisting` 通过。

---

## R03：收口对象生命周期 mutation

### 目标

避免 `Manager`、`Services`、`Probes`、`Layers` 到处直接写 `objects` / `objectsByHandle` / `hiddenObjects` / `uiNameToKey` / view/controller 表。

### 动作

在 Store 或 Lifecycle 上建立语义 API：

- `RegisterObject(objectInfo)`
- `UnregisterObject(objectInfo, reason)`
- `HideObject(objectInfo, reason)`
- `ShowObject(objectInfo, reason)`
- `AttachView(objectInfo, view, controller)`
- `DetachView(objectInfo, reason)`
- `RegisterChild(parentInfo, childInfo)`
- `DetachChild(childInfo)`

然后逐步替换：

- `OpenUI` / `OpenMvcUI` / `OpenView`
- `CloseUI`
- `FairyGuiServices:OpenServiceContainer`
- `FairyGuiProbes` 创建 probe 对象

### 验收

- 创建、隐藏、关闭、destroy、reopen、child UI 关闭路径都通过统一 API。
- `CloseUI` 中直接清多张表的代码明显减少。
- `tools/run_fgui_selftest.ps1 -Mode Lifecycle -StopExisting`、`-Mode LayerClose -StopExisting`、`-Mode All -StopExisting` 通过。

---

## R04：拆 `FairyGuiServices.lua`

### 前置条件

先完成 R03。否则 Services 拆文件会把 object 注册/关闭的直接写表逻辑扩散到更多文件。

### 目标

`FairyGuiServices` 保留为 public facade，内部按职责拆分。

| 新文件 | 职责 |
|---|---|
| `FairyGuiServiceCore.lua` | service 容器创建、皮肤解析、通用布局、关闭分发、统计公共逻辑 |
| `FairyGuiServiceToast.lua` | toast queue、dedupe、active toast 生命周期 |
| `FairyGuiServiceTip.lua` | normal tip / hover tip |
| `FairyGuiServiceLoading.lua` | loading ref count 与 loading UI |
| `FairyGuiServiceGuideMask.lua` | guide mask |
| `FairyGuiServiceMessageBox.lua` | message box / dialog |
| `FairyGuiServicePopupMenu.lua` | popup menu |

> 行数不是硬门槛。`FairyGuiServiceCore.lua` 可以作为 service 基础设施核心保留较大体量；真正的验收标准是职责是否集中、具体 service 是否已拆出、业务变更是否不需要反复改 Core。超过 400 行时只触发职责复查，不作为必须继续拆文件的理由。

### 动作

1. `FairyGuiServices.lua` 保留原 public 方法名，作为薄 wrapper 转发到子模块。
2. 子模块通过 `ServiceCore` 调用生命周期 API，不直接写 object/index 表。
3. 如果静态门禁仍检查 `FairyGuiServices.lua` 中的方法文本，优先保留 wrapper；需要彻底移走时，在同一步更新 `tools/check_fgui_static.ps1`。

### 验收

- `tools/check_fgui_static.ps1` 通过。
- `tools/run_fgui_selftest.ps1 -Mode CommonService -StopExisting`、`-Mode GuideMask -StopExisting`、`-Mode All -StopExisting` 通过。
- toast / loading / tip / mask / popup menu 行为不变。

---

## R05：迁移领域状态

### 目标

把 Events / Lists / Package / Controls / Services / Profiler 的状态也迁入 Store 的 domain state bag，删除这些模块把状态表挂到 `owner` 的模式。

### 顺序

1. Events / Timers：先迁移 `bindings`、`callbacks`、`transitionCallbacks`、`timers`。生命周期清理依赖这些状态，优先级最高。
2. Lists / Tree：迁移 `childrenByHandle`、list item、virtual list、tree 状态。注意 `CollectOwnedHandles` 依赖 list child handle。
3. Package / Resource：迁移 package refs、cache policy、resource fallback。
4. Controls / TextInput：迁移 text input policy 与 binding id。
5. Services / Profiler：迁移 service stats、toast/loading 状态和 perf stats。

### 验收

- 对应领域不再把状态表 alias 到 `owner`。
- 生命周期 residue 检查仍能覆盖 bindings、timers、children、list item、transition callbacks。
- 分别跑相关 selftest：`EventPayload`、`VirtualList`、`Tree`、`ResourcePolicy`、`ResourceFallback`、`TextInputPolicy`、`CommonService`、`All`。

---

## R06：拆 C++ `FairyGuiSystemInternal.h`

### 目标

降低内部头耦合。注意：不要只是新建一个 `FairyGuiPlatformIncludes.h` 后让所有 `.cpp` 继续 include 这个大聚合头；那只能移动问题，未必降低编译成本。

### 建议拆法

| 文件 | 职责 |
|---|---|
| `FairyGuiSystemImpl.h` | `FairyGuiSystemImpl` 类声明；允许包含 `cocos2d.h` 支撑基类和值类型，但 Ogre / FairyGUI / Win32 尽量前置声明 |
| `FairyGuiSystemCommonHelpers.h` | 环境变量、路径、DPI/input scale、UTF-8、统计字符串等跨实现文件工具 |
| `FairyGuiSystemRenderHelpers.h` | Ogre 材质、三角形转换、CPU clip / stencil clip 等渲染 helper |
| `FairyGuiSystemObjectHelpers.h` | range 控件、transition、package object 创建相关 helper |
| `FairyGuiSystemInputHelpers.h` | OIS key、输入命中日志、Win32 IME、文本编辑 helper |
| `FairyGuiSystemFairyIncludes.h` | 只给确实需要 FairyGUI/cocos/Ogre/Win32 完整类型的 `.cpp` 或 helper 使用 |
| `FairyGuiSystemInternal.h` / `FairyGuiSystemInternalHelpers.h` | 兼容门面；实现 `.cpp` 不再通过它们吃大聚合头 |

### 验收

- VS2017 clean rebuild 通过。
- `FairyGuiSystem.h` 仍保持 public facade，不暴露 Ogre / cocos / FairyGUI 细节。
- `FairyGuiSystemImpl.h` 不再 include `FairyGuiSystemFairyIncludes.h`、Ogre 头、FairyGUI 控件头或 Win32 头。
- `FairyGuiSystem*.cpp` 按需 include 具体 helper，不再 include `FairyGuiSystemInternal.h` / `FairyGuiSystemInternalHelpers.h` 聚合入口。
- `tools/check_fgui_static.ps1` 通过。
- `tools/run_fgui_selftest.ps1 -Mode All -StopExisting` 通过。

---

## R07：命名与兼容策略固化

### 目标

把命名规则和兼容入口策略写清楚，避免后续又出现同名文件和 require 路径歧义。

### 建议规则

- C++ 类型/文件：`FairyGui*`，例如 `FairyGuiSystem`、`FairyGuiLuaApi`。
- C++ 目录：`runtime/ui/fairygui/` 使用小写目录。
- Lua 内部模块：`res.scripts.manager.fairygui.*`。
- `bin/res/scripts/manager/FairyGuiManager.lua` 当前作为 compatibility facade 保留，但 `script_init.lua` 和新代码都使用真实路径 `res.scripts.manager.fairygui.FairyGuiManager`。
- 新代码不新增短路径 require；静态门禁禁止除 compatibility facade 外的 `res.scripts.manager.FairyGuiManager` 引用。业务 UI 入口仍通过 `FairyGuiManager` / Ctrl / View / Model。
- FGUI 子模块内不要写 `local self = self.owner`；需要调用 manager facade 时使用显式 `local owner = self.owner`，避免把子模块自身和外部 owner 混在同一个名字里。
- 工具、manifest、文档可使用 `fgui` 缩写，如 `fgui_autogen`、`check_fgui_static.ps1`。
- 是否删除 shim 需要单独决策，并同步更新 `script_init.lua`、历史脚本和 `tools/check_fgui_static.ps1`。

### 验收

- 文档、静态门禁和真实 require 路径一致。
- `tools/check_fgui_static.ps1` 能阻止短路径 require 和 `local self = self.owner` 回流。
- 如果修改 AGENTS.md，只追加 FGUI 命名小节，不影响其它项目规则。

---

## 验收命令建议

按改动面选择最小验证集：

| 改动类型 | 验收 |
|---|---|
| Lua 结构 / static guard | `powershell -ExecutionPolicy Bypass -File tools\check_fgui_static.ps1` |
| 生命周期 / 打开关闭 / Store 核心状态 | `powershell -ExecutionPolicy Bypass -File tools\run_fgui_selftest.ps1 -Mode All -StopExisting` + `-Mode LongLoop -StopExisting` |
| Service 拆分 | `-Mode CommonService -StopExisting`、`-Mode GuideMask -StopExisting`、`-Mode All -StopExisting` |
| Events / Lists / Controls | `-Mode EventPayload`、`-Mode VirtualList`、`-Mode Tree`、`-Mode TextInputPolicy` |
| Package / Resource | `-Mode ResourcePolicy`、`-Mode ResourceFallback` |
| C++ 内部头拆分 | VS2017 Debug x64 clean rebuild + `tools\check_fgui_static.ps1` + FGUI All selftest |

---

## 中止点

- R00 完成后：不会改变行为，只修正路线和兼容策略。
- R01 完成后：核心状态创建权已统一，但 alias 仍在，风险可控。
- R02 完成后：核心状态单一真源基本成立，可暂停后续 Services / C++ 拆分。
- R04 完成后：Lua 侧 service 结构清晰，后续可独立推进领域状态迁移。
- R06 完成后：C++ 内部依赖边界清晰，可单独合入。

最建议的第一轮执行范围是 **R00 + R01 + R02**。这三步能先把 FGUI Lua 侧最大的不稳定因素收口，同时不急着拆 Services 和 C++ 内部头。
