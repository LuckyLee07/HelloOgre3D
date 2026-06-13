# 有限状态机执行引擎（alias: ai-fsm）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

FSM 运行时：状态注册、转移（布尔/evaluator）、当前状态驱动；AgentStateController 实现 IDecisionDriver，可与 BT/DT 互换。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/fsm/`、`src/HelloOgre3D/sandbox/ai/fsm/states/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `AgentStateController.{h,cpp}` | 驱动 | IDecisionDriver；持 AgentFSM + AgentActionContext；`AddState/AddStateExByLua/AddTransition` |
| `AgentFSM.{h,cpp}` | 容器 | 状态注册、转移表、转移求值、当前状态 |
| `AgentState.h` | 基类 | OnEnter/OnLeave/OnUpdate(dt) |
| `AgentStateFactory.h` | 工厂 | `RegisterCreator<T>(type)` / `Create(type, agent)` |
| `states/*State.h` | 预置 | Idle/Move/Pursue/Shoot/Reload/Flee/RandomMove/Death + `AgentLuaState`(Lua 回调) |
| `AgentActionContext.h` | 上下文 | 封装导航/寻路/射击命令 |

## 4. 公开能力要点

- 9 预置 state + Lua 状态；evaluator 自动转移或 Lua `RequestState`。

## 5. 约束与红线

- **`RandomMoveState` 是桩**：OnEnter 选完随机点立刻 `SetTerminated(true)`、OnUpdate 空 → FSM 随机移动不工作（DT/BT 的 Lua RandomMoveAction 才完整）。
- 转移须先 `AddTransition` 声明，未声明的被拒。
- FSM 不拥有状态对象（工厂/Lua 持有）。

## 6. 数据流 / 与其他模块关系

`AIController.SetDriverByType("fsm") / InitDefaultDriver → AgentStateController`；每帧 `Tick → 转移求值 → currState.OnUpdate → ApplySteering`。关联 [[ai-controller]] [[ai-common]]。

## 7. 验证策略

- 回归 sample：`Sandbox6`（FSM + Num 键切状态）。

## 8. 已知 gap / 相关文档

- 待：RandomMoveState 补全或弃用（行为变更，基线后做）。`docs/planning/ai-technical-iteration-plan.md`。
