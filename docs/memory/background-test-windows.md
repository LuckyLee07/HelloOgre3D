---
name: background-test-windows
description: 自动验证窗口在后台启动，不抢占用户当前应用
type: feedback
---

2026-09-10 用户明确要求：“启动窗口请在后台启动，不要抢占当前活动窗口吧”。后续自动运行游戏验证时遵守该偏好；不要以启动后再切回应用代替避免抢焦点。当前 macOS 启动方式见 [验证工作流](../skills/verify.md)。

2026-09-11 在真实桌面会话确认：完全隐藏的 HWND 对该 D3D9 驱动创建设备不可靠；只用 `WS_EX_NOACTIVATE` 放在屏幕 `(0,0)` 又会以大窗口遮挡用户。Windows 的 `HELLO_WINDOW_BACKGROUND=1` 应创建 `hidden=false`、带 `WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW` 的无边框渲染窗口，并把位置计算到整个虚拟桌面范围之外；显示使用 `SW_SHOWNOACTIVATE`。不要再向 `Start-Process` 传 `-WindowStyle Hidden`，也不要通过 `SetForegroundWindow` 暂时抢焦点再切回。

验证日志应同时有 `[WindowMode] background=true hidden=false noActivate=true offscreen=true foregroundUnchanged=true` 与 `[WindowMode] background=true physical-input=disabled`。自动验证再设 `HELLO_AUDIO_SILENT=1`，避免测试声音打断工作。不要启用 smoke 的 `-Visible`，也不为取证把窗口切到前台。`WindowMode` 早于 Lua 的 smoke run-id 写入日志，启动器应从本次新增日志核验窗口安全字段，再用 run-id 隔离 sample 与自测结果。

2026-09-11 在用户允许续跑后，修正后的 10 秒与 40 秒 Windows Release smoke 均为 PASS；实际 1280×800 窗口位于 `-3264,-864`，日志确认 `offscreen=true`、`foregroundUnchanged=true` 与物理输入禁用。

同日进一步确认：窗口完全处于虚拟桌面外时，`MonitorFromWindow(..., MONITOR_DEFAULTTONULL)` 返回空；旧 `D3D9Device::validateDisplayMonitor` 因此让 Ogre 跳过 render target 更新，Lua 和 smoke 仍继续但 RenderCapture 只得到黑帧。仅对带 `noActivate` 的自动化窗口允许这个空 monitor，普通窗口仍要求与显示器相交。后台 smoke 只能证明逻辑链；涉及视觉结论时还要读取 RenderCapture 并确认是有效画面。
