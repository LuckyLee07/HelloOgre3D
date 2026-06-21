# FairyGUI 生产级收口 TODO

## 目标

本清单用于记录当前 FairyGUI 接入从“功能闭环可用”收敛到“生产级入口、边界、验证都稳定”的完成状态。这里的完成口径不是追平 env1 MiniUI 的全部历史能力，而是完成当前项目的生产级入口收敛：业务只面对稳定 Lua UI 框架，C++ runtime 只承担渲染、输入、资源、事件和 handle 桥接。

目标入口：

- Lua 业务只通过 `FairyGuiManager`、Ctrl/View/Model、AutoGen 访问 UI。
- Native 后端只暴露 `FairyGuiRuntime`，不再把 FGUI API 挂在 `GameManager` 的 tolua 导出上。
- 新增 FGUI Lua API 必须进入 `runtime/ui/fairygui/lua_bridge` 和 `runtime/RuntimeToLua.pkg`。
- 每轮结构变动必须能被静态检查、自测和生产 gate 覆盖。

## 完成状态

### P0：入口收敛与防回流

- [x] 从 `GameManager` tolua 导出中移除 `*FairyGui*` API，并删除 C++ 内部兼容转发；`GameManager` 只负责注册 `FairyGuiRuntime`。
- [x] `FairyGuiNativeApi` 默认只解析 `FairyGuiRuntime`，不再使用 `GameManager` fallback。
- [x] `tools/check_fgui_static.ps1` 增加旧入口护栏，检查：
  - `manager/fairygui` 下 Lua 文件不得出现 `GameManager`。
  - `GameManager.h` 的 `//tolua_begin` 到 `//tolua_end` 区间不得出现 `FairyGui`。
  - `game/GameToLua.cpp` 不得注册 `*FairyGui*` 方法。
- [x] 重新生成 `GameToLua.cpp`，旧 Lua binding 已消失。
- [x] 更新路线文档，明确 `GameManager` 不再保留 FGUI API 转发，不再作为 Lua FGUI 后端。
- [x] `tools/run_fgui_selftest.ps1 -Mode All` 已修复空跑问题，必须看到 `self test suite end: N / N` 且全通过才算成功。

### P0：验证门禁稳定

- [x] 静态检查覆盖 Lua 语法、AutoGen `--check --strict`、Python 编译和 FGUI 入口护栏。
- [x] VS2017 Debug x64 / Release x64 构建纳入生产 gate。
- [x] `run_fgui_selftest.ps1 -Mode All` 纳入生产 gate。
- [x] `run_fgui_selftest.ps1 -Mode LongLoop` 纳入生产 gate。
- [x] `run_fgui_selftest.ps1 -Mode Pressure` 纳入生产 gate。
- [x] `run_fgui_production_gate.ps1 -Mode Full` 作为生产级收口门禁保持可用。

### P1：C++ runtime 内部拆分

- [x] `FairyGuiLuaApi` 按 package/create、object/property、controller/list、input、event、diagnostics 拆分到 `runtime/ui/fairygui/lua_bridge`。
- [x] `FairyGuiLuaApi.h` 作为唯一 public bridge header，领域 cpp 不反向 include `GameManager`。
- [x] `tools/check_fgui_static.ps1` 增加 runtime ownership guard，阻止 `runtime/ui/fairygui` 反向依赖 `GameManager` / `ClientManager`。
- [x] `FairyGuiSystem` public header 已收缩为 runtime facade，业务层不再看到 FairyGUI、cocos 或 Ogre 细节类型。
- [x] `FairyGuiSystem` 内部按源码边界拆分：
  - `FairyGuiSystemObjects.cpp`：package、object handle、控件属性、controller、transition、list、对象 alias。
  - `FairyGuiSystemEvents.cpp`：事件监听注册、解绑、事件目标查找、Lua event dispatch。
  - `FairyGuiSystemInput.cpp`：鼠标、键盘、TextInput caret、IME hook、候选窗定位、输入坐标转换。
  - `FairyGuiSystemRender.cpp`：Ogre manual object 渲染、scissor/stencil、CPU clip、frame render stats、material/texture 管理。
  - `FairyGuiSystemInternal.h`：集中内部 FairyGUI/cocos/Ogre include 与 helper，避免 public facade 膨胀。
