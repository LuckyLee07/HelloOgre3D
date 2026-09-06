# 按改动面验证

供 [共享工作流](workflow.md) 或普通修复使用。输入是实际改动、目标行为与运行平台；输出是每项 PASS / FAIL / NOT RUN 及命令、关键日志和原因。

## 选择验证

用 `docs/registry.json` 反查相关模块与 sample/gate 候选，结合实际调用链选择必要回归；registry 是导航，不是“命中就全跑”的指令。零命中时根据代码找入口，不默认问用户选 all/skip。

| 改动 | 最小充分检查 |
|---|---|
| 文档/skill | 内容/引用、JSON/YAML 等适用格式、`git diff --check`；不跑游戏构建 |
| Lua | Lua 5.1 `luac -p <file>` + 对应 sample；已有匹配二进制时无需重编 |
| C++ / 构建 | 当前平台 Release 构建 + 受影响运行入口；新增编译文件或配置变更先生成工程 |
| ABI 布局（虚函数/继承/字段） | clean rebuild，避免旧对象文件 RTTI/布局不一致 |
| 绑定 | 头文件/导出声明/绑定/调用点一致性 + 构建 + 消费该 API 的 sample |
| 视觉/输入/导航 | 实际窗口/交互/截图；导航同时核对路径与 debug 绘制 |
| FGUI | 目标自测；复杂生命周期用 All；生产收口才跑 Full gate（按脚本含 Debug/Release） |
| 性能/Chapter9 对齐 | 对应 perf/parity/visual gate，保留可比日志；不扩展到无关 UI 或 AI 改动 |

AI driver/对象改动检查相关 `Sandbox6/7/8`；影响图绑定查 `Sandbox17/18`；其他入口见 registry。已有检查通过后仅在新改动、失败或未解决风险需要时扩大/复跑。不为低影响可逆修改增加镜像实现的测试。

## Windows

仓库根目录运行；PowerShell 示例：

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe' build\HelloOgre3D.sln /t:HelloOgre3D /p:Configuration=Release /p:Platform=x64 /m
powershell -ExecutionPolicy Bypass -File tools\run_sandbox_smoke.ps1 -Sample Sandbox8 -NoTail
```

需要生成工程时用 `tools\premake\premake5.exe --os=windows --file=premake/premake.lua vs2017 --with-fairygui` 并保持任务所需选项；`vs2017.bat` 会清理部分产物，先查看脚本。

按需使用 `tools/run_chapter9_parity_gate.ps1`、`tools/run_chapter9_visual_capture.ps1`、`tools/run_fgui_selftest.ps1`、`tools/run_fgui_production_gate.ps1` 或 smoke 的 `-Preset ai_perf_1000`。先读参数与当前环境；历史 Windows Python 绝对路径仅见记忆，不能照搬到其他机器。`-StopExisting` 会终止已有进程，只在确认属于本次测试或已授权时使用。

## macOS

工程生成用 `bash xcode.sh`。通过 `rg --files --hidden --no-ignore build -g project.pbxproj` 找生成工程，`xcodebuild -list -project <实际路径>` 查 target/configuration，再构建实际 target 的 Release；架构按生成配置和本机选择，不套用 Windows x64。只生成工程不等于编译通过。

M1 脚本覆盖的入口可运行：

```bash
python3 tools/run_m1_smoke.py --help
python3 tools/run_m1_smoke.py --samples Sandbox19
```

该脚本运行 `bin/HelloOgre3D` 并检查 marker/错误，支持的 sample 以实际 `--help` 为准；其他 sample 用原生程序和仓库现有环境变量启动，观察相应成功证据。不要把 M1 smoke 等同于 Windows Chapter9/FGUI 全套 gate。

## 日志与失败

自行读取 stderr、`bin/Sandbox.log`（Debug 为 `Sandbox_d.log`）和脚本输出。smoke 必须有目标 PASS marker 且无相关错误，进程仍存活或 timeout 本身不是通过证据。性能看 `[FramePerf]` 的 updateCall/perceptionSystem，并核对渲染开销和配置。

发现失败先分类为改动回归、已有问题或环境限制，再依据证据修复并复跑受影响检查；没有固定“失败两次就放弃”规则。缺少图形会话/平台时继续可执行检查并报告 NOT RUN，保留必要验收待办。只有必需验收满足才标完成。
