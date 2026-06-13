# 计划写作节点 skill（writing-plans）

> 由 `.claude/commands/hello-develop-design.md` 阶段 3 引用。

## 用途

把已批准的 spec 转成可由子代理执行的 bite-sized task plan。
假设执行者是熟练开发者但对 HelloOgre3D 代码与领域陌生；plan 要给到他照做不卡的颗粒度。DRY、YAGNI、频繁 commit。

## 入口

阶段 3 由命令文件 Read 本文档；输入是 Gate 1 通过的 spec 文件路径。

## 文件结构先于任务列表

写 plan 第一件事：列出**改哪些文件、新建哪些文件、每个文件职责**，并标明落在 `runtime / sandbox / game / bin/res/scripts` 哪一层（遵循 `AGENTS.md` 依赖流）。

- 每个文件单一职责；一起改的放一起（按职责分，不按技术层分）
- HelloOgre3D 是大型 C++ 项目，沿用现有文件风格，不单方面 restructure
- 涉及暴露给 Lua 的 C++ API 改动：plan 必须显式列出 **tolua 处理**步骤——**优先手术式改 `SandboxToLua.cpp` 对应绑定函数 + 头文件**；避免 `tolua.bat` 全量重生成（已知会引入难定位回归，见 docs 历史教训）

## 任务颗粒度（2-5 分钟 / 步）

HelloOgre3D 大量任务是改 C++ / 改 Lua sample / 改配置，多数**没有 gtest**。把"步骤验证"换成 `docs/skills/verify.md` 定义的策略：**Release 编译 + 目标 sample smoke 不退化**。典型 task：

```
- 改/加 C++ 或 Lua            ← 一步
- 同步 tolua 绑定（如改了导出 API）← 一步
- 编 Release x64               ← 一步
- 跑目标 sample smoke 确认 PASS ← 一步（验证门）
- commit                      ← 一步
```

## Plan 文件 Header（强制）

```markdown
# [功能名] 实施计划

**Goal:** [一句话]
**Architecture:** [2-3 句，标明涉及的层]
**Spec:** [link 回 docs/dev-design/specs/...]
**状态:** 草案，待 Review

---
```

`状态` 字段：写完=`草案，待 Review`；Gate 2 通过=`已批准，执行中`；阶段 6 全部非 BLOCKED=`已完成`。

## Task 结构

````markdown
### Task N: [组件名]

**Files:**
- Create: `精确/路径/file.cpp`
- Modify: `精确/路径/existing.cpp:123-145`

**层归属:** sandbox/ai/... （用于 verify 选回归 sample）
**验证策略:** Release 编译 + Sandbox<X> smoke（或 parity_gate / visual_capture / fgui_production_gate / skip）

- [ ] **Step 1：<具体改动，给代码块>**
- [ ] **Step 2：编 Release** `MSBuild build\HelloOgre3D.sln /p:Configuration=Release /p:Platform=x64 /m`
- [ ] **Step 3：跑 smoke** `tools\run_sandbox_smoke.ps1 -Sample Sandbox<X> -StopExisting -NoTail` → 预期 `status=PASS`
- [ ] **Step 4：commit**（精确 git add + 中文 `[dev]` message）
````

## 禁止占位（plan failure）

出现即视为 plan 写崩，必须修：

- "TBD"、"TODO"、"implement later"、"后续补充"、"类似 Task N"（必须重复写全，子代理可能乱序读）
- "加适当错误处理"、"处理边界情况"（不给具体做法）
- 描述做什么但不给怎么做（要代码就给代码块）
- 引用未在任何 task 定义的类型 / 函数

## HelloOgre3D 专属约束（每 task 必须显式）

1. **层归属** + **回归 sample**（verify 据此选 smoke 目标）
2. **验证策略**：Release 编译 / sample smoke / parity_gate / visual_capture / fgui_production_gate / 手动 / skip
3. **commit message 中文**，沿用仓库 `[dev]<描述>` 风格
4. **`git add` 精确路径**，不 `git add -A`
5. 改了导出给 Lua 的 C++ API → 必须有 tolua 同步步骤（手术式优先）
6. 不动 `src/Engine`、`src/External` vendored 代码，除非任务明确要求

## 自审

对照 spec：① spec 每条需求能指向某 task 某步骤？② 占位扫描；③ 类型/命名一致；④ 每 task 都标了层归属 + 验证策略 + tolua 步骤（如需）？发现就地修。

## 执行交接

写完 plan 后**不直接执行**——回到命令文件触发 Gate 2。
