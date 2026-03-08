# AGENTS.md

## 项目定位
`HelloOgre3D` 不是单纯的 Ogre 示例工程，而是一个持续演进中的 C++ / Lua 游戏沙盒项目。

项目根目标来自根目录 `README.md`：
- 通过重构游戏案例，把日常开发中的经验沉淀下来。
- 在保留可运行 sample 的前提下，逐步把框架、玩法、脚本、引擎适配分层理顺。
- 让项目既能承载教学式 chapter 示例，也能作为后续 AI / 角色控制 / 工具化能力的实验场。

因此，任何修改都应优先考虑：
- 保持已有 sample 行为稳定。
- 优先做局部、可验证、可回退的小改动。
- 尽量把“引擎耦合逻辑”和“玩法规则逻辑”继续拆开。

## 当前技术栈
当前仓库主要由以下部分组成：
- 渲染与场景：Ogre 1.10
- 输入：OIS
- 物理：Bullet
- UI：Gorilla
- 脚本：Lua 5.1 + tolua++ + LuaSocket + LuaPanda
- 寻路：Recast + Detour
- 转向行为：OpenSteer
- 构建系统：Premake
- 工程生成：Windows 下 Visual Studio，macOS 下 Xcode

## 项目演进脉络
根据 `README.md`，这个仓库当前已经完成或部分完成了这些阶段：

### Chapter 1 基础框架阶段
- 新建 `HelloOgre3D` 项目，用纯 C++ 实现基础 Ogre 相关能力。
- 引入 Lua / tolua，建立 C++ 与 Lua 的基础互通。
- 以 tolua 方式实现场景创建相关对象及接口。
- 接入 Bullet 物理世界。
- 接入 UI 显示系统。
- 接入输入响应系统。

### Chapter 2 智能体移动阶段
- 已完成智能体移动相关内容。
- 这部分能力主要体现在 sample、agent、steer、路径与对象控制链路中。

### Chapter 3 动画与状态控制阶段
- 使用 C++ 实现动画状态机。
- 后续又继续扩展为 C++ 实现有限状态机，且角色状态支持 C++ 与 Lua 双侧扩展。
- 这意味着：角色控制、状态切换、脚本扩展能力是当前仓库的重要主线，不要轻易打破已有状态机与 Lua 扩展接口。

### 绑定与脚本扩展阶段
- 已新增 `LuaPluginMgr`，用于将 C++ 对象与指定 Lua 文件绑定。
- `README.md` 明确记录了 Lua 调用栈、table/self、`lua_pcall` 参数个数等问题，因此这类绑定链路属于高风险区域。
- 任何涉及 Lua 绑定、`tolua` 生成、脚本宿主对象、Lua/C++ 生命周期的修改，都要非常谨慎。

### 当前重构阶段
`README.md` 明确提到，当前项目正在朝“重构解耦”方向推进，重点包括：
- 先处理 Object 体系中过长的继承链。
- 解决冗余问题，方便后续能力扩展。
- 逐步把引擎耦合逻辑与业务逻辑拆开。

因此，后续改动应默认服务于这个方向，而不是继续堆叠临时耦合。

## 中长期规划
`README.md` 中已经明确记录了后续待办方向。编码代理在做结构性修改时，应把这些能力视为未来兼容目标：
- 决策树
- 行为树
- 黑板
- 触发器
- 技能 timeline
- Jobs 多线程
- 崩溃上报模块
- optick 性能分析模块
- 结合 imgui 的性能分析面板

这意味着：
- 不要把当前结构继续往“单体 manager + 强耦合对象”方向推。
- 新增系统时，要考虑未来是否能挂接到 AI、技能、调试、性能分析链路中。
- 任何重构都应尽量为后续“系统可组合、可观测、可脚本扩展”创造条件。

## 构建与工程生成
- 本项目使用 `premake`，不是 CMake。
- `premake/premake.lua` 是工程生成和第三方依赖配置的事实来源。
- Windows 下从仓库根目录执行 `vs2022.bat` 生成 Visual Studio 工程。
- macOS 下从仓库根目录执行 `xcode.sh` 生成 Xcode 工程。
- `src/HelloOgre3D/tolua.bat` 用于重新生成 Lua 绑定代码。
- 如果修改了 `.pkg` 绑定定义，优先重新生成对应 `.cpp`，不要长期手改生成文件。

