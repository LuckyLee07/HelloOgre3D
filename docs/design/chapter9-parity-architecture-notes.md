# Chapter 9 复刻过程暴露的架构问题归档

> 日期：2026-06-11
>
> 范围：`src/HelloOgre3D` 与 `bin/res/scripts`。本文不是继续追 Sandbox17 / Sandbox18 的像素级调参，而是把这轮对齐 `HelloOgre3DX/src/chapter_9_tactics` 时暴露出的主要架构问题落档，方便后续按优先级优化。
>
> 相关背景文档：`docs/project-direction.md`、`docs/design/architecture-improvement-plan.md`、`docs/archive/SoldierObjectRefactor.md`、`docs/planning/ai-technical-iteration-plan.md`。

## 总体判断

当前最大问题不是“缺一个功能”，而是行为表现由多层零散系统共同决定：Lua sample、preset、agent script、C++ 行为树 action、AIController、Locomotion、Physics、Animation、InfluenceMap debug draw 都各管一段，但缺少一个清晰的“样例行为合同”。因此 Sandbox17 / Sandbox18 可以跑出 Chapter 9 的大体语义，却很难稳定复刻原版的细节节奏、移动观感、网格贴合和战术层显示。

换句话说，架构已经具备不少生产级地基的雏形，但许多边界还停在“能接上”的阶段，没有收口到“能解释、能复现、能回归”的阶段。

## 表现复刻工作原则

复刻旧版表现时，目标不是照搬旧版代码结构，而是在新版架构下复刻旧版可观察效果。旧版代码必须作为事实基准来读：它定义了坐标来源、更新时机、绘制语义、随机节奏、状态切换等表现合同；新版实现可以换模块、换数据结构、换服务边界，但不能在没有验证的情况下把表现语义改成“看起来更合理”的新方案。

后续做 Chapter9 或其他旧版效果对齐时，默认流程如下：

1. 先固定旧版表现基准：明确旧版截图 / trace / 日志来自哪一版代码；旧版未改动时不要重复截图，直接复用已有基准产物。
2. 逐段读旧实现：先确认旧版“看到的效果”由哪些代码决定，包括数据源、坐标、阈值、循环顺序、draw material、depth / y offset、更新频率。
3. 找新版职责对应点：不要只找同名类，而要找承担同一表现职责的新模块。
4. 列出差异再改：每个改动必须能对应到一条明确的新旧差异；不能只根据截图猜测去调 offset、颜色或参数。
5. 验证并可回退：用固定 seed、固定相机、固定时间点对比；改动不改善或解释不了差异时应撤回。

这次 influence grid 覆盖差异就是典型反例。旧版 `InfluenceMapUtilities` 用 navmesh 三角和 3D cell box 做 `triBoxOverlap`，但 `InfluenceMapDrawer` 绘制的是 cell 体素格底面；新版曾按 navmesh 三角表面计算 `surfaceY` 并贴面绘制。这个实现从“贴地 debug visual”角度看更合理，但它改变了旧版的可观察语义：坡道、台阶、小块边缘处会因为深度和高度差出现条纹、缺块和覆盖形状漂移。正确做法是保留新版 `InfluenceMapSystem` 架构，但让 debug cell 输出回到旧版等价的 cell 底面语义。

## 主要问题

### 1. Sample 表现没有明确合同

Sandbox17 / Sandbox18 的表现由 `sample_presets.lua`、`Sandbox17.lua`、`Sandbox18.lua`、agent 脚本和 C++ 系统共同拼出来。哪些参数是 Chapter 9 parity 必须保持的，哪些只是当前实现细节，目前没有单独声明。

直接后果是：视野角、感知频率、移动段时长、追击 repath、influence spread、debug draw y offset、agent script 入口等都可能影响表现，但它们散落在不同层。后续再调一次，很容易修好一个细节又漂掉另一个细节。

建议后续补一个 `Chapter9TacticsProfile` / `SampleBehaviorProfile` 概念，把以下内容集中声明：

