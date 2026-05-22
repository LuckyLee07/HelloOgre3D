# 项目后续发展路线图（AI 之外）

> 目标：记录 `HelloOgre3D` 在 AI 之外值得持续迭代的方向，方便后续按优先级拆任务、补文档、做回归。
>
> 边界：AI 相关路线独立维护在 `docs/ai-roadmap.md`；本文关注 UI、runtime、玩法基础设施、调试观测、构建测试、稳定性、对象系统等非 AI 主线。

## 1. 总体判断

项目后续不宜只继续堆 sample 或临时 manager。更值得投入的是能提升长期迭代效率的基础设施：

- UI 能承载真实业务。
- Runtime 能隔离引擎耦合。
- 玩法能力能通过 timeline、trigger、配置表复用。
- 调试、日志、Tracy、dump 能让问题可见。
- 构建、sample、自测能形成最小回归链路。
- 崩溃、资源、Lua 绑定问题能被定位和复现。

## 2. 优先级总览

| 优先级 | 方向 | 主要收益 | 相关文档 |
|---|---|---|---|
| P0 | FGUI 业务 UI 框架 | 让 UI 从测试页走向可承载业务界面 | `docs/fairygui-final-roadmap.md` |
| P0 | Runtime 适配层解耦 | 降低 Ogre / OIS / FGUI / Tracy 对玩法层的耦合 | `docs/IterationPlan.md` |
| P0 | 调试与可观测性 | 让 UI、对象、资源、性能问题可定位 | `docs/IterationPlan.md` |
| P1 | 技能 timeline / 触发器 | 为战斗、动画、命中、事件联动提供统一基础 | 本文 |
| P1 | 数据化配置与热重载 | 降低调数值、调 UI、调玩法的重编译成本 | 本文 |
| P1 | 构建、测试与回归体系 | 让 sample 行为和 UI 自测能持续验证 | 本文 |
| P2 | 崩溃上报与诊断 | 提升 C++ / Lua 混合项目的问题定位能力 | 本文 |
| P2 | 对象系统组件化深化 | 降低新增对象类型的复制粘贴成本 | `docs/IterationPlan.md` |
| P2 | 资源与内容管线 | 让材质、模型、UI 资源、配置资源更易检查和管理 | 本文 |

## 3. P0：FGUI 业务 UI 框架

### 目标

把当前 FGUI 接入从“能打开测试界面”推进到“能稳定承载真实业务 UI”。

### 建议能力

- 输入补齐：键盘、滚轮、文本输入、IME、焦点管理。
- 常用控件补齐：Progress、Slider、ComboBox、Transition、虚拟列表。
- UI 生命周期标准化：Open、Reopen、Show、Hide、Close、Destroy、Resize。
- UI 资源管理：package 预加载、引用计数、场景切换清理、泄漏 dump。
- UI 调试面板：当前打开 UI、layer、binding、package、事件数量。
- 更真实的业务样例：主界面、弹窗、列表、Toast、Loading、GuideMask 组合。

### 完成标准

- [ ] 常用控件 API 能支撑一个接近真实业务的复合页面。
- [ ] `HELLO_FGUI_SELF_TEST_ALL=1` 覆盖主要生命周期和资源清理。
- [ ] UI 关闭后 binding、timer、cache、package refCount 能回到预期状态。
- [ ] 有一个面向业务开发的 FGUI 接入示例和开发约定。

## 4. P0：Runtime 适配层解耦

### 目标

继续把 Ogre / OIS / Gorilla / FairyGUI / Tracy 等中间件相关逻辑从 `game`、`sandbox` 中剥离，收口到 `runtime`。

### 建议能力

- Ogre helper 收口：相机、窗口、viewport、debug draw、资源路径辅助。
- 输入适配收口：OIS 输入到 gameplay / UI 的分发规则。
- UI runtime 收口：FGUI 渲染、输入、事件桥、handle 管理。
- Profiling 收口：统一 `H3D_PROFILE_*` 宏和 Tracy 开关。
- 平台适配收口：Windows / macOS 渲染系统、资源复制、路径大小写。

### 完成标准

