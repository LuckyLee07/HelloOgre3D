# AGENTS.md

## 项目定位

`HelloOgre3D` 不是单纯的 Ogre 示例工程，而是一个持续演进中的 C++ / Lua 游戏沙盒项目。

根目标来自 `README.md`：
- 通过重构游戏案例，把日常开发中的经验沉淀下来。
- 在保留可运行 sample 的前提下，逐步把框架、玩法、脚本、引擎适配分层理顺。
- 让项目既能承载教学式 chapter 示例，也能作为后续 AI、角色控制、工具化能力的实验场。

默认工作原则：
- 保持已有 sample 行为稳定。
- 优先做局部、可验证、可回退的小改动。
- 尽量把“引擎耦合逻辑”和“玩法规则逻辑”继续拆开。
- 结构性改动应服务于解耦、缩短继承链、便于后续 AI / UI / 工具 / 调试扩展。

## 快速命令

常用命令以仓库根目录为工作目录执行。

| 场景 | 命令 | 说明 |
|---|---|---|
| 生成 VS2017 工程 | `vs2017.bat` | 默认设置 `ENABLE_FGUI=1`，并清理 `build`、`lib` 和 `bin` 下部分运行产物。 |
| VS2017 构建 HelloOgre3D | `"C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe" build\HelloOgre3D.sln /t:HelloOgre3D /p:Configuration=Debug /p:Platform=x64 /m` | 当前 Windows 构建验证默认使用 VS2017 MSBuild。 |
| 直接调用 premake | `tools\premake\premake5 --os=windows --file=premake/premake.lua vs2017` | 需要自定义参数时使用，例如 `--with-tracy`、`--with-fairygui`。 |
| 生成 Xcode 工程 | `xcode.sh` | 依赖本机已安装 `premake5`，输出到 `build/`。 |
| 重新生成 Lua 绑定 | `src\HelloOgre3D\tolua.bat` | 修改 `.pkg` 或暴露给 Lua 的 C++ API 后执行。 |
| FGUI 集中自测 | `cd bin; $env:HELLO_FGUI_SELF_TEST_ALL="1"; .\HelloOgre3D.exe` | 用于 FGUI 打开、关闭、输入、生命周期、资源清理回归。 |
| FGUI 长循环自测 | `cd bin; $env:HELLO_FGUI_LONG_LOOP_SELF_TEST="1"; .\HelloOgre3D.exe` | 用于重复打开关闭 UI，观察资源和生命周期问题。 |

构建配置：
- Premake solution：`HelloOgre3D`
- Configurations：`Debug`、`Release`
- Platforms：`x86`、`x64`
- 可执行文件输出：`bin/`
- 静态库输出：`libs/`
- 工程与中间目录：`build/`
- 运行日志：`bin/Sandbox.log`

## 当前技术栈

- 渲染与场景：Ogre 1.10
- 输入：OIS
- 物理：Bullet
- UI：Gorilla + FairyGUI 接入中
- 脚本：Lua 5.1 + tolua++ + LuaSocket + LuaPanda
- 寻路：Recast + Detour
- 转向行为：OpenSteer
- 性能分析：Tracy 接入中
- 构建系统：Premake
- 工程生成：Windows 下 Visual Studio，macOS 下 Xcode

## 架构依赖流

当前项目的依赖方向应尽量保持自下而上：

```text
src/Engine + src/External
  -> src/HelloOgre3D/runtime
    -> src/HelloOgre3D/sandbox
      -> src/HelloOgre3D/game + src/HelloOgre3D/client
        -> bin/res/scripts
          -> bin/res/scripts/samples
```

约束：
- `runtime` 承接 Ogre / OIS / Gorilla / FairyGUI / Tracy 等引擎或中间件适配。
- `sandbox` 承接对象系统、AI、物理、脚本桥接、服务与 sample 所需的可复用玩法基础设施。
- `game` 和 `client` 做应用编排、窗口、主循环、manager 汇聚，不继续堆具体玩法规则。
- Lua 脚本负责 sample 编排、AI 行为配置、UI 业务组织和运行时扩展。

## 目录职责

