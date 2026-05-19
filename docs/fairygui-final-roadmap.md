# FairyGUI 最终目标与落地路线

## 1. 最终目标

FairyGUI 接入的最终目标不是简单显示 `.fui`，而是在当前项目中形成一套可维护、可扩展、可调试的 Lua 驱动 UI 子系统。

目标使用方式：

```lua
local view = FairyGuiManager:Open("Act37TestView", {
	roleId = 1001
})
```

界面逻辑只关心业务：

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
- 已增加轻量 ViewController 基础设施：
  - `bin/res/scripts/ui/BaseFairyGuiView.lua`
  - `FairyGuiManager:OpenView(...)`
  - `FairyGuiManager:CloseView(...)`
  - `FairyGuiManager:GetView(...)`
- 已增加示例 View：
  - `bin/res/scripts/ui/views/Act37TestView.lua`
- 已增加最小 click 事件桥：
  - `BaseFairyGuiView:AddClick(childPath, callback)`
  - `FairyGuiManager:AddClick(...)`
  - `GameManager:addFairyGuiClickListener(...)`
  - `FairyGuiSystem::AddObjectHandleClickListener(...)`
- 已增加最小鼠标输入桥：
  - OIS mouse move/down/up 转发到 `FairyGuiSystem`
  - `FairyGuiSystem` 通过 FairyGUI `InputProcessor::touchDown/touchMove/touchUp` 驱动点击
  - UI 命中时阻断相机鼠标输入

当前仍缺失：

- 完整输入转发，包括键盘、滚轮、拖拽和更严格的焦点处理。
- `Changed / ClickItem / RightClick` 等更多事件类型。
- UI 注册表。
- UI layer。
- Dialog / Toast / Loading 等通用 UI 能力。
- 资源释放、缓存和调试面板。

## 3. 分阶段落地路线

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

### Phase 4: 输入转发与命中处理

目标：将 OIS 鼠标、键盘事件转成 FairyGUI 可消费的输入事件。

验收：

- Button hover/click 状态正常。
- UI 命中时相机和游戏输入不响应。
- UI 外点击仍保留原有游戏输入行为。

### Phase 5: 常用 UI 能力

优先级：

1. Button click。
2. Text 设置。
3. Visible / Position / Size。
4. Loader 图片。
5. List itemRenderer / clickItem。
6. Slider / ProgressBar changed。
7. Transition 播放。
8. Popup / Dialog。
9. Toast / Tip。
10. Loading。

目标接口示例：

```lua
self:SetText("txt_name", "Soldier")
self:SetVisible("btn_close", true)
self:SetProgress("bar_hp", 0.75)
self:PlayTransition("show")
```

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
  - `FairyGuiSystem::Update`
  - `FairyGuiSystem::Render`
  - `FairyGuiSystem::LoadPackage`
  - `FairyGuiSystem::CreateObject`
  - `FairyGuiSystem::DispatchEvent`

## 4. 近期完成定义

FGUI 接入近期可以认为完成的标准：

- 可以通过 `FairyGuiManager:Open("Act37TestView")` 打开界面。
- 点击按钮能回调 Lua。
- UI 关闭时事件自动解绑。
- UI 命中时能阻断游戏输入。
- 至少一个样例界面覆盖按钮、文本、列表、弹窗。
- 重复打开/关闭样例界面无明显内存增长。
- Tracy 能看到 FairyGUI update/render/event 的基本耗时。