- agent script 与 driver 类型。
- blackboard 默认值。
- movement / pursue / perception / tactics 参数。
- influence map 绘制模式、投影模式、cell size、draw offset。
- 需要固定的随机种子、出生点、事件节奏和 smoke / parity trace 期望。

### 2. 复刻缺少可量化对比链路

这轮问题里，“动作流畅度差”“步子迈得大”“移动偏慢”“网格偏上”都属于视觉观感问题。如果只有肉眼看截图或录像，很难定位是移动速度、动画播放速度、状态切换、路径目标、物理同步还是 draw offset 的锅。

建议把 parity trace 做成正式回归能力，而不是临时诊断工具。至少记录：

- agent 每帧位置、朝向、速度、目标点、路径剩余距离。
- 当前 BT node / action、RUNNING/SUCCESS/FAILURE。
- 当前动画状态、clip time、播放速度、blend 权重、状态切换原因。
- physics transform 与 render transform 的差值。
- influence layer 的 active cell、最高值、绘制高度和投影来源。

这样后续要对齐 Chapter 9 时，可以先看数据曲线，再决定是调动画、移动、AI 决策还是绘制层。

### 3. 行为、移动、动画的权威边界还不够清楚

项目已经有 `AICommand`、`AgentLocomotion`、`PhysicsComponent`、`RenderComponent`、`SoldierAnimController` 等分层，但实际表现仍然容易被多处共同影响。当前缺一个稳定的链路：

```text
AI 决策
  -> MovementIntent / CombatIntent
  -> Locomotion 速度与目标
  -> Physics 权威位置
  -> Render 同步
  -> AnimationIntent 与动画状态机
```

当这个链路不显式时，Sandbox17 这种“语义对齐但观感不对”的问题会很难查：AI action 可能每帧重设目标，Locomotion 可能分段移动，动画状态可能进入 slow movement，physics / render offset 也可能造成脚底不贴地的错觉。

建议后续把“移动意图”和“动画意图”作为一等数据记录到 trace，并减少 action 直接碰表现层细节。

### 4. AI 数据通道仍有分裂

DT / BT / FSM、Blackboard、AgentActionContext、Lua action 参数、sample preset 之间还没有完全统一。部分能力已经迁到 `AIController`，但 driver 创建、黑板注入、FSM 数据、Soldier 专属 owner 假设仍然让复用成本偏高。

这会影响两个方向：

- 做新 AI 对象类型时，容易被 `SoldierObject` 假设卡住。
- 做 sample parity 时，同一个“目标、敌人、移动点、状态意图”可能从不同数据通道读写，难以保证一致。

建议优先收口：

- driver factory / registry，避免在 `AIController` 里继续堆字符串分支。
- Blackboard owner 泛化到 `AgentObject` / `BaseObject`。
- FSM 与 BT / DT 尽量共享同一套 Blackboard 或明确桥接规则。
- Lua action 的入参和 blackboard key 做 schema 化。

### 5. Lua 配置层缺少 schema 和继承边界

`sample_presets.lua` 已经承担了很多职责：sample 参数、AI blackboard、战术层配置、debug draw、legacy parity、smoke trace。它很方便，但也容易变成“所有差异都塞进去”的中心文件。

建议把配置分成几类：

- sample identity：名字、入口、agent 数量、spawn。
- behavior profile：AI driver、agent script、blackboard、移动/感知/追击参数。
- visual/debug profile：网格、材质、y offset、info panel、trace。
- parity profile：固定随机、legacy 对照、预期事件节奏。

同时为 preset 做一个轻量校验：缺 key、拼错 key、类型错误时在启动阶段直接报清楚，而不是运行时表现漂移。

### 6. InfluenceMap 的教学版与 C++ 版边界需要再清楚

Sandbox17 是 Lua-first 版本，Sandbox18 是 C++ InfluenceMapSystem 第一切片。两者现在语义相近，但视觉与性能目标并不完全相同：Lua 版更适合教学对照，C++ 版更适合生产化、限频和统计。

如果后续要继续对齐 Chapter 9，需要明确：

- Lua legacy parity 是“原书效果对照面”。
- C++ InfluenceMapSystem 是“生产化实现面”。
- 两者可以共享事件语义和 profile，但不要把 legacy 的每个视觉细节都硬塞成 C++ 默认行为。

