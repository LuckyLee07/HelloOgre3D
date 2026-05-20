# FairyGUI 成熟业务框架 TODO

## 目标

当前目标不是只把 `.fui` 显示出来，而是把当前项目里的 `FairyGuiSystem(C++) + FairyGuiManager(Lua) + MVC/AutoGen` 演进到接近 env1 `MiniUIManager` 的成熟业务框架。

最终期望：

- 业务只通过 `FairyGuiManager:Open("UIName", param)` 打开界面，并拿到 Ctrl 作为业务入口。
- UI 的生命周期、层级、资源、事件、适配和清理规则稳定，不依赖调用方临时补逻辑。
- `.fui` 导出后可以通过工具生成 AutoGen/View/Ctrl/Model 基础代码和注册表。
- 大型 UI 增多后，可以用 Dump、日志、Tracy 和自测样例定位资源泄漏、事件泄漏、层级错乱和渲染开销。

## 成熟度验收标准

- [ ] 新增一个业务 UI 只需要放入 `.fui`/资源，执行生成工具，补 Ctrl/Model 业务逻辑，再在注册表声明少量策略。
- [ ] 普通页面、弹窗、提示、引导层、顶层遮罩可以共存，关闭顺序和输入遮挡符合预期。
- [ ] UI 重复打开、隐藏缓存、强制销毁、场景切换清理都有明确规则和日志可查。
- [ ] Click、Changed、ClickItem、RightClick、Drag、Touch、Wheel、Key、TextInput 事件能稳定桥接到 Lua，并在 UI 关闭后自动解绑。
- [ ] fullscreen/adapt/margin/safe area 在窗口尺寸变化后统一重算，页面只关注自身布局策略。
- [~] Package、纹理、材质、绑定数、打开 UI 数、渲染命令数、三角形数有统一 Dump；基础 `DumpHealth`、package/UI 引用 Dump 和第一版资源告警已具备，长期循环泄漏判定仍需继续补。
- [ ] AutoGen 能覆盖常用控件、列表 item、controller、transition，并能在 CI 或本地命令里检查生成文件是否过期。
- [ ] 至少有一组真实业务样例覆盖页面、弹窗、列表、遮罩、拖拽、文本输入、场景清理和性能观测。

## 当前完成快照

- [x] 官方 FairyGUI-cocos2dx/libfairygui 已接入，并通过 `cocos-lite` 适配到 Ogre。
- [x] `FairyGuiSystem` 已接入 `ClientManager` 生命周期，支持 package 加载、对象 handle、事件桥接、输入转发和 Ogre 绘制。
- [x] Lua 侧已有 `FairyGuiManager.lua`，支持 `OpenUI/CloseUI/CloseAll`、registry、View、MVC、Ctrl 业务入口。
- [x] 基础生命周期已具备：`OnCreate/OnReopen/OnShow/OnHide/OnClose/OnRemove/OnDestroy/OnResize`。
- [x] 基础层级已具备：`Background/Normal/Popup/Guide/Top/Toast`、layer root、sorting order、modal mask、UI stack、popup stack。
- [x] 基础资源策略已具备：package 缓存、refCount、`cache` 隐藏复用、关闭时可卸载 package。
- [x] 基础事件已具备：Click、Changed、ClickItem、RightClick、TouchBegin/End、DragStart/Move/End。
- [x] Act37 MVC、Act38 列表、LayerProbe、MaskProbe 已作为样例或自测入口。
- [~] 渲染裁剪与遮罩已有矩形 scissor/stencil 状态桥接和 MaskProbe 样例，但还不是完整 GPU 像素级 stencil。
- [~] AutoGen 已能生成 Act38 MVC 基础代码和 registry，但批量化、CI 化、更多控件覆盖还没完成。
- [~] 输入已覆盖鼠标、滚轮、ESC 关闭、键盘焦点和 `GTextInput` 最小文本输入，但 Tab 焦点切换、复杂编辑行为和 IME 还没补齐。
- [x] 关闭销毁路径已有生命周期残留检测，覆盖 binding、timer、child/list cache、focus、stack、layer、view/controller map。

## Phase 0: 可观测性与自测闭环

这部分是后续继续迭代的地基，优先级最高。

