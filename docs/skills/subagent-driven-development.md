# 子代理派发节点 skill（subagent-driven-development）

> 由 `.claude/commands/hello-develop-design.md` 阶段 4 引用。

## 用途

按 plan 的 task 列表逐任务派发独立子代理执行；每任务完成后两阶段 review（spec 符合性 → 代码质量），再跑 `docs/skills/verify.md` 验证门。

**为什么用子代理**：把任务委托给独立上下文的子代理，精确控制其指令与上下文，不让主会话历史污染它，同时保留主会话做编排。

**连续执行**：task 之间**不**回头问"继续吗"；用户跑 `/hello-develop-design` 就是托付执行。只在以下情况停：BLOCKED 无法自解 / 根本性歧义阻断 / 全部完成。

## 入口

阶段 4 由命令文件 Read 本文档；输入是 Gate 2 通过的 plan 文件路径。

## 总流程

```
[Read plan 一次，抽出所有 task 全文 + 上下文，创建 TodoWrite]
         ↓
对每个 task：
  [派发 implementer 子代理] → [实现→编译→自审] → [spec reviewer] →
  [若差异→修→再 review] → [code quality reviewer] → [若问题→修] →
  [调用 docs/skills/verify.md 验证门] → [TodoWrite 标 complete]
         ↓
[全部完成 → 派发 final reviewer 跑整体 review] → [回命令文件进阶段 5]
```

## 模型选择

- **机械实现**（单文件 / 接口明确）→ haiku
- **集成 / 判断**（多文件协调）→ sonnet（HelloOgre3D 多数 C++/Lua 改动默认 sonnet）
- **架构 / review** → opus

## Implementer 子代理简报模板（中文）

```
---
任务编号：<plan §x>
目标：<plan 中该任务目标>
上下文：spec=<path> plan=<path> 关联文档=<阶段0按README索引读到的docs>
期望产出：改动文件=<列表>  验证方式=<plan 声明的策略>
约束：
  - 提交信息中文，沿用仓库 [dev]<描述> 风格；git add 精确路径
  - 遵循 AGENTS.md 依赖流与位置真源规则
  - 改了导出给 Lua 的 C++ API → 手术式同步 SandboxToLua.cpp 绑定，勿全量 tolua.bat 重生成
  - 不动 src/Engine、src/External 除非任务明确要求
报告：单消息回报改了哪些文件 / 是否 commit / 卡点 / 状态（DONE / DONE_WITH_CONCERNS / NEEDS_CONTEXT / BLOCKED）
---
```

## Implementer 状态处理

| 状态 | 处理 |
|---|---|
| `DONE` | 进入 spec reviewer |
| `DONE_WITH_CONCERNS` | correctness/scope 类必须处理；observation 记下即可 |
| `NEEDS_CONTEXT` | 补缺失上下文，re-dispatch |
| `BLOCKED` | 评估：上下文不足→补+重派；推理不够→升模型；任务太大→拆；plan 有错→上抛人类。**绝不**不变更地强制重试 |

## Reviewer 简报

**Spec reviewer**：
```
任务 <x> 已提交；对照 spec 与 plan 检查：① 是否完整实现本 task 涉及需求 ② 是否多做 spec 外功能 ③ plan 所有 step 是否真执行。回报：✅符合 / ❌<问题列表>
```

**Code quality reviewer**：
```
任务 <x> spec 符合性已过；审代码质量：① 与 HelloOgre3D 既有风格一致 ② 错误处理合理（不过度防御/不缺关键校验）③ 命名/结构/注释 ④ 性能/内存红线 ⑤ 是否违反 AGENTS.md 依赖流/位置真源。回报：✅approved / ❌<按严重度分组>
```

## 并行 vs 串行

- 改动文件无交集 + 互无逻辑依赖 → 并行（默认上限 3）
- 否则串行
- **绝不**并行派发 implementer + reviewer；reviewer 必须等 implementer 完成
- **绝不**并行派发改有冲突文件的多个 implementer

## 红旗（绝不）

- 在 master 上直接派发实施（先确认在功能分支；HelloOgre3D 默认分支 master，结构性改动应先建分支）
- 跳过任一 review（spec 或 quality）
- 未修问题就进下一 task
- 让子代理自己 Read plan（必须直接给全文）
- 缺场景化上下文（子代理要懂任务在大图里的位置）
- 接受"差不多"（reviewer 找出问题 = 未完成）
- 跳过 `docs/skills/verify.md` 验证门 = 跳过 task

## tolua 教训（强约束）

涉及 Lua 绑定的 task：implementer 必须**手术式**改 `SandboxToLua.cpp` 对应绑定函数 + 头文件签名，**禁止** `src/HelloOgre3D/tolua.bat` 全量重生成（已知引入难定位运行时回归）。verify 时务必跑用到该绑定的 sample（如影响图改动跑 Sandbox17 **和** Sandbox18）。
