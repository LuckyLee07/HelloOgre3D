# AIController 组件（alias: ai-controller）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

AI 控制面板组件：聚合 Blackboard + driver + VisionSensor + MemoryStore + PerceptionResultCache，提供感知驱动（TickPerception）、行为驱动（TickAI）、driver 切换。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/components/ai/AIController.{h,cpp}`

## 3. 关键类 / 文件

| 成员 | 角色 | 说明 |
|---|---|---|
| `m_blackboard` | 持有 | [[ai-common]] Blackboard，注入给 driver |
| `m_driver` (IDecisionDriver*) | 持有 | BT/DT/FSM，`SetDriverByType` 创建 |
| `m_visionSensor`/`m_memoryStore`/`m_perceptionCache` | 感知 | TickPerception 填充，见 [[ai-perception]] |

## 4. 公开能力要点

- `SetDriverByType("fsm"/"dt"/"bt")`、`GetBlackboard`、`HasEnemy`/`CanShootEnemy`/`GetEnemy`、`SetMovePosition`/`IsTargetReached`、`TickPerception`/`TickAI`。

## 5. 约束与红线

- **driver 工厂未做（P7）**：`SetDriverByType` 仍 string if-else + 各 `Set*Driver` inline `new`。
- `HasEnemy` 已改读 PerceptionResultCache（4b，等价）；`CanShootEnemy` **刻意不改**（它 re-publish EnemySighted 战术事件 + 用 shootDistance/requirePath=false 不同语义）。
- TickPerception 在 driver/Agent_Update 之前跑（保证缓存先填充）。
- owner 经 GetSoldierOwner 仍假设 SoldierObject（与 [[components]] P5 同源）。

## 6. 数据流 / 与其他模块关系

`SoldierObject.Update → TickAI → driver.Tick`；`AgentPerceptionSystem 或 inline → TickPerception → 写 Blackboard + 缓存`；指令经 AICommand → AgentActionContext。

## 7. 验证策略

- 回归 sample：`Sandbox6`(fsm)/`Sandbox7`(dt)/`Sandbox8`(bt) 全过；战斗看 pursue/fire + `-RuntimeDiag` 的 cache 行。

## 8. 已知 gap / 相关文档

- 待：driver 工厂/registry（P7）、Blackboard owner 泛化到 BaseObject。`docs/planning/ai-technical-iteration-plan.md`、`docs/design/architecture-improvement-plan.md` P7。
