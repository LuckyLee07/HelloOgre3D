# Agent 指令与工作流审计（2026-09-06）

## 依据与范围

本次在线检索并读取 OpenAI 官方文档，目标模型保持 GPT-6 Astra：

- [Using GPT-6 Astra](https://developers.openai.com/api/docs/guides/latest-model)：重点是 initiative、instruction following、delegation、testing。该页当前明确标识 gpt-6-astra；动态 URL 将来可能改变。
- [AGENTS.md discovery](https://developers.openai.com/codex/guides/agents-md)：根指令与子目录指令的加载规则、默认 32 KiB 项目文档预算。
- [Build skills](https://developers.openai.com/codex/skills)：渐进加载、明确 description、仓库 `.agents/skills` 自动发现。

以下改法是结合本仓库的工程判断，不是官方规定的固定行数、流程或性能保证。官方 best-practices 聚合页抓取返回 404，未把该页搜索摘要用作依据。

覆盖根 AGENTS.md、全部自有 docs/skills 节点与两个既有 SKILL.md、Claude 命令、spec/plan 目录约定和文档索引。核对 registry、绑定模块、记忆、Premake/Xcode、M1 smoke 与 FGUI AutoGen 工作流。仓库没有自有 .github CI workflow；vendored OIS 的 GitHub issue 模板不属于本次编辑范围。

Claude 两份 settings 仅包含 permissions，已核对配置结构；其中共享 settings 含历史 Windows 命令和外部仓库路径。它们不是 GPT 模型配置，本次保留既有权限，不通过扩大授权来优化执行。全局技能、宿主模型设置和其他任务未提交代码不在修改范围。

## 发现与处理

| 问题 | 处理 |
|---|---|
| 根文件重复目录表、历史路线、格式、验证和 FGUI 规则 | 保留不可丢的项目约束，历史/详细程序改为按需链接 |
| Claude 命令与 Codex 技能各自声明不同流程真源 | 新增 `docs/skills/workflow.md`；三个技能入口和命令只路由 |
| Codex 入口只放在 docs 下，不能依赖其自动发现 | 新增 `.agents/skills/hello-develop-design/SKILL.md`；旧 docs 分发位置保留兼容 |
| 新功能一律多方案、逐问题/逐章节审批，再过三道 Gate | 按意图区分局部实现、设计、完整实施和显式 gated 模式；只在最后一种保留 spec/plan 审批 |
| 每 task 固定子代理实现、两轮 review 和全量验证 | 允许独立复杂子问题按需委派；共享产物串行验证，不写死 Claude 模型或工具名 |
| 固定 2–5 分钟步骤、预写代码、每步 commit | 计划围绕职责、依赖、验收，按风险确定粒度 |
| spec/plan 自动 commit/amend，续跑可 restore/删草案 | 提交依据用户授权；续跑依据目标、diff 和证据，保留草案和用户工作 |
| 根文件要求 tolua 全量生成，记忆/模块/工作流禁止 | 根文件明确当前局部同步例外，并保留头文件、导出声明、绑定和 sample 的一致性要求 |
| 验证强制用户选择，Windows x64/机器 Python 路径写死 | 按实际改动自动选验证；Windows VS2017 与 macOS 原生流程分开；路径先探测 |
| Debug 绝对禁止与 FGUI Full gate 的 Debug/Release 冲突 | 默认 Release；专项 gate 按其脚本运行，调试需要不受绝对禁令限制 |
| doc-sync 即使 auto 也必须等待，并提及根 MEMORY | 必要文档同步纳入收尾，只使用 docs/memory 真源 |
| 把全部非 BLOCKED（包括 skip）当完成 | 必要验收未满足时保留待验证状态，区分 PASS/FAIL/NOT RUN |

FGUI AutoGen 文档的受管生成物、手写代码保护、dry-run/check 等是具体工具约束，保留原文；它没有 Astra 模型设置或通用多轮审批流程，不因文件名包含 workflow 而重写。

## 体积与验证

- AGENTS.md：253 → 59 行，17,657 → 6,551 字节，减少约 63%。
- 原有核心指令集合（AGENTS、Claude 命令/技能、docs/skills）加新共享流程：55,409 → 21,515 字节，减少约 61%；另新增一个约 1 KB 的仓库发现入口。这是文件 UTF-8 字节量，不是模型 token 实测。
- 三个 SKILL.md 均通过 skill-creator 的 quick_validate；相同入口正文保持一致。
- 修改文档中的 Markdown 本地链接全部存在；新写/重写文本使用 CRLF，局部改动的 docs/README.md 保持既有 LF。
- `git diff --check` 通过。未运行游戏构建/运行测试，因没有改动产品代码。

静态情景核对（不声称模型端到端实测）：

| 请求 | 期望路径 |
|---|---|
| “修复一个 Lua 条件” | 局部定位/修复 + 语法与目标 sample，不自动设计审批 |
| “只设计团队记忆接口” | 设计交付，不执行实现/提交 |
| “设计并实现一个跨模块功能” | 设计/计划后持续实施和验证，常规取舍不重复确认 |
| “按 gated 流程做” | 完整 spec/plan 各一次审批，已有批准可续用 |
| “审计 AGENTS.md” | 技能 description 排除误触发功能设计流程 |
| “macOS 改 C++” | 实际 Xcode Release + 相关原生 sample，不执行 Windows MSBuild |
| “改 Lua 导出” | 局部同步声明与绑定 + 构建/运行，不例行全量生成 |
| “提交这些改动” | 检查精确范围和已有 stage，按本地身份/单行中文规范提交 |

尚未执行独立模型 A/B 或新会话发现实测，因此不能宣称任务耗时、正确率或费用已经改善。后续可在隔离 checkout 上对同一代表任务比较完成率、非必要澄清次数、重复验证次数、总耗时与实际 token；质量和必需回归证据优先于更短输出。新会话才能可靠验证新的 AGENTS 指令链加载；技能变更通常自动发现，未出现时重启会话。
