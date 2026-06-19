# AI 公共基础（alias: ai-common）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

AI 子系统三大基础：Blackboard（黑板）、AIScheduler（tick 调度预算）、AICommand（指令协议）、IDecisionDriver（BT/DT/FSM 统一接口）。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/common/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `Blackboard.{h,cpp}` | 知识库 | 7 类标量(Agent/Float/Int/Bool/String/Vec3/ObjectId)+4 类数组；entry 带 confidence/ttl/source/decay；无脏标记每帧全量 |
| `AIScheduler.{h,cpp}` | 调度 | `Configure(enabled,tickIntervalMs,maxTicksPerFrame)`；`ShouldTick(agentId)` 错峰预算；ticked/skipped 统计 + Tracy；Lua 全局 `SandboxAIScheduler` 直接配置/诊断 |
| `AIUpdateSystem.{h,cpp}` | AI 帧编排 | 无状态 helper；统一执行 scheduler begin/tick、spatial rebuild、批量 perception、TeamBlackboard sync 与 AI perf stats 写回 |
| `AICommand.h` | 指令 | 11 类（Idle/Move/Attack/MoveTo/RequestState/UseSkill/...）静态工厂 |
| `IDecisionDriver.h` | 接口 | `Init()/Tick(deltaMs)`；实现者 BT/DT/FSM driver |

## 4. 公开能力要点

- Blackboard 类型化读写（tolua 导出）；AIScheduler interval/budget/round-robin，已导出为 `SandboxAIScheduler`；AICommand 动作通道。

## 5. 约束与红线

- Blackboard 无脏标记、条件每帧全量求值（性能项 G7 未做）。
- 缺 Object(by id)/Timer/嵌套数组类型（G6）。
- AICommand 种类固定在枚举，新增需改 enum + KindToString。

## 6. 数据流 / 与其他模块关系

[[ai-controller]] 创建并持有 Blackboard，注入给 driver；[[ai-perception]] 写感知 key；BT/DT/FSM 读；`GameManager → SandboxAIScheduler → AIScheduler` 负责 Lua 配置/诊断，`ObjectManager` 每帧通过 `AIUpdateSystem` 统一执行 scheduler tick 判定、批量感知和 TeamBlackboard sync。

## 7. 验证策略

- 回归 sample：`Sandbox9`（Knowledge→Blackboard）、`Sandbox16`（scheduler 压力）、带 `RuntimeDiag` 的 `Sandbox12`（scheduler summary）；`ai_perf_1000`。

## 8. 已知 gap / 相关文档

- 待：G6 黑板类型扩展、G7 条件缓存/脏标记。`docs/design/behavior-tree-gap-analysis.md`、`docs/planning/ai-technical-iteration-plan.md`。
