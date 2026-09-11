---
type: project
date: 2026-09-12
---

# Ogre GL3+ 透明叠加材质必须显式绑定 shader

在 Apple OpenGL 4.1 的 GL3+ core profile 中，只有 texture unit、`scene_blend alpha_blend` 与 fixed-function pass 的 visual plane 可能完整加载资源却不出现在画面中；日志也不一定报材质错误。Sandbox19 的地表旧化、接触阴影和植被应继续使用 `relay_overlay` 的 GLSL 150 program，不能删除 program ref 并期待固定管线回退。

验证这类素材时必须查看真实 GL 抓帧，同时检查 PNG 实际 alpha 统计。ImageMagick 6 在本机栅格化 SVG 渐变时产生了比源 stop-opacity 更宽、更强的 alpha 场，最终不透明度因此在 material tint 中显式限制；只看 SVG 源值不足以判断游戏内强度。
