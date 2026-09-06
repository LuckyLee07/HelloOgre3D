# AGENTS.md

## 项目与真源

HelloOgre3D 是长期维护的个人游戏 AI 与玩法实验项目，核心是把 AI 能力讲清楚、跑起来、看得见、能实验。生产级质量约束具体模块；外部玩家、发行、固定日历交付、完整引擎或 UGC 平台不是完成前提。

- 开工先读 [项目记忆索引](docs/memory/MEMORY.md)，只加载与任务相关的条目。非显然、跨会话有用、代码未记录的经验写入 `docs/memory/`，一文件一事实并更新索引；工具私有项目记忆只保留指针。
- 方向与优先级见 [project-direction](docs/project-direction.md)、[当前周期](docs/cycle-01.md)、[backlog](docs/backlog.md)。README 开发日记和 `docs/archive/` 仅作历史参考，不据此扩展当前任务。
- 模块定位用 [registry](docs/registry.json) 和 [文档索引](docs/README.md)，按需读命中模块与直接相关资料，再核对源码；不全量加载 related_docs。

## 协作与执行

- 中文沟通，先说结果、证据和必要限制。任务明确就执行到实现、相关验证与必要文档同步完成；常规实现选择自行判断，仅在影响结果的关键歧义或缺少授权时询问。
- 用户当前指令和已有授权优先于 skill 的流程默认值。只要求设计/审计时遵守该范围；明确要求实现时不因模板中的阶段名重复索取批准。
- 小修直接处理。跨模块功能、设计或显式 `hello-develop-design` 使用 [共享工作流](docs/skills/workflow.md)，按任务复杂度选节点。
- 先检查工作区改动，保留用户工作；默认沿用当前 checkout，不为流程仪式切分支。需要隔离实验时可使用独立目录/worktree，遵守当前环境权限。
- 搜索优先 `rg`；独立只读检查可批量并行。工具与模型使用当前环境实际提供的能力，不把文档里的工具名当作可调用保证。

## 架构与工程约束

- 保持现有 sample 可运行并符合章节目标，尤其 `Sandbox6/7/8` 的 FSM/DT/BT 回归面。优先局部、可验证、可回退的改动。
- `runtime` 收口 Ogre/OIS/FGUI/Tracy 等适配；`sandbox` 放对象、AI、物理、脚本桥和可复用玩法设施；`game/client` 负责应用编排；Lua 负责 sample、行为配置和 UI 业务。不要向 `common` 增加 Ogre 专属 helper 或扩大单体 manager。
- AI 热点放 C++，复用现有 driver、Blackboard 和事件设施。行为异常先追 Lua → AI driver → runtime/输入/debug 链路，按证据定位。
- 有有效 Bullet 刚体时，PhysicsComponent/刚体是位置与朝向真源；否则 RenderComponent transform 是真源，不造假刚体。同步由 `RenderComponent::Update` 负责，对象层只触发；物理对象视觉偏移走 `SetVisualOffset(...)`。
- 修改平台逻辑同时检查 Windows/macOS 条件编译。工程配置真源是 `premake/premake.lua`，路径按实际大小写（`src/external`）处理。vendored 引擎/第三方、资源只在任务确有必要时局部修改并说明原因。
- 保持目标文件风格、编码和换行，不顺手格式化。C/C++ 用 Tab；新增文本默认 CRLF；含中文的 C/C++ 编码见 [编码经验](docs/memory/crlf-encoding-gotcha.md)。使用现有日志系统。

## Lua / FGUI 高风险边界

- 改 Lua 导出接口同步检查头文件、`.pkg`/其引用的导出声明、绑定 cpp、Lua 调用点和受影响 sample；明确 C++/Lua 所有权、GC、callback/ref 清理和 table/self 调用约定。
- **当前 tolua 例外**：已有全量生成导致 Sandbox18 崩溃的记录，默认局部同步绑定，禁止把 `tolua.bat` 当作例行步骤。详情见 [绑定模块](docs/modules/scripting-tolua.md)。修复生成链本身应作为明确任务，在隔离输出中比较并完成相关运行回归后再替换。
- FGUI C++ 管适配、输入、handle 和事件桥；Lua 管生命周期、业务和资源策略，不长期持有底层 FairyGUI/Cocos 裸指针。复杂 UI 用 Ctrl/Model/View/AutoGen，简单调试 UI 可轻量化。
- FGUI 接入与生成查 [AutoGen 工作流](docs/fgui/fairygui-autogen-workflow.md)，阶段验收查 [生产 gate](docs/fgui/fairygui-production-gate.md)。

## 验证与完成

按 [验证工作流](docs/skills/verify.md) 选择最小充分验证面，已有相关检查通过后，仅因新改动、失败或未解决风险扩大/重跑。

| 改动 | 必要证据 |
|---|---|
| 文档/skill | 内容与引用核对、相关格式/元数据检查、`git diff --check`；不构建游戏 |
| Lua/gameplay | Lua 5.1 语法检查 + 受影响 sample 运行证据 |
| C++/构建 | 当前平台构建；运行时默认 Release + 相关 sample。新增源文件/工程配置变更先生成工程；ABI 布局变更 clean rebuild |
| Lua 绑定 | 上述接口同步检查 + Release 构建 + 消费该绑定的 sample |
| 相机/输入/渲染/导航 | 对应真实窗口或截图/交互证据；导航同时看路径与 debug 绘制 |
| FGUI | 对应自测；复杂生命周期改动跑 All，阶段收口跑生产 gate |
| 性能 | 保留 Tracy 埋点，记录可比基线与日志 |

- 自己运行并读取 stderr、`bin/Sandbox.log`（Debug 为 `Sandbox_d.log`）及 smoke 日志定位。静态通过、编译通过、运行通过、视觉通过分开报告。
- 缺平台/图形会话时完成可执行检查，明确未验证项与原因，不把环境失败、超时或未执行标为 PASS。
- 构建入口：Windows `vs2017.bat`（会清理部分产物，先检查脚本）或直接 Premake + VS2017 MSBuild；macOS `bash xcode.sh` + 生成的 Xcode target。具体命令见验证工作流。

## Git

- 不回退/覆盖无关改动；不执行 `git reset --hard`、`git checkout --` 等破坏性操作，除非用户明确要求。
- 提交需用户授权，流程不会自动 commit/amend。只暂存本次精确路径/改动块，检查已有 staged 内容。默认仅本地提交，push 需明确要求。
- 提交说明仅一行 `[前缀]<中文一句话>`，不写 body 或模型署名（含 `Co-Authored-By`）。前缀：`[feat]` 新能力、`[fix]` 缺陷、`[refactor]` 结构清理、`[docs]` 文档/注释、`[dev]` 无法拆开的混杂改动兜底。
- 用仓库本地 git 身份，不用 `-c user.name=...` 临时覆盖，不把身份写进文档。`git config --local user.name` 为空时先问用户再配置。
- 根因、设计取舍与验证证据写相关 `docs/`；git log 只记录这次改了什么。
