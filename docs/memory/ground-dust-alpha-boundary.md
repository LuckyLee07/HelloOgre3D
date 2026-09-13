---
name: ground-dust-alpha-boundary
description: GroundDust 旧 SVG 渐变经 ImageMagick 6 生成宽 alpha 场，需显式 rgba 路径避免贴片边界
type: project
---

2026-09-13 用同一 Sandbox19 1600×900 macOS GL 入口回放对比 `Relay/GroundDust` 材质 tint alpha 0.065、0.09、0.14。0.09 在地面抽样区域只产生约 2–4 RGB 级差，正常画面仍几乎看不出旧化层次；0.14 产生约 8–11 RGB 级差，但前景出现清楚的圆形暗斑边界，像覆盖色块。该 PNG 的 512² alpha 约 59.36% 非零，约 58.83% 为 255（ImageMagick 6 栅格化后的宽场），所以单调提高材质 alpha 不能同时得到可读细节和无边界过渡。两档试验没有保留，材质恢复 0.065。原始图片与日志在本地 `tmp/goal-p3-ground-dust-20260913/`；基线图在 `tmp/goal-p3-service-bay-20260913/stages/`。

后续已用显式 `rgba(...)` 的稀疏路径/小块替代渐变：同一 ImageMagick 6 输出的非透明像素约 4.85%、全不透明为 0，再由材质 tint 0.25 控制整体强度；`-strip` 使两次 PNG 重建字节一致。改变的是图案空间结构，不是把旧版 alpha 放大。实机图优先于 SVG stop-opacity 与材质数值判断；回归见 [P3 分轮记录](../dev-design/plans/2026-09-13-sandbox19-p3-iteration.md)。
