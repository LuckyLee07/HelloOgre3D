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
| `m_driver` (IDecisionDriver*) | 持有 | BT/DT/FSM，`SetDriverByType` 走 driver factory/registry 创建 |
| `m_visionSensor`/`m_memoryStore`/`m_perceptionCache` | 感知 | TickPerception 填充，见 [[ai-perception]] |

## 4. 公开能力要点

- `SetDriverByType("fsm"/"dt"/"bt")`、`GetBlackboard`、`HasEnemy`/`CanShootEnemy`/`GetEnemy`、`SetMovePosition`/`IsTargetReached`、`TickPerception`/`TickAI`。

## 5. 约束与红线

- `SetDriverByType` 仅做字符串归一化，实际创建走 driver factory/registry；新增 driver 类型应注册 factory，不再扩散 inline `new`。
- 对象查询、寻路和战术事件发布只通过 `SandboxServices.objects` / `SandboxServices.sandbox` 获取服务；缺服务时走空指针降级，不再回退全局 `g_*`。
- `HasEnemy` 已改读 PerceptionResultCache（4b，等价）；`CanShootEnemy` 仍保留 shootDistance/requirePath=false 的重查语义并 re-publish EnemySighted，成功路径会同步 PerceptionResultCache。
- TickPerception 在 driver/Agent_Update 之前跑（保证缓存先填充）。
- DT/BT driver 创建已只依赖 `AgentObject* + Blackboard*`；`GetSoldierOwner` / `GetOwner` 保留为旧 Lua/Soldier 脚本兼容桥（与 [[components]] P5 同源）。
- FSM 通用条件通过 `AgentActionContext` 读取 AIController/Weapon/Attrib，不再让 evaluator 或 Move/Shoot/Pursue/Reload state 直接 cast `SoldierObject`。

## 6. 数据流 / 与其他模块关系

`AgentObject/SoldierObject.Update → TickAI → driver.Tick`；`AgentPerceptionSystem 或 inline → TickPerception → 写 Blackboard + 缓存`；指令经 AICommand → AgentActionContext。

## 7. 验证策略

- 回归 sample：`Sandbox6`(fsm)/`Sandbox7`(dt)/`Sandbox8`(bt) 全过；战斗看 pursue/fire + `-RuntimeDiag` 的 cache 行。

## 8. 已知 gap / 相关文档

- 待：Blackboard Lua owner 兼容面、DeathState 与 Soldier 动画表现桥继续从 SoldierObject 适配中拆出。`docs/planning/ai-technical-iteration-plan.md`、`docs/design/architecture-improvement-plan.md` P5/P7。
