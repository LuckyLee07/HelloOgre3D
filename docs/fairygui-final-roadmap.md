# FairyGUI 最终目标与落地路线

## 1. 最终目标

FairyGUI 接入的最终目标不是简单显示 `.fui`，而是在当前项目中形成一套可维护、可扩展、可调试的 Lua 驱动 UI 子系统。

目标使用方式：

```lua
local ctrl = FairyGuiManager:Open("Act37TestMvc", {
	roleId = 1001
})

ctrl:SetText("txt_name", "Soldier")
ctrl:AddClick("btn_close", function()
	ctrl:Close()
end)
```

复杂 UI 默认走 MVC：`Open/Get` 返回 Ctrl，Ctrl 作为业务入口；AutoGen 负责生成控件绑定，View 负责封装控件访问，Model 负责界面数据。

简单调试或轻量页面仍可使用 ViewController：

```lua
local Act37TestView = Class("Act37TestView", ClassList.BaseFairyGuiView)

function Act37TestView:OnCreate(handle, param)
	self:AddClick("btn_close", function()
		self:Close()
	end)

	self:Refresh(param.roleId)
end
```

目标分层：

```text
Lua gameplay / sample
  |
  v
UI ViewController
  |
  v
FairyGuiManager.lua
  - UI 注册表
  - 生命周期
  - layer 管理
  - cache 管理
  - callback 分发
  - 自动解绑
  |
  v
GameManager tolua API
  |
  v
runtime/ui/fairygui/FairyGuiSystem
  - package 加载
  - GObject handle 管理
  - 输入转发
  - 事件桥接
  - Ogre 渲染适配
```

核心原则：

- C++ 侧负责 runtime 适配、渲染、输入、对象 handle、事件桥。
- Lua 侧负责 UI 组织、生命周期、业务逻辑、界面状态。
- Lua 不长期持有 `fairygui::GObject*`，优先通过 handle 和 binding id 通信。
- 参考 env1 MiniUI 的事件 key 分发思路，但不直接搬 MiniUI 源码和完整绑定体系。

## 2. 当前状态

已经完成或部分完成：

- 已接入官方 `FairyGUI-cocos2dx/libfairygui`。
- 已增加 `cocos-lite` 兼容层。
- 已增加 `FairyGuiSystem` 并接入 `ClientManager` 生命周期。
- 已支持 `.fui` package 加载和 `UIPackage::createObject`。
- 已支持将 FairyGUI 渲染命令转成 Ogre `ManualObject` overlay 绘制。
- 已在 Lua 侧增加 `FairyGuiManager.lua`。
- 已支持 `OpenUI / CloseUI / CloseAll`。
- 已增加 `UIRegistry.lua`，支持用逻辑名打开 UI。
- 已增加轻量 ViewController 基础设施：
  - `bin/res/scripts/ui/BaseFairyGuiView.lua`
  - `FairyGuiManager:OpenView(...)`
  - `FairyGuiManager:CloseView(...)`
  - `FairyGuiManager:GetView(...)`
- 已增加轻量 MVC 基础设施：
  - `bin/res/scripts/ui/FairyGuiBaseCtrl.lua`
  - `bin/res/scripts/ui/FairyGuiBaseModel.lua`
  - `bin/res/scripts/ui/FairyGuiBaseMvcView.lua`
  - `bin/res/scripts/ui/FairyGuiAutoGen.lua`
  - `FairyGuiManager:Open("Name")` 对 MVC UI 返回 Ctrl
  - `FairyGuiManager:GetController(...)`
- 已增加示例 View：
  - `bin/res/scripts/ui/views/Act37TestView.lua`
- 已增加示例 MVC：
  - `Act37TestMvcAutoGen.lua`
  - `Act37TestMvcCtrl.lua`
  - `Act37TestMvcModel.lua`
  - `Act37TestMvcView.lua`
  - 覆盖 `Popup/cache/modal/fullScreen/reopen/resize/complex events`
- 已增加基础控件操作桥：
  - child handle 查询和缓存
  - `SetText / SetVisible / SetPosition / SetSize`
  - `SetIcon / SetLoaderUrl / SetControllerIndex`
- 已增加基础生命周期：
  - `OnCreate / OnReopen / OnShow / OnHide`
  - `OnClose / OnRemove / OnDestroy / OnResize`
