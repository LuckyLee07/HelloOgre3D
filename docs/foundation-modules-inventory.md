# 地基模块盘点（已有 / 缺 / 故意不做）

> 文档状态：**历史盘点 / 参考清单**。本文保留“有哪些地基模块、哪些已有、哪些故意不做”的清单价值；其中“第一个垂直切片”“数据驱动沙盒”等排期表述已被 `docs/project-direction.md` 和 `docs/long-term-iteration-plan.md` 取代。
>
> 上位：方向与整体优先级以 `docs/project-direction.md`（北极星）为准；长期阶段以 `docs/long-term-iteration-plan.md` 为准。本文与 `architecture-improvement-plan.md` / `behavior-tree-gap-analysis.md` / `trigger-system-gap-analysis.md` 互补。
>
> 状态图例：✅ 已有可用 ｜ 🟡 部分（有骨架/雏形，需扩） ｜ ❌ 缺 ｜ 🚫 故意不做。证据指向本仓库；核对于 2026-05-29，标“需确认”的未逐行验证。

---

## 0. 一句话总览

**“缺的”高度集中在“内容即数据”这条脊梁上；引擎级地基你基本都有（站在 Ogre/Bullet/Lua/FairyGUI 肩上）；网络/多人这块直接不做。** 所以范围比感觉上小一个量级：真正要补的就是 **数据/序列化/事件/空间** 几样，且第一个切片只需碰其中一小撮。

两个本轮核对纠正的事实：
- **事件系统不是从零**：`sandbox/core/event/` 已有全局单例事件总线骨架（见 B 区），不是“只有 Local 事件”。
- **命名冲突预警**：仓库已有 `SandboxContext`（作事件 payload，`sandbox/core/event/SandboxContext.h`）。`architecture-improvement-plan.md` P1 提议的依赖注入 context **不要也叫 `SandboxContext`**，建议改名（如 `SandboxServices` / `SandboxWorldContext`），避免撞名。

---

## A. 数据与序列化（脊梁，缺得最集中）

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 / 最小实现 |
|---|---|---|---|---|
| 配置加载 ConfigManager | 🟡 | ✅ | 🟡 | 已有 **Lua 版** `bin/res/scripts/manager/ConfigManager.lua` + `config/sample_presets.lua`（固定 seed / agent 数 / 队伍 / preset）。缺通用 Def 表加载与校验。切片用 Lua 表 Def 即可，不必先扩。 |
| CSV 读取 | ❌ | ✅ | ❌ | 全仓无 CSV。终极要（策划友好、批量数值）。**切片可用 Lua 表绕过**。最小：双行表头（中文注释 + 英文字段名）+ 纯 Lua `CsvLoader.lua`，按 ID 取行。 |
| Def 原型 + 实例化约定 | ❌ | ✅ | ✅ | 概念性缺失：没有“数据 → 实例化对象”的统一约定。**切片必需**。最小：Lua 表 Def（`{components={...}, attribs={...}}`）+ 一个装配器按 Def 建组件（复用现有 `AgentFactory` 风格）。 |
| 存档 / 序列化 | ❌ | ✅ | ❌ | 无游戏存档（`PhysicsWorld` 里的 serialize 是 Bullet 内部，不算）。**切片不需要持久化**。最小（later）：Lua table → JSON dump/load；远期再考虑二进制/分块。 |
| 热重载 | ❌ | ✅ | 🟡 | 无统一机制。切片层面**廉价**：Lua 重跑 Def/BT 文件即可。终极：Def“就地替换字段”（运行时对象持指针，不能换指针）。 |
| 反射 / 属性元数据 | ❌ | 🟡 | ❌ | 缺。**仅编辑器层（L3）要**（属性面板 + 序列化 + 多通道）。两轨教训：**别给运行时玩法对象上反射**。切片/运行时都不需要。 |

## B. 运行时核心服务

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 / 最小实现 |
|---|---|---|---|---|
| 事件系统 | 🟡 | ✅ | ✅ | **已有全局单例骨架** `SandboxEventDispatcherManager`（`sandbox/core/event/`）：`CreateDispatcher / Subscribe(name,cb)->Token / Unsubscribe / Emit(name, SandboxContext)`，payload 带 `eventType/scope/sender/target/team/timeMs` + 调试统计（emit 数、分类型计数、最近 32 条）。**缺四件套**：缓冲跨帧分发 + 防递归、每帧节流、队列上限、过滤器即订阅；**缺参数化事件名** `事件?objid=N`；scope 字段有但未真正路由 Local/Team/Global。切片：**扩这个骨架即可，不用从零**。详见 `trigger-system-gap-analysis.md` T-G1/T-G2。 |
| 对象注册表 + id 分配 | 🟡 | ✅ | 🟡 | `ObjectManager` 是 registry，但职责过载（update 循环 / AI 调度 / 输入转发 / navmesh 混在一起，见架构方案 C3）。切片够用，清理后置。 |
| 计时器 Timer | ❌ | ✅ | 🟡 | 无 Timer 类，计时靠 deltaMs 累加。终极要（触发器计时、技能 CD、延迟事件）。切片可先不用（区域触发不依赖计时）。最小：`TimerService` 注册回调 + 到点触发（可直接发事件）。 |
| 主循环 / 帧驱动 | ✅ | ✅ | ✅ | deltaMs 已贯通 `ClientManager → GameManager → 各 Update`。 |
| 日志 | ✅ | ✅ | ✅ | `bin/Sandbox.log`，项目既有日志系统。 |
| 随机数 / 种子 | ✅ | ✅ | ✅ | ConfigManager 固定 seed（可复现）。 |
| 数学 / 几何 | ✅ | ✅ | ✅ | Ogre 提供 Vector3 / Quaternion / AABB 等。 |
| 依赖注入 context | ❌ | ✅ | 🟡 | 缺（现在靠 `g_*` 全局单例耦合）。架构方案 P1 的核心。**注意改名**（别叫 SandboxContext，已被占用）。切片阶段最好顺带做，否则新代码继续粘全局。 |

