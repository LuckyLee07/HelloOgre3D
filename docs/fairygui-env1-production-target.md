# FairyGUI 对标 env1 生产级 MiniUI 最终目标

## 文档定位

本文档用于记录 HelloOgre3D 的 FairyGUI 接入在长期阶段希望追到的生产级 MiniUI 目标。

注意：

- 本文档定义的是最终目标和能力地图，不改变当前迭代顺序。
- 当前开发仍按 `docs/fairygui-business-framework-todo.md`、`docs/fairygui-final-roadmap.md` 和近期任务安排推进。
- env1 作为能力对标对象，路径为 `F:\env1_trunk`，重点参考其 `MiniUIManager`、`MiniUI` 控件层、AutoGen/MVC 业务用法和输入/IME 链路。

## 对标结论

env1 的 FairyGUI/MiniUI 已经不是 sample 级接入，而是生产项目级 UI 框架：

- 有完整控件层：`GRoot`、`GComponent`、`GList`、`GTree`、`GTextInput`、`GGraph`、`GLoader3D`、`GSlider`、`GComboBox`、`Transition`、`Controller` 等。
- 有完整输入链路：`InputManager::enableIME`、`CCIMEDispatcher`、`UIEditBox`、`GTextInput`，覆盖软键盘、密码、数字、多行、最大长度和输入限制。
- 有真实业务 UI 闭环：大量 `*AutoGen.lua`、`*Ctrl.lua`、`*View.lua`、`*Model.lua`，业务侧已长期使用 `AddPackage/OpenUI/CloseUI/GetUI/InstMVC/UnInstMVC`。
- 有平台适配经验：`AdaptSpecialShapedScreen`、`fullScreen` 参数、root/rootNode 挂载和复杂业务界面适配。
- 有较完整 Lua 绑定：大量控件 API 已暴露给业务 Lua。

HelloOgre3D 当前 FGUI 的优势是架构更清晰、自测更集中、生命周期和资源观测更明确；差距主要在生产控件面、IME、真实业务 UI 规模、渲染 parity 和工具链成熟度。

长期目标不是完整照搬 env1，而是在保持当前项目分层和 handle-based 桥接优势的前提下，补齐 env1 证明过的生产能力。

## 最终使用形态

复杂业务 UI 默认走 MVC：

```lua
local ctrl = FairyGuiManager:Open("ShopMain", {
	roleId = 1001,
	tab = "daily"
})

ctrl:SelectTab("daily")
ctrl:RefreshGoods(goodsList)
```

业务层默认只依赖 Ctrl：

- AutoGen 负责生成控件路径、类型、controller、transition、list item 元数据。
- View 负责封装控件访问和局部 UI 表现。
- Model 负责界面数据缓存和转换。
- Ctrl 负责业务入口、事件注册、请求回调和界面状态流转。
- FairyGuiManager 负责生命周期、层级、资源、输入、事件和调试观测。

简单调试 UI 可以继续使用轻量 View，但真实业务 UI 默认不直接在外部操作底层 FairyGUI 对象。

## 最终验收标准

- [ ] 新增一个业务 UI 时，有固定流程：导出 FairyGUI 资源、生成 AutoGen/View/Model/Ctrl、注册 UI、补业务逻辑、跑本地检查。
- [ ] 业务侧通过 `FairyGuiManager:Open/Get/Close` 或 Ctrl API 完成常规交互，不需要直接依赖底层对象指针。
- [ ] 页面、弹窗、Toast、Loading、MessageBox、PopupMenu、GuideMask、DebugPanel 可以共存，层级和关闭顺序稳定。
- [ ] TextInput 支持中文 IME、普通英文输入、数字输入、密码输入、长度限制、输入限制、focus/killFocus、submit/change 事件。
- [ ] List 支持 itemRenderer、item reuse、virtual list、selectedIndex、scrollToIndex、clickItem payload 和复杂 item AutoGen 绑定。
- [~] Tree/TreeNode、GGraph/DrawNode、ComboBox、Slider、ProgressBar、Transition、Controller 等常用控件具备业务可用 API；Tree 最小封装已具备，复杂 Tree item AutoGen 仍待补。
- [ ] fullscreen、center、fit、margin、safe area、异形屏适配在 resize 后统一重算。
- [~] package、texture、material、render command、triangle、event binding、timer、handle、cache UI 都有可 dump 的观测数据；当前已覆盖 package、texture/material source 与尺寸、render/draw/clip/stencil/switch、event binding、timer、handle、cache UI，引用 UI/package 明细仍可继续细化。
- [ ] 大量 UI 长循环打开关闭后，资源、事件、timer、focus、controller、view/model/ctrl 引用能稳定回零。
- [ ] 有至少一个接近真实业务规模的 benchmark UI，长期作为 FGUI 回归基准。