- 已增加基础 layer / sorting order：
  - `Background / Normal / Popup / Guide / Top / Toast`
  - 每个 layer 会按需创建 FairyGUI root 子容器
  - `createFairyGuiContainer / addFairyGuiObjectToParent` 支持 Lua 按父容器挂载
  - `BringToFront`
  - modal mask handle
- 已增加第一版 UI 栈 / Popup 栈：
  - `uiStack / popupStack`
  - `GetTopUI / GetTopPopup`
  - `CloseTop / CloseTopPopup`
  - `DumpStacks`
  - 打开、重开、置顶、关闭、缓存隐藏时维护栈状态
  - `popupMode = stack / single / replace` 和 `popupGroup` 第一版打开策略
  - 支持 `priority / sortingPriority / sortingOrder`、`BringToFront` 稳定置顶和 modal mask 绑定顶层弹窗关闭
  - `closeOnEscape` 与 `FairyGuiManager_HandleKeyPressed`，支持 ESC 关闭顶层弹窗
  - `CloseLayer / CloseGroup / CloseScene / ChangeScene` 第一版统一清理入口
  - registry 支持 `group / uiGroup / scene / sceneName / closeOnSceneChange / destroyOnSceneChange`
- 已增加基础资源策略：
  - package 加载缓存
  - package 引用计数
  - `cache` 隐藏复用
  - `unloadPackageOnClose / clearPackage` 控制关闭时释放 package
- 已增加基础 screen adapt：
  - `fullScreen / adaptScreen`
  - `marginLeft / marginRight / marginTop / marginBottom`
  - `center / alignX / alignY / fitInScreen`
  - `designWidth / designHeight / scaleMode` 设计坐标换算
  - resize 后统一重算并回调 `OnResize`
- 已增加最小 click 事件桥：
  - `BaseFairyGuiView:AddClick(childPath, callback)`
  - `FairyGuiManager:AddClick(...)`
  - `GameManager:addFairyGuiClickListener(...)`
  - `FairyGuiSystem::AddObjectHandleClickListener(...)`
- 已增加通用事件桥：
  - `AddChanged`
  - `AddClickItem`
  - `AddRightClick`
  - `AddTouchBegin / AddTouchEnd`
  - `AddDragStart / AddDragMove / AddDragEnd`
- 已增加最小鼠标输入桥：
  - OIS mouse move/down/up 转发到 `FairyGuiSystem`
  - `FairyGuiSystem` 通过 FairyGUI `InputProcessor::touchDown/touchMove/touchUp` 驱动点击
  - UI 命中时阻断相机鼠标输入
- 2026-05-20 已完成一轮 VS2017 `Release|x64` 运行时验收：
  - `HELLO_FGUI_ACT38_SELF_TEST=1`：`Act37TestMvc` 打开和 `OnReopen` 正常，`Act38Test` 能加载 `act_38_test.fui`，List API 自测通过。
- `HELLO_FGUI_INPUT_SELF_TEST=1`：调试点击能触发 `TouchBegin / TouchEnd`、按钮 `Click`、列表 `ClickItem`、关闭按钮和 `CloseTopPopup`，关闭后 UI 栈与 Popup 栈归零。
- `HELLO_FGUI_LAYER_SELF_TEST=1` 与 `HELLO_FGUI_LAYER_CLOSE_SELF_TEST=1`：`Normal / Popup / Top / Toast` layer root 可创建，`CloseTopPopup -> CloseLayer("Top") -> CloseGroup("LayerProbe")` 可按预期清理样例 UI。
- `HELLO_FGUI_COMMON_SERVICE_DEMO=1`：启动后可按节奏展示 Toast 队列、Loading 引用计数、Tip、MessageBox、PopupMenu 和 GuideMask，方便肉眼检查通用 UI 服务效果。
- FGUI 样例、自测和 demo 入口集中在 `bin/res/scripts/samples/fgui_init.lua`，主 `game_init.lua` 只负责通用启动流程。

当前仍需继续完善：

