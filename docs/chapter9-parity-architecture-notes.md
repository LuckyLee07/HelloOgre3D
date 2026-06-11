# Chapter 9 复刻过程暴露的架构问题归档

> 日期：2026-06-11
>
> 范围：`src/HelloOgre3D` 与 `bin/res/scripts`。本文不是继续追 Sandbox17 / Sandbox18 的像素级调参，而是把这轮对齐 `HelloOgre3DX/src/chapter_9_tactics` 时暴露出的主要架构问题落档，方便后续按优先级优化。
>
> 相关背景文档：`docs/project-direction.md`、`docs/architecture-improvement-plan.md`、`docs/SoldierObjectRefactor.md`、`docs/ai-technical-iteration-plan.md`。

## 总体判断

当前最大问题不是“缺一个功能”，而是行为表现由多层零散系统共同决定：Lua sample、preset、agent script、C++ 行为树 action、AIController、Locomotion、Physics、Animation、InfluenceMap debug draw 都各管一段，但缺少一个清晰的“样例行为合同”。因此 Sandbox17 / Sandbox18 可以跑出 Chapter 9 的大体语义，却很难稳定复刻原版的细节节奏、移动观感、网格贴合和战术层显示。

换句话说，架构已经具备不少生产级地基的雏形，但许多边界还停在“能接上”的阶段，没有收口到“能解释、能复现、能回归”的阶段。

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

## 不建议短期做的事

- 为了 1:1 复刻 Chapter 9，把 Sandbox17 整体改回 legacy Lua 风格。
- 一次性重写 ObjectManager / GameManager。
- 在没有 trace 的情况下继续微调大量 movement / animation 魔法数。
- 把 Lua 教学版和 C++ 生产版强行做成完全同一个实现。

## 一句话总结

当前架构已经能表达 Chapter 9 的战术语义，但缺少 profile、trace、意图链路和服务边界，所以细节表现很难稳定复刻；后续应先把“可比较、可解释、可回归”的地基补齐，再逐步收口 AI、移动、动画和战术绘制的边界。
