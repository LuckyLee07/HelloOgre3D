# 行为树执行引擎（alias: ai-behavior）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

响应式行为树运行时：组合/装饰/动作节点 + reactive 重评估，由 BehaviorTreeDriver 驱动 Lua 叶与 C++ 条件。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/behavior/`（Lua 树配置在 `bin/res/scripts/ai/behavior/`，见 [[ai-scripts]]）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `BehaviorNode.h` | 基类 | 3 态 RUNNING/SUCCESS/FAILURE + Tick/Reset |
| `BehaviorComposite.{h,cpp}` | 组合 | Sequence/Selector/**Parallel**/**RandomSelector**；`ConfigureReevaluation(reevaluateMs)` 反应式重评估 |
| `BehaviorDecorator.h` | 装饰 | Inverter/ForceSuccess/ForceFailure |
| `BehaviorAction.h` / `LuaBehaviorAction.h` | 动作 | OnInitialize/OnUpdate/OnCleanUp；Lua 回调叶 |
| `LuaCondition.h` | 条件 | 每帧求值无缓存 |
| `BehaviorTreeDriver.{h,cpp}` | 驱动 | IDecisionDriver；持所有节点所有权 + `NewXxx` 工厂（tolua 不暴露构造器） |
| `BehaviorTrace.h` | 调试 | label/status 逐帧 |

## 4. 公开能力要点

- 节点：Sequence/Selector/Parallel/RandomSelector/Inverter/ForceSuccess/ForceFailure/Wait/LuaAction/Condition。
- **G1（重评估）、G4（Parallel/Random）已完成**（2026-06）。

## 5. 约束与红线

- 节点所有权全归 driver；Lua 只经工厂拿指针，勿自管生命周期。
- 性能项：`BehaviorTreeDriver` 已支持 trace sampling（`HELLO_BT_TRACE_SAMPLE_INTERVAL` / `SetDebugTraceSampleInterval`）并在 RuntimeDiag 输出 `[BTStats]`；仍缺 instance pool / node result cache / blackboard dirty 依赖 / 距离 LOD。
- 开放：G2 事件节点（依赖统一事件总线）、G3 参数运行时求值、G5 子树复用。

## 6. 数据流 / 与其他模块关系

`AIController.SetDriverByType("bt") → BehaviorTreeDriver`；树由 Lua `BehaviorTreeLoader` 装配；每帧 `driver.Tick → root.Tick`，叶读写 [[ai-common]] Blackboard。

## 7. 验证策略

- 回归 sample：`Sandbox8`（BT 战斗兵，与 DT `Sandbox7` 行为应等价）。

## 8. 已知 gap / 相关文档

- `docs/design/behavior-tree-gap-analysis.md`（G1/G4 已完成、G2/G3/G5–G11 开放）、`docs/planning/ai-technical-iteration-plan.md`。