## 平台约束
这个仓库虽然最初偏向 Windows + Direct3D9 路线，但现在已经在推进 macOS / Xcode 迁移：
- 非 Windows 下，`ClientManager` 会尝试加载 `RenderSystem_GL` / `RenderSystem_GL3Plus`。
- `premake` 路径已统一到 `src/external`，避免大小写问题影响 macOS 文件系统。
- 修改路径、包含关系、资源路径时，要注意大小写敏感问题。
- 修改平台相关代码时，不要默认只考虑 Win32 分支。

## 目录分层
- `src/HelloOgre3D/game`：应用编排层、游戏侧 manager、对外入口层。
- `src/HelloOgre3D/sandbox`：对象系统、脚本桥接、输入、AI、物理、服务、样例实现。
- `src/HelloOgre3D/runtime`：运行时适配层。凡是和 Ogre / OIS / Gorilla 等引擎或中间件强绑定、但又不属于纯玩法规则的逻辑，优先放这里。
- `bin/res/scripts`：运行时 Lua 脚本。
- `bin/res/scripts/samples`：样例脚本，是验证行为回归的重要入口。
- `media`：材质、shader、模型、贴图、粒子等资源。
- `src/Engine`、`src/External`：引擎及第三方 vendored 代码。除非任务明确要求，否则不要随意修改。

## 分层原则
- `game` 和 `sandbox` 不应继续直接积累新的 Ogre 专属 helper；如果能由 `runtime` 承接，就放到 `runtime`。
- 引擎耦合的相机、窗口、输入、渲染辅助逻辑，优先放到 `runtime` 的具体子目录，例如 `runtime/ogre`。
- 不要把 Ogre 相关 helper 再塞回 `common`，除非它确实与引擎实现无关。
- 遇到 Ogre 升级带来的行为回归，先保证旧玩法语义不变，再做体验微调。
- 如果一个改动的本质是“隔离引擎依赖”或“修复升级兼容性”，优先在 `runtime` 层收口。

## Lua 与绑定规则
- 这个项目很多可见行为由 Lua 控制，修改 C++ 时必须关注脚本侧联动。
- 任何影响 sample 行为的 C++ 修改，都应同步检查 `bin/res/scripts/samples` 下对应脚本。
- 修改绑定给 Lua 的 C++ 接口时，应同时更新：
  - `.pkg` 绑定定义
  - 生成代码
  - 脚本调用点
- `README.md` 已记录过 tolua 对象释放导致的内存泄露问题，因此 Lua/C++ 生命周期、GC、脚本对象释放链路都属于高风险区域。
- 涉及 `LuaPluginMgr`、宿主对象绑定、Lua 环境表、pcall 参数处理时，优先保持现有调用约定稳定。

## 修改建议
- 优先做最小 diff，保持当前文件既有风格。
- 除非任务明确要求重构，不要大规模移动目录或重命名整块系统。
- 生成文件尽量通过生成链更新，不要手工长期维护。
- 资源、材质、配置文件只在与任务直接相关时才改。
- 行为类问题优先检查：脚本层、`runtime` 适配层、输入链路、调试绘制，再考虑玩法代码。
- 结构性改动应优先服务于“解耦、缩短继承链、便于后续 AI / 工具 / 调试扩展”这个长期方向。

## 验证清单
修改后按最小相关面验证：
- 工程生成或构建脚本有改动：重新执行 `vs2022.bat` 或 `xcode.sh`。
- tolua 绑定定义有改动：执行 `src/HelloOgre3D/tolua.bat`。
- Lua / gameplay 行为有改动：检查受影响的 `bin/res/scripts/samples` 样例。
- 相机 / 输入 / 渲染问题：优先手动跑对应 sample，因为这类问题通常是行为问题，不是纯编译问题。
- 导航 / 寻路问题：同时验证路径结果和 debug 绘制结果。
- 平台兼容修改：至少检查 Windows / macOS 分支是否仍保持条件编译正确。

## 文件格式与工具
- 仓库同时面向 Windows 和 macOS，但现有工具链仍明显偏 Windows。
- 修改批处理、脚本、工程文件时注意平台差异。
- 保持与周边文件一致的编码和换行风格。
- 如有需要，可参考 `src/HelloOgre3D/tools.py` 处理 CRLF 和编码规范化。

## 默认工作习惯
- 搜索优先使用 `rg`。
- 优先阅读根目录 `README.md` 与 `premake/premake.lua`，确认当前项目阶段和构建意图。
- 如果 Ogre 1.10 迁移后某个行为异常，先检查 `runtime` 适配层，再判断是否真的需要改业务层。
- 如果修改会影响 sample 展示效果，优先保持 sample 表现与原章节目标一致。