- [ ] 新增 Ogre / OIS / FGUI 相关 helper 时优先进入 `runtime`。
- [ ] `game` 层只做编排，不继续承载具体引擎适配细节。
- [ ] macOS / Windows 条件编译集中、清晰，不在业务逻辑中散落平台分支。
- [ ] 关键 runtime 适配点有 sample 或手动验证说明。

## 5. P0：调试与可观测性

### 目标

让项目复杂度提升后，问题能通过面板、日志、dump、Tracy 快速定位。

### 建议能力

- FGUI debug panel：打开 UI、layer、事件绑定、package 引用、资源警告。
- Object / Component inspector：对象 id、位置、组件、生命状态、脚本绑定。
- Lua callback / binding 统计：回调数量、悬空绑定、最近错误。
- Resource dump：材质、贴图、package、mesh、脚本资源引用。
- Navigation / Physics / Render debug overlay：路径、碰撞、射线、debug mesh。
- Tracy zone 持续覆盖：Lua 调用、AI tick、FGUI update/render/event、导航查询。

### 完成标准

- [ ] 至少有一个运行时 debug 入口能 dump UI / 对象 / 资源状态。
- [ ] 关键系统耗时能在 Tracy 中看到。
- [ ] Lua error 能带上 sample 名称、脚本文件、调用上下文。
- [ ] 资源和 callback 泄漏能在自测或关闭阶段暴露。

## 6. P1：技能 Timeline / 触发器系统

### 目标

把攻击、换弹、命中、受击、技能释放从零散 action 整理成 timeline + event notify，作为玩法扩展基础。

### 建议能力

- SkillTimeline：按时间点触发 notify。
- 技能阶段：Cast、Windup、Active、Recovery、Cooldown。
- HitWindow：命中窗口、判定范围、目标过滤。
- TriggerVolume：区域触发、技能范围、任务区域、机关区域。
- 事件联动：timeline notify 进入统一事件系统，供动画、音效、UI、AI、任务逻辑订阅。

### 完成标准

- [ ] 普通射击或换弹至少有一个 timeline 化样例。
- [ ] 技能事件能驱动动画 notify、命中判定、UI 提示或日志输出。
- [ ] TriggerVolume 至少能覆盖进入、停留、离开三类事件。
- [ ] timeline 配置与表现层动画状态机边界清晰。

## 7. P1：数据化配置与热重载

### 目标

减少“改 C++、重编译、重启客户端”才能调内容的情况。

### 建议配置对象

- 角色属性。
- 武器属性。
- 技能参数。
- UI registry。
- Sample preset。
- 资源路径和 package 预加载策略。

### 建议能力

- Lua 配置表或轻量 manifest 作为第一阶段格式。
- ConfigManager 负责加载、校验、dump。
- 开发模式支持 reload。
- 配置错误输出具体文件、字段、期望类型。

### 完成标准

- [ ] 至少一种玩法数据可以通过配置改动生效，不需要改 C++。
- [ ] 配置错误能明确报出文件、字段、原因。
- [ ] sample preset 可以固定人数、出生点、随机种子、UI 测试参数。
- [ ] reload 不泄漏旧 Lua table、callback 或 C++ 对象。

## 8. P1：构建、测试与回归体系

### 目标

从纯手动 sample 验证，逐步补成轻量但稳定的回归链路。

### 建议能力

- Smoke test：启动、加载 sample、跑固定帧、退出。
- FGUI 自测整合：继续维护 `HELLO_FGUI_*_SELF_TEST`。
- AI / Navigation / Lua binding 可参考 FGUI 的环境变量自测模式。
- 固定随机种子和 sample preset。
- 构建脚本记录输出位置、日志位置、失败排查步骤。
- 后续可接本地批处理 CI 或 GitHub Actions。

### 完成标准

- [ ] 文档中能清楚说明“改了哪类代码，跑哪类验证”。
- [ ] 至少有一个非 FGUI 的自测入口。
- [ ] 启动失败、资源缺失、Lua 错误能在日志中明确体现。
- [ ] sample 行为回归不完全依赖人工肉眼观察。

## 9. P2：崩溃上报与诊断

### 目标

提升 C++ / Lua 混合运行时的崩溃定位能力。

