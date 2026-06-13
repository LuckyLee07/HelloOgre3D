---
name: run-and-diagnose
description: 改完代码自己跑 exe、自己读日志定位，不要把验证甩给用户
metadata:
  type: feedback
---

完成代码改动（尤其 Lua / tolua 绑定）后，**自己跑 exe、自己读日志、自己定位 bug**，别默认把验证甩给用户（用户明确说过"这不应该是你自己打 log 运行然后分析嘛"）。

**流程：**
1. **编译**：若与用户的 VS 抢文件锁（CLI MSBuild 报 `LNK1104 .obj` 被占），让用户告知"已编译通过"再继续；否则自己编 Release（见 [[build-release-for-runtime]]）。
2. **运行抓日志**：`cd e:/Workspace/HelloOgre3D/bin && timeout 25 ./HelloOgre3D.exe > stdout.log 2> stderr.log`（25s 够初始化+几帧；exit 124 是 timeout 正常）。smoke 场景用 `tools/run_sandbox_smoke.ps1`。
3. **优先看 stderr**：Lua pcall 错误在此——`CallFuncV1 lua_pcall error`、`attempt to call`、`attempt to index nil`、`attempt to call field 'new'`（后者见 [[tolua-no-new]]）。
4. **次看 `Sandbox_d.log`（Debug）/`Sandbox.log`（Release）**：含 `Lua:[xxx.lua:N]` 行 + 渲染信息。
5. **清理**：跑完生成大量 `*_prepshadow.log`/`NavMeshDebug_*.log`，下次跑前清掉。

只有需要 GUI 交互（按键/鼠标）才打回用户。初始化阶段的 Lua/tolua/agent 创建/首批帧逻辑问题都能从日志看出。性能看 `[FramePerf]`（见 [[validation-loop]]）。
