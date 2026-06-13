# 验证节点 skill（verify）

> 由 `.claude/commands/hello-develop-design.md` 阶段 4 每任务结束后调用；也可被 `docs/skills/subagent-driven-development.md` 在 quality review 后调用。

## 用途

子代理完成一个 task 并通过两阶段 review 后，根据 task 的**层归属 / 回归 sample**（plan 里已声明），选对应验证策略执行，确认行为不退化。

## 调用契约

输入：task 改动文件列表 + plan 声明的验证策略 + 回归 sample；spec/plan 路径（报告引用）。
输出：PASSED / FAILED / SKIPPED；FAILED 附失败摘要与重做建议。
副作用：跑 MSBuild / smoke / gate 脚本。

## 模块反查（registry 驱动）

先把 task 改动文件反查 `docs/registry.json` 的命中模块，用其 `verify` 字段作为权威回归目标（优先于 plan 的口头声明）：

```
对每个改动 file_path：
  对 registry.entries[alias]：
    若 file_path 以 entries[alias].source_paths 任一项为前缀 → alias 入命中集 M
对每个 m ∈ M：读 entries[m].verify.{sample[], gate[]} → 合并成本 task 的回归集
无命中：报"该改动不属任何已登记模块"，问用户手动给命令或 skip
```

`registry.conventions` 给出各 build/smoke/gate/tolua 的标准命令串，展示选项时引用它。命中多个模块时合并去重回归集（如改 ai-tactics + scripting-tolua → 跑 Sandbox17+18 + parity_gate）。

## HelloOgre3D 验证策略库

plan 的每个 task 声明下列之一（或组合）。展示给用户选择：

```
[验证门] 任务 <编号> 改动层=<sandbox/ai/...> 回归 sample=<Sandbox X>
可选验证：
  [1] Release 编译       —— 改了 C++ 必做
  [2] sample smoke       —— run_sandbox_smoke.ps1 -Sample/-Preset <X>
  [3] Chapter9 行为对齐   —— run_chapter9_parity_gate.ps1（动 AI/影响图/战术时）
  [4] Chapter9 视觉对齐   —— run_chapter9_visual_capture.ps1（动渲染/影响图绘制时）
  [5] FGUI 生产验收       —— run_fgui_production_gate.ps1（动 UI 时）
  [6] ai_perf 基线        —— run_sandbox_smoke.ps1 -Preset ai_perf_1000（动 AI 热点时）
请选择：单选 / all / skip
```

### 各策略的具体命令

**[1] Release 编译**（改 C++ 必做；记忆：给用户跑的版本必须 Release）

```
MSBuild build\HelloOgre3D.sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal /nologo
```
- MSBuild 路径优先 VS2022（`C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe`），无则用 vswhere 探测。
- 预期：退出码 0，产物 `bin\HelloOgre3D.exe`（Release ~9MB；Debug ~34MB 说明配置错了）。
- 改了导出给 Lua 的 C++ API：**优先手术式改 `SandboxToLua.cpp` 绑定 + 头文件**；`tolua.bat` 全量重生成已知会引入难定位回归（曾导致 Sandbox18 栈溢出 0xC0000409），非必要不用。

**[2] sample smoke**

```
tools\run_sandbox_smoke.ps1 -Sample Sandbox<X> -StopExisting -NoTail
# 或带 preset：-Preset <preset 名>
```
预期日志出现 `[SMOKE] status=PASS`。崩溃会报 `process exited code=<非0>`。
- 改 AI driver / 对象 / 组件 → 回归 `Sandbox6`(FSM) / `Sandbox7`(DT) / `Sandbox8`(BT)
- 改感知 / 记忆 → `Sandbox10`；团队 → `Sandbox12`；影响图 → `Sandbox13`/`Sandbox17`/`Sandbox18`
- 改战斗/动画且要看到目标获取 → 加 `-RuntimeDiag` 看 per-agent 状态

**[3] Chapter9 行为对齐**（trace 位置/目标/影响图格子，带容差）

```
tools\run_chapter9_parity_gate.ps1 -Python "E:\SoftWare\Python\Python310\python.exe"
```
预期 `[ParityCompare] PASS`。注意默认 `python` 是坏 shim，必须显式指定 Python310（见记忆 parity-gate-python）。

**[4] Chapter9 视觉对齐**（old/new 定时截图对照）

```
tools\run_chapter9_visual_capture.ps1 -CaptureMs "2500"
```
产物在 `tmp\chapter9_visual_*\{old,new}\`，肉眼或裁剪放大对比。

**[5] FGUI 生产验收**

```
powershell -ExecutionPolicy Bypass -File tools\run_fgui_production_gate.ps1 -Mode Full -StopExisting
```

**[6] ai_perf 基线**（动 AI 热点系统时量化开销，对照 `docs/perf/ai-perf-release-baseline-20260612.md`）

```
tools\run_sandbox_smoke.ps1 -Preset ai_perf_1000 -StopExisting -NoTail   # 自动开 FramePerf
```
看 `[FramePerf] objects/perception/ai` 分项，**判断 AI 成本看 updateCall/perceptionSystem，不看 cpuFrame**（VM 上帧时间被渲染 engineGap 主导）。

## 执行

用 Bash/PowerShell 工具跑用户选定命令；超时默认 10 min。

## 失败处理

```
[验证门] 任务 <编号> 验证失败：
  策略：<编号>   命令：<命令>   退出码：<code>   输出尾部：<最后 50 行>
请选择：
  [r] 修一下让我重跑：你给修法，我重派 implementer 子代理
  [f] 把失败回灌子代理重做：自动重派，附完整失败上下文
  [s] 跳过：标 BLOCKED，进下一 task
```

重做次数上限 1（命令文件层面控制），第 2 次失败自动 BLOCKED。

## 全 skip 反馈

连续 skip 不重复警告（不烦人），但阶段 6 收尾报告红字标"X 个验证被跳过"。