- [x] Act37 MVC 打开、重复打开、关闭自测。
- [x] Act38 列表加载、列表数量、关闭自测。
- [x] LayerProbe 真实 layer 打开和关闭自测。
- [x] MaskProbe 真实 mask 可视化样例。
- [x] MaskProbe 打开后关闭清理自测。
- [x] Lua Dump 增加渲染命令数、三角形数、材质数、纹理数和 runtime handle/binding 数。
- [x] Lua Dump 增加 package -> UI 引用关系，能看到 refCount、open/hidden/cache 数和当前引用 UI。
- [x] Lua Dump 增加资源引用告警，能标出 refCount 异常、包空挂引用、隐藏 UI 缺引用和可见 UI 脱离打开栈等问题。
- [x] Dump 增加打开 UI 数、隐藏 UI 数、layer root 数、binding 数的汇总行。
- [x] Dump 增加对象 handle 数和 child handle 缓存数。
- [x] 增加 `HELLO_FGUI_LIFECYCLE_SELF_TEST=1` 自测，覆盖关闭后残留 binding/timer/cache/focus 检测。
- [~] C++ 增加 FairyGUI renderer/material/texture 状态 Dump 入口；当前已有基础计数，仍缺更细的 renderer/material/texture 明细。
- [ ] Tracy 增加 UI Open/Close/Event/Render zones 和计数器。
- [x] 增加 `HELLO_FGUI_SELF_TEST_ALL=1` 一键 FGUI 自测入口，集中跑 Act37、Act38、Layer、Mask、Input、Lifecycle、Cleanup。

## Phase 1: 生命周期标准化

- [x] 定义并接入 `OnCreate`，只做一次性控件绑定和初始化。
- [x] 定义并接入 `OnReopen`，已存在实例重复打开时刷新参数和置顶。
- [x] 定义并接入 `OnShow/OnHide`，可见性变化时回调。
- [x] 定义并接入 `OnClose/OnRemove/OnDestroy`，关闭、移除、销毁分层处理。
- [x] 增加 `OnOpen`，作为每次打开都会执行的统一入口，避免业务误用 `OnShow`。
- [x] 明确 Ctrl/View/Model 生命周期职责，并在基类里加默认调用顺序说明和断言。
- [ ] 对 reopen/cache/destroy 三种路径补完整样例和自测。
- [x] UI 关闭后检测残留 binding、child/list cache、focus、stack/layer/map，并输出警告。
- [x] UI 关闭后检测残留 timer/delay 回调，避免异步回调命中已销毁 Ctrl/View。
- [ ] 补缓存隐藏路径的 timer 策略：隐藏时继续运行、暂停或由业务 OnHide 主动清理。
- [ ] 支持界面级 `Close(reason)`，让业务知道关闭原因。

## Phase 2: UI 层级与栈系统

- [x] layer root 已具备：`Background/Normal/Popup/Guide/Top/Toast`。
- [x] UI stack、Popup stack、`GetTopUI/GetTopPopup/CloseTop/CloseTopPopup` 已具备。
- [x] `popupMode = stack/single/replace` 和 `popupGroup` 已具备第一版。
- [x] `CloseLayer/CloseGroup/CloseScene/ChangeScene` 已具备第一版。
- [ ] 明确每个 layer 的默认 sorting order、输入穿透规则、是否参与返回键关闭。
- [ ] 支持 modal 点击空白关闭、modal 点击穿透禁止、modal 与 popup 绑定销毁。
- [ ] 支持同 layer 内按打开时间、priority、手动 `BringToFront` 的稳定排序。
- [ ] 支持 UI 打开时指定 parent/root，承接 MiniUIManager 的 `rootNode` 思路。
- [ ] 支持引导层高亮区域和可点击穿透区域。
- [ ] 增加真实业务 layer 场景样例，覆盖页面、弹窗、引导、toast 同屏。

## Phase 3: 资源策略

- [x] package 加载缓存和 refCount 已具备。
- [x] `cache`、`unloadPackageOnClose`、`clearPackage` 已具备。
- [x] `closeOnSceneChange`、`destroyOnSceneChange` 已具备 registry 声明。
- [ ] 增加场景级 preload 配置和统一预加载入口。
- [ ] 增加 package 按 group/tag 批量卸载。
- [~] 增加资源泄漏 Dump，输出 package refCount、UI 引用、打开栈引用；当前已有第一版引用关系输出和资源告警规则，后续继续补长循环后的泄漏断言。
- [ ] 增加纹理尺寸、材质数量、render command 数的统计入口。
- [ ] 明确缓存 UI 的资源保留策略，避免隐藏 UI 长期占用大贴图。
- [ ] 增加资源缺失时的 fallback 日志，记录 package/object/childPath。