- 完整输入转发，包括键盘、滚轮、输入框/IME 和更严格的焦点处理；鼠标点击链路已通过调试注入验证。
- 复杂事件的事件数据透传已补第一版，当前 `itemHandle / itemIndex / x / y / button / touchId / wheelDelta / dragDeltaX / dragDeltaY` 可到 Lua；后续继续补更复杂的 item 数据与焦点场景。
- UI 栈、弹窗栈和 layer root 已具备第一版，已支持 ESC 关闭顶层弹窗、`single / replace / stack` 打开策略、按 layer/group/scene 统一清理、priority 置顶和 modal 空白关闭；后续仍需更多真实弹窗样例。
- reopen / cache / hide / destroy 已补 cache 自测和 `Close(reason)`，后续继续沉淀业务规范。
- Dialog / Toast / Loading / MessageBox / Tip / GuideMask 等通用 UI 能力已有动态第一版；Toast 已补排队/去重，Loading 已补引用计数/超时清理，GuideMask 已补矩形高亮和点击穿透，后续补资源化样式。
- 屏幕适配已有第一版统一布局规则：居中弹窗、贴边 popup、Toast/Loading 文案区域和设计坐标 GuideMask 会在 resize 后重算；可用 `HELLO_FGUI_SCREEN_ADAPT_SELF_TEST=1` 与 `HELLO_FGUI_SCREEN_ADAPT_DEMO=1` 验证。
- package 预加载、场景级清理、资源泄漏 Dump 和调试面板仍需继续补。
- AutoGen 已能从 FairyGUI 导出 XML 生成 manifest、MVC 骨架和生成 registry；后续要补 CI 化检查、批量生成入口和更完整的控件类型覆盖。

下一轮优先级建议：

1. 渲染裁剪与遮罩：当前已完成第一版矩形 scissor 裁剪和 stencil 状态桥接。`cocos-lite` 会维护 scissor enabled/rect，且每帧会重置 renderer 的裁剪状态；`StencilStateManager` 会把 `write/test/restore`、嵌套深度和倒置标记透传给 `Renderer`；`FairyGuiSystem` 会在 `CustomCommand` 后同步状态，并在写入 Ogre `ManualObject` 前对三角形做 CPU 裁剪。复杂 UI 的 `ScrollPane / List` 已具备基础越界裁剪能力；普通/倒置 mask 当前按 stencil 图形实际索引顶点的包围矩形裁剪。已新增 `FGUI_OpenMaskProbe()` / `HELLO_FGUI_MASK_SELF_TEST=1`，用于打开真实 `GComponent::setMask()` 的普通与倒置 mask 可视化样例。后续仍需补像素级非矩形 GPU stencil 和更低成本的渲染队列状态切换方案。
2. 输入补齐：鼠标点击链路已验证，下一步补滚轮、键盘、文本输入/IME 和焦点状态，保证 `GTextInput`、滚动列表、快捷键关闭等交互可用。
3. 资源与性能观测：package refCount、material/texture 数量、render command 数、triangle 数和事件分发次数已接入 Dump，C++ FGUI update/render/load/create/event 已接入 Tracy；后续补 Lua Open/Close 耗时和调试面板。
4. 通用控件能力：补 `Progress / Slider / ComboBox / Transition` 等常用接口，再用一个更接近业务的页面组合做验收。

## 3. MiniUIManager 参考清单与当前缺口

env1 的 `MiniUIManager` 和 `UIManager` 不建议直接搬进当前项目，但它们已经覆盖了一批成熟游戏 UI 必需能力，可以作为后续迭代的功能清单。

当前项目的取舍原则：

- 保留当前 `FairyGuiSystem(C++) + FairyGuiManager(Lua)` 的 handle 桥接路线。
- 不让 Lua 长期直接依赖 MiniUI/Cocos 节点对象，避免把 env1 的运行时耦合带进 Ogre 项目。
- 吸收 MiniUIManager 的生命周期、层级、适配、资源策略和统计经验。
- 复杂 UI 默认让业务层拿 Ctrl，不让外部业务直接依赖 AutoGen 或 View。

### 3.1 值得参考的实现

MiniUIManager 中值得借鉴的能力：

1. 重复打开复用
   - 已打开的 UI 再次打开时不重新创建节点，而是刷新参数、置顶、重新显示。
   - 当前框架已有同 key 复用雏形，但还需要补 `OnReopen(param)`，避免把首次创建和再次打开都塞进 `OnShow`。

2. 打开列表和关闭列表
   - env1 的 `UIManager` 区分 `uiCtrlOpenList` 和 `uiCtrlCloseList`，关闭后可以隐藏并缓存。
   - 当前框架目前主要是 `objects / views / controllers` 映射，缺少 cache 策略和关闭列表。

