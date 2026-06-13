---
name: validation-loop
description: 混合 Lua/C++ 改动的最小验证环：luac -p + git diff --check + Release x64 build + run_sandbox_smoke
metadata:
  type: feedback
---

用户对改动期望 **observable 验证**（日志/smoke/成功构建），不接受停在理论；若只做了静态分析要**明说**。

混合 Lua/C++ 运行时改动的**最小验证环**：
1. `luac -p <改的 lua>` —— Lua 语法检查
2. `git diff --check` —— 行尾/空白（提示级，见 [[crlf-encoding-gotcha]]）
3. **Release x64 build**（见 [[build-release-for-runtime]]）—— `MSBuild build\HelloOgre3D.sln /p:Configuration=Release /p:Platform=x64 /m`
4. `tools\run_sandbox_smoke.ps1 -Sample <SandboxX> -StopExisting -NoTail` —— 看 `status=PASS`

按改动面追加：AI/影响图 → `run_chapter9_parity_gate.ps1`（Python 见 [[parity-gate-python]]）/ `run_chapter9_visual_capture.ps1`；UI → `run_fgui_production_gate.ps1`；AI 热点性能 → `ai_perf_1000`（看 `[FramePerf] perceptionSystem/updateCall`）。

**改导出给 Lua 的 C++ API**：手术式改 `SandboxToLua.cpp` 对应绑定函数 + 头文件，**禁 `tolua.bat` 全量重生成**（曾致 Sandbox18 崩溃 0xC0000409 栈溢出）；改完跑用到该绑定的 sample（影响图绑定改动跑 Sandbox17 **和** 18）。

**Why:** Chapter9 性能/表现问题反复出现，靠肉眼截图猜会反复试错；先看 `FramePerf`（分 sandboxLua/updateCall/frameWait）再定方向。`/hello-develop-design` 的 `docs/skills/verify.md` 已把这套固化。
