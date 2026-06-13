## HelloOgre3D ##

目标：打造一个**数据驱动的沙盒玩法运行时 + 编辑器**——大世界、多生物、可配置 AI、生物触发器、UGC 玩法；内容是可作者编辑、可序列化、可热重载的数据，而非代码。（项目身份已从早期“教学 sample 沙盒”切换，定调见 `docs/project-direction.md`。）

## 当前路线索引 ##

- 🧭 方向总纲（北极星，统领以下所有文档）：`docs/project-direction.md`
- 项目级路线：`docs/archive/project-roadmap.md`
- 架构优化实施方案（基于代码现状评审，含证据与可执行步骤）：`docs/design/architecture-improvement-plan.md`
- 生产级沙盒/UGC 可借鉴架构参考（MiniGame系深读提炼）：`docs/reference/reference-minigame-patterns.md`
- 可视化编辑器实施方案（AI 行为树 + 触发器，自写 FairyGUI 积木编辑器）：`docs/archive/visual-editor-implementation-plan.md`
- 可视化编辑器开发任务清单（可逐条认领，带真实接口签名）：`docs/archive/visual-editor-task-breakdown.md`
- 行为树执行模型差异与补强分析（对比 MiniGame，列出需补能力）：`docs/design/behavior-tree-gap-analysis.md`
- 触发器/事件系统差异与补强分析（对比 MiniGame，列出需补能力）：`docs/archive/trigger-system-gap-analysis.md`
- 地基模块盘点（已有 / 缺 / 故意不做 + 切片最小集）：`docs/archive/foundation-modules-inventory.md`
- C++ 对象模型与组件化边界：`docs/design/cpp-object-model-refactor-roadmap.md`
- Soldier / Agent 对象链路重构：`docs/archive/SoldierObjectRefactor.md`
- AI 后续能力路线：`docs/planning/ai-roadmap.md`、`docs/reference/AIArchitectureBeyondBook.md`
- FGUI 专项路线：`docs/fgui/fairygui-final-roadmap.md`、`docs/fgui/fairygui-business-framework-todo.md`
- FGUI 阶段性生产验收：`docs/fgui/fairygui-production-gate.md`

## 当前待办总览 ##

详细任务不在 README 维护，避免和路线文档重复。当前优先级按以下方向推进：

- L0 地基（最高优先）：解耦去全局单例（`SandboxServices` 注入）、CSV/Lua 表 Def 数据驱动实例化、事件总线扩“四件套 + 参数化事件名”。
- 第一个垂直切片：数据定义的生物 + 区域触发器（玩家进入→生物攻击）+ 数据驱动行为树 + 热重载，全程手写数据、不做编辑器。
- 缓做：可视化编辑器（AI / 触发器）、UGC / Mod、世界 streaming。
- 维持：FGUI 转为“编辑器 UI 基座 + 回归门禁”，不再占主线 P0。

> 优先级与完整定调以 `docs/project-direction.md`（北极星）为准；文档导航见 `docs/README.md`。

2024-03-15：
1、剥离出最基础的ogre3d的依赖库，挪到src/external目录下

2、新增HelloOgre3D项目，用纯C++实现一些ogre3d相关的内容

3、新增lua/tolua库，编写基础C++类并进行tolua

4、以tolua的形式实现chapter1项目搭建的内容
(0)搭建新框架
(1)tolua场景创建相关的对象及接口
(2)加入bullet物理世界
(3)加入UI显示系统
(4)加入输入响应系统

5、完成chapter2智能体移动的内容

2024-05-22：
6、内存泄露问题进度：内存问题早查明是由于tolua对象没释放导致，使用gc释放即可
但由于最近在忙暑期版本天天加班太累，所以停了段时间没更新

7、新增个lua线程缓存池，以备需要之时

8、加入luasocket库及LuaPanda插件实现vscode中的lua断点调试功能

2024-08-10：
7月末公司项目发布暑期版本，这段忙碌的时间终于过去，后续会继续补上新的内容

8、使用C++实现动画状态机 -- 即第3章角色动画的内容

2025-03-01：
9、新增LuaPluginMgr用来将C++对象与指定lua文件绑定
实现要点及总结：
(1)lua_pcall时narg参数要算好，否则会导致call错，特别是pcall带表函数时
(2)lua调用C++函数传递table时，要手动算好参数个数，直接用tolua++不行
(3)lua_pushvalue(m_pState, -2);  // 将 table 复制一份压栈（self）
   lua_setfenv(m_pState, -2);    // 不需要self即不带冒号调用时 设置函数环境为 table

10、创建战士角色对象，用直接控制的方式实现角色间的状态切换

2025-04-15： 宝宝出生快半个月啦，陪产假马上就要结束，又要投入到暑期版本的开发中去了
下一步计划用C++实现有限状态机控制角色间的状态切换

11、C++实现有限状态机控制角色间的状态切换，角色状态可同时使用C++和lua实现，方便扩展

2025-07-27：暑期版本的开发已近尾声，终于抽空补上了有限状态机的开发及Lua扩展
下一步计划决策树及行为树的开发，同样使用C++开发及支持lua扩展

2025/10/30：
最近工作的内容都是跟重构解耦相关，刚好目前这个项目也需要做个重构，因此计划先把Object那块的长继承链以及冗余问题给解决掉，也方便后续内容的扩展


当时新增代办（已拆分到当前路线文档）：AI、技能 timeline、触发器、Jobs、崩溃诊断、性能分析与调试面板等方向，后续以“当前路线索引”和“当前待办总览”为准。

## macOS / Xcode 迁移说明（进行中）

当前仓库默认是 Windows + Direct3D9 路线。已补充第一批跨平台改造：

1. `HelloOgre3D` 入口、日志、时间函数、文件工具的 Win32 依赖已改为条件编译。
2. `ClientManager` 在非 Windows 下改为尝试加载 `RenderSystem_GL` / `RenderSystem_GL3Plus`。
3. `premake` 路径统一为 `src/external`（避免大小写在部分 macOS 文件系统下出错）。
4. 新增 `xcode.sh`（依赖本机安装 `premake5`）用于生成 Xcode 工程。

生成方式：

```bash
./tools/import_ogre_opengl.sh v1.10.0
./xcode.sh
```

如果项目根目录存在 `ogre-1.10.0.tar.gz`，脚本会优先使用本地包离线导入。
