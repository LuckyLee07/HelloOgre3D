# HelloOgre3D 长期迭代规划

> 上位：`docs/project-direction.md` 是北极星；本文把北极星拆成可执行的长期阶段。若本文与北极星冲突，以北极星为准。
>
> 定位：本文是后续 6-12 个月的主规划入口，用来统一 AI、性能、Chapter9 parity、架构解耦、工具链和长期生产级目标。细节实施仍落到 `docs/ai-technical-iteration-plan.md`、`docs/chapter9-parity-architecture-notes.md`、`docs/architecture-improvement-plan.md` 等专项文档。

## 1. 总体方向

`HelloOgre3D` 当前阶段仍以 AI 学习与实验沙盒推进，但每一步都应沉淀为生产级游戏项目 / 玩法运行时的基础设施。

一句话目标：

> 以 AI sample 为验证面，把感知、团队、战术、行为树、调试、性能和可复现能力逐步产品化。

核心原则：

- `sample` 是验证面，不是一次性演示脚本。
- Lua 负责讲清楚行为、配置 sample 和保留可读性。
- C++ 负责承载高频热路径、批量系统、缓存、调度和统计。
- 每个阶段必须能用 smoke、trace、perf counter 或固定 seed 验证。
- 不急于铺开 UGC、编辑器、大世界和完整 Def 管线；等 AI runtime、性能基线和服务边界稳定后再进入。

## 2. 阶段一：可比较、可复现、可测量

目标：先让项目能稳定判断行为差异和性能退化来自哪里。

优先任务：

- 固化 `Sandbox16` 作为 AI pressure / `ai_perf_*` 性能入口。
- 固化 `Sandbox17` / `Sandbox18` 作为 Chapter9 Lua 对照面和 C++ 生产化实现面对照。
- 补 Release x64 的 `ai_perf_100 / 500 / 1000` 基线。
- 扩展 `ai_perf` 对照维度：spatial on/off、scheduler on/off、perception cache on/off、Lua callback count。
- 将 parity trace 正式化：位置、速度、目标点、当前 action、动画状态、physics/render 差值、influence draw 高度和投影来源。
- 固定 sample profile：随机 seed、spawn、movement、perception、tactics、debug draw、smoke 期望。

验收标准：

- 行为差异能从 trace 解释，而不是只靠截图猜。
- 性能退化能定位到 spatial、perception、memory/blackboard、BT、Lua callback 或 debug draw。
- `Sandbox17` / `Sandbox18` 的 profile 和 trace 默认关闭、开关可控，不污染普通运行。

## 3. 阶段二：AI 热点 C++ system 化

目标：把已有 AI sample 背后的高频能力收口为可扩展 C++ 底座。

优先顺序：

1. `PerceptionResultCache`
   - 缓存 current target、last known、confidence、age、candidate count、scan cost。
   - BT / Lua 只读结果，不重复扫描世界。
   - 保持现有 Blackboard key 兼容。

2. `TacticalQueryService`
   - 收口 `FindBestSupportPosition`、`FindLowThreatPosition`、`ScorePosition`、`SampleLayer`。
   - Lua 只传 query 类型、权重和参数，不遍历网格。
   - 从 `ObjectManager` tactical facade 逐步迁出。

3. `TeamBlackboardService` 二期
   - 补 `SupportRequested`、`SupportResponded`、`FocusTarget`、`RetreatPoint`、`FormationSlot` 等 fact。
   - fact 支持 TTL、priority、source、统计和 sample 生命周期清理。
   - Lua `TeamBlackboard.lua` 保留薄 facade。

4. `InfluenceMapSystem` 二期
   - dirty region / interval 更新。
   - cover / crowd / support schema。
   - layer debug draw、draw height、projection mode、draw order 变成显式配置。

5. BehaviorTree runtime 生产化
   - instance pool。
   - node result cache。
   - blackboard dirty key 与节点依赖表。
   - tick bucket / distance LOD / per-frame budget。
   - trace sampling，关闭 trace 时没有明显额外开销。

验收标准：

- 1000 agent 场景避免全量每帧重算。
- Lua 行为节点不新增全局对象扫描。
- RuntimeDiag / FramePerf 能输出 perception、team、tactics、BT 各自成本。
- `Sandbox8` / `Sandbox10` / `Sandbox12` / `Sandbox15` / `Sandbox18` 行为不退化。

## 4. 阶段三：AI、移动、动画、物理的意图链路

目标：减少“AI 语义对了但表现漂移”的问题。

目标链路：

```text
AI 决策
  -> MovementIntent / CombatIntent
  -> Locomotion
  -> Physics 权威位置
  -> Render 同步
  -> AnimationIntent
```

优先任务：

- 先把 `MovementIntent` / `AnimationIntent` 作为 trace 字段落地，不急于重写所有 action。
- Move / Pursue / Shoot / Reload / Flee / Idle 等 action 统一记录意图。
- Locomotion 根据意图处理速度、路径、目标点和 repath。
- Animation 根据意图和实际速度选状态，而不是被多个 action 直接改状态。
- physics transform 与 render transform 的差值进入 trace。