- [x] 静态检查增加 public header facade guard，防止 `FairyGuiSystem.h` 重新暴露 FairyGUI/cocos/Ogre/RenderCommandSink/GObject/GRoot。

### P1：Lua UI 框架清晰化

- [x] `bin/res/scripts/manager/FairyGuiManager.lua` 收敛为兼容 facade，真实实现位于 `manager/fairygui/FairyGuiManager.lua`。
- [x] `manager/fairygui` 子模块职责已拆分为 profiler、lifecycle、package、services、layers、events、lists、controls、probes、native facade。
- [x] layer、resource、event、service、debug、profiler 的公共依赖统一由 `FairyGuiManager` facade 聚合。
- [x] 新 UI 默认走 Ctrl/View/Model/AutoGen；静态检查禁止 `bin/res/scripts/ui` 直接依赖 `FairyGuiNativeApi`、`NativeApi`、`FairyGuiRuntime`、`GameManager`。
- [x] 静态检查禁止业务 views 直接散点调用 `FairyGuiManager:CreateObject` / `LoadPackage`。
- [x] DebugPanel 输出 native backend、open UI、binding、package、render、resource、event、service、IME、stencil 的生产诊断摘要。
- [x] FGUI DebugPanel 与 AI DebugPanel 共用 profiler/debug panel 风格，已提供 `BuildAiDebugPanelLines` / `ShowAiDebugPanel`；AI 面板支持 `focusAgentId` 与 `filterText` 参数化筛选。

### P1：生产体验补齐

- [x] 复杂列表、虚拟列表、Tree、ComboBox、Slider、Progress、Transition、嵌套弹窗已有 Lua facade API、自测入口或生产覆盖。
- [x] 输入链路覆盖 Windows IME 组合态统计、候选窗口定位、TextInput policy、Tab 焦点、快捷键和 ESC 规则的边界样例。
- [x] 资源链路覆盖 scene preload、package group/tag 卸载、缺失资源 fallback、泄漏告警和 clean-state 回零。
- [x] 渲染链路明确当前生产 backend 为 `shapeCpu`，GPU stencil 作为后续可选升级；当前 gate 覆盖 CPU shape clip、mask/stencil 统计和 DebugPanel 可观测。
- [x] `tools/check_fgui_static.ps1` 增加 production feature coverage guard，防止上述生产样例、自测和诊断入口退化。

### P2：对标 env1 MiniUI 的长期能力

- [x] UI 打开/关闭策略已有产品级规范入口：页面、弹窗、全屏、toast、guide、loading、debug 的默认层级和互斥规则由 registry、layer policy、stack/popup policy 和服务层承接。
- [x] UI 配置表/声明式 registry 支持 package、object、layer、cache、scene、group、priority、resource policy、requires。
- [x] 常态化 UI 压测与报告归档入口已建立：`Pressure` selftest、Render/Perf/Resource/Health dump、production gate 和 DebugPanel snapshot。
- [x] FGUI DebugPanel 与未来 AI DebugPanel 共用观测风格，形成项目内统一 runtime debug view。

## 当前验收口径

FGUI 生产级入口收敛以 `docs/fgui/fairygui-production-gate.md` 为准：

```powershell
powershell -ExecutionPolicy Bypass -File tools\run_fgui_production_gate.ps1 -Mode Full -StopExisting
```

Full gate 必须覆盖：

- VS2017 Debug|x64 / Release|x64 构建。
- `tools/check_fgui_static.ps1` 全量静态检查。
- `tools/run_fgui_selftest.ps1 -Mode All`。
- `tools/run_fgui_selftest.ps1 -Mode LongLoop`。
- `tools/run_fgui_selftest.ps1 -Mode Pressure`。
- `git diff --check`。

env1 MiniUI 的全部长期能力仍以 `docs/fgui/fairygui-env1-production-target.md` 作为能力地图继续追踪；本清单只记录当前项目这轮生产级入口收敛的闭环。