3. UI 生命周期
   - MiniUIManager 有 `onLoad / init / onShow / onRemove`，UIManager 有 `Refresh / Reset`。
   - 当前框架已有 `OnCreate / OnShow / OnHide / OnDestroy` 和 MVC `Start / RegisterUIEvents / Refresh / Reset`，但还需要明确每个回调的职责边界。

4. 层级与置顶
   - `PopupUI` 通过 sorting order 把界面提到最上层。
   - 当前框架缺少 layer 容器、同层排序、弹窗栈和 modal 阻断规则。

5. rootNode 和场景挂载
   - MiniUIManager 能根据 `param.rootNode` 挂到指定根节点，否则挂当前 UI 场景。
   - 当前框架暂时统一加到 FairyGUI root，后续应支持 `layer/root/parent` 参数。

6. 全屏和异形屏适配
   - MiniUIManager 有 `fullScreen` 配置、边缘适配、缓存适配信息并在尺寸变化时重算。
   - 当前框架只完成基础 resize，需要补页面级 layout 策略和 resize 后重适配。

7. 资源释放策略
   - `CloseUI(name, isClearRes)` 可控制是否移除 package。
   - 当前框架缺少 package 引用计数、按场景预加载、按 UI 关闭策略卸载。

8. 显示统计
   - MiniUIManager 在 UI show/end 时有统计入口。
   - 当前框架后续应把 UI 打开、关闭、事件、渲染批次数接入日志、Tracy 或调试面板。

9. 通用页面创建
   - `UniversalCratePage` 这类接口说明项目里存在动态创建子页面、挂到背景控件、按层级打开的需求。
   - 当前框架后续应支持“子组件/子页面”能力，而不是只有顶层 UI。

### 3.2 当前框架仍缺的内容

优先级从高到低：

1. 事件系统补齐
   - `Changed`
   - `ClickItem`
   - `RightClick`
   - `TouchBegin / TouchEnd`
   - `DragStart / DragMove / DragEnd`
   - 事件解绑和关闭 UI 后的回调失效验证。

2. UI 层级系统
   - `Background / Normal / Popup / Top / Toast / Guide`。
   - 同层 sorting order。
   - 弹窗 modal 遮罩。
   - 点击空白关闭弹窗。
   - UI 命中后输入阻断按层级处理。

3. 生命周期标准化
   - `OnCreate`：只做一次性控件绑定。
   - `OnOpen`：每次打开入口。
   - `OnReopen`：已存在实例再次打开。
   - `OnShow`：可见性变为 true。
   - `OnHide`：可见性变为 false。
   - `OnClose`：业务关闭。
   - `OnDestroy`：释放对象和解绑。
   - MVC 对应 `Start / RegisterUIEvents / Refresh / Reset / Dispose`。

4. 缓存和销毁策略
   - registry 增加 `cache = true/false`。
   - `Close` 根据 cache 决定 hide 还是 destroy。
   - 支持 `Destroy(name)` 强制销毁。
   - 支持 `CloseAll(layer)` 和场景切换清理。

5. 资源管理
   - package 引用计数。
   - package 预加载。
   - package 显式释放。
   - atlas texture/material 释放。
   - 资源泄漏 Dump。

6. 控件能力补齐
   - `SetProgress`
   - `SetSelectedIndex / GetSelectedIndex`
   - `SetGrayed / SetTouchable`
   - `SetTitle / SetValue`
   - `PlayTransition / StopTransition`
   - `List` 数据绑定、itemRenderer、虚拟列表、item 复用和大数据刷新性能。

7. AutoGen 生成链
   - 从 FairyGUI 导出数据生成 `*AutoGen.lua`。
   - AutoGen 只维护控件路径和强类型访问。
   - 业务代码只写 Ctrl/Model/View。
   - 生成文件和手写文件分离，避免后续覆盖业务代码。
   - 当前已提供 `tools/fairygui_autogen.py` 作为第一版生成入口，支持通过 manifest 或 `--control` 列表生成 MVC 四件套。
   - 当前已提供 `tools/fairygui_manifests/act_37_test.json` 作为真实 fui 样例 manifest。

8. 调试和观测
   - `FairyGuiManager:Dump()`
   - `DumpOpenUIs / DumpPackages / DumpBindings`
   - UI 打开关闭日志开关。
   - Tracy zone 覆盖 `Open / Close / LoadPackage / CreateObject / DispatchEvent / Render`。
   - 可选调试面板显示当前打开 UI、layer、绑定事件数、package 引用数。

