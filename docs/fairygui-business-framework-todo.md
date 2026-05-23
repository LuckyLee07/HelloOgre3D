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
- [~] Package、纹理、材质、绑定数、打开 UI 数、渲染命令数、三角形数有统一 Dump；基础 `DumpHealth`、package/UI 引用 Dump、资源告警、source/alias 计数、同路径动态 loader 复用、texture 尺寸、draw/batch/clip/stencil/switch 统计、服务层统计和长循环/压力回零自测已具备，Tracy viewer 实测待补。
- [x] AutoGen 能覆盖常用控件、列表 item、controller、transition，并能在 CI 或本地命令里检查生成文件是否过期。
- [x] 至少有一组真实业务样例覆盖页面、弹窗、列表、遮罩、拖拽、文本输入、场景清理和性能观测；当前由 `HELLO_FGUI_BUSINESS_FLOW_SELF_TEST=1` 串联验证。

## 当前完成快照

- [x] 官方 FairyGUI-cocos2dx/libfairygui 已接入，并通过 `cocos-lite` 适配到 Ogre。
- [x] `FairyGuiSystem` 已接入 `ClientManager` 生命周期，支持 package 加载、对象 handle、事件桥接、输入转发和 Ogre 绘制。
- [x] Lua 侧已有 `FairyGuiManager.lua`，支持 `OpenUI/CloseUI/CloseAll`、registry、View、MVC、Ctrl 业务入口。
- [x] 基础生命周期已具备：`OnCreate/OnReopen/OnShow/OnHide/OnClose/OnRemove/OnDestroy/OnResize`。
- [x] 基础层级已具备：`Background/Normal/Popup/Guide/Top/Toast`、layer root、sorting order、modal mask、UI stack、popup stack。
- [x] 基础资源策略已具备：package 缓存、refCount、`cache` 隐藏复用、关闭时可卸载 package。
- [x] 基础事件已具备：Click、Changed、ClickItem、RightClick、TouchBegin/End、DragStart/Move/End。
- [x] Act37 MVC、Act38 列表、LayerProbe、MaskProbe 已作为样例或自测入口。
- [x] 通用 UI 服务已有第一版动态实现：Toast、Tip、Loading、GuideMask、MessageBox/Dialog、PopupMenu。
- [~] 渲染裁剪与遮罩已有矩形 scissor/stencil 状态桥接和 MaskProbe 样例，但还不是完整 GPU 像素级 stencil。
- [x] AutoGen 已能生成 Act37/Act38 MVC 基础代码、manifest、独立 registry 和聚合 registry，并已具备批量扫描、本地 `--check --strict` 与常用控件元数据覆盖。
- [~] 输入已覆盖鼠标、滚轮、ESC 关闭、键盘焦点、Tab 焦点切换和 `GTextInput` 编辑第一版；IME commit 最小链路已验证，Windows 原生组合/候选窗仍待补。
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
- [x] Dump 增加事件分发总数、分类型计数和最近一次事件命中信息。
- [x] 增加 `HELLO_FGUI_LIFECYCLE_SELF_TEST=1` 自测，覆盖关闭后残留 binding/timer/cache/focus 检测。
- [x] 增加 `HELLO_FGUI_CACHE_SELF_TEST=1` 自测，覆盖 cache hide、timer 暂停、reopen 复用和强制 destroy 回零。
- [x] 增加 `HELLO_FGUI_COMMON_SERVICE_SELF_TEST=1` 自测，覆盖 Toast、Tip、Loading、GuideMask、MessageBox、PopupMenu 创建和清理。
- [x] 增加 `HELLO_FGUI_POPUP_RULE_SELF_TEST=1` 自测，覆盖 popup priority、modal 空白关闭和低优先级弹窗保留。
- [x] 增加 `HELLO_FGUI_GUIDE_MASK_SELF_TEST=1` 自测，覆盖 GuideMask 矩形高亮区域和遮罩外点击关闭。
- [x] 增加 `HELLO_FGUI_EVENT_PAYLOAD_SELF_TEST=1` 自测，覆盖事件坐标、button、touch id 和 drag delta payload。
- [x] 增加 `HELLO_FGUI_SCREEN_ADAPT_SELF_TEST=1` 自测，覆盖居中弹窗、贴边 popup、设计坐标 GuideMask 和 Toast 区域布局。
- [x] 增加 `HELLO_FGUI_SCREEN_ADAPT_DEMO=1` 可视化演示，方便肉眼检查屏幕适配规则。
- [x] 增加 `HELLO_FGUI_BUSINESS_FLOW_SELF_TEST=1` 组合业务流自测，串联页面、弹窗、列表、遮罩、拖拽、文本输入、场景清理、DebugPanel 和性能观测。
- [x] 增加 `HELLO_FGUI_RESOURCE_POLICY_SELF_TEST=1` 资源策略自测，覆盖 scene preload、package group/tag 卸载、使用中 package 跳过和最终回零。
- [x] 增加 `HELLO_FGUI_PRESSURE_SELF_TEST=1` 压力自测，覆盖 1/5/20/50 弹窗规模、Act38 列表数据扩容、Health/Render/Perf/Resource 采样和关闭回零。
- [x] 增加 `HELLO_FGUI_LAYER_BOUNDARY_SELF_TEST=1` 层级边界自测，覆盖 safe area、layer policy、ESC 关闭、指定 parent 和 Tab 焦点切换。
- [x] 增加 `HELLO_FGUI_COMMON_SERVICE_DEMO=1` 可视化演示，按节奏展示 Toast 队列、Loading 引用计数和通用服务关闭。
- [x] FGUI 样例、自测和 demo 入口已迁到 `bin/res/scripts/samples/fgui_init.lua`，`game_init.lua` 只保留主启动壳。
- [x] C++ 增加 FairyGUI renderer/material/texture 状态 Dump 入口；当前已有基础计数、source/alias 区分、动态 loader 资源复用、texture 尺寸和 frame render 明细。
- [~] Tracy 增加 UI Open/Close/Event/Render zones 和计数器；当前 C++ 已覆盖 Update、Render、LoadPackage、CreateObject、DispatchEvent、服务层 counters、frame counters、draw/clip/stencil/switch/max batch counters，Lua 侧已补 Open/Close/Event/LoadPackage/CreateObject/Service 轻量耗时 Dump，Tracy viewer 实测仍待补。
- [x] 增加 `HELLO_FGUI_DEBUG_PANEL_DEMO=1` 可视化调试面板，显示 Health、Render、Perf 和服务层概要。
- [x] 增加 `HELLO_FGUI_SELF_TEST_ALL=1` 一键 FGUI 自测入口，集中跑 Act37、Act38、Layer、Mask、Input、Lifecycle、BusinessFlow、Cleanup。
- [x] 增加 `HELLO_FGUI_LONG_LOOP_SELF_TEST=1` 长循环自测入口，循环打开关闭 Act37、Act38、Layer、Mask、TextInput 并检查资源/生命周期回零。
- [x] 增加 `tools/check_fgui_static.ps1` 静态检查入口，覆盖 Lua 语法、FGUI AutoGen Python 编译和 Act37/Act38 生成链 `--check --strict`。
- [x] `tools/run_fgui_selftest.ps1` 默认检查运行日志中的 `OGRE EXCEPTION`、`call_func error` 和 `self test result: false`，并在 FGUI 自动化模式下跳过章节 Sandbox 场景初始化，避免运行时自测假绿。

