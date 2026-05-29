# 行为树执行模型差异与补强分析（HelloOgre3D vs MiniGame）

> 目的：对比本项目 `HelloOgre3D` 与成熟沙盒 `MiniGame_Dev` 的**行为树运行时执行模型**，找出为支撑“生产级多生物 + 可视化 AI 配置”需要补的能力，并给出优先级与落地落点。
>
> 方法：两边均**直接读源码**得出（非文档）。本项目证据指向本仓库 `src/HelloOgre3D/sandbox/ai/behavior/`；MiniGame 证据指向其仓库 `Source/SandboxGame/Modules/BehaviorTreeAI/`（详见 `docs/reference-minigame-patterns.md`）。
>
> 上位：整体优先级以 `docs/project-direction.md`（北极星）为准。本文补强项对应北极星 L1-⑤（BT 补强）；其中 G4/G3/G1 是“第一个垂直切片”里数据驱动 AI 行为正确性的关键。
>
> 关联：补强项最终会以新任务并入 `docs/visual-editor-task-breakdown.md`；可视化编辑器的“积木库丰富度 / 参数绑定 / 行为正确性”直接受这些能力制约。

---

## 1. 一句话结论

本项目 BT 是一个**正确但极简**的实现：`Sequence/Selector/Inverter/ForceSuccess/ForceFailure/Wait/Condition/Action`，组合节点能从 running 子节点续跑（有 running 记忆），条件每帧求值。它**够跑通 Sandbox8**，但缺三类对“生产级反应式 AI + 可视化配置”关键的能力：

1. **反应性（中断/重评估 或 事件驱动）** —— 本项目 BT 一旦进入某分支只会“续跑”，**不会因为更高优先级条件变化而打断切换**；也没有事件节点。生物“被攻击立刻反击 / 敌人出现立刻警戒”这类反应做不出来。
2. **参数运行时求值（GetValue）** —— 本项目节点参数基本是 build 期静态绑定，无法让积木把参数绑到黑板键/表达式上、tick 时再求值。可视化编辑器“给积木配动态参数”受限。
3. **更丰富的节点类型与黑板类型** —— 无 Parallel/Random/Priority/Loop/子树；黑板只有 6 种标量类型，无 Object-by-id/数组/Timer。积木库会很贫。

> 注意：**不是要全盘照搬 MiniGame。** 它的实例热回收、5 态返回值、LOD 等部分要么是性能优化（可后置），要么本项目已有等价物（tick 调度由 `AIScheduler` 承担）。下面按“对目标的必要性”排优先级，不是按 MiniGame 有什么排。

---

## 2. 执行模型逐项对比