### 建议能力

- Windows minidump。
- 崩溃时收集 `bin/Sandbox.log`。
- 记录当前 sample、Lua 主入口、最近 Lua error。
- 记录构建配置、平台、渲染系统、资源根路径。
- 可选接入 Breakpad，或先做轻量本地 dump。

### 完成标准

- [ ] C++ 崩溃后能生成可分析的 dump 或最小诊断文件。
- [ ] Lua 层错误和 C++ 崩溃日志能关联到同一次运行。
- [ ] 崩溃诊断文档记录打开 dump、查符号、看日志的步骤。

## 10. P2：对象系统组件化深化

### 目标

降低新增 Monster、NPC、机关、载具等对象类型时复制 Soldier 逻辑的成本。

### 建议能力

- RenderComponent。
- PhysicsComponent。
- AgentLocomotion。
- WeaponComponent。
- SkillComponent。
- Health / Attribute / Buff。
- AnimController / AnimProfile 接口化。

### 完成标准

- [ ] 新对象类型不需要复制整套 SoldierObject。
- [ ] 组件目录和职责边界清晰。
- [ ] 动画、物理、武器、技能不再全部硬绑单一对象类。
- [ ] 现有 sample 行为保持稳定。

## 11. P2：资源与内容管线

### 目标

让材质、模型、贴图、UI package、Lua 配置等内容资源更容易检查、引用和清理。

### 建议能力

- 资源 manifest：记录资源来源、运行时路径、用途。
- 资源引用 dump：谁加载了什么，是否释放。
- FGUI package / atlas 检查。
- media 路径大小写检查，服务 macOS 迁移。
- 常见资源缺失时输出明确日志。

### 完成标准

- [ ] 缺资源时日志能给出明确路径和调用上下文。
- [ ] UI package、贴图、材质至少有基础引用统计。
- [ ] macOS 大小写敏感路径问题能在脚本或文档中提前检查。

## 12. 近期执行队列（当前安排）

这部分记录当前最新判断，后续短期开发优先按这里排。专项细节仍以 `docs/fairygui-business-framework-todo.md`、`docs/ai-roadmap.md` 和对应实现文档为准。

### 第一阶段：FGUI 压力样例与性能观测

当前 FGUI 的 AutoGen、真实业务 UI、资源预加载、group/tag 卸载和自测闭环已经具备基础，下一步优先验证复杂 UI 增长后的稳定性和观测能力。

建议任务：

- 增加 UI 压力样例，覆盖 1/5/20/50 个弹窗、列表大数据量、频繁打开关闭。
- 补 texture、material、render command、triangle 等更细的统计入口。
- 将 package、view、binding、timer、cache、资源引用计数继续接入 dump 或 Tracy 观测链路。
- 补服务层统计和 UI Tracy counter，便于对比不同 UI 规模下的开销。

验收：

- 压力样例能通过脚本或环境变量稳定触发。
- UI 数量、资源引用、渲染统计和关闭后回零状态能在日志或 dump 中看到。
- `run_fgui_selftest.ps1` 至少能覆盖一条压力或性能观测路径。

### 第二阶段：FGUI 输入与层级边界

性能和资源观测稳定后，补业务 UI 手感与规则一致性。

建议任务：

- Tab 焦点顺序。
- Windows IME 最小可用链路。
- 每个 layer 的默认输入穿透、sorting order、ESC 关闭参与规则。
- 打开 UI 时指定 parent/root。
- safe area 配置字段占位。

验收：

- 页面、弹窗、Toast、Guide、Top 层同屏时，输入命中、穿透和关闭顺序符合固定规则。
- 文本输入至少覆盖英文、删除、提交、焦点切换和 IME 最小链路。

### 第三阶段：FGUI 复杂控件与开发工作流

这一阶段目标是继续降低新增业务 UI 的成本。

建议任务：

- Controller change listener、page id/name 访问。
- List virtual list、item renderer、item click payload 完整化。
- Tree/TreeNode 最小可用封装。
- 子页面 / 子组件挂载，支持一个大 UI 拆成多个业务子模块。
- AutoGen 生成 View/Ctrl/Model 模板时保留用户代码区。
- 文档补齐新增 UI 的完整流程。

