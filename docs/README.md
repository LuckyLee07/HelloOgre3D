# docs 索引（按状态分类）

> 这份索引只解决一件事：**施工/阅读前先按状态找对文档**。方向与优先级看 🧭 北极星；按 🔨 active 文档施工；📚 reference 只作依据不直接施工；🗄 historical 的“优先级/排期”已被北极星取代，只看其具体条目、不看其排序。
>
> 一句话治理规则：**任何取舍先翻北极星 `project-direction.md`；它与其它文档冲突时，以北极星为准。**

## 🧭 北极星（最高真源）

| 文档 | 作用 |
|---|---|
| `project-direction.md` | 项目身份(数据驱动沙盒运行时+编辑器)、核心主轴(内容即数据)、L0–L4 模块优先级、第一个垂直切片、先做/缓做/别做。**统领以下所有文档。** |

## 🔨 Active 规划（按这些施工）

| 文档 | 作用 | 对应北极星层 |
|---|---|---|
| `high-priority-todo.md` | 当前高优先级施工 TODO（L0 地基 + 第一个垂直切片 + 必要 BT 补强） | L0/L1 执行 |
| `foundation-modules-inventory.md` | 地基模块盘点(已有/缺/不做)+ 切片最小 base 集 | L0–L2 家底 |
| `architecture-improvement-plan.md` | 架构优化实施(解耦 SandboxServices / 组件化 / 所有权 / 感知)，带 file:line 证据与验收 | L0/L1/L2 落地 |
| `cpp-object-model-refactor-roadmap.md` | C++ 对象模型分阶段路线(术语已同步当前继承链) | L1-④ 依据 |
| `behavior-tree-gap-analysis.md` | BT 执行模型差异与补强(G1 中断 / G3 参数求值 / G4 Parallel·Random …) | L1-⑤ |
| `trigger-system-gap-analysis.md` | 触发器/事件系统差异与补强(T-G1 事件总线四件套 / T-G2 参数化事件名 …) | L0-③ / L1-⑥ |
| `visual-editor-implementation-plan.md` | 可视化编辑器实施方案(自写 FGUI 积木编辑器) | L3 |
| `visual-editor-task-breakdown.md` | 可视化编辑器开发任务清单(带真实接口签名) | L3 |

## 📚 Reference（参考依据，不直接施工）

| 文档 | 作用 |
|---|---|
| `reference-minigame-patterns.md` | 生产级沙盒 MiniGame 可借鉴架构调研(已纠正反射/Blockly/dev-VM 表述)；“为什么这么做”的依据 |

## 🗄 Historical / 补充（具体条目可参考，排序以北极星为准）

| 文档 | 作用 | 状态 |
|---|---|---|
| `project-roadmap.md` | 早期非 AI 主线路线(UI/runtime/调试/构建/稳定性) | 历史；FGUI-first 排期已被北极星取代 |
| `ai-roadmap.md` | AI 长期愿景(Knowledge/Perception/Tactics 等) | 补充；近期 AI 工作以 `behavior-tree-gap-analysis.md` 为准 |
| `AIArchitectureBeyondBook.md` | AI 架构延伸思考 | 补充 |
| `SoldierObjectRefactor.md` | Soldier/Agent 对象链路重构记录 | 历史/记录 |

## 🎨 FGUI 专项（编辑器 UI 基座 + 回归门禁，不占主线优先级）

| 入口 | 作用 |
|---|---|
| `fgui/fairygui-final-roadmap.md`、`fgui/fairygui-business-framework-todo.md` | FGUI 业务框架路线与 TODO（已基本完成） |
| `fgui/fairygui-production-gate.md` | FGUI 阶段性生产验收(配 `tools/run_fgui_production_gate.ps1`) |
| `fgui/`(其余) | AutoGen 工作流、资源、标准流程等专项 |

> 说明：FGUI 已接近生产级，定位转为“可视化编辑器的 UI 基座 + 回归门禁”，不再是项目主线 P0。

## 📊 Evidence（实测证据）

| 入口 | 作用 |
|---|---|
| `perf/fgui-tracy-sample-*.md` | FGUI Tracy 性能实测报告（visible pressure 等） |

---

## 阅读路径建议

- **想知道项目往哪走 / 该先做什么** → `project-direction.md`（北极星）。
- **想知道还缺哪些地基、下一步碰什么** → `foundation-modules-inventory.md`。
- **要动某个系统**：架构/解耦 → `architecture-improvement-plan.md`；BT → `behavior-tree-gap-analysis.md`；触发器/事件 → `trigger-system-gap-analysis.md`；编辑器 → `visual-editor-*`。
- **想了解参考来源(MiniGame 怎么做的)** → `reference-minigame-patterns.md`。
- 顶层项目说明与命令见仓库根 `README.md` 与 `AGENTS.md`。