| 维度 | HelloOgre3D（现状） | MiniGame | 差距 |
|---|---|---|---|
| 返回值 | 3 态：RUNNING/SUCCESS/FAILURE（`BehaviorNode.h:16-22`） | 5 态：SUCCESS/FAIL/WAIT/LOGIC_WAIT/NO_RUN | 本项目 RUNNING 兼任跨帧；够用，差异不关键 |
| 节点生命周期 | `Tick()` + `Reset()`；Action 另有 `OnInitialize/OnUpdate/OnCleanUp`（`LuaBehaviorAction.cpp:32-59`） | 6 段：RunBefore/ActivateCondition/OnStart/RunActive/RunBack/OnEnd | 本项目更简；Action 已有 init/update/cleanup，基本够 |
| running 记忆 | ✅ `m_runningIdx` 续跑（`BehaviorComposite.cpp:35,63`） | ✅ `m_activeChild` | 对齐 |
| **中断/重评估(abort)** | ❌ 无。Selector/Sequence 只从 `m_runningIdx` 续跑，**不回头重判更高优先级条件**（`BehaviorComposite.cpp:30-84`） | ✅ `InterruptType None/Self/LowerPriority/Both`，高优先级条件变化即打断切换 | **关键缺口** |
| **事件节点** | ❌ 无 | ✅ 11 类事件节点 + 实例/全局/AINPC 三级分发，接事件总线 | **关键缺口** |
| 组合节点 | Sequence / Selector | + Parallel/SimpleParallel/Random/Priority/Branch/Condition/Loop/LoopInterval | 缺并行、随机、循环、子树式 |
| 装饰器 | Inverter / ForceSuccess / ForceFailure | + Single/三合一 Decorator(Inversion/Once/While) | 缺 Once/While/Loop 装饰 |
| 叶子任务 | LuaAction（Lua）+ Condition（Lua/C++ evaluator） | Lua 任务 + 14+ C++ 原生任务(移动/攻击/挖块/导航) | 本项目热点任务尚未 C++ 化（在 Lua）|
| 条件求值 | **每帧求值**，无缓存（`LuaCondition.cpp:33-51`） | 帧内缓存 + 黑板脏标记 `bbChange/needrun`，黑板未变不重算 | 缺缓存/脏标记（性能项） |
| **参数运行时求值** | ❌ 参数 build 期静态（condition `cfg.params` build 时展开） | ✅ `BTGetValue`：参数绑 Lua 函数，tick 时从黑板/表达式求值 | **可视化配置关键缺口** |
| 黑板类型 | 6 种：Agent/Float/Int/Bool/String/Vec3（`Blackboard.h:29-46`） | 27 种：+ Object(actorId)/Item/Player/Area/Timer/各类数组 | 缺对象id/数组/Timer |
| 子树复用 | ❌ 无 | ✅ 多文件子树装配 + 节点模板 CpoyNode | 缺（编辑器复用要用） |
| 每节点超时 | ❌ 无 | ✅ `m_curTimeover` | 可后置 |
| 实例池/热回收 | ❌ 无 | ✅ scriptKey 分桶复用 | 可后置（多生物再说） |
| tick 调度/LOD | ✅ 由 `AIScheduler` 错峰 + budget（独立于 BT） | BT 内置距离分帧 LOD | 本项目已有等价物，位置不同 |
| 调试 trace | ✅ `BehaviorTrace`（label/status 逐帧） | `BehaviorTreeDebugMgr` 回传 | 本项目已有基础，差按 uuid 查询（见可视化任务 T0.3） |

---

## 3. 补强项（按优先级）

> 优先级判据：**对“反应式多生物 AI”和“可视化编辑器可用性”的必要性**，不是 MiniGame 有没有。

### P0 —— 不补，反应式 AI 和可视化配置立不住

#### G1. 行为中断 / 条件重评估（abort）
- **是什么**：让 Selector/Sequence 在续跑当前分支前，先重判“更高优先级、带条件守卫的兄弟分支”；若更高优先级条件变为真，则 `Reset()` 当前运行分支、切过去。
- **为什么 P0**：这是“生物对环境/战斗即时反应”的根本机制。没有它，生物进入“巡逻”分支后即便敌人出现/被攻击也要等当前分支自然结束才会切。本项目当前完全没有。
- **怎么加（落点）**：
  - 给 Composite 增加“带条件守卫的子分支”概念 + 中断模式（参考 MiniGame `InterruptType`）。最小可行版：Selector 每帧从 0 重判“条件型子节点”，命中更高优先级分支且与当前 running 不同 → 对 running 分支调 `Reset()` 再切。
  - 文件：`src/HelloOgre3D/sandbox/ai/behavior/BehaviorComposite.cpp`（Selector/Sequence Tick 加 reevaluation 路径）；`BehaviorNode` 已有 `Reset()` 可用。
  - 配置/编辑器：节点加 `interrupt = "self"|"lowerPriority"|"both"|"none"` 字段，`BehaviorTreeLoader.lua` 透传。
- **替代/配合**：可与 G2（事件节点）二选一起步，但**长期两者都要**。先做 reevaluation 更通用、改动更集中。

#### G2. 事件驱动节点
- **是什么**：BT 能被外部事件（被击/碰撞/敌人发现/全局指令）即时驱动，而非纯轮询条件。
- **为什么 P0**：与 G1 互补——reevaluation 解决“条件变了切分支”，事件节点解决“外部一次性事件触发反应”。生物“被打→仇恨/反击”最自然用事件。
- **怎么加（落点）**：依赖**统一事件总线**（见 `reference-minigame-patterns.md` §3，本项目当前仅 Local 事件）。新增 `BehaviorEventNode`：注册关心的事件名到树实例，事件到来置标志，Tick 时消费。
  - 文件：新增 `src/HelloOgre3D/sandbox/ai/behavior/BehaviorEventNode.{h,cpp}`；树实例需维护“事件名→节点”表 + `OnEvent(name, payload)` 入口；事件总线 `OnEvent` 同时喂触发器与 BT（同一条事件两路分发）。
