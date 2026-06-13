# 设计探索节点 skill（brainstorming）

> 由 `.claude/commands/hello-develop-design.md` 阶段 1 引用。

## 用途

把模糊的功能想法变成可执行的设计与 spec，通过中文对话逐步明确。
入口：用户跑 `/hello-develop-design <功能描述>`，命令进入阶段 1 时 Read 本文档并严格执行。

<HARD-GATE>
本节点完成前严禁写任何代码、调任何实现 skill、新建任何源文件。
唯一允许的写文件操作是 `docs/dev-design/specs/<日期>-<topic>-design.md`。
</HARD-GATE>

## 反模式："这太简单不需要设计"

每个新功能都走本流程。"简单"的功能往往是隐藏假设最多的地方。
设计可以短（几句话也行），但**必须**呈现并获得用户确认。

## 检查清单（按顺序）

1. **探索项目上下文** — 阶段 0 已按 `docs/README.md` 索引读了相关文档；此处补充看 git log、相关源码 / sample
2. **逐个提出澄清问题** — 单次单问题，多选优先（用 AskUserQuestion 工具）
3. **提出 2-3 候选方案** — 含 tradeoff 与推荐，第一个写推荐方案
4. **分节呈现设计** — 每节根据复杂度伸缩；每节后问"OK 吗？"
5. **写 spec** — 保存到 `docs/dev-design/specs/<YYYY-MM-DD>-<topic>-design.md`
6. **Spec 自审** — placeholder / 一致性 / 范围 / 模糊性四项扫描，发现就地修
7. **请用户 review spec** — Gate 1，等待用户响应
8. **过渡到阶段 2** — 通过 Gate 1 后把控制权交回命令文件

## 核心原则

- **单次单问题**：不在一条消息里塞多个问题
- **多选优先**：开放问题改成选项题（AskUserQuestion），用户更容易回答
- **YAGNI**：不必要的功能从设计里删
- **多方案对比**：永远提 2-3 个方案再定一个
- **逐节确认**：设计大节呈现一节确认一节，不要打包推送
- **可回退**：用户对某节有疑义，回去补问而不是硬推
- **对齐北极星**：方案必须符合 `docs/project-direction.md`；脱离当前 AI 验证面 / 一次性铺开 Def/触发器/UGC/streaming 的，提示暂缓

## 红旗（出现以下念头立即停手）

| 念头 | 应对 |
|---|---|
| "这就是简单问题，不用走流程" | 走流程，简单功能写简短设计 |
| "我先实现一版让用户看" | 严禁；设计未确认不可写代码 |
| "把所有问题一次问完" | 改成单问题，多选 |
| "用户应该自己能想出方案" | AI 必须主动提 2-3 方案 |

## 设计分节必含

- 目的与背景（含非目标）
- 设计原则
- 架构与组件（标明落在 runtime / sandbox / game / Lua 哪一层，遵循 `AGENTS.md` 依赖流）
- 数据流（如适用）
- 错误处理与失败语义
- 与现有系统 / sample / 规则的关系（哪些 sample 会被影响、是回归面）
- 已知局限

## spec 文件写作

文件路径：`docs/dev-design/specs/<YYYY-MM-DD>-<topic-kebab>-design.md`

Header 必含：

```markdown
- **日期**：YYYY-MM-DD
- **作者**：<git config user.name>
- **状态**：草案，待 Review
```

`状态` 字段是阶段 0 续跑探测的唯一依据，**严禁省略**。通过 Gate 1 后命令文件会把它改为 `已批准`。

## 自审清单

写完 spec 后用新鲜眼睛过一遍：

1. **Placeholder 扫描**：有无 "TBD"、"TODO"、模糊段落 → 就地补
2. **内部一致性**：架构与功能描述是否互相矛盾 → 修
3. **范围检查**：单 plan 是否吃得下；吃不下要提示拆分子项目
4. **模糊性检查**：任何能被两种方式理解的需求 → 选一个写死

## 不做的事

- 不开启 git worktree（HelloOgre3D 直接在主目录改）
- 不强推 TDD（C++/Lua 按 plan 自定验证策略，多数走"编译 + smoke 回归"，见 `docs/skills/verify.md`）
- 不提前为"将来可能做编辑器/UGC"污染当前设计（`docs/project-direction.md` §7 工作原则）
