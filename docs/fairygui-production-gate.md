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
- `GameManager` tolua 导出不得注册 `*FairyGui*` API，新增 Lua API 必须走 `RuntimeToLua.pkg` / `FairyGuiRuntime`。
- `runtime/ui/fairygui` 不得反向依赖 `GameManager` / `ClientManager`。
- `FairyGuiSystem.h` 必须保持 runtime facade，不暴露 FairyGUI、cocos、Ogre、`RenderCommandSink`、`GObject`、`GRoot` 等实现细节。
- 业务 UI 脚本不得直连 `FairyGuiNativeApi` / `NativeApi` / `FairyGuiRuntime` / `GameManager`，必须走 `FairyGuiManager`、Ctrl/View/Model、AutoGen。
- 生产覆盖入口不得退化：BusinessFlow、ComplexControls、VirtualList、Tree、ResourcePolicy、ResourceFallback、TextInputPolicy、Pressure、LayerBoundary、Mask、DebugPanel、AiDebugPanel 都必须保留自测或诊断入口。
- All 自测必须输出满额通过结果。
- Native backend 自测必须通过，Health/DebugPanel 应显示 `FairyGuiRuntime`。
- LongLoop/Pressure 的最终 clean-state 必须满足运行时残留回零：open/hidden UI、binding、transition callback、timer、object handle、child cache、view/controller、package ref、resource warnings 均为 0。
- 日志不得出现 `OGRE EXCEPTION`、`PANIC:`、`call_func error`、`self test result: false` 或 `self test case: ... FAIL`。
- `git diff --check` 通过。

## 使用原则

- 改 C++ runtime、tolua、premake、FGUI 渲染/输入/事件桥：跑 Full。
- 改 Lua 生命周期、资源、layer、AutoGen、DebugPanel：至少跑 RuntimeOnly。
- 改文档或纯工具脚本：至少跑 Fast。
- 如果 Full gate 失败，优先处理第一个失败步骤；不要继续叠加新功能。

## 当前口径

当前项目的生产级入口收敛目标已经落到 gate：

- C++ public facade 与内部实现边界可静态检查。
- Lua 业务入口与 native backend 边界可静态检查。
- 真实业务流、复杂控件、资源策略、输入策略、压力与 DebugPanel 观测有自动化覆盖。
- 当前 mask/stencil 生产 backend 明确为 `shapeCpu`，提供可观测 CPU shape clip fallback；像素级 GPU stencil/alpha mask 作为后续渲染后端升级，不阻塞当前生产级入口收敛。

env1 MiniUI 全量能力追平仍由 `docs/fairygui-env1-production-target.md` 独立跟踪。