## Phase 1: 生命周期标准化

- [x] 定义并接入 `OnCreate`，只做一次性控件绑定和初始化。
- [x] 定义并接入 `OnReopen`，已存在实例重复打开时刷新参数和置顶。
- [x] 定义并接入 `OnShow/OnHide`，可见性变化时回调。
- [x] 定义并接入 `OnClose/OnRemove/OnDestroy`，关闭、移除、销毁分层处理。
- [x] 增加 `OnOpen`，作为每次打开都会执行的统一入口，避免业务误用 `OnShow`。
- [x] 明确 Ctrl/View/Model 生命周期职责，并在基类里加默认调用顺序说明和断言。
- [x] 对 reopen/cache/destroy 三种路径补样例和自测。
- [x] UI 关闭后检测残留 binding、child/list cache、focus、stack/layer/map，并输出警告。
- [x] UI 关闭后检测残留 timer/delay 回调，避免异步回调命中已销毁 Ctrl/View。
- [x] 补缓存隐藏路径的 timer 策略：默认隐藏时清理 UI timer，可通过 `pauseTimersOnHide = false` 保留。
- [x] 支持界面级 `Close(reason)`，让业务知道关闭原因。

## Phase 2: UI 层级与栈系统

- [x] layer root 已具备：`Background/Normal/Popup/Guide/Top/Toast`。
- [x] UI stack、Popup stack、`GetTopUI/GetTopPopup/CloseTop/CloseTopPopup` 已具备。
- [x] `popupMode = stack/single/replace` 和 `popupGroup` 已具备第一版。
- [x] `CloseLayer/CloseGroup/CloseScene/ChangeScene` 已具备第一版。
- [x] 明确每个 layer 的默认 sorting order、输入穿透规则、是否参与返回键关闭；当前由 `FairyGuiLayers` layer policy 统一记录 `baseOrder/popup/closeOnEscape/inputMode`。
- [x] 支持 modal 点击空白关闭、modal 点击穿透禁止、modal 与 popup 绑定销毁。
- [x] 支持同 layer 内按打开时间、priority、手动 `BringToFront` 的稳定排序。
- [x] 支持 UI 打开时指定 parent/root，承接 MiniUIManager 的 `rootNode` 思路；当前支持 `parentHandle/rootHandle/parentKey/rootLayer`。
- [x] 支持引导层高亮区域和可点击穿透区域。
- [x] 增加真实业务 layer 场景样例，覆盖页面、弹窗、引导、toast 同屏；当前由 `BusinessFlow` 组合自测覆盖。