## P0：业务可用闭环

预计 1-2 周。该阶段目标是让当前 FGUI 从“框架闭环”推进到“真实业务 UI 可以日常使用”。

### 1. Windows IME / GTextInput 对齐

对标 env1 的 `InputManager::enableIME + CCIMEDispatcher + UIEditBox + GTextInput` 链路。

待补能力：

- [ ] 中文 IME 输入、组合字、候选提交。
- [ ] Backspace、Delete、Enter/Submit、方向键和光标位置基础处理。
- [ ] placeholder、password、maxLength、restrict、inputType。
- [ ] focus、killFocus、点击输入框自动聚焦、关闭 UI 后自动失焦。
- [ ] TextInput Changed、Submit、FocusIn、FocusOut 事件。
- [ ] 输入框关闭、场景切换、缓存隐藏后的 IME 状态清理。

验收建议：

- [ ] 增加 `HELLO_FGUI_IME_SELF_TEST=1` 或扩展现有输入自测。
- [ ] 增加一个可视化 TextInput demo，人工验证中文输入。

### 2. AutoGen / MVC 生产化

待补能力：

- [ ] AutoGen user-code preserve 区，避免业务手写代码被生成覆盖。
- [ ] 生成 `widgets` / `rootNode` 类似 env1 的稳定访问结构。
- [ ] Controller、Transition、ListItem 生成更贴近业务调用。
- [ ] 新建 UI 的脚手架命令或固定模板。
- [ ] 生成前后差异检查更清晰，方便接入本地检查或 CI。
- [ ] 补完整“新增一个业务 UI”的工作流文档。

验收建议：

- [ ] 从零新增一个业务 UI，只通过生成链和少量业务代码完成。
- [ ] `--check --strict` 能发现 manifest、AutoGen、registry 过期。

### 3. 真实业务 Benchmark UI

目标不是再做一个小 demo，而是做一个长期回归用的复合业务界面。

建议内容：

- [ ] 主页面：多 Tab / 多状态展示。
- [ ] 列表：普通列表、虚拟列表、复杂 item、选中态、点击 payload。
- [ ] 弹窗：MessageBox、详情弹窗、二级弹窗。
- [ ] 服务：Toast、Loading、PopupMenu、GuideMask。
- [ ] 输入：TextInput、搜索框、数字输入。
- [ ] 调试：DebugPanel 可观察当前 UI、资源和事件状态。
- [ ] 清理：Close、CloseTop、CloseLayer、CloseScene、ChangeScene 都有覆盖。

验收建议：

- [ ] 扩展 `HELLO_FGUI_BUSINESS_FLOW_SELF_TEST=1`，让它成为真实业务基准自测。
- [ ] 可视化 demo 能在人工运行时看到完整 UI 流程。

### 4. FGUI DebugPanel 产品化

待补能力：

- [~] 当前打开 UI 列表、layer、sorting order、popup stack、modal mask；当前已显示 open/hidden UI、layer baseOrder、UI/popup stack，modal mask 明细待补。
- [x] 当前 focus 对象、输入命中对象、鼠标坐标、最近事件。
- [x] package refCount、preload、group/tag、cache UI。
- [x] handle 数量、event binding 数量、timer 数量、child cache 数量。
- [x] render stats、resource stats、service stats。
- [~] 支持刷新、关闭指定 UI、dump 指定 package 或 UI；当前已支持刷新/关闭 DebugPanel 和 snapshot API，交互式 dump 指令待补。

验收建议：

- [x] DebugPanel 能辅助定位层级、输入、资源泄露和事件泄露问题。

## P1：常用控件能力补齐

预计 1-2 周。该阶段主要追 env1 生产 UI 中常用控件能力。

### 1. List / Virtual List

- [x] itemRenderer 标准协议。
- [x] item reuse 生命周期。
- [~] selectedIndex / selectedItem：`selectedIndex` 已具备，`selectedItem` 语义封装待补。
- [~] scrollToIndex / scrollToBottom：`scrollToView` 已具备，`scrollToBottom` 语义封装待补。
- [x] clickItem payload 标准化。
- [ ] 复杂 item 支持 AutoGen 子结构。
- [x] 大列表压力自测：`HELLO_FGUI_VIRTUAL_LIST_SELF_TEST=1` 覆盖 80 条虚拟列表和 item handle 复用统计。

### 2. Controller / Transition

- [ ] controller page id/name 查询。
- [ ] page changed listener。
- [ ] 设置 page 时能按 name 或 index。
- [ ] transition play/stop/replay。
- [ ] transition complete callback。
- [ ] transition 状态查询。

