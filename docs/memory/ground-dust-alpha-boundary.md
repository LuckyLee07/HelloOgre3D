---
name: ground-dust-alpha-boundary
description: GroundDust 现有 PNG 宽 alpha 场使透明度调高后先显出圆形贴片边界
type: project
---

2026-09-13 用同一 Sandbox19 1600×900 macOS GL 入口回放对比 `Relay/GroundDust` 材质 tint alpha 0.065、0.09、0.14。0.09 在地面抽样区域只产生约 2–4 RGB 级差，正常画面仍几乎看不出旧化层次；0.14 产生约 8–11 RGB 级差，但前景出现清楚的圆形暗斑边界，像覆盖色块。该 PNG 的 512² alpha 约 59.36% 非零，约 58.83% 为 255（ImageMagick 6 栅格化后的宽场），所以单调提高材质 alpha 不能同时得到可读细节和无边界过渡。两档试验没有保留，材质恢复 0.065。原始图片与日志在本地 `tmp/goal-p3-ground-dust-20260913/`；基线图在 `tmp/goal-p3-service-bay-20260913/stages/`。

下一次改善地面应先改变贴层图案的空间结构和羽化，或用不同尺度的局部细节，而不是仅放大现有 tint alpha。实机图优先于 SVG stop-opacity 与材质数值判断。见 [P3 分轮记录](../dev-design/plans/2026-09-13-sandbox19-p3-iteration.md)。
