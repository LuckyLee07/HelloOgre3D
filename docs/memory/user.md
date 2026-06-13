---
name: user
description: 用户画像与协作偏好：code-anchored 真值核对、verify-first、要 observable 验证、方向先看 project-direction
metadata:
  type: user
---

用户在 Windows 上跨多个仓库做具体工程（`E:\Workspace\HelloOgre3D`、`MineCraft`、`F:\MiniStar`、`G:\MiniGame_Adv\Source`），常做：对照代码的文档真值核对、就地改文档、运行时/性能调试、仓库架构分析、交付物打包、基于现状的产品方向解读、对未提交 diff 的仓库卫生审计。

**协作偏好（按此工作）：**
- 任务一旦清楚，**要执行不要罗列选项**。
- 文档/架构分析：要答案**贴合当前代码/配置/方向**，不要听着漂亮的泛泛之谈；方向问题**先对 `docs/project-direction.md`** 解读，别给字面功能清单或通用平台话术。
- 改/审文档：当成**对照仓库现状的真值核对**，不是摘要；反馈属于原文就**直接改源文档**；用户更新文档后再问，要**重新核对**而非沿用上次结论。
- review/fix：**先 verify、再做最小必要修复**，不要投机式重写或大重构。
- 性能问题：默认给 **observable 验证**（日志 + smoke/build），别停在理论。
- 仓库分析：**自顶向下**（项目目的→入口链→模块边界→关键运行时资源/配置），不是裸文件列表；问"现在怎么工作"要**端到端追运行时链路**，不只总结一个类。
- 静态分析结论**明确标注是静态**，别把"读代码"夸成"已构建/已运行验证"。

**How to apply:** 见 [[validation-loop]]（验证环）、[[build-release-for-runtime]]、[[no-greenfield-event-infra]]。