## Phase 4: 事件桥接与输入补齐

- [x] Click、Changed、ClickItem、RightClick、TouchBegin/End 已具备。
- [x] DragStart、DragMove、DragEnd 已具备。
- [x] UI 命中后阻断相机鼠标输入已具备。
- [x] ESC 关闭顶层弹窗已具备。
- [ ] 事件 payload 补齐：item handle、item index、坐标、button、touch id、drag delta。
- [x] 鼠标滚轮桥接到 FairyGUI ScrollPane/List，正负 delta 已通过 `event.wheelDelta` 传到 Lua。
- [x] 键盘焦点最小桥接：点击或 Lua `Focus` 到 `GTextInput` 后，键盘输入优先进入 FairyGUI。
- [x] `GTextInput` 最小文本输入桥接：普通字符、Backspace、Enter/Submit、Changed 事件。
- [ ] Tab 焦点切换规则。
- [ ] Windows IME 输入链路评估和最小可用实现。
- [x] UI 关闭后事件解绑自测，确保 root/child handle 绑定不会回调已销毁 Ctrl/View。
- [ ] 事件统计 Dump，输出每类事件分发次数和最近一次命中对象。

## Phase 5: 通用 UI 服务

- [ ] `Toast` 服务：短提示、排队、去重、自动关闭。
- [ ] `Loading` 服务：引用计数、超时、场景切换清理。
- [ ] `MessageBox/Dialog` 服务：确认、取消、参数化文案、按钮回调。
- [ ] `PopupMenu` 服务：列表项、点击外部关闭、跟随鼠标或控件定位。
- [ ] `Tip` 服务：控件 hover 提示、延迟显示、离开隐藏。
- [ ] `GuideMask` 服务：遮罩、高亮区域、引导点击穿透。
- [ ] 通用服务全部接入 layer、modal、资源、生命周期和 Dump。

## Phase 6: 控件能力与复杂 UI

- [x] 基础 child handle 缓存、`SetText/SetVisible/SetPosition/SetSize` 已具备。
- [x] `SetIcon/SetLoaderUrl/SetControllerIndex` 已具备。
- [x] List item count、selected index、scroll to view 已具备第一版。
- [ ] ProgressBar 封装。
- [ ] Slider 封装。
- [ ] ComboBox 封装。
- [ ] Controller 变化监听和名字/id 双模式访问。
- [ ] Transition 播放、停止、完成回调。
- [ ] List 虚拟列表、item renderer、item click payload 完整化。
- [ ] Tree/TreeNode 最小可用封装。
- [ ] 子页面/子组件挂载能力，支持一个大 UI 内拆多个业务子模块。

## Phase 7: 屏幕适配

- [x] `fullScreen/adaptScreen/margin` 已具备第一版。
- [x] resize 后统一回调 `OnResize` 已具备。
- [ ] 对齐 MiniUIManager 的 fullscreen/adapt 规则，明确设计分辨率、缩放模式和边缘策略。
- [ ] 增加 safe area 或异形屏预留字段，先在配置层占位。
- [ ] 增加 layer root resize 后的统一布局重算。
- [ ] 增加不同窗口尺寸下的可视化样例和日志自测。
- [ ] 对弹窗居中、toast 区域、guide mask 区域补统一适配规则。

## Phase 8: 渲染裁剪与遮罩

- [x] ScrollPane/List 的基础矩形裁剪已具备。
- [x] stencil 状态已桥接到 cocos-lite renderer，并能传到 Ogre 绘制链路。
- [x] MaskProbe 可视化样例已具备。
- [ ] GPU stencil 像素级遮罩。
- [ ] `GGraph/DrawNode` 最小绘制能力，支持图形 mask。
- [ ] alpha mask 或复杂图片 mask 的兼容策略。
- [ ] 裁剪、mask、batch 切换的性能统计。
- [ ] 多 mask 嵌套自测样例。

## Phase 9: AutoGen 与工具链