## Phase 3: 资源策略

- [x] package 加载缓存和 refCount 已具备。
- [x] `cache`、`unloadPackageOnClose`、`clearPackage` 已具备。
- [x] `closeOnSceneChange`、`destroyOnSceneChange` 已具备 registry 声明。
- [x] 增加场景级 preload 配置和统一预加载入口；`FairyGuiManager:PreloadScene(sceneName, options)` 会扫描 registry 并记录 package scene/group/tag 元数据。
- [x] 增加 package 按 group/tag 批量卸载；`UnloadPackageGroup/UnloadPackageTag/UnloadPackagesByFilter` 会跳过 ref/open/hidden/stack 仍在使用的 package。
- [x] 增加资源泄漏 Dump，输出 package refCount、UI 引用、打开栈引用；当前已有第一版引用关系输出、资源告警规则和长循环回零断言。
- [x] 增加纹理尺寸、材质数量、render command 数的统计入口；当前已有 texture/material source/alias、render command、triangle、draw command、draw triangle、material/texture switch、clip/stencil/custom command 和 max batch 计数，同路径动态 loader 资源复用已验证。
- [ ] 明确缓存 UI 的资源保留策略，避免隐藏 UI 长期占用大贴图。
- [ ] 增加资源缺失时的 fallback 日志，记录 package/object/childPath。

## Phase 4: 事件桥接与输入补齐

