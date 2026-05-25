# FairyGUI Production Gate

## 目标

本文档定义 FGUI 当前阶段的“生产级收口”验收口径。后续如果要判断一轮 FGUI 改动是否足够稳定，优先跑 `tools/run_fgui_production_gate.ps1`，不要再靠人工记忆拼命令。

## Gate 分级

### Fast

用于 Lua、文档、工具或小范围 UI 改动后的快速验收。

```powershell
powershell -ExecutionPolicy Bypass -File tools\run_fgui_production_gate.ps1 -Mode Fast -StopExisting
```

覆盖内容：

- `tools/check_fgui_static.ps1`
- `tools/run_fgui_selftest.ps1 -Mode All`
- `git diff --check`

### RuntimeOnly

用于未改 C++ 编译工程，但改了生命周期、资源、层级、输入或 DebugPanel 逻辑的验收。

```powershell
powershell -ExecutionPolicy Bypass -File tools\run_fgui_production_gate.ps1 -Mode RuntimeOnly -LongLoopCount 5 -StopExisting
```

覆盖内容：

- Fast 全部内容
- `tools/run_fgui_selftest.ps1 -Mode LongLoop`

### Full

用于 FGUI 大拆分、C++ runtime/lua bridge 改动、合主前或阶段性收口。

```powershell
powershell -ExecutionPolicy Bypass -File tools\run_fgui_production_gate.ps1 -Mode Full -StopExisting
```

覆盖内容：

- VS2017 Debug|x64 `HelloOgre3D` 构建
- VS2017 Release|x64 `HelloOgre3D` 构建
- `tools/check_fgui_static.ps1`
- `tools/run_fgui_selftest.ps1 -Mode All`
- `tools/run_fgui_selftest.ps1 -Mode LongLoop`
- `tools/run_fgui_selftest.ps1 -Mode Pressure`
- `git diff --check`

Selftest 步骤默认允许 2 次尝试；如果 D3D9 启动阶段偶发 `Cannot create device`，gate 会短暂等待后重试一次，仍失败才中断。

Gate 会启动 `HelloOgre3D.exe` 并创建 D3D9 设备，必须在可用桌面会话中运行。需要肉眼观察窗口时可追加 `-Visible`。

## 通过标准

- VS2017 Debug|x64 和 Release|x64 构建成功。
- Lua 语法、Python 工具编译、AutoGen `--check --strict` 通过。
- `manager/fairygui` 子模块不得直接依赖 `GameManager`，必须走 `FairyGuiNativeApi`。
- All 自测必须输出 `self test suite end: 30 / 30` 或后续更新后的满额通过数。
- Native backend 自测必须通过，Health/DebugPanel 应显示 `FairyGuiRuntime`。
- LongLoop/Pressure 的最终 clean-state 必须满足运行时残留回零：open/hidden UI、binding、transition callback、timer、object handle、child cache、view/controller、package ref、resource warnings 均为 0。
- 日志不得出现 `OGRE EXCEPTION`、`PANIC:`、`call_func error`、`self test result: false` 或 `self test case: ... FAIL`。
- `git diff --check` 通过。

## 使用原则

- 改 C++ runtime、tolua、Premake、FGUI 渲染/输入/事件桥：跑 Full。
- 改 Lua 生命周期、资源、layer、AutoGen、DebugPanel：至少跑 RuntimeOnly。
- 改文档或纯工具脚本：至少跑 Fast。
- 如果 Full gate 失败，优先处理第一个失败步骤；不要继续叠加新功能。

## 当前仍非一步完成的边界

Full gate 证明当前 FGUI 闭环稳定，但不等于已经追平 env1 的全部 MiniUI 能力。仍需继续迭代的生产能力包括：

- 更大规模真实业务 UI 样例。
- GPU 像素级 stencil / alpha mask parity。
- 更完整的 TextInput 输入策略和人工 IME 验证。
- 复杂 List/Tree item 的 AutoGen 子结构。
- Win32 和更多平台组合的构建验证。