9. 屏幕适配
   - root resize 后自动通知打开的 UI。
   - 支持 fullscreen 页面。
   - 支持安全区/边缘留白策略。
   - 支持页面自定义 `OnResize(width, height)`。

10. 子组件和动态节点
   - 支持把一个 fui component 挂到指定父控件。
   - 支持子页面独立 Ctrl。
   - 支持关闭父 UI 时递归清理子 UI。

### 3.3 不建议直接照搬的部分

- 不直接引入 `MiniUISceneMgr`，当前项目应由 `FairyGuiSystem` 维护 FairyGUI root。
- 不让业务 Lua 直接持有底层 C++ `GObject*`，仍通过 handle 和封装 API。
- 不复制 env1 的全局依赖链，例如 `ClientMgr / UIFrameMgr / UIDisplayStatisticsForShowStart`。
- 不把所有 UI 能力堆到单个 Manager，复杂能力应逐步拆到 layer、resource、event、debug 等子模块。
- 不把 View 作为外部业务入口，复杂 UI 对外优先暴露 Ctrl。

### 3.4 推荐迭代顺序

1. 补全事件桥接，先让 Ctrl 能承接常见交互。
2. 增加 layer 和 popup/modal，解决复杂页面共存。
3. 明确生命周期和 cache 策略，解决重复打开、关闭、销毁。
4. 增加资源引用计数和 Dump，避免 fui/atlas 泄漏。
5. 做 List/itemRenderer 和 Transition，覆盖常见复杂 UI。
6. 做 AutoGen 生成链，让手写业务代码和生成代码分离。
7. 增加调试面板和 Tracy 观测，让 UI 性能和泄漏可见。

## 4. 分阶段落地路线

### Phase 1: 事件桥接

目标：让 Lua View 能响应 FairyGUI 控件事件。

第一版先支持点击：

```lua
function Act37TestView:OnCreate(handle, param)
	self:AddClick("btn_close", function()
		self:Close()
	end)
end
```

实现要点：

- Lua 分配 `callbackId`，保存 `callbackId -> function`。
- C++ 注册 FairyGUI listener，捕获 `callbackId`。
- FairyGUI 事件触发后，C++ 调 Lua 全局分发函数。
- `bindingId` 用于精确解绑。
- View `OnDestroy` 时统一清理 bindings。

验收：

- 点击 FairyGUI 按钮能触发 Lua callback。
- 关闭 View 后旧 callback 不再触发。
- 多次打开/关闭同一个 View 不产生重复回调。

### Phase 2: UI 注册表

目标：统一 UI 名称、资源路径、组件名、层级和缓存策略。

新增：

```text
bin/res/scripts/ui/UIRegistry.lua
```

示例：

```lua
return {
	Act37TestView = {
		viewClass = "res.scripts.ui.views.Act37TestView",
		package = "act_37_test",
		component = "main_pifushoumai",
		layer = "Normal",
		cache = false,
	}
}
```

目标接口：

```lua
FairyGuiManager:Open("Act37TestView", param)
FairyGuiManager:Close("Act37TestView")
FairyGuiManager:Get("Act37TestView")
```

### Phase 3: UI Layer 管理

至少支持：

```text
Background
Normal
Popup
Top
Toast
```

每个 layer 在 FairyGUI root 下对应一个容器，`Open` 时按 registry 配置加入对应层。

当前第一版使用真实 layer root 容器、sorting order 和 Lua 栈共同维护复杂 UI 顺序：