- [x] Click、Changed、ClickItem、RightClick、TouchBegin/End 已具备。
- [x] DragStart、DragMove、DragEnd 已具备。
- [x] UI 命中后阻断相机鼠标输入已具备。
- [x] ESC 关闭顶层弹窗已具备。
- [x] 事件 payload 补齐：item handle、item index、坐标、button、touch id、drag delta。
- [x] 鼠标滚轮桥接到 FairyGUI ScrollPane/List，正负 delta 已通过 `event.wheelDelta` 传到 Lua。
- [x] 键盘焦点最小桥接：点击或 Lua `Focus` 到 `GTextInput` 后，键盘输入优先进入 FairyGUI。
- [x] `GTextInput` 最小文本输入桥接：普通字符、Backspace、Enter/Submit、Changed 事件。
- [x] `GTextInput` 编辑第一版：按 UTF-8 codepoint 维护 caret，支持 Left/Right/Home/End、Backspace、Delete 和中间插入。
- [x] Tab 焦点切换规则；当前按 UI stack sorting order 和每个 UI 的 `focusOrder` 收集可聚焦 handle，并由 `HELLO_FGUI_LAYER_BOUNDARY_SELF_TEST=1` 覆盖。
- [~] Windows IME 输入链路评估和最小可用实现；当前已支持 Unicode commit 进入 `GTextInput` 并由 `HELLO_FGUI_IME_SELF_TEST=1` 覆盖，原生组合态/候选窗待补。
- [x] UI 关闭后事件解绑自测，确保 root/child handle 绑定不会回调已销毁 Ctrl/View。
- [x] 事件统计 Dump，输出每类事件分发次数和最近一次命中对象。

## Phase 5: 通用 UI 服务

- [~] `Toast` 服务：已有短提示、自动关闭、排队和去重，样式资源化待补。
- [~] `Loading` 服务：已有 modal loading、引用计数和超时清理，样式资源化待补。
- [~] `MessageBox/Dialog` 服务：已有第一版确认/取消按钮和回调，视觉样式待资源化。
- [~] `PopupMenu` 服务：已有第一版列表项和外部关闭，控件跟随定位待补。
- [~] `Tip` 服务：已有第一版指定坐标提示，hover 延迟/离开隐藏待补。
- [~] `GuideMask` 服务：已有第一版遮罩、文案、矩形高亮和点击穿透，资源化样式与多形状高亮待补。
- [x] 通用服务全部接入 layer、modal、生命周期和 Dump。

## Phase 6: 控件能力与复杂 UI

- [x] 基础 child handle 缓存、`SetText/SetVisible/SetPosition/SetSize` 已具备。
- [x] `SetIcon/SetLoaderUrl/SetControllerIndex` 已具备。
- [x] List item count、selected index、scroll to view 已具备第一版。
- [x] ProgressBar 封装：C++ handle 支持 `min/max/value`，Lua `Manager/View/Ctrl/ListItem` 提供 `SetProgress/SetValue/GetValue`。
- [x] Slider 封装：复用 `min/max/value` handle API，Lua 提供 `SetSliderValue/GetSliderValue` 别名。
- [x] ComboBox 封装：支持 `selectedIndex/value` 读写，Lua `Manager/View/Ctrl/ListItem` 已封装。
- [~] Controller 变化监听和名字/id 双模式访问：`SetControllerIndex` 已支持空名字访问第一个 controller，专用变化监听和按 id/page name 访问待补。
- [x] Transition 播放、停止、完成回调：支持按名字或默认第一个 transition 播放/停止，完成回调派回 Lua，并纳入关闭残留检查。
- [ ] List 虚拟列表、item renderer、item click payload 完整化。
- [ ] Tree/TreeNode 最小可用封装。
- [ ] 子页面/子组件挂载能力，支持一个大 UI 内拆多个业务子模块。

## Phase 7: 屏幕适配

- [x] `fullScreen/adaptScreen/margin` 已具备第一版。
- [x] resize 后统一回调 `OnResize` 已具备。
- [x] Windows 高 DPI 下 FairyGUI 初始化、渲染、输入和 Lua 适配统一使用 logical screen size。
- [~] 对齐 MiniUIManager 的 fullscreen/adapt 规则，已有设计分辨率、缩放模式和边缘策略第一版。
- [x] 增加 safe area 或异形屏预留字段；当前 `SetSafeArea/GetSafeArea` 与 `useSafeArea/safeArea` 布局参数已具备第一版。
- [x] 增加 layer root resize 后的统一布局重算。
- [x] 增加不同窗口尺寸下的可视化样例和日志自测。
- [x] 对弹窗居中、toast 区域、guide mask 区域补统一适配规则。

