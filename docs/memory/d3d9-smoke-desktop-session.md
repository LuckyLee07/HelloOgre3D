---
type: project
description: Ogre D3D9 smoke 必须运行在可访问实际桌面图形设备的会话中
---

# D3D9 smoke 需要桌面图形会话

`tools/run_sandbox_smoke.ps1` 即使未传 `-Visible`，仍会创建 Ogre D3D9 窗口和渲染设备。
受限或隔离的执行会话可能在 Lua sample 初始化之前报
`RenderingAPIException: Cannot create device!`，随后 smoke 只表现为“未确认 run id”。

遇到这组症状时，先看 `bin/Sandbox.log` 是否停在 `D3D9Device::createD3D9Device`；
若是，应改在可访问实际桌面的会话中重跑，不能把它归因到 sample 逻辑。