- `FairyGuiManager:EnsureLayerRoot(layer)` 会按需创建 `Layer_Normal / Layer_Popup ...` 容器。
- `Open` 和 modal mask 会挂载到对应 layer root，而不是全部直接挂到 FairyGUI root。
- layer root 会在窗口 resize 后同步刷新尺寸，并保持各层基础 sorting order。
- 打开的 UI 会在 resize 后统一重算 `fullScreen/adaptScreen/margin/center/align/fitInScreen` 布局，并同步刷新 modal mask、GuideMask 分段遮罩、Toast/Loading 文案位置。
- `FGUI_OpenLayerSample()` 可打开 `Normal / Popup / Top / Toast` 四层验证样例，`FGUI_CloseLayerSample()` 可清理样例。
- 也可设置 `HELLO_FGUI_LAYER_SELF_TEST=1` 在启动后自动打开 layer 验证样例并输出 dump。
- `FGUI_RunLayerCloseSelfTest()` 会按 `CloseTopPopup -> CloseLayer("Top") -> CloseGroup("LayerProbe")` 验证真实 layer root 下的关闭和栈清理；也可设置 `HELLO_FGUI_LAYER_CLOSE_SELF_TEST=1` 自动执行。
- `docs/fairygui-assets.md` 记录 `bin/res/assets` XML 源数据、`bin/res/fuires` 运行时 package 和 manifest 的对应关系。
- `uiStack` 记录所有可入栈 UI 的打开顺序。
- `popupStack` 记录 `Popup / Guide / Top / Toast` 等弹窗层 UI。
- `BringToFront` 会同步刷新 sorting order 和栈顶。
- `CloseTop(layer)` 可关闭指定层或全局栈顶 UI。
- `CloseTopPopup()` 可关闭当前最上层弹窗。
- `DumpStacks()` 用于调试当前栈状态。
- `popupMode` 支持 `stack / single / replace`，`popupGroup` 用于同组弹窗互斥。
- `priority / sortingPriority / sortingOrder` 支持同层稳定置顶，modal mask 点击只关闭当前顶层绑定弹窗。
- `GuideMask` 支持矩形高亮区域，遮罩外区域可关闭，空洞区域可点击穿透。
- `closeOnEscape` 默认允许 ESC 关闭顶层弹窗，设置为 `false` 可禁止。
- `CloseLayer(layerName, forceDestroy)` 按层关闭。
- `CloseGroup(groupName, forceDestroy)` 按 `group / uiGroup / popupGroup` 关闭。
- `CloseScene(sceneName, forceDestroy)` 按场景关闭，默认跳过 `closeOnSceneChange = false` 的常驻 UI。
- `ChangeScene(sceneName, forceDestroy)` 会先清理旧场景，再切换当前 UI 场景名。

registry 可补充：

```lua
Act37TestMvc = {
	layer = "Popup",
	group = "Sample",
	scene = "Default",
	closeOnSceneChange = true,
	destroyOnSceneChange = false,
}
```

后续仍需补：

- layer root 已有 `LayerProbe` 验证样例，后续还需要补更真实的业务页面组合，例如主界面 + 多弹窗 + 顶层 Toast。
- 弹窗策略的更多真实样例覆盖，例如多组弹窗、强制销毁 replace、关闭动画后移除。
- modal mask 点击关闭的更多样例覆盖。
- 场景切换时和玩法主流程的正式接入点，例如章节切换、sample 切换、重载脚本。

### Phase 4: 输入转发与命中处理

目标：将 OIS 鼠标、键盘事件转成 FairyGUI 可消费的输入事件。

验收：

- Button hover/click 状态正常。
- UI 命中时相机和游戏输入不响应。
- UI 外点击仍保留原有游戏输入行为。

### Phase 4.5: FGUI 真实交互验证待办

当前 VS2017 `Release|x64` 已确认可以编译通过，`bin/HelloOgre3D.exe` 可以从 `bin` 目录正常启动，并能加载 `act_37_test.fui`、打开 `Act37TestMvc`、触发 `OnReopen`。

本轮暂缓真实点击验收，后续需要回头补一轮手动或可控注入验证：

- 手动点击 `Act37TestMvc` 的 `btn_buy / btn_detail / reward_list / btn_close`。
- 预期 `bin/Sandbox.log` 出现 `action click`、`reward item click`、`close click`、`OnClose / OnRemove` 等日志。
- 重点确认 `reward_list` 的 `ClickItem` 能透传 `itemIndex / itemData / itemHandle`。
- 当前脚本自动化点击不可靠：OIS/DirectInput 读取到的 `evt.state.X.abs / Y.abs` 可能停在固定值，例如 `50,50`，不能作为真实交互结论。
- 已补 Debug/Test 专用输入注入入口，Lua 可通过 `FairyGuiManager:DebugInjectClick(x, y, button)` 或全局 `FGUI_DebugInjectClick(x, y, button)` 直接按客户端坐标注入点击。
- 可设置 `HELLO_FGUI_INPUT_SELF_TEST=1` 启动自测；自测会依次注入 `btn_buy`、`reward_list`、`btn_close`，用于验证 `action click`、`reward item click`、`close click` 链路。
- 可设置 `HELLO_FGUI_INPUT_DEBUG=1` 输出命中对象日志，便于定位坐标和 FairyGUI `hitTest` 结果。
- 若后续继续改 `cocos-lite` 坐标系统，需要重点复查 `Node::convertToNodeSpace / convertToWorldSpace` 与 FairyGUI `hitTest` 的一致性。