## C. 资源 / 资产

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 |
|---|---|---|---|---|
| 资源加载（mesh/material/texture） | ✅ | ✅ | ✅ | Ogre + FairyGUI 各自管线。 |
| 资源引用统计 / dump | 🟡 | ✅ | ❌ | `runtime/diagnostics/RuntimeResourceDiagnostics` 已承接 Ogre mesh/material/texture/skeleton/program 与 resource group 统计。 |
| 资源 manifest / 路径大小写检查 | ❌ | 🟡 | ❌ | 缺（`project-roadmap` 早有记。服务 macOS 迁移用）。切片不需要。 |
| 资源打包 / 热更 | 🚫 | 🚫 | 🚫 | 产品向，掌握项目不做。 |

## D. 空间 / 世界

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 / 最小实现 |
|---|---|---|---|---|
| 场景 / 世界管理 | 🟡 | ✅ | ✅ | 单场景 sandbox 可用；无多 world / 分块。切片够用。 |
| 空间分区查询 | ❌ | ✅ | 🟡 | 缺。`AIController::FindNearestEnemy` 全场线性扫 `getAllAgents()`（O(n²) 风险）。多生物必需。**切片生物少，先线性，标 TODO**。最小：均匀网格（uniform grid）按范围查；接口先抽好（架构方案 P7 的 `PerceptionQuery`）。 |
| 世界 streaming / 分块 | 🚫 | 🟡 | ❌ | 远期：取决于“大世界”到底是无缝大地图还是单场景多生物。先不做，等定清规模。 |

## E. 脚本 / 绑定

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 |
|---|---|---|---|---|
| Lua VM | ✅ | ✅ | ✅ | Lua 5.1 + 线程池缓存。 |
| tolua 绑定 | ✅ | ✅ | ✅ | `GameToLua / SandboxToLua / RuntimeToLua` + `ManualToLua`。 |
| LuaPluginMgr（C++ 对象绑 Lua 文件） | ✅ | ✅ | ✅ | 已用（AI action/condition、script component）。 |
| UGC 白名单沙盒（loadstring+setfenv） | 🟡 | ✅ | ❌ | LuaPluginMgr 有 env 表雏形；UGC 级安全沙盒缺。切片（自己写的 Def/BT）不需要沙盒；做触发器编辑器/UGC 时再上。 |

## F. AI / 玩法基础

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 |
|---|---|---|---|---|
| 数据驱动行为树加载器 | ✅ | ✅ | ✅ | `bin/res/scripts/ai/behavior/BehaviorTreeLoader.lua`（读 `{node,children,action,condition...}` 建 C++ 树）。 |
| BT 内核 / FSM / DT driver | ✅ | ✅ | ✅ | `sandbox/ai/{behavior,fsm,decision,common}`。补强见 `behavior-tree-gap-analysis.md`（G4/G3/G1）。 |
| Blackboard | ✅ | ✅ | ✅ | 6 类型（Agent/Float/Int/Bool/String/Vec3）。终极需补 Object-by-id/数组（BT gap G6）。 |
| AIScheduler（分帧错峰） | ✅ | ✅ | 🟡 | 已有 budget + 错峰；缺距离 LOD（BT gap）。 |
| 感知原语库 | ❌ | ✅ | 🟡 | 散在 AIController。终极收口（架构 P7）。切片先线性。 |
| 触发器运行时（ECA） | ❌ | ✅ | ✅ | 缺。切片必需。详见 `trigger-system-gap-analysis.md` T-G3/T-G5/T-G6。 |
| 技能 timeline | ❌ | 🟡 | ❌ | 缺；北极星未强调，缓做。 |

## G. UI / 编辑器基础

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 |
|---|---|---|---|---|
| FairyGUI 运行时 | ✅ | ✅ | ❌ | 接近生产级。切片不做 UI，但它是 L3 编辑器的 UI 基座。 |
| 积木编辑器框架 | ❌ | 🟡 | ❌ | L3。见 `visual-editor-*`。掌握导向下属“兴趣选择”。 |
| 撤销重做框架 | ❌ | 🟡 | ❌ | L3，编辑器阶段。 |

## H. 调试 / 工具

