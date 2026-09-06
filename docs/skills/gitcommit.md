# 本地提交

仅在用户要求提交或本次上下文已有明确提交授权时使用；遵循根 `AGENTS.md` Git 规范。调用工作流本身、批准设计或实施不自动授权 commit/amend/push。

1. 检查 `git status --short`、目标 diff、已有 staged 内容与 `git config --local user.name`。身份为空先询问再配置。
2. 根据本次任务选择精确文件/改动块，保留用户的无关 staged/unstaged 工作。若同一文件含无法区分归属的改动，先澄清归属。
3. 暂存后检查实际 staged diff、必要验证证据及单行中文 `[前缀]subject`；防止提交夹带原有暂存内容。按有意义的变更拆分，不强制每 task 提交。
4. 本地提交并报告 hash。失败保留状态并诊断，不自动清空 stage、绕过 hook、amend 或 push。

提交格式和五类前缀只在 `AGENTS.md` 维护，不在节点重复定义。已有明确授权且范围清楚时不再请求确认。