建议把点源扩散、navmesh 投影、网格贴地、层透明度、draw order、cell 轮廓绘制做成显式配置，并在 debug summary 里打印当前模式。

### 7. sandbox 仍然受全局 manager 和反向依赖牵制

已有文档里提过 `SandboxServices` 方向，这轮复刻也再次暴露了这个问题：AI、物理、导航、对象查询、debug draw、UI 信息面板仍然经常通过全局 manager 或跨层访问拼起来。

这会导致：

- 单独测试某个 AI / movement / influence 系统困难。
- 很难并行跑两个 world 或两个 sample profile。
- 某个 sample 的调参容易污染其他 sample。

建议继续推进 `SandboxServices`，优先迁移高频热点：

- AIController 查对象、查导航。
- PhysicsComponent 取 physics world。
- InfluenceMap / Tactics 系统查对象和 debug draw。
- sample smoke / trace 取运行时服务。

### 8. 对象组件化仍有 legacy facade 包袱

`AgentObject` / `SoldierObject` 已经挂了不少组件，但对象层仍保留大量转发接口，组件 owner 也还有具体子类假设。结果是：看起来组件化了，但新增一种 agent 或复用某个能力时，还是容易复制 Soldier 的 facade。

后续优化不建议大拆，而是按“停止新增 legacy forwarder”来推进：

- 新能力优先通过 typed component / component key 访问。
- 老 Lua API 保留，但标记为 legacy facade。
- 常用组件方法补齐 tolua 暴露，让 Lua 不必绕回 SoldierObject。
- 新 agent 类型作为试金石，验证不用复制 SoldierObject 大量转发也能跑起来。

### 9. Manager 职责仍偏胖

`ObjectManager` 同时承担 registry、update、AI 调度、输入转发、navmesh、debug visual、tactical influence 绘制等职责。`GameManager` 也还留有 UI / Lua / 输入 / 服务聚合的历史包袱。

这类问题短期不一定影响 sample 跑起来，但会影响定位和回归。建议后续按职责拆出薄服务：

- ObjectRegistry：对象注册与查询。
- ObjectUpdateSystem：对象和组件 update 顺序。
- TacticalDebugDrawService：战术层绘制。
- SampleRuntime / SampleServices：sample 入口拿服务，不直接穿透 manager。

### 10. 所有权与生命周期仍有旧式风险

raw pointer、`SAFE_DELETE`、Lua callback、tolua 对象生命周期这些历史问题还在。它们不一定是当前 Chapter 9 视觉差异的直接原因，但会让调试不稳，尤其是 sample 频繁切换、脚本热重载、UI debug panel 开关时。

建议中期推进：

- 组件容器改 `unique_ptr` 管理所有权。
- callback token 集中管理并在析构时统一解绑。
- Lua 持有 C++ 对象时明确 owner / observer 语义。
- debug 工具里增加对象销毁、组件 detach、Lua env 清理的统计。

## 建议优先级

### P0：先补“能比较”的能力

先不要继续靠肉眼追 Chapter 9 细节。优先把 Sandbox17 / Sandbox18 的 movement、animation、AI action、influence draw 关键数据 trace 出来，并把 Chapter 9 parity profile 固化。这样后续每次改动都有数据对比。

验收：

- 固定 seed 跑 Sandbox17 / Sandbox18，能输出稳定 trace。
- trace 能说明某个 agent 在同一时间点的位置、速度、动画状态、当前 action。
- influence debug summary 能打印当前绘制模式和高度来源。

### P1：收口 profile 与配置 schema

把 Chapter 9 相关配置从“散落 key”收口为明确 profile，给 preset 做启动校验。先服务 Sandbox17 / Sandbox18，不追求一次性覆盖所有 sample。

验收：

- Chapter 9 profile 的关键行为参数集中可见。
- 缺字段或类型错能启动时报错。
- legacy parity 与 C++ production profile 差异可以一眼看清。

### P2：打通 AI / movement / animation 的意图链路

