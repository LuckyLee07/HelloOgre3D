---
name: build-release-for-runtime
description: bin/HelloOgre3D.exe 卡顿时先确认是不是 Debug 构建；给用户跑的版本要编 Release
metadata:
  type: feedback
---

Debug 与 Release 构建**都输出到同一个 `bin/HelloOgre3D.exe`**（互相覆盖）。Debug 构建开了 `/Od` + `_ITERATOR_DEBUG_LEVEL=2`，Ogre 渲染内部大量 STL 容器迭代在迭代器调试下慢几十倍，导致运行时 ~3 FPS（cpuFrame ~256ms、engineGap/渲染 ~223ms）。Release 同场景 cpuFrame ~14ms、engineGap ~8ms（快 ~20 倍）。

**Why:** 改 3D 影响图时反复用 `Configuration=Debug` 编译，把用户原本的 Release exe 覆盖成 Debug，用户报"现在那么卡"。排查发现关掉影响图网格+红圈后仍 3 FPS → 是基础场景渲染慢 → Debug 构建所致，与影响图改动无关。

**How to apply:** 默认用 `Configuration=Release /p:Platform=x64` 编译给用户跑的版本；只有需要断点/断言调试时才编 Debug，且编完 Debug 后记得重编 Release 还原 bin。判断 bin 是哪个构建：Debug ~34MB、Release ~9MB。Release 日志写 `bin/Sandbox.log`，Debug 写 `bin/Sandbox_d.log`。性能排查用 `HELLO_PERF_STALL_LOG=1` + `HELLO_PERF_SUMMARY_INTERVAL_MS`，看 `[FramePerf] stall` 的 cpuFrame/engineGap/sandboxLua。判断 AI 真实成本看 `updateCall`/`perceptionSystem`，不看 `cpuFrame`（VM 上帧时间被渲染 engineGap 主导）。见 [[validation-loop]]。
