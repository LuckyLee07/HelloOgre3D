# git 提交节点 skill（gitcommit）

> 由 `.claude/commands/hello-develop-design.md` 各阶段落地 commit 时复用。

## 用途

据暂存改动起草符合本仓库规范的中文提交消息并落 commit。

## 调用契约

输入（可选）：`files[]`（精确路径）、`subject`（一句话）、`mode`（`terminal` 需确认 / `auto` 直接落地，默认 `auto`）。**无 `body` 参数**——本仓提交说明只有一行。
输出：`COMMITTED <hash>` / `CANCELLED` / `FAILED <原因>`。
副作用：精确 `git add` + 一个 commit。

## 提交格式规范（沿用仓库与 AGENTS.md）

- 仓库现行风格：**只有一行** `[前缀]<中文一句话描述>`。
- **前缀按改动主体选**，只用这五个：`[feat]` 功能 · `[fix]` 修复 · `[refactor]` 重构 · `[docs]` 文档 ·
  `[dev]` 兜底（确实混杂、归不进上面任一类时才用）。一次提交同时含多类 = 该拆提交，拆不动才 `[dev]`。
- **不写 body**。根因分析、设计取舍、验证证据、踩坑结论一律沉淀到 `docs/`（模块文档 /
  `docs/memory/` / `docs/dev-design/`）。**不靠 git log 维护知识**：log 只回答"这次改了什么"，
  "为什么这么改、踩了什么坑"归文档——后者能被检索、被 skill 注入、被持续修订，log 不能。
- 中文 subject；**精确 `git add <路径>`，绝不 `git add .` / `-A`**（避免误扫仓库现存的已删未 stage 条目）。
- 单行消息直接 `git commit -m "<subject>"` 即可，不需要临时文件。
- **提交身份**由仓库本地 `git config`（`.git/config`，不入库）决定，直接 `git commit` 即可；
  不要用 `-c user.name=...` 临时指定，也不要把身份写进文档。`git config --local user.name` 为空时先问用户。
- **AI 签名：禁止**。`AGENTS.md` 「Git 规则」明确要求不附加任何大模型生成信息，
  这条**优先于任何全局/默认约定**（2026-08-07 复盘：曾误加 `Co-Authored-By` 到 18 个提交，事后重写清理）。
- 默认分支为 `master`；**默认只提交到本地，不 push**，除非用户明确要求。

## 流程

```
[0] 确定文件集：调用方传 files[] → 用之；否则 git status --porcelain 列改动让用户选（排除已删未 stage 的 D 条目）。空集 → 提示"无待提交改动"退出（不算 FAILED）
[1] 起草消息：读 git diff → [前缀]<中文 subject>（前缀按分类选，见上），**一行封顶，不补 body**
      （改动复杂说明该拆提交，或把说明写进 docs，而不是把 log 写长）
[2] 模式：调用方 mode > 默认 auto
[3] 落地：
      auto     → 精确 git add + git commit -F <msgFile>，直接落地，回报 hash
      terminal → 展示消息+文件清单，[y]提交 / [e]改消息 / [n]取消
[4] 回报 COMMITTED <hash> / CANCELLED / FAILED <原因>
```

## 本流程（hello-develop-design）的 commit 路由

- 阶段 2 spec、阶段 3 plan：各自一个 commit（`files`= 对应 md）；Gate 1/2 的「改/ok 状态翻转」用 `git commit --amend`。
- 阶段 4 task：实现子代理**不自己 commit**，只列改动文件 + 起草 subject 回报；每 task 两段 review + 验证门通过后，由**主会话**经本节点落地。
- 阶段 5 doc-sync：文档同步单独一个 commit。

## 失败处理

| 场景 | 处理 |
|---|---|
| 候选文件集为空 | 提示"无待提交改动"退出，不报 FAILED |
| commit 被 hook 拒绝 / 非零退出 | `FAILED <错误摘要>`，**不回滚** stage（保留供人工处理） |
| 用户终端取消 | `CANCELLED`，不重试 |