验收标准：

- 动作偏慢、卡顿、脚底不贴地、追击不流畅时，trace 能说明是 AI、Locomotion、Animation、Physics 还是 Render 同步问题。
- `Sandbox6` / `Sandbox7` / `Sandbox8` / `Sandbox17` / `Sandbox18` 行为不退化。

## 5. 阶段四：生产级运行时边界

目标：让 sandbox 从“能跑 sample”收口到“能维护、能测试、能扩展”的运行时边界。

优先任务：

- `SandboxServices` 第一切片继续推进：对象查询、导航、物理世界、debug draw、tactical query 通过服务访问。
- `ObjectManager` 减负：registry、update、AI scheduling、debug draw、navmesh、tactics 逐步拆成薄服务。
- 新代码禁止继续新增 `g_ObjectManager` / `g_SandboxMgr` / `g_GameManager` 直接依赖。
- 组件化继续深化：新能力优先通过 typed component 获取，不再给 `SoldierObject` 继续加单组件转发。
- Lua/C++ 生命周期治理：callback token、对象销毁、sample reload、UI debug panel 开关都要能安全清理。

验收标准：

- 新增 agent 类型不需要复制 Soldier 的大量 facade。
- 关键系统可在服务注入下单独验证或最小 smoke。
- 新增 C++ system 有明确 owner、生命周期、debug summary 和回归入口。

## 6. 阶段五：调试工具与工程门禁

目标：让生产级能力不只存在于代码里，也能被稳定验证。

优先任务：

- AI RuntimeDiag / 文本面板统一：perception snapshot、memory、team facts、influence score、BT trace、scheduler stats。
- FGUI debug panel 与 AI debug 信息保持同一观测风格。
- `run_sandbox_smoke.ps1` 覆盖关键 sample 和 `ai_perf_*` preset。
- Release x64 成为性能回归默认基线，Debug 只做诊断补充。
- `git diff --check`、Lua 语法检查、目标 sample smoke 作为最小验证组合。

验收标准：

- 改 AI 热点系统时有最小命令验证。
- 性能数据能长期对比，不再只记录单次截图或手感。
- 文档中的“完成”能对应实际 smoke、trace 或 perf 证据。

## 7. 阶段六：编辑器、UGC、数据管线后置进入

目标：在 AI runtime 和验证体系稳定后，再进入内容生产工具链。

进入条件：

- AI runtime 有可复现 sample 和 Release 性能基线。
- Perception / Team / Tactical / BT runtime 的服务边界稳定。
- 事件系统、配置 schema、生命周期清理已经能支撑一个完整垂直切片。
- 至少有一个接近生产级的 NPC 行为回路：感知、记忆、团队、战术、移动、动画、debug、perf 都能闭环。

后续方向：

- 轻量 Def / schema / manifest。
- 触发器运行时和 ECA。
- 可视化行为树 / 触发器编辑器。
- UGC / Mod 打包。
- 大世界 streaming。

这些方向不是取消，而是等待地基成熟后再小步引入。

## 8. 近期执行队列

建议下一轮按以下顺序推进：

| 顺序 | 任务 | 主要输出 | 验证 |
|---|---|---|---|
| 1 | Release x64 `ai_perf_1000` 基线 | Debug / Release 对照报告 | `run_sandbox_smoke.ps1` + FramePerf |
| 2 | `PerceptionResultCache` | C++ cache + Lua/BT 只读结果 | `Sandbox10` / `Sandbox16` / `ai_perf_*` |
| 3 | `TacticalQueryService` 第一版 | 独立 tactical query service | `Sandbox18` smoke |
| 4 | `InfluenceMapSystem` 二期 | interval / dirty region / layer debug draw | `Sandbox13` / `Sandbox18` |
| 5 | BT runtime cache / LOD 第一版 | ticked / skipped / cached / invalidated stats | `Sandbox8` / `Sandbox15` |
| 6 | AI RuntimeDiag 统一输出 | perception/team/tactics/BT/scheduler 一页摘要 | 指定 sample dump |
| 7 | `SandboxServices` 第一切片门禁 | 新代码服务访问约束 | 静态 grep + sample smoke |

## 9. 暂缓清单

短期不优先做：

- 完整 ECS 框架引入。
- 大量新增 Lua AI task。
- 完整 Def / CSV / CreatureAssembler 管线。
- 触发器编辑器和积木编辑器。
- UGC / Mod 打包。
- 世界 streaming。
- 一次性重写 `ObjectManager` / `GameManager`。

## 10. 成功标志

下一阶段成功不是“文档更多”或“sample 更多”，而是：

- 100 / 500 / 1000 agent 的性能曲线清楚。
- Chapter9 差异能用 trace 解释。
- Lua 不再承担感知、战术、团队共享等热路径扫描。
- C++ system 有明确边界、统计和 smoke。
- 每个 AI 概念 sample 都能继续运行，并且更容易解释、复现和回归。