| 目录 | 职责 | 注意事项 |
|---|---|---|
| `src/HelloOgre3D/client` | 应用入口与启动封装。 | 不放具体玩法规则。 |
| `src/HelloOgre3D/game` | 应用编排层、`ClientManager`、`GameManager`、debug 绘制入口。 | 避免继续沉积 Ogre helper 和玩法细节。 |
| `src/HelloOgre3D/runtime` | 引擎 / 中间件适配层，如 profiling、FairyGUI、未来 Ogre runtime helper。 | 引擎耦合逻辑优先在这里收口。 |
| `src/HelloOgre3D/sandbox` | 对象系统、AI、物理、脚本绑定、服务、样例实现基础设施。 | 可放玩法基础能力，但要避免变成单体 manager。 |
| `src/HelloOgre3D/common` | 跨层通用工具和 Lua VM 等历史公共代码。 | 不要再塞 Ogre 专属 helper，除非确实与引擎实现无关。 |
| `bin/res/scripts` | 运行时 Lua 脚本。 | 修改 C++ 行为时要同步检查脚本调用点。 |
| `bin/res/scripts/samples` | sample 入口，是行为回归的重要验证面。 | 影响 sample 表现的改动必须手动或脚本验证。 |
| `bin/res/scripts/ai` | AI 决策树、行为树、条件、action 配置。 | 后续 AI 方向参考 `docs/ai-roadmap.md`。 |
| `bin/res/scripts/ui` | FairyGUI Lua 侧 UI 框架和业务 UI。 | 复杂 UI 默认走 MVC / AutoGen 约定。 |
| `media` | 材质、shader、模型、贴图、粒子等资源。 | 只在任务直接相关时修改。 |
| `premake` | 工程生成和第三方依赖配置事实来源。 | 新增 C++ 文件或库配置优先检查这里。 |
| `src/Engine`、`src/External` | vendored 引擎与第三方代码。 | 除非任务明确要求，否则不要随意修改。 |
| `docs` | 迭代路线、FGUI、AI、资源与工具文档。 | 结构性决策应同步记录到对应文档。 |

## 项目演进脉络

根据 `README.md`，仓库当前已经完成或部分完成：

- Chapter 1：Ogre 基础框架、Lua / tolua、Bullet、UI、输入。
- Chapter 2：智能体移动、steer、路径与对象控制链路。
- Chapter 3：动画状态机与有限状态机，角色状态支持 C++ 与 Lua 双侧扩展。
- Lua 绑定扩展：新增 `LuaPluginMgr`，支持 C++ 对象与 Lua 文件绑定。
- 当前重构方向：处理 Object 体系长继承链、减少冗余、拆分引擎耦合逻辑与业务逻辑。
- AI 后续方向：以 `docs/ai-roadmap.md` 为事实来源。
- 非 AI 后续方向：以 `docs/project-roadmap.md` 为事实来源。
- FGUI 后续方向：优先参考 `docs/fairygui-final-roadmap.md`、`docs/fairygui-business-framework-todo.md`、`docs/fairygui-autogen-workflow.md`。

## 中长期规划

`README.md`、`docs/ai-roadmap.md` 与 `docs/project-roadmap.md` 已记录的兼容目标包括：

- 决策树、行为树、黑板、知识源、感知、通信、影响力地图、战术层。
- 触发器、技能 timeline、团队 Blackboard、AI 调试面板、AI 更新调度。
- Jobs 多线程、崩溃上报模块、性能分析模块、imgui / FGUI 调试面板。

设计新增系统时默认考虑：
- 是否能挂接到 AI、技能、调试、性能分析链路。
- 是否方便被 Lua 配置和 sample 验证。
- 是否能保持系统可组合、可观测、可脚本扩展。

## 编码约定

| 类别 | 约定 |
|---|---|
| 格式 | 保持目标文件既有风格，不顺手格式化无关代码。 |
| 缩进 | C/C++ 文件缩进使用 Tab，不把既有 Tab 转为空格。 |
| 换行 | 项目文本文件默认使用 CRLF；Windows 下 C/C++ 源文件、头文件必须保持 CRLF。新增或生成 `.md`、`.ps1`、`.py`、`.lua`、`.json`、`.bat` 等文本文件也默认按 CRLF 落盘。不要无意转换整文件换行。 |
| 命名 | 优先沿用周边代码风格，不为统一命名做无关重命名。 |
| 日志 | 使用项目既有日志路径和日志系统，不在核心代码里随意新增 `printf` / `std::cout`。 |
| 注释 | 只在复杂逻辑前加必要说明，避免解释显而易见的代码。 |
| 内存 | 明确 C++ 对象所有权，特别是 tolua 暴露对象、Lua callback、UI binding、C++ manager 持有关系。 |
| 生成文件 | `.pkg` 改动后优先通过生成链更新绑定 cpp，不长期手改生成文件。 |
| 第三方 | `src/Engine`、`src/External` 中的 vendored 代码除非必要不改；必要修改要局部且注明原因。 |

## Lua 与绑定规则

- 很多可见行为由 Lua 控制，修改 C++ 时必须关注脚本侧联动。
- 修改暴露给 Lua 的 C++ 接口时，应同步更新：
  - `.pkg` 绑定定义
  - 生成代码
  - Lua 调用点
  - 受影响 sample
- `LuaPluginMgr`、宿主对象绑定、Lua 环境表、`lua_pcall` 参数个数、table/self 调用约定属于高风险区域。
- `README.md` 已记录 tolua 对象释放导致的内存泄露问题，Lua/C++ 生命周期、GC、脚本对象释放链路必须谨慎。
- Lua action / condition / UI callback 尽量保持调用约定稳定；需要破坏兼容时，应同步更新 sample 和文档。

## AI 专项规则

