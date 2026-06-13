# 文档同步节点 skill（doc-sync）

> 由 `.claude/commands/hello-develop-design.md` 阶段 5 调用，触发 Gate 3。

## 用途

plan 全部 task 跑完后，把本次改动沉淀回项目文档，保持文档↔代码一致（避免出现我们审计过的"文档落后于代码"）。

## 调用契约

输入：阶段 4 累计的改动文件集 + 关键结论（性能数据 / 行为变化 / 新增系统 / 踩坑）。
输出：同步报告（更新了哪些 docs、是否写 MEMORY、是否落 commit）。
副作用：改 `docs/` 下相关文档；可能改根 `MEMORY` 索引（若有非显然、跨会话有用的事实）。

## Gate 3

```
[Gate 3] 本次改动涉及：<改动面摘要>
建议同步的文档：<候选 docs 列表（按下方映射推断）>
是否同步文档？
  [u] update：更新候选 docs 的相关章节（状态/已完成项/新增系统）
  [m] update + MEMORY：同时把跨会话有用的非显然事实写入记忆
  [s] skip：跳过；报告标"文档未同步"
```

即便 auto mode 也必须等待用户响应。

## 改动面 → 候选文档映射（据 docs/README.md 索引）

| 改动面 | 同步文档 |
|---|---|
| AI 感知/记忆/团队/战术/BT/DT/FSM | `docs/planning/ai-technical-iteration-plan.md`、`docs/planning/high-priority-todo.md`，必要时 `docs/planning/ai-roadmap.md` |
| 架构解耦 / 组件化 / 服务边界 | `docs/design/architecture-improvement-plan.md`（§7 跟踪清单）、`docs/design/cpp-object-model-refactor-roadmap.md` |
| 行为树节点/能力 | `docs/design/behavior-tree-gap-analysis.md` |
| 性能 | `docs/perf/` 下新建或更新基线报告 |
| Chapter9 表现/对齐 | `docs/design/chapter9-parity-architecture-notes.md` |
| FGUI | `docs/fgui/` 对应文档 |
| 方向/优先级变化 | `docs/project-direction.md`、`docs/planning/long-term-iteration-plan.md` |
| 新增/废弃文档或状态翻转 | `docs/README.md` 索引 |

## 模块知识库维护（registry / docs/modules）

doc-sync 先维护模块知识库（`/hello-develop-design` skill 的核心资产）：

1. 把本次改动文件反查 `docs/registry.json` 命中模块 M。
2. 对每个 m ∈ M：更新 `docs/modules/<m>.md` 的相关节（§4 能力 / §5 约束 / §7 验证 / §8 gap），按 `docs/modules/_template.md` 结构；同步 registry 该条的 `constraints_summary`（若约束变化）。
3. 阶段 0 记的"模块文档缺失"待办：按模板新建 `docs/modules/<alias>.md`。
4. 新增/废弃模块：增删 registry 条目；新系统则加 alias + source_paths + verify。

## 执行：u / m 模式

1. 先做上面的「模块知识库维护」。再对每个候选**专项文档**：Read → 在对应章节就地更新（把"待办"翻成"已完成/部分完成"并附证据 file:line、补新增系统说明、勾选跟踪清单）。**沿用我们做文档审计的粒度**：已解决/部分完成/仍成立分明，不要把半成品标成全完成。
2. 若改动产生了**跨会话有用、非显然、且代码本身不记录**的事实（如某工具的环境坑、某构建/运行约定）→ 按记忆协议写入 `MEMORY`（一文件一事实 + 索引一行）；代码已记录的（结构、修复、git 历史）不写。
3. `git add <精确路径>` + 中文 `[dev]` commit（经 `docs/skills/gitcommit.md`）。

## 执行：s 模式

不动文件；返回 SKIPPED；阶段 6 收尾报告标"文档未同步，建议后续补"。

## 失败处理

任一步失败：不回滚已写入变更；报告 FAILED 并附位置；阶段 6 标"doc-sync 部分失败"。
