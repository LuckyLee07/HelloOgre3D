# docs 索引

> 目的：让阅读和施工先找对文档，避免被历史路线图带偏。
>
> 治理规则：方向取舍先看 `project-direction.md`；当前在做什么看 `cycle-01.md`；想做什么看 `backlog.md`；具体实现看对应模块文档 `modules/<alias>.md` 或专项文档。历史文档（`archive/`）只看证据和具体条目，不看旧排期。
>
> 单一真源约定：
> - **进度状态**只在 `backlog.md`（唯一带 `[ ]`/`[x]` 的清单）与 `cycle-01.md`。其它文档不再记录进度。
> - **P1–C5 / P8–P11 的证据与方案**看 `design/architecture-improvement-plan.md` §7/§9；backlog 只记"做不做"，不复制细节。
> - **当前性能基线**看 `perf/` 里日期最新的一份（见 §8）。
>
> 2026-08-04 收敛：原 `planning/` 下 5 份规划文档（long-term-iteration-plan / ai-technical-iteration-plan / ai-roadmap / high-priority-todo / playable-vertical-slice-todolist）已全部归档到 `archive/`——其中 ai-roadmap 的 P0/P1 七项已全部完成、可玩切片已有阶段成果。保留「方向 + 当前 cycle + 候选池」三份入口。2026-09-05 按个人项目定位修订：长期能力地图与详细阶段设计在方向文档中维护，取消外部试玩和固定周期交付的硬门槛。

## 0. 目录结构

[2026-09-06 Sandbox19 稳定性记录](stability-2026-09-06.md)：分离力 NaN、诊断 probe 生命周期、完整对局及验证边界。

```
docs/
├── README.md                  本索引
├── project-direction.md       北极星（方向总纲，不含状态）
├── cycle-01.md                当前阶段（问题、最小成果、证据与复盘）
├── backlog.md                 唯一活清单（候选池，只有这里有 [ ]/[x]）
├── registry.json              模块知识库注册表（/hello-develop-design 阶段0 + verify 用）
├── architecture.html          架构总览渲染图
├── design/      设计与架构分析（活跃改造参考）
├── reference/   外部参考（MiniGame / 设计概念）
├── archive/     历史 / 暂缓方向（只作证据参考）
├── modules/     模块知识库：每子系统一份参考文档（_template.md + 21 模块）
├── skills/      /hello-develop-design 的节点 skill 方法论文档
├── dev-design/  /hello-develop-design 产物：specs/ 与 plans/
├── fgui/        FairyGUI 专项
└── perf/        性能与实测证据
```

## 1. 必读入口（只有三份）

| 文档 | 用途 |
|---|---|
| `project-direction.md` | 方向与长期演进设计。含目标、阶段依赖、M1–M3 详细设计与验收，不含任务勾选状态。 |
| `cycle-01.md` | **当前在做什么**。一次认领一个主要问题，以证据复盘，允许缩小、调整和暂停。 |
| `backlog.md` | **想做什么**。唯一带 `[ ]`/`[x]` 的清单；候选池不排期，认领时才拉进 cycle。 |

> 历史排期文档（`archive/long-term-iteration-plan.md`、`ai-technical-iteration-plan.md`、`ai-roadmap.md`、`high-priority-todo.md`、`playable-vertical-slice-todolist.md`）保留作脉络与证据，**不再作为入口**。

## 2. 设计与架构分析（design/，当前施工参考）

| 文档 | 用途 |
|---|---|
| `design/chapter9-parity-architecture-notes.md` | Chapter9 对齐暴露的问题：profile、trace、意图链路、InfluenceMap 视觉边界。 |
| `design/architecture-improvement-plan.md` | 架构解耦证据和方案：SandboxServices、组件化、所有权、反向依赖。⚠️ P6/C5 已解决、P1/P5/P7 部分完成，详见文内复核状态与 §7 跟踪清单。 |
| `design/cpp-object-model-refactor-roadmap.md` | C++ 对象模型与组件化长期路线。 |
| `design/behavior-tree-gap-analysis.md` | 行为树执行模型补强参考。⚠️ G1(重评估)/G4(Parallel/Random) 已完成，开放项为 G2/G3/G5–G11。 |

## 3. 模块知识库（modules/）

每个子系统一份参考文档（职责/源码位置/关键类/约束红线/验证策略/gap），由 `registry.json` 索引、`/hello-develop-design` 阶段 0 按功能描述自动注入。共 21 模块：

- AI：`ai-perception` `ai-tactics` `ai-team` `ai-navigation` `ai-behavior` `ai-decision` `ai-fsm` `ai-common` `ai-controller`
- 对象/组件：`objects` `objects-anim` `components` `core-object` `systems-physics`
- 系统/脚本/运行时：`systems-manager` `systems-service` `scripting-tolua` `runtime` `samples` `ai-scripts` `fgui`

新模块 / 改约束经 `skills/doc-sync.md` 同步，并更新 `registry.json`。

## 4. 参考资料（reference/）

