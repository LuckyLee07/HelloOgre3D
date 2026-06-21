# AI 脚本库（alias: ai-scripts）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

Lua 侧 AI 行为库：DT/BT action+condition、知识源、Lua 影响图、团队黑板 facade、agent 行为骨架。

## 2. 源码位置

- `bin/res/scripts/ai/`、`bin/res/scripts/agent/`

## 3. 关键文件

| 路径 | 角色 |
|---|---|
| `ai/decision/`（SoldierDecisionTree、SoldierEvaluators、actions/*、ActionIntent、MoveHelpers） | DT 树/评估/动作 |
| `ai/behavior/`（SoldierBehaviorTree、SoldierConditions、BehaviorTreeLoader、config/） | BT 树/条件/加载 |
| `ai/knowledge/`（KnowledgeSource、Chapter7Knowledge） | 知识源 |
| `ai/tactics/InfluenceMap.lua` | Lua 影响图（逐格扩散） |
| `ai/team/TeamBlackboard.lua` | C++ 服务的 Lua facade |
| `ai/events/AIEvents.lua` | Lua AI 事件 facade：规范 Chapter8/9 event table、TeamBlackboard EnemySighted payload、Chapter9 legacy message payload，并转发到 C++ tactical event service |
| `agent/*Agent.lua` | Soldier/Decision/Behavior/Indirect/Knowledge/Chapter9/PerceptionPressure 各驱动骨架 |

## 4. 公开能力要点

- DT/BT 树装配、condition/action、知识源→Blackboard、Lua 影响图层、团队 fact、AI event table 规范化、agent 生命周期。

## 5. 约束与红线

- **Lua 仍干重活**（待下沉 C++）：`InfluenceMap.lua` 逐格 radial 扩散、Sandbox11 视线双重 for；`Chapter7Knowledge` 已改读 C++ perception/cache 结果。
- `ActionIntent` 已加 movement/animation intent trace。
- DT(`Sandbox7`) 与 BT(`Sandbox8`) 行为应等价。
- `AIEvents.lua` 是 Lua 侧事件发布 facade；Sandbox11 Chapter8 comms、TeamBlackboard EnemySighted 事件、Sandbox17/18 Chapter9 事件发布与 Chapter9 legacy message 队列已走它，Sandbox17 legacy callback bridge 带 sample/owner guard，且 `chapter9_tactics_legacy_parity` smoke 验证 stale sample/owner callback 不会发布事件；后续关注通用 reload/destroy callback 生命周期。
- Lua callback（evaluator/condition）在 reload/agent 销毁须清理（见 [[scripting-tolua]]）。

## 6. 数据流 / 与其他模块关系

`agent:setPluginEnv → require *Agent.lua → 初始化 DT/BT/Knowledge`；每帧 driver.Tick 回调 Lua action/condition；读写 [[ai-common]] Blackboard、查 [[ai-tactics]]/[[ai-team]]。

## 7. 验证策略

- 回归 sample：`Sandbox7/8`(DT/BT 对标)、`Sandbox9`(知识)、`Sandbox13`(影响图)；`run_chapter9_parity_gate`；legacy event bridge 生命周期 guard 用 `chapter9_tactics_legacy_parity` preset。

## 8. 已知 gap / 相关文档

- 待：Lua 重活下沉 C++（PerceptionResultCache/TacticalQueryService/InfluenceMap 二期）、callback 生命周期治理。`docs/planning/ai-technical-iteration-plan.md`、`docs/design/chapter9-parity-architecture-notes.md`。