## Phase 8: 渲染裁剪与遮罩

- [x] ScrollPane/List 的基础矩形裁剪已具备。
- [x] stencil 状态已桥接到 cocos-lite renderer，并能传到 Ogre 绘制链路。
- [x] MaskProbe 可视化样例已具备。
- [ ] GPU stencil 像素级遮罩。
- [ ] `GGraph/DrawNode` 最小绘制能力，支持图形 mask。
- [ ] alpha mask 或复杂图片 mask 的兼容策略。
- [x] 裁剪、mask、batch 切换的性能统计；当前 frame render snapshot 已记录 clip/stencil/custom command、材质/纹理切换和 max batch。
- [ ] 多 mask 嵌套自测样例。

## Phase 9: AutoGen 与工具链

- [x] `fairygui_asset_manifest.py` 能整理 `.fui` 导出资源清单。
- [x] `fairygui_autogen.py`/`fairygui_generate_ui.py` 已能生成 Act38 MVC 示例。
- [x] `GeneratedUIRegistry.lua` 已作为生成 registry 示例。
- [x] 批量扫描 `bin/res/fuires` 和 `bin/res/assets`；`tools/fgui_autogen/fairygui_batch_generate.py` 默认扫描 `bin/res/assets/*/package.xml`，复用现有 manifest/registry 策略并批量调用生成或 `--check`。
- [x] 根据 xml 自动识别控件类型、列表、controller、transition；manifest 记录主组件和子组件的 `controls/controllers/transitions`，List 记录 defaultItem、item component、selectionController。
- [x] AutoGen 生成 child path 常量和类型化访问函数；`*AutoGen.lua` 输出 `ControlPath/ControlType/ControllerPath/TransitionName/ListItem` 以及查询方法。
- [x] 生成 View/Ctrl/Model 模板时保留用户代码区，避免覆盖业务逻辑；`--force` 刷新 scaffold 时会合并 `FairyGUIUserCode` 标记区。
- [x] `--check` 模式接入本地验证，检测生成文件是否过期；当前覆盖 manifest、`*AutoGen.lua`、独立 registry 和 `GeneratedUIRegistry.lua`，手写 View/Model/Ctrl 只提示 note。
- [ ] 文档补新增 UI 的完整流程。

## Phase 10: 性能与稳定性

- [x] FGUI 打开/关闭耗时统计，`DumpPerfStats` 已输出 count/success/fail/avg/max/last。
- [x] package 加载耗时统计，`loadPackage` 已纳入 Lua 侧 perf stat。
- [x] 事件分发耗时统计，`DumpEventStats` 已输出 avg/max/last elapsedMs。
- [x] render command、triangle、texture/material 统计；当前已输出 source/alias 计数、texture 尺寸、draw/batch/clip/stencil/switch 明细，并复用同路径动态 loader 资源。
- [x] UI 数量压力样例，覆盖 1/5/20/50 个弹窗和 Act38 列表项规模；可通过 `tools/run_fgui_selftest.ps1 -Mode Pressure` 运行。
- [~] Tracy zones 和 frame counters 接入 UI 子系统；C++ FGUI update/render/load/create/event/service/draw/clip/stencil/switch/max batch counters 已接入，Lua open/close/event/load/create/service 轻量耗时已接入 Dump，Tracy viewer 实测待补。
- [x] 长时间打开关闭循环自测，检测 handle、binding、timer、view/controller、package refCount 和资源告警是否回零。
- [~] Release|x64、Debug|x64、Win32 的最小构建验证清单；当前 VS2017 Debug|x64 和 Release|x64 已复验通过，Win32 待补。

## 合主干前风险收敛