- **前置**：统一事件总线。**因此 G2 排在 G1 之后**（G1 无前置，可先做）。

#### G3. 参数运行时求值（GetValue）
- **是什么**：节点参数不再只是常量，可绑“黑板键”或“求值函数”，tick 时取值。例：MoveTo 的目标位置绑黑板 `enemyPos`；Wait 时长绑黑板。
- **为什么 P0（对编辑器）**：可视化编辑器的“参数槽”要能选“填常量 / 绑黑板键 / 嵌取值积木”三种（plan §5.3 的 InputType）。没有 GetValue，积木参数只能填死值，编辑器价值大打折扣。
- **怎么加（落点）**：
  - 约定参数三态：常量 / 黑板键(`bbkey`) / 取值函数(Lua)。最小版先支持“常量 + 黑板键”（覆盖大多数场景），取值函数后置。
  - 本项目 Lua 叶子节点已能拿到 `blackboard`（`LuaBehaviorAction::OnUpdate(deltaMs, owner, blackboard)`），所以**Lua 任务侧**实现 GetValue 成本低：约定 `subparam` 里 `{kind="bbkey", key="enemyPos"}`，任务内 `blackboard:GetVec3(key)`。
  - C++ 原生任务的 GetValue 后置（等 C++ 任务化时再说）。
  - 文件：`BehaviorTreeLoader.lua`（参数解析约定）；Lua 任务模板（`controller.subparam` 取值约定，照搬 MiniGame `BTTask_*` 契约）。

#### G4. 补齐常用组合/装饰节点：Parallel、Random（最少这两个）
- **是什么**：Parallel（多分支同时跑 + 成功/失败策略）、Random（按权重随机选分支）。
- **为什么 P0（对编辑器）**：积木库太贫会让编辑器没人用。Parallel 是“一边移动一边攻击/警戒”的刚需；Random 是“随机闲逛/随机技能”的刚需。
- **怎么加（落点）**：`BehaviorComposite.{h,cpp}` 加 `BehaviorParallel`（先支持 oncefail/oncesuccess/all 策略）、`BehaviorRandom`（权重）；`BehaviorTreeDriver` 加 `NewParallel/NewRandom` 工厂；`BehaviorTreeLoader.lua` 加节点类型。

### P1 —— 生产级多生物需要，但可在编辑器 MVP 后补

#### G5. 子树复用
- 让一棵树引用另一棵（通用战斗子树复用）。落点：`BehaviorTreeLoader` 支持 `{node="SubTree", ref="combat_common"}`，加载时递归构建并挂到包装节点下。编辑器 M5 阶段会用。

#### G6. 黑板类型扩展
- 加 `Object(by handle/id)`、数组类型、`Timer`。落点：`Blackboard.{h,cpp}` 加对应 typed map + tolua 导出。优先加 Object-by-id（目标引用稳定）和 number/object 数组（目标列表）。

#### G7. 条件缓存 + 黑板脏标记
- 黑板未变则条件不重算（`bbChange/needrun`）。落点：`Blackboard` set 时打 dirty；节点声明依赖的 key；Tick 短路。**性能项**，多生物规模上来再做；与 `AIScheduler` 的错峰是两层优化，不冲突。

### P2 —— 优化/打磨，按需

- G8. 每节点超时保护（`timeover`）——防卡死。
- G9. 5 态返回值（WAIT/LOGIC_WAIT 细分）——本项目 RUNNING 够用，非必要。
- G10. 实例池/热回收（scriptKey 分桶）——多生物大规模时的 GC 优化。
- G11. 热点任务 C++ 化——把高频 Lua 任务(移动/攻击/感知)逐步下沉 C++（配合 `architecture-improvement-plan.md` 的感知原语库 P7）。

---

## 4. 与可视化编辑器路线的衔接

补强项不是孤立的，它们决定编辑器“能放什么积木、参数能怎么配、行为对不对”：