### Phase 5: 常用 UI 能力

优先级：

1. Button click。
2. Text 设置。
3. Visible / Position / Size。
4. Loader 图片。
5. List itemRenderer / clickItem / virtual list / item reuse。
6. Slider / ProgressBar changed。
7. Transition 播放。
8. Popup / Dialog。
9. Toast / Tip / MessageBox。
10. Loading / GuideMask。

目标接口示例：

```lua
self:SetText("txt_name", "Soldier")
self:SetVisible("btn_close", true)
self:SetProgress("bar_hp", 0.75)
self:PlayTransition("show")
```

当前已落地的 List 基础能力：

- `SetListData / GetListData`
- `RefreshList / RefreshListItem`
- `AppendListItem / UpdateListItem / RemoveListItem / ClearList`
- item renderer 支持 `SetText / SetIcon / SetLoaderUrl / SetVisible / SetPosition / SetSize / SetControllerIndex`
- `Act38Test` 已作为 List + Controller + MVC 标准样例，`HELLO_FGUI_ACT38_SELF_TEST=1` 会覆盖打开、追加 item、刷新 item、dump count。

### Phase 5.5: AutoGen 生成链

第一版生成链先不直接解析 `.fui` 二进制，而是读取导出辅助 manifest 或命令行控件列表，生成 Lua 侧 MVC 骨架。这样可以先固定工程结构，后续再替换 manifest 来源。

命令示例：

```bash
python tools/fairygui_autogen.py ^
	--manifest tools/fairygui_manifests/act_37_test.json
```

生成内容：

- `Act37TestMvcAutoGen.lua`：继承 `FairyGuiAutoGen`。
- `Act37TestMvcView.lua`：继承 `FairyGuiBaseMvcView`，集中缓存控件 handle。
- `Act37TestMvcModel.lua`：继承 `FairyGuiBaseModel`。
- `Act37TestMvcCtrl.lua`：继承 `FairyGuiBaseCtrl`，作为业务入口。

默认策略：

- AutoGen 可以覆盖。
- Ctrl / Model / View 默认不覆盖，避免误删手写业务代码。
- 生成后需要把脚本输出的 registry 片段合并到 `bin/res/scripts/ui/UIRegistry.lua`。

### Phase 6: 资源缓存与释放

目标：

- package 加载记录引用计数。
- View 关闭时根据 `cache` 决定销毁或隐藏。
- package 可按场景预加载。
- package 可显式释放。
- atlas texture/material 从 Ogre 侧正确释放。

### Phase 7: 调试与性能观测

建议增加：

- `FairyGuiManager:Dump()`
- `FairyGuiManager:DumpViews()`
- `FairyGuiManager:DumpPackages()`
- `FairyGuiManager:SetDebugEventLogEnabled(enabled)`
- Tracy zone：
  - `FairyGuiSystem::Update`（已接）
  - `FairyGuiSystem::Render`（已接）
  - `FairyGuiSystem::LoadPackage`（已接）
  - `FairyGuiSystem::CreateObject`（已接）
  - `FairyGuiSystem::DispatchEvent`（已接）
- 渲染统计：
  - draw call / triangle / material / texture 数量。
  - List 刷新耗时和 item renderer 次数。
  - package / atlas / material 泄漏计数。

## 5. 近期完成定义

FGUI 接入近期可以认为完成的标准：

- 可以通过 `FairyGuiManager:Open("Act37TestView")` 打开界面。
- 可以通过 `FairyGuiManager:Open("Act37TestMvc", param)` 打开 MVC 界面并返回 Ctrl。
- Ctrl 可以完成常见控件操作，不要求外部业务直接拿 AutoGen/View。
- 点击按钮能回调 Lua。
- UI 关闭时事件自动解绑。
- UI 命中时能阻断游戏输入。
- 至少一个样例界面覆盖按钮、文本、列表、弹窗。
- 重复打开/关闭样例界面无明显内存增长。
- Tracy 能看到 FairyGUI update/render/event 的基本耗时。