### 3. Tree / TreeNode

- [x] 创建 root/node。
- [x] 添加、删除、清空节点。
- [x] 展开/收起。
- [x] 选中和点击事件。
- [x] 节点数据绑定。
- [ ] Tree item AutoGen 支持。

### 4. DragDrop / ScrollBar / ComboBox

- [ ] Drag start/move/end payload 标准化。
- [ ] Drop target 命中和业务回调。
- [ ] ScrollBar 基础 API。
- [ ] ComboBox item、selectedIndex、change 事件。
- [ ] Slider / ProgressBar 业务 API 完整化。

### 5. 子页面 / 子组件挂载

- [ ] 一个 UI 内可挂载另一个 AutoGen/MVC 子组件。
- [ ] 子组件生命周期跟随父 UI。
- [ ] 子组件事件和 timer 自动清理。
- [ ] 支持指定 parent/rootHandle/rootKey 打开子界面。

## P2：渲染与资源生产能力

预计 1-2 周。该阶段补足 env1 中更接近完整 FairyGUI 后端的能力。

### 1. GPU Stencil / 复杂 Mask

- [ ] 像素级 GPU stencil mask。
- [ ] 普通 mask、倒置 mask、嵌套 mask。
- [ ] 图片 mask / alpha mask 兼容策略。
- [x] mask 状态切换统计。
- [ ] 多 mask 自测样例。

### 2. GGraph / DrawNode

- [ ] 矩形、圆形、线段等基础图形。
- [ ] 图形颜色、透明度、尺寸更新。
- [ ] GGraph 作为 mask 的基础能力。
- [ ] DrawNode 渲染统计。

### 3. 资源 Fallback 与统计

- [ ] 缺 package、缺 component、缺 image、缺 font 有统一 fallback 和日志。
- [~] texture 尺寸、格式、引用 UI、引用 package 可 dump；当前已补尺寸和 source，引用 UI/package 明细待补。
- [x] material 数量和切换统计。
- [x] render command、batch、triangle、clip、mask 切换统计。
- [ ] package preload、group/tag unload、scene preload 策略稳定。

### 4. 压力测试

- [ ] 大量页面打开关闭。
- [ ] 大量列表 item。
- [ ] 大量 mask / transition。
- [ ] 大量输入框和事件绑定。
- [ ] 长循环后资源、事件、timer、focus、handle 回零。

## P3：平台适配与工具链完善

预计 1 周起，视目标平台继续扩展。

### 1. Safe Area / 分辨率适配

对标 env1 的 `AdaptSpecialShapedScreen` 思路。

- [ ] fullScreen / fit / center / margin 策略统一。
- [ ] safe area resize 后统一重算。
- [ ] 弹窗、Toast、PopupMenu、GuideMask 共享适配规则。
- [ ] 异形屏或特殊窗口比例预留策略。

### 2. 资源导出工作流

- [ ] 明确 FairyGUI 编辑器导出目录。
- [ ] 明确 package、图片、字体、manifest、registry 的生成与提交规则。
- [ ] 新增 UI 的操作步骤固定化。
- [ ] 工具输出对错误路径、重复 UI 名、缺失 package 给出清晰提示。

### 3. 本地检查 / CI 化

- [ ] Lua 语法检查。
- [ ] AutoGen Python 编译检查。
- [ ] AutoGen `--check --strict`。
- [ ] FGUI selftest all。
- [ ] FGUI long loop。
- [ ] 资源和生命周期泄露检查。

## 推荐迁移顺序

如果后续按 env1 生产级 MiniUI 继续追平，推荐顺序如下：

1. Windows IME / GTextInput。
2. AutoGen / MVC 生产化。
3. 真实业务 Benchmark UI。
4. DebugPanel 产品化。
5. List / Virtual List。
6. Controller / Transition。
7. Tree / TreeNode。
8. DragDrop / ComboBox / ScrollBar。
9. GPU stencil / complex mask。
10. GGraph / DrawNode。
11. Safe area / 异形屏 / 资源导出工作流。

## 与当前计划的关系

本文档是长期目标，不是立即插队任务。

当前近期开发仍按已有计划执行：

- 短期任务继续参考 `docs/fairygui-business-framework-todo.md`。
- 总体路线继续参考 `docs/fairygui-final-roadmap.md`。
- AutoGen 细节继续参考 `docs/fairygui-autogen-workflow.md`。
- 资源规范继续参考 `docs/fairygui-assets.md`。

当当前计划中的阶段任务完成后，可以回到本文档按 P0-P3 对照检查，决定是否进入下一轮生产级 MiniUI 追平工作。
