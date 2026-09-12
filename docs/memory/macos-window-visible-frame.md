---
name: macos-window-visible-frame
description: macOS 标题栏窗口的请求尺寸可能被当前显示器可见工作区压缩，验收必须读取实际内容尺寸
type: project
---

2026-09-12 在 Apple M1 Pro 桌面会话验证：Ogre Cocoa 普通标题栏窗口请求 1920×1080 时，当前显示器的菜单栏/Dock 可见工作区把实际内容约束为 1920×945；请求 1280×720 和 1600×900 则按原尺寸创建。这个结果取决于当前显示器与系统工作区，不应硬编码 945，也不应删掉供更大/外接显示器使用的 1920×1080 档。

窗口启动与 resize 验收必须读取 `RenderWindow::getMetrics`、`EventHandle_WindowResized` 的实际值和抓帧像素尺寸，不能仅凭 `Video Mode` 请求值声称 1080p 通过。产品 UI 显示实际内容尺寸；日志同时保留 requested 与 pixels，便于区分配置意图和操作系统约束。
