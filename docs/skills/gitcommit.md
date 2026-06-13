# git 提交节点 skill（gitcommit）

> 由 `.claude/commands/hello-develop-design.md` 各阶段落地 commit 时复用。

## 用途

据暂存改动起草符合本仓库规范的中文提交消息并落 commit。

## 调用契约

输入（可选）：`files[]`（精确路径）、`subject`（一句话）、`body`、`mode`（`terminal` 需确认 / `auto` 直接落地，默认 `auto`）。
输出：`COMMITTED <hash>` / `CANCELLED` / `FAILED <原因>`。
副作用：精确 `git add` + 一个 commit。

## 提交格式规范（沿用仓库与 AGENTS.md）

- 仓库现行风格：首行 `[dev]<中文一句话描述>`（见 `git log`，如 `[dev]新增 Chapter9 单角色移动观感对比探针`）。
- 中文 subject；**精确 `git add <路径>`，绝不 `git add .` / `-A`**（避免误扫仓库现存的已删未 stage 条目）。
- 多行消息走临时文件 `git commit -F <msgFile>`（UTF-8 无 BOM 写入），不用 `-m` 拼多行。
- 默认分支为 `master`；结构性改动应先在功能分支上提交（与 brainstorm/plan 阶段确认）。
- AI 签名：按全局 commit 约定处理（本仓库历史 commit 未带签名；若全局规则要求 `Co-Authored-By`，附在 body 末尾）。

## 流程

```
[0] 确定文件集：调用方传 files[] → 用之；否则 git status --porcelain 列改动让用户选（排除已删未 stage 的 D 条目）。空集 → 提示"无待提交改动"退出（不算 FAILED）
[1] 起草消息：读 git diff → [dev]<中文 subject>；多步改动补 body
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