明确 AI action 只产出意图，Locomotion 负责移动，Animation 根据意图和实际速度选状态。短期可以先加 trace 和薄封装，不必马上重写所有 action。

验收：

- move / pursue / shoot 不直接各自随意改动画状态。
- 动画状态切换能从 trace 解释。
- Sandbox6/7/8/17 行为不退化。

### P3：推进 SandboxServices 与组件化减负

按热点逐步减少 `g_*` 访问和对象层 forwarder。不要大规模搬目录，先让新代码走正确路径，旧 facade 稳定保留。

验收：

- AIController / PhysicsComponent / InfluenceMapSystem 的关键依赖从 services 注入。
- 新增 agent 类型可以复用 AIController / Locomotion / Render / Physics 的一部分能力。
- 老 sample 行为不变。

## 处理路线建议

### 第一阶段：先止血，补对比与合同

这一阶段不改核心行为，只补“能看清差异”的地基，风险最低，收益最高。

1. 固化 `Chapter9TacticsProfile`。
   - 把 Sandbox17 / Sandbox18 里影响表现的关键参数集中成 profile：driver、agent script、blackboard、movement、pursue、perception、influence draw、spawn、seed。
   - 保留两个明确分支：`legacyParity` 用于对照原 Chapter 9；`cppProduction` 用于 C++ 化实现。
   - 启动时打印 profile 摘要，避免之后不知道当前跑的是哪套参数。

2. 把 parity trace 做成正式回归工具。
   - 先覆盖位置、速度、目标点、当前 action、动画状态、physics/render 差值、influence draw 高度。
   - trace 只在开关打开时跑，默认不影响 sample。
   - 之后再遇到“看起来不自然”，先看 trace 曲线，而不是直接调魔法数。

3. 为 preset 加轻量校验。
   - 先只校验 Chapter 9 相关 profile：字段是否存在、类型是否正确、关键数值是否落在合理范围。
   - 错误在 sample 启动阶段报清楚，不让表现漂移静默发生。

### 第二阶段：收口行为链路，减少表现漂移

这一阶段开始动代码边界，但仍然以薄封装和 trace 为主，不做大重写。

1. 引入 `MovementIntent` / `AnimationIntent` 的最小形态。
   - AI action 产出“我要移动/追击/射击”的意图。
   - Locomotion 根据意图驱动速度、路径、目标点。
   - Animation 根据意图和实际速度选状态。
   - 短期可以先是结构体 + trace 字段，不必马上重写所有 action。

2. 统一 move / pursue / shoot 对动画的影响方式。
   - 避免多个 action 分别直接调用动画状态切换。
   - 保留旧接口作为 facade，但新路径统一写 intent。
   - 验收以 Sandbox6/7/8/17 不退化为准。

3. 明确 InfluenceMap 的两种目标。
   - Lua 版继续作为教学和 Chapter 9 对照面。
   - C++ 版作为生产化实现面，重点是可配置、限频、统计、query。
   - 绘制高度、nav 投影、grid outline、draw order 都变成显式配置，并写进 debug summary。

### 第三阶段：拆全局依赖，推进服务化

这一阶段处理 `architecture-improvement-plan.md` 里的地基问题，但要按热点小步迁移。

1. 落 `SandboxServices` 的第一切片。
   - 先不要抽太多接口，先把服务对象按值传进去。
   - 优先迁移 AIController 查对象/导航、PhysicsComponent 取 physics world、InfluenceMap/Tactics 查对象与 debug draw。
   - `g_*` 保留为填充 services 的来源，不急着删。

2. 给 sandbox 组件加静态门禁。
   - 新代码禁止直接 include `GameManager` / `ClientManager`。
   - 新代码禁止在组件层新增 `g_ObjectManager` / `g_SandboxMgr` / `g_GameManager` 直接访问。
   - 先做检查提示，再逐步变成 CI/脚本门禁。

3. 把 sample runtime 访问收口。
   - sample smoke、trace、debug panel 通过 sample services 访问运行时状态。
   - 避免每个 sample 都直接穿透 manager 找对象、找导航、找绘制入口。

### 第四阶段：收 AI 数据通道和组件债

