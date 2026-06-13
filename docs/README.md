# docs 索引

> 目的：让阅读和施工先找对文档，避免被历史路线图带偏。
>
> 治理规则：方向取舍先看 `project-direction.md`；阶段排期看 `long-term-iteration-plan.md`；具体实现看对应专项文档。历史文档只看证据和具体条目，不看旧排期。

## 1. 必读入口

| 文档 | 用途 |
|---|---|
| `project-direction.md` | 北极星。定义当前阶段是 AI 学习与实验沙盒，长期目标是生产级游戏项目 / 玩法运行时。 |
| `long-term-iteration-plan.md` | 长期迭代总规划。把后续阶段拆成可比较/可复现、AI 热点 C++ 化、意图链路、运行时边界、工具门禁、编辑器/UGC 后置。 |
| `ai-technical-iteration-plan.md` | AI 技术执行计划。落到 spatial、perception cache、TeamBlackboard、TacticalQuery、InfluenceMap、BT runtime 和 benchmark。 |

## 2. 当前施工参考

| 文档 | 用途 |
|---|---|
| `chapter9-parity-architecture-notes.md` | Chapter9 对齐暴露的问题：profile、trace、意图链路、InfluenceMap 视觉边界。 |
| `architecture-improvement-plan.md` | 架构解耦证据和方案：SandboxServices、组件化、所有权、反向依赖。⚠️ P6/C5 已解决、P1/P5/P7 部分完成，详见文内复核状态与 §7 跟踪清单。 |
| `high-priority-todo.md` | 执行记录 + 细粒度 backlog。不要把它当最高优先级入口。 |
| `cpp-object-model-refactor-roadmap.md` | C++ 对象模型与组件化长期路线。 |
| `behavior-tree-gap-analysis.md` | 行为树执行模型补强参考。⚠️ G1(重评估)/G4(Parallel/Random) 已完成，开放项为 G2/G3/G5–G11，详见文内复核状态。 |

## 3. 参考资料

| 文档 | 用途 |
|---|---|
| `minigame-ai-production-reference.md` | MiniGame AI 生产级架构参考，聚焦 AOI、缓存、调度、BT runtime、Lua/C++ 分界。 |
| `reference-minigame-patterns.md` | MiniGame 通用沙盒架构参考。 |
| `AIArchitectureBeyondBook.md` | Chapter 7-9 概念在当前架构下的设计参考。 |
| `ai-roadmap.md` | AI 长期愿景和能力地图，排期以新总规划为准。 |

## 4. 历史 / 暂缓

| 文档 | 状态 |
|---|---|
| `project-roadmap.md` | 历史非 AI 主线整理。UI/runtime/调试/构建条目可参考，排序已失效。 |
| `foundation-modules-inventory.md` | 历史地基盘点。保留模块清单价值，数据驱动切片排期已失效。 |
| `trigger-system-gap-analysis.md` | 触发器方向暂缓。事件系统四件套可在服务 AI/生产级边界时小步引入。 |
| `SoldierObjectRefactor.md` | 历史重构记录。查旧设计和对象链路时参考。 |
| `visual-editor-implementation-plan.md` / `visual-editor-task-breakdown.md` | 编辑器/UGC 后置方向，当前不作为主线施工。 |

## 5. FGUI 专项

| 文档 | 用途 |
|---|---|
| `fgui/fairygui-production-gate.md` | FGUI 验收入口，配 `tools/run_fgui_production_gate.ps1`。 |
| `fgui/fairygui-production-convergence-todo.md` | FGUI 生产级收口状态。 |
| `fgui/fairygui-business-ui-standard-flow.md` | 业务 UI 标准接入流程。 |
| `fgui/fairygui-final-roadmap.md` / `fgui/fairygui-business-framework-todo.md` | FGUI 长期能力地图和历史 TODO。 |
| `fgui/` 其余文档 | AutoGen、资源、接入计划、重构计划等专项资料。 |

## 6. 性能与实测证据

| 文档 | 用途 |
|---|---|
| `perf/ai-perception-baseline-20260602.md` | AI perception pressure Debug x64 基线，含 100 / 500 / 1000 agent 对照。 |
| `perf/ai-perf-release-baseline-20260612.md` | AI perf **Release x64** 基线，含 100/500/1000 分项耗时；结论：帧时间被 VM 渲染(engineGap)主导，AI 主成本是 perceptionSystem。 |
| `perf/ai-spatial-filter-retest-20260602.md` | Spatial filter 复测，记录 candidates / filtered / reject / queryMs。 |
| `perf/fgui-tracy-sample-*.md` | FGUI Tracy 实测报告。 |

## 7. 阅读路径

- 判断项目方向：读 `project-direction.md`。
- 安排后续阶段：读 `long-term-iteration-plan.md`。
- 做 AI 热点系统：读 `ai-technical-iteration-plan.md`，再读对应 C++/Lua 实现。
- 处理 Chapter9 视觉/行为差异：读 `chapter9-parity-architecture-notes.md`。
- 做架构解耦：读 `architecture-improvement-plan.md`。
- 查参考项目怎么做：读 `minigame-ai-production-reference.md` 或 `reference-minigame-patterns.md`。
