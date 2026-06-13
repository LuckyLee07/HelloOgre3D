# 决策树执行引擎（alias: ai-decision）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

决策树运行时：Branch（Lua evaluator 选子）+ Action（三阶段生命周期），DecisionTreeDriver 驱动并管理所有权。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/decision/`（Lua 树在 `bin/res/scripts/ai/decision/`，见 [[ai-scripts]]）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `DecisionNode.h` | 基类 | `Resolve()` 返回选中 Action |
| `DecisionBranch.h` | 分支 | `SetEvaluator(luaRef)`，Resolve 调 evaluator 取 **1-based** 子索引 |
| `DecisionAction.h` / `LuaDecisionAction.h` | 动作 | UNINITIALIZED→RUNNING→TERMINATED 状态机；Lua 叶 |
| `DecisionTree.h` | 容器 | Resolve→Initialize→Update→CleanUp 生命周期 |
| `DecisionTreeDriver.{h,cpp}` | 驱动 | IDecisionDriver；持所有节点所有权；`NewTree/NewBranch/NewLuaAction` |

## 4. 公开能力要点

- 节点：Branch / Action / LuaAction。同一时刻仅一个 Action RUNNING（线性，无并行/重评估）。

## 5. 约束与红线

- evaluator 必须返回 **1-based** 索引。
- driver 持所有 Branch/Action/Tree 所有权；Lua 不保活。
- Action 终结后从根重新 Resolve（无分支续跑）。
- 与 [[ai-behavior]] `Sandbox8` 行为应等价，可交叉验证。

## 6. 数据流 / 与其他模块关系

`AIController.SetDriverByType("dt") → DecisionTreeDriver`；树由 Lua `SoldierDecisionTree` 装配；共享 [[ai-common]] Blackboard。

## 7. 验证策略

- 回归 sample：`Sandbox7`（DT 战斗兵）。

## 8. 已知 gap / 相关文档

- 无单独 gap 文档；作为 Chapter6 教学样本保留。`docs/planning/ai-technical-iteration-plan.md`。
