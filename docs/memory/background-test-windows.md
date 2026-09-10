---
name: background-test-windows
description: 自动验证窗口在后台启动，不抢占用户当前应用
type: feedback
---

2026-09-10 用户明确要求：“启动窗口请在后台启动，不要抢占当前活动窗口吧”。后续自动运行游戏验证时遵守该偏好；不要以启动后再切回应用代替避免抢焦点。当前 macOS 启动方式见 [验证工作流](../skills/verify.md)。

同日用户开始工作时再次强调后台复跑。Windows 的 `Start-Process -WindowStyle Hidden` 本身不足：Ogre 普通创建窗口会调用 `SW_SHOWNORMAL`。现有游戏入口已支持 `HELLO_WINDOW_BACKGROUND=1`，从创建时隐藏 D3D9 窗口并禁用硬件输入；验证日志应有 `[WindowMode] background=true hidden=true foregroundUnchanged=true` 与 `physical-input=disabled`。自动验证同时设 `HELLO_AUDIO_SILENT=1`，避免测试声音打断工作。不要启用 smoke 的 `-Visible`，也不为取证把窗口切到前台。
