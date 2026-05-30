# 项目方向总纲（北极星）

> 这份文档是 `HelloOgre3D` 的最高层方向定调。做路线取舍时先看这里；如果其他 `docs/` 文档与本文冲突，以本文为准。

## 1. 项目身份

`HelloOgre3D` 当前目标重新收敛为 **AI 学习与实验沙盒**。

项目重点不是先做生产级数据驱动沙盒、UGC 或编辑器，而是通过一个可运行、可观察、可逐步扩展的 sandbox，系统学习并验证游戏 AI 的核心能力：

- FSM、DecisionTree、BehaviorTree 的执行模型与取舍。
- Blackboard、感知、记忆、知识源、团队通信、影响力地图、战术层。
- AI 行为与运动、寻路、转向、战斗动作之间的衔接。
- AI 调试、日志、trace、性能观测与可复现验证。

sample 仍然是核心资产：它们是 AI 学习章节、实验场和回归面。后续改动应尽量让某个 sample 更清楚地展示一个 AI 概念，而不是优先服务编辑器、UGC 或大世界内容生产。

## 2. 核心主轴：AI 能力递进

主线从“内容数据化”调整为“AI 学习阶梯”：

```text
基础对象 / 组件 / 服务上下文
  -> AI driver 统一入口
    -> Blackboard + 行为树 / 决策树 / FSM
      -> 感知 + 记忆 + 知识源
        -> 团队通信 + TeamBlackboard
          -> InfluenceMap + 战术选择
            -> AI 调试面板 / trace / 性能观测 / 可复现测试
```

判断基准：

- 能让 AI 概念更清楚、更可观察、更容易实验的，升优先级。
- 能保留并强化 Sandbox6 / Sandbox7 / Sandbox8 等 AI sample 的，升优先级。
- 单纯推进 Def 管线、触发器编辑器、UGC、世界 streaming 的，暂缓。
- 数据配置可以作为 AI 实验的辅助，但不是当前主目标。

## 3. 近期优先级

| 优先级 | 方向 | 完成定义 |
|---|---|---|
| P0 | AI 感知与记忆闭环 | agent 能发现敌人、写入 blackboard、丢失后保留最后已知位置，并在 sample 中可观察。 |
| P0 | 行为树 / Blackboard 可观测性 | 能看到当前 BT trace、关键 blackboard、感知目标和决策理由摘要。 |
| P0 | 保持 AI driver / 组件入口清爽 | 新增 AI 能力优先走 `AIController`、组件和 `SandboxServices`，减少全局单例和对象层转发。 |
| P1 | 团队通信与 TeamBlackboard | 一个 agent 发现敌人后，同队 agent 能共享信息并改变行为。 |
| P1 | InfluenceMap 与战术移动 | AI 能根据危险/偏好区域选择移动目标，并有 debug 显示。 |
| P1 | AI 更新调度与性能观测 | 多 agent 场景下感知、知识源、影响力地图不会每帧集中全量更新。 |
| P2 | 录制、回放、可复现测试 | 固定随机种子后 sample 行为可复现，异常行为能从日志中解释。 |

暂缓方向：

- Lua 生物 Def / CreatureAssembler / CSV 管线。
- TriggerRuntime / TriggerVolume / ECA 触发器玩法切片。
- 可视化积木编辑器、触发器对象库、UGC / Mod 打包。
- 世界 streaming。

这些能力不是永久否定，只是当前不作为主线；后续如果某个能力能直接服务 AI 学习目标，再以更小范围重新引入。

## 4. 已保留与已清理的方向

保留：

- `SandboxServices` 与组件依赖收口。
- 行为树 `Parallel` / `Random` / reactive 重评估。
- Blackboard 的 object-id、数组、运行时参数读取。
- AI 感知查询接口与 `AgentPerceptionResult`。
- 组件 owner 访问收敛与 typed component 查询。

清理：

- Lua 生物 Def 与 `CreatureAssembler`。
- `TriggerRuntime` / `TriggerVolume` / `BehaviorEventRuntime`。
- 数据驱动触发器到 BT 的 `Sandbox9` 垂直切片。
- 只为上述切片服务的 BT `Event` 节点入口。

## 5. 下一个学习切片

下一阶段建议围绕 **感知、记忆、最后已知位置** 做 AI 学习切片：

> 一个敌人进入视野后，AI 将目标 id、位置、距离写入 blackboard；敌人离开视野后，AI 保留 lastKnown 位置并尝试移动过去；到达后如果仍未重新发现目标，则回到巡逻或待机。

这条切片适合作为当前 AI 主线，因为它串起了：

- 感知查询。
- Blackboard。
- BehaviorTree 条件和 action。
- 运动 / 寻路。
- 可观察 debug 输出。

它比 Def/Trigger/UGC 更贴近“学习 AI”的目标，也能继续复用已经完成的 BT 与感知基础。

## 6. 文档关系

| 文档 | 当前定位 |
|---|---|
| `docs/ai-roadmap.md` | AI 长期路线事实来源，优先参考。 |
| `docs/high-priority-todo.md` | 当前迭代任务清单，必须随方向变化更新。 |
| `docs/behavior-tree-gap-analysis.md` | 行为树学习与补强参考。 |
| `docs/trigger-system-gap-analysis.md` | 触发器方向暂缓，作为历史参考。 |
| `docs/foundation-modules-inventory.md` | 数据驱动地基盘点暂缓，作为历史参考。 |
| `docs/project-roadmap.md` | 非 AI 路线历史参考，优先级低于本文。 |
| `docs/visual-editor-implementation-plan.md` / `docs/visual-editor-task-breakdown.md` | 编辑器方向暂缓，作为历史参考。 |

## 7. 工作原则

- 优先让 AI 行为可运行、可观察、可解释。
- 每次新增 AI 能力都尽量绑定一个 sample 或 smoke 验证面。
- 保留小步迭代，但每一步都应该推进一个明确 AI 概念。
- 不为了“未来可能做编辑器/UGC”提前污染当前代码结构。
- 如果历史文档或历史提交引入了偏离 AI 学习目标的实现，优先做正向清理，而不是继续在上面叠功能。