这一阶段更偏中期重构，建议等前面 trace/profile 稳定后再做。

1. AIController driver factory / registry。
   - `SetDriverByType("bt")` 继续保留给 Lua。
   - 内部从字符串 if-else 改成 enum / registry。
   - 新增 driver 不再改 AIController 主体分支。

2. Blackboard owner 泛化。
   - 从 `SoldierObject*` 收到 `AgentObject*` 或 `BaseObject*`。
   - 先保证非 Soldier agent 能挂 AIController 并读写 blackboard。
   - FSM 的 AgentActionContext 与 Blackboard 做桥接或明确边界。

3. 停止新增对象层 legacy forwarder。
   - 新 Lua 和新 C++ 代码优先拿 typed component。
   - 老 SoldierObject / AgentObject API 保留兼容，但标注 legacy。
   - 补齐关键组件 tolua 暴露，减少 Lua 被迫绕回对象层。

4. 所有权和生命周期后置处理。
   - 组件容器 `unique_ptr`、callback token 统一解绑、Lua/C++ owner 语义这些都重要，但 blast radius 大。
   - 建议等 sample trace 稳定后，按组件类型逐个迁移。

### 推荐实际排期

| 顺序 | 任务 | 类型 | 主要收益 | 风险 |
|---|---|---|---|---|
| 1 | Chapter9 profile 固化 + 启动摘要 | Lua / 配置 | 防止参数继续散落 | 低 |
| 2 | parity trace 正式化 | Lua + 少量 C++ 查询 | 后续差异可量化 | 低 |
| 3 | preset schema 校验 | Lua | 错误早暴露 | 低 |
| 4 | MovementIntent / AnimationIntent trace | C++ / Lua 边界 | 找到动作不自然根因 | 中 |
| 5 | InfluenceMap visual/profile 显式化 | Lua + C++ debug | 网格贴地和显示模式可控 | 中 |
| 6 | SandboxServices 第一切片 | C++ 架构 | 降低全局耦合 | 中 |
| 7 | driver factory + Blackboard owner 泛化 | C++ AI | 支撑更多 agent 类型 | 中高 |
| 8 | legacy forwarder 减负 + ownership 迁移 | C++ 对象模型 | 兑现组件化收益 | 高 |

## 当前落地记录

- 2026-06-11：新增 `bin/res/scripts/config/chapter9_tactics_profile.lua`，把 Chapter 9 profile 定义、关键字段校验、启动摘要和 trace 元数据集中到独立模块。
- 2026-06-11：`chapter9_tactics_lua`、`chapter9_tactics_legacy_parity`、`chapter9_tactics_cpp` 已分别标记为 `luaTeaching`、`legacyParity`、`cppProduction`。
- 2026-06-11：Sandbox17 / Sandbox18 启动时会打印 `[Chapter9Profile]` 摘要；`ParityTrace` 的 start 记录会带 profile 元数据。
- 2026-06-11：Sandbox18 补了轻量 parity snapshot，默认不开，开启 `HELLO_PARITY_TRACE=1` 时输出 agent 与 tactical query 关键状态。
- 2026-06-11：新增 `ActionIntent` trace；Move / Pursue / RandomMove / Idle / Shoot / Reload / Flee / Die action 在 `HELLO_PARITY_TRACE` 或 `HELLO_INTENT_TRACE` 开启时记录 movement / animation intent，Sandbox17 / Sandbox18 的 parity snapshot 会带上对应 intent。

## 不建议短期做的事

- 为了 1:1 复刻 Chapter 9，把 Sandbox17 整体改回 legacy Lua 风格。
- 一次性重写 ObjectManager / GameManager。
- 在没有 trace 的情况下继续微调大量 movement / animation 魔法数。
- 把 Lua 教学版和 C++ 生产版强行做成完全同一个实现。

## 一句话总结

当前架构已经能表达 Chapter 9 的战术语义，但缺少 profile、trace、意图链路和服务边界，所以细节表现很难稳定复刻；后续应先把“可比较、可解释、可回归”的地基补齐，再逐步收口 AI、移动、动画和战术绘制的边界。