| 文档 | 用途 |
|---|---|
| `reference/minigame-ai-production-reference.md` | MiniGame AI 生产级架构参考：AOI、缓存、调度、BT runtime、Lua/C++ 分界。 |
| `reference/reference-minigame-patterns.md` | MiniGame 通用沙盒架构参考。 |
| `reference/AIArchitectureBeyondBook.md` | Chapter 7-9 概念在当前架构下的设计参考。 |

## 5. 历史 / 暂缓（archive/）

| 文档 | 状态 |
|---|---|
| `archive/long-term-iteration-plan.md` | 2026-08-04 归档。长期迭代总规划（阶段一~六）。阶段一/二内容已基本兑现，排期口径已废。 |
| `archive/ai-technical-iteration-plan.md` | 2026-08-04 归档。AI 技术执行计划，被各 `modules/*.md` 的现状描述取代。 |
| `archive/ai-roadmap.md` | 2026-08-04 归档。P0/P1 七项（调试面板/事件规范化/更新调度/感知组件化/TeamBlackboard/InfluenceMap/BT 数据化）已全部完成，仅 P2 录制回放未做（已转入 `backlog.md`）。 |
| `archive/high-priority-todo.md` | 2026-08-04 归档。主体是 5–6 月迭代记录，与 `git log` 重复。 |
| `archive/playable-vertical-slice-todolist.md` | 2026-08-04 归档。可玩纵切片已于 2026-07-11 全部完成并通过手感验收。 |
| `archive/project-roadmap.md` | 历史非 AI 主线整理。UI/runtime/调试/构建条目可参考，排序已失效。 |
| `archive/foundation-modules-inventory.md` | 历史地基盘点。保留模块清单价值，数据驱动切片排期已失效。 |
| `archive/trigger-system-gap-analysis.md` | 触发器方向暂缓。事件系统四件套可在服务 AI/生产级边界时小步引入。 |
| `archive/SoldierObjectRefactor.md` | 历史重构记录。查旧设计和对象链路时参考。 |
| `archive/visual-editor-implementation-plan.md` / `archive/visual-editor-task-breakdown.md` | 编辑器/UGC 后置方向，当前不作为主线施工。 |

## 6. /hello-develop-design 工作流（skills/ + dev-design/）

[2026-09-06 指令审计](agent-instructions-audit-2026-09-06.md)：官方依据、冲突处理、精简结果与验证边界。

| 路径 | 用途 |
|---|---|
| `skills/brainstorming.md` `writing-plans.md` `subagent-driven-development.md` `verify.md` `gitcommit.md` `doc-sync.md` | 按需读取的设计、实施、验证与收尾节点；统一流程见 [skills/workflow.md](skills/workflow.md) |
| `.agents/skills/hello-develop-design/SKILL.md` | Codex 仓库自动发现入口；Claude 入口与旧 Codex 分发入口共享同一流程 |
| `skills/workflow.md` | 工作流真源：按用户意图与改动风险选择流程 |
| `dev-design/specs/` `dev-design/plans/` | 工作流产出的设计 spec 与实施 plan |

## 7. FGUI 专项（fgui/）

| 文档 | 用途 |
|---|---|
| `fgui/fairygui-production-gate.md` | FGUI 验收入口，配 `tools/run_fgui_production_gate.ps1`。 |
| `fgui/fairygui-production-convergence-todo.md` | FGUI 生产级收口状态。 |
| `fgui/fairygui-business-ui-standard-flow.md` | 业务 UI 标准接入流程。 |
| `fgui/fairygui-final-roadmap.md` / `fgui/fairygui-business-framework-todo.md` | FGUI 长期能力地图和历史 TODO。 |
| `fgui/` 其余文档 | AutoGen、资源、接入计划、重构计划等专项资料。 |

## 8. 性能与实测证据（perf/）

| 文档 | 用途 |
|---|---|
| `perf/ai-perf-release-baseline-20260710.md` | **当前 Release x64 基线**（AI perf）。做性能对比先以本文为基准。 |
| `perf/ai-perf-release-baseline-20260612.md` | 历史 Release x64 基线（已被 20260710 取代）；结论：帧时间被 VM 渲染(engineGap)主导，AI 主成本是 perceptionSystem。 |
| `perf/ai-perception-baseline-20260602.md` | AI perception pressure Debug x64 基线，100/500/1000 agent 对照。 |
| `perf/ai-spatial-filter-retest-20260602.md` | Spatial filter 复测，candidates/filtered/reject/queryMs。 |
| `perf/fgui-tracy-sample-*.md` | FGUI Tracy 实测报告。 |

## 9. 阅读路径

- 判断项目方向：`project-direction.md`。
- 知道现在该做什么：`cycle-01.md`（当前周期）；挑下一件事：`backlog.md`（候选池）。
- 做某子系统：先看 `modules/<alias>.md`（约束 + 验证），再读对应 C++/Lua 实现。
- 处理 Chapter9 视觉/行为差异：`design/chapter9-parity-architecture-notes.md`。
- 做架构解耦：`design/architecture-improvement-plan.md`。
- 新功能开发：`/hello-develop-design <功能描述>`（见 `skills/`）。
