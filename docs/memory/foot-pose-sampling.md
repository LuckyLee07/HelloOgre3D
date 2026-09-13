---
name: foot-pose-sampling
description: 足底滑移要在最终显示层应用后采样，Lua Update 中的骨骼位置不是画面姿态
type: project
---

2026-09-13 Sandbox19 站立侧移诊断中，`Sandbox_Update` 内的 `SoldierObject:GetBonePosition` 读取的是仿真动作姿态：`SoldierLocomotionLayer::Restore` 在下一次仿真前撤回显示层，而 `GameManager::RenderPresentation` 后才应用动作混合。Lua 中曾看到两脚连续按根速度平移，错误推断整段无步态。

测最终脚滑要在 `GameManager::RenderPresentation` 调用 `AnimComponent::RenderPresentation` 后、渲染前，从角色渲染节点读取两脚世界位置；对同一仿真步的多帧采样按显示帧区分。只在脚低且未换支撑脚的区间比较世界水平位移，并同时核对脚高与实机图；单独降低位移数字可能造成屈膝或悬空。原始对照与失败实验在本地 `tmp/goal-foot-render-20260913/`、`tmp/goal-p3-foot-20260913/`。