| 模块 | 状态 | 终极要 | 切片要 | 现状 / 落点 |
|---|---|---|---|---|
| FGUI DebugPanel | ✅ | ✅ | ❌ | 已有 snapshot 面板。 |
| Tracy 性能 | ✅ | ✅ | 🟡 | 已接入 FGUI / AIScheduler counters。 |
| 诊断 dump | ✅ | ✅ | 🟡 | `runtime/diagnostics`，对象/资源/AI 摘要。 |
| smoke / selftest | ✅ | ✅ | 🟡 | `tools/run_sandbox_smoke.ps1` + FGUI selftest 套件。 |
| BT 运行态可视化（按 uuid 高亮） | ❌ | ✅ | 🟡 | 缺。可视化任务 E0/E1；也服务切片的 AI 调试。 |

## I. 网络 / 多人

| 模块 | 状态 | 说明 |
|---|---|---|
| 网络同步 / 多人 / 服务器 | 🚫 | **故意全部不做**。单机掌握项目，砍掉这块能省下巨量复杂度（MiniGame 的同步策略、reflex 的 NO_SYNC/SYNC_ONLYHOST 等都不用碰）。这是“不照搬”里最大的一刀。 |

---

## 1. 第一个垂直切片的最小 base 集（关键结论）

切片 = “数据定义的生物 + 区域触发器（玩家进入→生物攻击）+ 数据驱动 BT + 热重载”。逐项看它**真正逼你新建/扩的 base**：

| # | base | 动作 | 量级 |
|---|---|---|---|
| 1 | Def 约定（Lua 表）+ 装配器 | 新写（按 Def 建组件，复用 AgentFactory 风格） | 小 |
| 2 | 事件总线四件套 + 参数化事件名 | **扩**现有 `SandboxEventDispatcherManager`（加缓冲跨帧/节流/上限 + `?objid=` 名） | 中 |
| 3 | 区域事件源 TriggerVolume | 新写（最小 AABB，每帧/低频扫区域内对象，进出发事件） | 小 |
| 4 | 触发器运行时（注册 event+condition+action，手写 Lua + 沙盒可选） | 新写（`TriggerRuntime.lua`，注册到事件总线） | 小 |
| 5 | 行为树 | **已有**加载器；按需补 G4(Parallel/Random)/G3(参数求值) 个别节点 | 小 |
| 6 | 事件喂 BT（生物对被击/发现反应） | 事件总线加 BT 入口 + `BehaviorEventNode`（与 BT G2 协同） | 中 |

**切片明确不需要的**（消焦虑）：存档/序列化、CSV（用 Lua 表）、反射、空间分区（先线性）、计时器（区域触发不依赖）、UGC 沙盒、网络。

> 结论：切片真正的“新地基大件”就两个——**事件总线扩四件套 + 触发器运行时**；其余要么已有（BT 加载器、事件骨架、ConfigManager）、要么用 Lua 表绕过、要么先标 TODO。“缺一整个引擎”的感觉，落到下一步其实就这么点。

### 切片 base 依赖关系

```
Def(Lua表)+装配器 ──┐
                    ├─► 生物实例(组件) ──► BT(已有加载器, 补G4/G3) ──► 事件喂BT(BehaviorEventNode)
事件总线(扩四件套)──┤                                                      ▲
   + 参数化事件名   └─► TriggerVolume(区域事件源) ──► 触发器运行时(ECA) ───┘
```

---

## 2. 跟踪清单（地基侧）

**切片最小集（先做）**
- [ ] Def（Lua 表）约定 + 装配器
- [ ] 事件总线扩四件套 + 参数化事件名（扩 `SandboxEventDispatcherManager`）
- [ ] TriggerVolume 区域事件源
- [ ] 触发器运行时（ECA，手写 Lua）
- [ ] 事件喂 BT + `BehaviorEventNode`
- [ ] BT 补 Parallel/Random + 参数求值（按需）

**终极要、但切片后再补**
- [ ] CSV 读取（双行表头 + Lua 解析器）
- [ ] 存档 / 序列化（Lua table → JSON 起步）
- [ ] 计时器 TimerService
- [ ] 空间分区查询（uniform grid，替换线性扫描）
- [ ] 依赖注入 context（去 `g_*` 全局；**改名**避开 SandboxContext）
- [ ] Blackboard 补 Object-by-id / 数组
- [ ] 反射 + 属性元数据（**仅编辑器层**）

**故意不做**
- [ ] ~~网络同步 / 多人~~
- [ ] ~~资源打包 / 热更 / 多渠道~~
- [ ] ~~世界 streaming~~（除非“大世界”规模确需，再议）

---

## 3. 对其他文档的小修正

- `trigger-system-gap-analysis.md` 原说“本项目几乎没有触发器系统，只有 Local 事件”——**应细化为**：已有全局单例事件总线**骨架**（`SandboxEventDispatcherManager`，name→dispatcher + subscribe/emit/token + scope/team 字段），缺的是“四件套 + 参数化事件名 + scope 路由 + ECA + 触发器对象库”。结论（要补这些）不变，但起点比“从零”高。
- `architecture-improvement-plan.md` P1 的依赖注入 context **改名**，避开已被占用的 `SandboxContext`。