- AI 长期路线以 `docs/ai-roadmap.md` 为准。
- 当前已有 FSM / DecisionTree / BehaviorTree / Blackboard 能力，新增 AI 功能时优先复用现有 driver 和 Blackboard。
- Knowledge、Perception、Tactics 迁移时，应保持 C++ 热点逻辑与 Lua 配置逻辑分离。
- 感知扫描、影响力传播、寻路预算等热点逻辑优先放 C++；Lua 侧负责编排、配置和可读性。
- 行为类问题优先检查脚本层、AI driver、runtime 适配层、输入链路、debug 绘制，再考虑大规模改对象层。
- 修改 AI sample 行为时，至少检查 `bin/res/scripts/samples` 下对应入口。

## FGUI 专项规则

- FGUI 长期路线参考：
  - `docs/fairygui-final-roadmap.md`
  - `docs/fairygui-business-framework-todo.md`
  - `docs/fairygui-autogen-workflow.md`
  - `docs/fairygui-assets.md`
- C++ 侧负责 runtime 适配、渲染、输入、handle、事件桥；Lua 侧负责 UI 生命周期、业务逻辑、MVC、资源策略。
- Lua 不长期直接持有 FairyGUI / Cocos 底层对象指针，优先通过 handle 和 binding id 通信。
- 复杂 UI 默认走 Ctrl / Model / View / AutoGen 结构；简单调试 UI 可用轻量 View。
- 改 FGUI 输入、生命周期、资源、layer、cache 时，优先跑对应 `HELLO_FGUI_*_SELF_TEST`。

## 平台约束

- 仓库虽然最初偏 Windows + Direct3D9，但现在已经推进 macOS / Xcode 迁移。
- 非 Windows 下，`ClientManager` 会尝试加载 `RenderSystem_GL` / `RenderSystem_GL3Plus`。
- `premake` 路径已统一到 `src/external`，修改路径、包含关系、资源路径时注意大小写敏感问题。
- 修改平台相关代码时，不要只考虑 Win32 分支。
- Windows 专属 API、DirectX、OIS win32 分支要用条件编译隔离。

## 验证清单

按最小相关面验证，不要求每次全量验证：

| 改动类型 | 验证方式 |
|---|---|
| 工程生成或 premake 改动 | Windows 下执行 `vs2017.bat` 或 VS2017 `MSBuild.exe`；macOS 下执行 `xcode.sh`。 |
| `.pkg` / Lua 绑定改动 | 执行 `src\HelloOgre3D\tolua.bat`，检查生成代码和 Lua 调用点。 |
| Lua / gameplay 行为改动 | 检查并运行受影响的 `bin/res/scripts/samples`。 |
| AI 决策 / 行为树 / FSM 改动 | 至少验证 `Sandbox6` / `Sandbox7` / `Sandbox8` 中相关入口，必要时补专用 sample。 |
| 导航 / 寻路改动 | 同时验证路径结果和 debug 绘制结果。 |
| 相机 / 输入 / 渲染问题 | 优先手动跑对应 sample，因为这类问题通常不是纯编译问题。 |
| FGUI 改动 | 运行对应 `HELLO_FGUI_*_SELF_TEST`，复杂生命周期改动优先跑 `HELLO_FGUI_SELF_TEST_ALL=1`。 |
| 性能相关改动 | 保留 Tracy 埋点，必要时记录前后对比。 |
| 平台兼容改动 | 至少检查 Windows / macOS 分支条件编译是否正确。 |

## 文件格式与工具

- 搜索优先使用 `rg`。
- 优先阅读 `README.md`、`premake/premake.lua` 和相关 `docs/*.md`，确认当前阶段和构建意图。
- 保持与周边文件一致的编码和换行风格；默认新建或生成文本文件使用 CRLF。
- 如需把 LF 文本转换为 CRLF，可运行 `tools\dos2unix\lf2crlf.bat <path>`；如需处理编码规范化，可参考 `src/HelloOgre3D/tools.py`。
- 当前项目未配置 MiniGame 那种 `code-review-graph` MCP；默认使用 `rg`、文件阅读和局部构建验证。

## 修改建议

- 优先做最小 diff，保持当前文件既有风格。
- 除非任务明确要求重构，不要大规模移动目录或重命名整块系统。
- 不要把当前结构继续往“单体 manager + 强耦合对象”方向推。
- 引擎耦合的相机、窗口、输入、渲染辅助逻辑优先放到 `runtime`。
- 新增资源、材质、配置文件只在与任务直接相关时修改。
- 如果 Ogre 1.10 迁移后某个行为异常，先检查 `runtime` 适配层，再判断是否需要改业务层。
- 如果修改会影响 sample 展示效果，优先保持 sample 表现与原章节目标一致。

## Git 规则

- 可能存在用户未提交的工作区改动，禁止回退或覆盖无关改动。
- 不使用 `git reset --hard`、`git checkout --` 等破坏性命令，除非用户明确要求。
- 如需提交，提交说明使用中文，不附加任何大模型生成信息。