验收：

- 新 UI 能按“导出资源 -> 生成 manifest/AutoGen -> 注册 -> 编写 View/Ctrl/Model -> 自测”的流程落地。
- 生成工具不会覆盖已有业务逻辑。

### 第四阶段：AI P0 基础设施

AI 方向短期不直接堆 chapter 7-9 的大能力，先铺调试、事件和调度基础，避免后续状态不可观测。

建议任务：

- AI Debug Panel 第一版，复用 FGUI DebugPanel 基础。
- Local / Team / Global 事件规范化。
- AI 更新调度、刷新频率、分帧预算和 Tracy 观测。

验收：

- 能选择 agent 查看 driver/state、Blackboard、Knowledge、Perception、最近事件和 BT trace 摘要。
- AI 事件 payload 字段稳定，agent/sample/UI 关闭后不会留下无效 callback。
- 20+ agent 场景下 AI 更新不会集中在固定帧产生明显 spike。

### 第五阶段：项目工程化与长期维护

当 FGUI 和 AI 的核心闭环都更稳后，再补项目级工具能力，减少后续迭代成本。

建议任务：

- 非 FGUI smoke test / selftest。
- 资源缺失与 Lua 错误诊断增强，日志带 sample、script、UI name、object handle、调用上下文。
- ConfigManager、sample preset、固定随机种子、热重载。
- Windows minidump / Breakpad / 轻量 crash report。
- Object / Component 继续拆分，避免新增角色时复制 SoldierObject。

验收：

- 常见启动失败、资源缺失、Lua 错误和崩溃能从日志或 dump 关联到同一次运行。
- 至少一个非 FGUI sample 可以脚本化验证。
- 新对象类型和新 sample 的接入成本下降。

## 13. 建议迭代顺序

### Milestone A：UI 可用、问题可见

1. FGUI 输入和常用控件补齐。
2. FGUI 资源和生命周期 dump。
3. UI debug panel 第一版。
4. Tracy 和日志覆盖 UI 关键路径。

验收：复杂 UI 页面能稳定打开、交互、关闭、复用，生命周期问题能定位。

### Milestone B：Runtime 边界清晰

1. Ogre / OIS / FGUI helper 收口到 `runtime`。
2. 平台分支收敛。
3. debug draw / profiling / input 适配规则文档化。

验收：新增引擎耦合能力时，有明确落点，不继续污染 `game` / `sandbox`。

### Milestone C：玩法基础设施成型

1. 技能 timeline 第一版。
2. TriggerVolume 第一版。
3. 配置表和 ConfigManager 第一版。
4. Sample preset。

验收：至少一个技能或战斗行为由 timeline + config 驱动。

### Milestone D：稳定性与回归

1. 非 FGUI 自测入口。
2. Smoke test。
3. 崩溃 dump。
4. 资源缺失和 Lua error 诊断增强。

验收：常见崩溃、资源缺失、Lua 错误能从日志和 dump 中定位。

## 14. 跟踪清单

- [x] FGUI UI 压力样例，覆盖 1/5/20/50 弹窗或列表规模。
- [~] FGUI texture / material / render command / triangle 明细统计（已有 source/alias 计数和同路径动态 loader 复用，尺寸/切换明细待补）。
- [ ] FGUI 服务层统计和 Tracy counter。
- [ ] FGUI 键盘、滚轮、文本输入、IME、焦点补齐。
- [ ] FGUI Progress / Slider / ComboBox / Transition / 虚拟列表补齐。
- [ ] FGUI UI debug panel 第一版。
- [ ] Runtime 中 Ogre / OIS / FGUI helper 收口。
- [ ] Object / Component inspector 第一版。
- [ ] Resource dump 第一版。
- [ ] SkillTimeline 第一版。
- [ ] TriggerVolume 第一版。
- [ ] ConfigManager 第一版。
- [ ] Sample preset 第一版。
- [ ] 非 FGUI 自测入口。
- [ ] Smoke test。
- [ ] Windows minidump 或轻量崩溃诊断。
- [ ] 资源 manifest / 路径大小写检查。
- [ ] AnimController / AnimProfile 接口化。