| 编辑器能力 | 依赖的 BT 补强 |
|---|---|
| 积木库够丰富、能拼出真实行为 | G4（Parallel/Random）、G1（中断分支） |
| 参数槽能“绑黑板键/动态值” | G3（GetValue） |
| 生物能对战斗/环境即时反应 | G1（重评估）+ G2（事件节点） |
| 大 UI/通用战斗逻辑复用 | G5（子树） |
| 目标引用/目标列表类积木 | G6（黑板 Object/数组） |

**建议把这些插进 `visual-editor-task-breakdown.md` 的 Epic 顺序**：
- E0/E1（uuid + 调试回传）**不依赖**任何 BT 补强，可先做。
- **在 E2/E3（积木框架 + AI 编辑器打通）之前或并行**，至少补 **G1 + G3 + G4**——否则编辑器做出来积木又少、参数又静态、行为又不会反应，验收会很尴尬。
- **G2（事件节点）** 与触发器编辑器 E4 共享“统一事件总线”前置，可一起规划。
- G5–G11 跟随 E5（打磨）与多生物规模化按需推进。

---

## 5. 落地顺序建议（BT 侧）

> 与可视化任务 E0/E1 并行推进，互不阻塞。

1. **G4 补 Parallel + Random**（改动集中在 `BehaviorComposite`/`Driver`/`Loader`，最易、立刻让积木库像样）。
2. **G3 参数运行时求值（先做“常量 + 黑板键”两态）**（Lua 任务侧约定，成本低，解锁编辑器动态参数）。
3. **G1 行为中断/条件重评估**（最有价值的行为能力，改 Selector/Sequence Tick + 节点 interrupt 字段）。
4. **G6 黑板加 Object-by-id + 数组**（目标引用/目标列表）。
5. **统一事件总线就绪后 → G2 事件节点**（与触发器编辑器 E4 协同）。
6. G5 子树、G7 条件缓存、G8–G11 按规模与性能需要补。

---

## 6. 验证

- 每补一类节点/能力：在 `Sandbox8` 用手写 `config/*.lua`（现有 `BehaviorTreeLoader` 格式）验证行为；开 `debugTrace` 看 trace。
- 中断/事件这类行为正确性：构造“巡逻中敌人出现→应立即切到战斗”的最小场景验证切换确实发生。
- 不破坏现有 `SoldierBT.lua` 行为（回归 `Sandbox6/7/8`）。

---

## 7. 关键证据索引

**HelloOgre3D（本仓库）**：
- `src/HelloOgre3D/sandbox/ai/behavior/BehaviorNode.h`（3 态 Status + Tick/Reset）
- `BehaviorComposite.cpp`（Sequence/Selector running 记忆、**无重评估**，:30-84）
- `BehaviorDecorator.cpp`（Inverter/ForceSuccess/ForceFailure，:18-60）
- `LuaCondition.cpp`（条件**每帧求值无缓存**，:33-51）
- `LuaBehaviorAction.cpp`（Action 生命周期 OnInitialize/OnUpdate/OnCleanUp + 拿到 blackboard，:32-59）
- `BehaviorTreeDriver.h`（工厂方法清单，:37-45）
- `Blackboard.h`（6 种类型，:29-46）
- `bin/res/scripts/ai/behavior/BehaviorTreeLoader.lua`（数据驱动加载器，已支持的节点类型）

**MiniGame（外部参考，见 `reference-minigame-patterns.md` §对应章节）**：
- `BTNodeBase.cpp`（6 段生命周期、缓存短路）、`BTNodeDef.h`（5 态返回值）
- `BTNodeLogic.cpp`（`InterruptType` 重评估/abort，:528-806）
- `BTNodeParallel/Random/Priority/Loop/Branch.cpp`（组合节点语义）
- `BTGetValue.cpp` + `BTMain.lua`（参数运行时求值）
- `BehaviorTreeInstance.cpp`（`bbChange/needrun` 脏标记 :591-647、LOD :228-267）
- `BTNodeEvent.cpp` + `BehaviorTreeManager.cpp`（事件节点 + 三级分发）
- `BTBlackboard.h` + `BehaviorTreeData.h`（27 种黑板类型）