- [x] MouseWheel 保留正负 delta，并把 `wheelDelta` 传给 Lua 事件。
- [x] MaskProbe 从 runtime Act38 资源硬编码迁出，改为 sample 参数传入资源。
- [x] VS2017 Release|x64 构建通过。
- [x] FairyGUI 样例自测通过：Act37、Act38、MaskProbe、MouseWheel、LayerBoundary。
- [x] 提交前筛掉未跟踪的本地配置、大包、exe/zip、工具缓存；`.gitignore` 已收口 `.claude/`、`.vscode/`、`vs2017-profile.bat`、Python `__pycache__` 和 Tracy viewer 下载产物。

## 迭代顺序

### 当前下一轮建议

FGUI 的 AutoGen、真实业务 UI、资源策略、DebugPanel 和自测闭环已经具备基础，后续短期优先按下面顺序推进：

1. Tracy 实测收尾：frame render 明细和 counters 已补，下一步用 Tracy viewer 做一次可视化采样记录。
2. 输入与层级边界：Tab 焦点顺序、layer 输入穿透 / ESC / sorting order 规则、指定 parent/root 打开 UI、safe area、`GTextInput` caret 编辑和 IME commit 第一版已完成；下一步补 Windows 原生组合/候选窗和更细输入命中可视化。
3. 真实业务 Benchmark 与 AI 调试承接：用 DebugPanel snapshot 承接 AI 调试面板第一版，同时补一个更接近真实业务规模的页面组合做长期回归基准。

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
- [~] Tab 焦点顺序已具备第一版并接入 LayerBoundary 自测；IME commit 第一版已接入，Windows 原生组合/候选窗待补。

### Iteration 3: 通用 UI 服务

- [x] Toast 第一版。
- [x] Loading 第一版。
- [x] Dialog/MessageBox 第一版。
- [x] PopupMenu 第一版。
- [x] Tip 第一版。
- [x] GuideMask 第一版。

### Iteration 4: AutoGen 工程化

- [x] 批量生成入口。
- [x] `--check` 生成一致性检查。
- [x] 控件类型覆盖扩展。
- [~] 新增 UI 标准流程文档；AutoGen 工作流已补用户代码区和 `--force` 保留规则，后续还需补完整业务 UI 示例。

### Iteration 5: 渲染 parity 与性能

- [ ] GPU stencil。
- [ ] GGraph/DrawNode。
- [ ] 多 mask 嵌套样例。
- [~] Tracy UI 性能计数器；frame render counters 已补，Tracy viewer 实测待补。
- [x] 资源策略观测入口：scene preload、package group/tag 卸载和使用中跳过自测。
- [x] UI 压力样例。
- [x] Frame render 明细：draw command/triangle、material/texture switch、clip/stencil/custom command 和 max batch 已接入 Dump、DebugPanel 和 Pressure 日志。

### Iteration 6: DebugPanel 产品化第一步

- [x] 增加 DebugPanel snapshot API，统一汇总 health、render、perf、event、layer、binding 和 package 状态。
- [x] DebugPanel 面板显示 open/hidden UI、top UI、top popup、focus owner、最近事件、资源告警、layer、binding、package 和 service 概要。
- [x] 增加 `DebugPanelSelfTest`，并纳入 `HELLO_FGUI_SELF_TEST_ALL=1` 套件。
- [~] 交互式筛选、dump 指定 UI/package、关闭指定业务 UI 和 AI 专用视图待补。

## 每轮迭代后的核查规则

每完成一轮，都按下面顺序核实是否更接近成熟业务框架：

1. 更新本 TODO，把完成项勾选，把新增缺口补入对应 Phase。
2. 跑最小相关构建或自测。
3. 检查 UI 打开栈、package refCount、binding count 是否符合预期。
4. 检查是否引入新的业务调用负担，如果新增 UI 还需要临时绕过 Manager，则继续补 Manager 能力。
5. 选择下一轮最高价值任务继续迭代。
