# docs 索引

> 目的：让阅读和施工先找对文档，避免被历史路线图带偏。
>
> 治理规则：方向取舍先看 `project-direction.md`；阶段排期看 `planning/long-term-iteration-plan.md`；具体实现看对应模块文档 `modules/<alias>.md` 或专项文档。历史文档（`archive/`）只看证据和具体条目，不看旧排期。

## 0. 目录结构

```
docs/
├── README.md                  本索引
├── project-direction.md       北极星（方向总纲）
├── registry.json              模块知识库注册表（/hello-develop-design 阶段0 + verify 用）
├── architecture.html          架构总览渲染图
├── planning/    规划与排期（长期/AI/backlog）
├── design/      设计与架构分析（活跃改造参考）
├── reference/   外部参考（MiniGame / 设计概念）
├── archive/     历史 / 暂缓方向（只作证据参考）
├── modules/     模块知识库：每子系统一份参考文档（_template.md + 21 模块）
├── skills/      /hello-develop-design 的节点 skill 方法论文档
├── dev-design/  /hello-develop-design 产物：specs/ 与 plans/
├── fgui/        FairyGUI 专项
└── perf/        性能与实测证据
```

## 1. 必读入口（根 + planning/）

| 文档 | 用途 |
|---|---|
| `project-direction.md` | 北极星。定义当前阶段是 AI 学习与实验沙盒，长期目标是生产级游戏项目 / 玩法运行时。 |
| `planning/long-term-iteration-plan.md` | 长期迭代总规划。可比较/可复现、AI 热点 C++ 化、意图链路、运行时边界、工具门禁、编辑器/UGC 后置。 |
| `planning/ai-technical-iteration-plan.md` | AI 技术执行计划。spatial、perception cache、TeamBlackboard、TacticalQuery、InfluenceMap、BT runtime、benchmark。 |
| `planning/ai-roadmap.md` | AI 长期愿景和能力地图，排期以总规划为准。 |
| `planning/high-priority-todo.md` | 执行记录 + 细粒度 backlog。不要当最高优先级入口。 |

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
| `archive/project-roadmap.md` | 历史非 AI 主线整理。UI/runtime/调试/构建条目可参考，排序已失效。 |
| `archive/foundation-modules-inventory.md` | 历史地基盘点。保留模块清单价值，数据驱动切片排期已失效。 |
| `archive/trigger-system-gap-analysis.md` | 触发器方向暂缓。事件系统四件套可在服务 AI/生产级边界时小步引入。 |
| `archive/SoldierObjectRefactor.md` | 历史重构记录。查旧设计和对象链路时参考。 |
| `archive/visual-editor-implementation-plan.md` / `archive/visual-editor-task-breakdown.md` | 编辑器/UGC 后置方向，当前不作为主线施工。 |

## 6. /hello-develop-design 工作流（skills/ + dev-design/）

| 路径 | 用途 |
|---|---|
| `skills/brainstorming.md` `writing-plans.md` `subagent-driven-development.md` `verify.md` `gitcommit.md` `doc-sync.md` | 六阶段开发工作流的节点 skill 方法论（命令在 `.claude/commands/hello-develop-design.md`） |
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
| `perf/ai-perf-release-baseline-20260612.md` | AI perf **Release x64** 基线；结论：帧时间被 VM 渲染(engineGap)主导，AI 主成本是 perceptionSystem。 |
| `perf/ai-perception-baseline-20260602.md` | AI perception pressure Debug x64 基线，100/500/1000 agent 对照。 |
| `perf/ai-spatial-filter-retest-20260602.md` | Spatial filter 复测，candidates/filtered/reject/queryMs。 |
| `perf/fgui-tracy-sample-*.md` | FGUI Tracy 实测报告。 |

## 9. 阅读路径

- 判断项目方向：`project-direction.md`。
- 安排后续阶段：`planning/long-term-iteration-plan.md`。
- 做某子系统：先看 `modules/<alias>.md`（约束 + 验证），再读对应 C++/Lua 实现。
- 做 AI 热点系统：`planning/ai-technical-iteration-plan.md`。
- 处理 Chapter9 视觉/行为差异：`design/chapter9-parity-architecture-notes.md`。
- 做架构解耦：`design/architecture-improvement-plan.md`。
- 新功能开发：`/hello-develop-design <功能描述>`（见 `skills/`）。