- [x] `fairygui_asset_manifest.py` 能整理 `.fui` 导出资源清单。
- [x] `fairygui_autogen.py`/`fairygui_generate_ui.py` 已能生成 Act38 MVC 示例。
- [x] `GeneratedUIRegistry.lua` 已作为生成 registry 示例。
- [ ] 批量扫描 `bin/res/fuires` 和 `bin/res/assets`。
- [ ] 根据 xml 自动识别控件类型、列表、controller、transition。
- [ ] AutoGen 生成 child path 常量和类型化访问函数。
- [ ] 生成 View/Ctrl/Model 模板时保留用户代码区，避免覆盖业务逻辑。
- [ ] `--check` 模式接入本地验证，检测生成文件是否过期。
- [ ] 文档补新增 UI 的完整流程。

## Phase 10: 性能与稳定性

- [ ] FGUI 打开/关闭耗时统计。
- [ ] package 加载耗时统计。
- [ ] 事件分发耗时统计。
- [ ] render command、triangle、texture/material 切换统计。
- [ ] UI 数量压力样例，覆盖 1/5/20/50 个弹窗或列表项规模。
- [ ] Tracy zones 和 frame counters 接入 UI 子系统。
- [ ] 长时间打开关闭循环自测，检测 handle、binding、package refCount 是否回零。
- [ ] Release|x64、Debug|x64、Win32 的最小构建验证清单。

## 合主干前风险收敛

- [x] MouseWheel 保留正负 delta，并把 `wheelDelta` 传给 Lua 事件。
- [x] MaskProbe 从 runtime Act38 资源硬编码迁出，改为 sample 参数传入资源。
- [x] VS2017 Release|x64 构建通过。
- [x] FairyGUI 样例自测通过：Act37、Act38、MaskProbe、MouseWheel。
- [ ] 提交前筛掉未跟踪的本地配置、大包、exe/zip、工具缓存。

## 迭代顺序

### Iteration 1: 观测与 MaskProbe 清理闭环

- [x] MaskProbe 打开后自动关闭，并 dump UI stack/package/render stats。
- [x] Lua 增加 `GetRenderStats/DumpRenderStats`。
- [x] C++ 将 `FairyGuiSystem::GetLastRenderCommandCount/GetLastTriangleCount` 和基础 material/texture/handle/binding 计数暴露给 Lua。
- [x] MaskProbe 样例资源从 runtime 硬编码迁出，改由 Lua sample 明确传入。
- [x] VS2017 Release|x64 编译通过。
- [x] 运行 `HELLO_FGUI_MASK_SELF_TEST=1`，确认打开和关闭日志正常。

### Iteration 2: 输入补齐

- [x] 鼠标滚轮桥接，负向/正向 delta 可到达 Lua `event.wheelDelta`。
- [x] 键盘按键桥接到 FairyGUI 焦点对象。
- [x] `GTextInput` 最小输入闭环。
- [x] 输入自测覆盖按钮、列表、滚轮、ESC、文本输入。
- [x] 生命周期关闭残留检测和自测，覆盖直接子控件事件绑定、timer、焦点清理和 Manager map 清理。
- [ ] Tab 焦点顺序和 IME 输入链路。

### Iteration 3: 通用 UI 服务

- [ ] Toast。
- [ ] Loading。
- [ ] Dialog/MessageBox。
- [ ] PopupMenu。
- [ ] GuideMask。

### Iteration 4: AutoGen 工程化

- [ ] 批量生成入口。
- [ ] `--check` 生成一致性检查。
- [ ] 控件类型覆盖扩展。
- [ ] 新增 UI 标准流程文档。

### Iteration 5: 渲染 parity 与性能

- [ ] GPU stencil。
- [ ] GGraph/DrawNode。
- [ ] 多 mask 嵌套样例。
- [ ] Tracy UI 性能计数器。
- [ ] UI 压力样例。

## 每轮迭代后的核查规则

每完成一轮，都按下面顺序核实是否更接近成熟业务框架：

1. 更新本 TODO，把完成项勾选，把新增缺口补入对应 Phase。
2. 跑最小相关构建或自测。
3. 检查 UI 打开栈、package refCount、binding count 是否符合预期。
4. 检查是否引入新的业务调用负担，如果新增 UI 还需要临时绕过 Manager，则继续补 Manager 能力。
5. 选择下一轮最高价值任务继续迭代。
