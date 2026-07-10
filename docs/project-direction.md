# 项目方向总纲（北极星）

> 这份文档是 `HelloOgre3D` 的最高层方向定调。做路线取舍时先看这里；如果其他 `docs/` 文档与本文冲突，以本文为准。

## 1. 项目身份

`HelloOgre3D` 当前阶段以 **AI 学习与实验沙盒** 为主要推进方式；长期目标是演进为 **生产级游戏项目 / 生产级玩法运行时**。

> 方向说明（2026-07-02，经讨论修订）：长期品类 **暂不锁定**。核心决定是：**让这套 AI / 寻路 / 多 agent 技术落到一个“能上手玩”的应用上，避免只做纯技术深挖而没有应用场景**。至于具体做成什么游戏（战术指挥 / FPS / 中间件……）**不在纸上定，而是先在现有战斗 sample 里加“玩家操控一个单位”的最小可玩切片，用实际手感来定方向**。
>
> 因此：
> - 暂缓清单（Lua 生物 Def / CSV、触发器、可视化编辑器、UGC、存档、世界 streaming）不再一刀切“永久后置”，改为 **“按纵切片实际需要激活”**：哪一条挡住了可玩切片就激活哪一条，不主动铺开。
> - 存在一批 **无悔动作**（不管最终品类是什么都用得上）：AI 底座收口（感知 / 团队 / 战术 C++ 化）、地基清理（见 `docs/design/architecture-improvement-plan.md` §9 的 P8 / P9）、性能基线。这些可在品类未定时先推进。
> - 注：此前一版曾把终点写死为“生产级 AI 运行时 / 中间件”，现修订为上述“应用接地、品类由切片探路”。

现阶段不直接铺开完整生产级数据驱动沙盒、UGC 或编辑器，而是通过一个可运行、可观察、可逐步扩展的 sandbox，系统学习并验证游戏 AI 与玩法基础设施的核心能力：

- FSM、DecisionTree、BehaviorTree 的执行模型与取舍。
- Blackboard、感知、记忆、知识源、团队通信、影响力地图、战术层。
- AI 行为与运动、寻路、转向、战斗动作之间的衔接。
- AI 调试、日志、trace、性能观测与可复现验证。

sample 仍然是核心资产：它们是当前阶段的 AI 学习章节、实验场和回归面。后续改动应尽量让某个 sample 更清楚地展示一个 AI 概念，同时把对象、组件、脚本、调试、性能和数据边界沉淀成可复用的生产级地基。编辑器、UGC 或大世界内容生产是长期目标的一部分，但现阶段只在能支撑当前验证面时小步引入。

## 2. 核心主轴：AI 能力递进 + 生产级地基

当前主线从泛化的“内容数据化”收敛为“AI 学习阶梯”，但每一层都应尽量按生产级项目的模块边界、可观测性和可验证性设计：

```text
基础对象 / 组件 / 服务上下文
  -> AI driver 统一入口
    -> Blackboard + 行为树 / 决策树 / FSM
      -> 感知 + 记忆 + 知识源
        -> 团队通信 + TeamBlackboard
          -> InfluenceMap + 战术选择
            -> AI 调试面板 / trace / 性能观测 / 可复现测试
              -> 稳定运行时 / 数据驱动 / 工具链 / 编辑器 / UGC
```

判断基准：

- 能让 AI 概念更清楚、更可观察、更容易实验的，升优先级。
- 能保留并强化 Sandbox6 / Sandbox7 / Sandbox8 等 AI sample 的，升优先级。
- 脱离当前 AI 验证面、一次性铺开完整 Def 管线、触发器编辑器、UGC、世界 streaming 的，暂缓。
- 数据配置可以作为 AI 实验的辅助，但不是当前主目标。
- 能沉淀稳定对象系统、脚本边界、调试观测、性能基线和工程化验证的，按生产级方向小步推进。
- 能让 100 / 500 / 1000 agent 场景避免全量每帧重算，并沉淀 spatial / AOI、分桶调度、结果上限、Lua/C++ 热路径边界和 Release 性能基线的，升优先级。

## 3. 近期优先级

| 优先级 | 方向 | 完成定义 |
|---|---|---|
| P0 | AI 感知与记忆闭环 | agent 能发现敌人、写入 blackboard、丢失后保留最后已知位置，并在 sample 中可观察。 |
| P0 | 行为树 / Blackboard 可观测性 | 能看到当前 BT trace、关键 blackboard、感知目标和决策理由摘要。 |
| P0 | 保持 AI driver / 组件入口清爽 | 新增 AI 能力优先走 `AIController`、组件和 `SandboxServices`，减少全局单例和对象层转发。 |
| P1 | 团队通信与 TeamBlackboard | 一个 agent 发现敌人后，同队 agent 能共享信息并改变行为。 |
| P1 | InfluenceMap 与战术移动 | AI 能根据危险/偏好区域选择移动目标，并有 debug 显示。 |
| P1 | AI 更新调度与性能观测 | 多 agent 场景下感知、知识源、影响力地图、BT tick 不会每帧集中全量更新，并能用 100 / 500 / 1000 agent preset 记录 Debug / Release 基线。 |
| P1 | 生产级运行时边界 | 新增能力有清晰对象/组件/服务/脚本边界，并能被 sample、smoke 或日志验证。 |
| P2 | 录制、回放、可复现测试 | 固定随机种子后 sample 行为可复现，异常行为能从日志中解释。 |

暂缓方向：

- Lua 生物 Def / CreatureAssembler / CSV 管线。
- TriggerRuntime / TriggerVolume / ECA 触发器玩法切片。
- 可视化积木编辑器、触发器对象库、UGC / Mod 打包。
- 世界 streaming。

这些能力不是永久否定，而是长期生产级目标的一部分；只是当前不作为主线一次性铺开。后续如果某个能力能直接服务 AI 学习目标、生产级地基收口或清晰的垂直切片，再以更小范围重新引入。

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

## 5. 当前落地切片

当前唯一实现主线是 **`Sandbox19` 可玩战术遭遇战**：玩家与 AI 复用同一个 `SoldierObject`，只通过互斥的 `PlayerController` / `AIController` 切换控制来源。

这条切片已经串起：

- 玩家相机相对移动、朝向、射击与换弹。
- AI 感知、BehaviorTree、团队关系和战斗组件。
- FPS 相机锚点、小地图、HUD、胜负与重置。
- `player_soldier` / `ai_soldier` profile 的 controller 互斥断言。

下一步先做手动手感验收和小范围硬化，再依据实际体验决定偏 FPS、战术指挥或继续保持 AI 实验场；不在验收前铺开新的系统主线。

## 6. 文档关系

| 文档 | 当前定位 |
|---|---|
| `docs/planning/ai-roadmap.md` | AI 长期愿景和能力地图，近期排期以本文和 `docs/planning/long-term-iteration-plan.md` 为准。 |
| `docs/planning/long-term-iteration-plan.md` | 长期迭代总规划，把北极星拆成阶段、近期执行队列和暂缓清单。 |
| `docs/planning/ai-technical-iteration-plan.md` | AI 技术落地顺序，承接 spatial、perception、team、tactics、BT runtime 与 benchmark。 |
| `docs/reference/minigame-ai-production-reference.md` | MiniGame 生产级 AI 性能与架构参考；只吸收热路径系统化、AOI、缓存、调度和 benchmark 思路，不照搬历史包袱。 |
| `docs/planning/high-priority-todo.md` | 当前迭代任务清单，必须随方向变化更新。 |
| `docs/design/behavior-tree-gap-analysis.md` | 行为树学习与补强参考。 |
| `docs/archive/trigger-system-gap-analysis.md` | 触发器方向暂缓，作为历史参考。 |
| `docs/archive/foundation-modules-inventory.md` | 生产级地基盘点参考；近期只选能支撑当前验证面的部分。 |
| `docs/archive/project-roadmap.md` | 长期生产级路线参考，阶段优先级低于本文。 |
| `docs/archive/visual-editor-implementation-plan.md` / `docs/archive/visual-editor-task-breakdown.md` | 长期编辑器工具链参考；近期只在支撑当前验证面时推进。 |

## 7. 工作原则

- 优先让 AI 行为可运行、可观察、可解释。
- 每次新增 AI 能力都尽量绑定一个 sample 或 smoke 验证面。
- 保留小步迭代，但每一步都应该推进一个明确 AI 概念或生产级地基能力。
- 不为了“未来可能做编辑器/UGC”提前污染当前代码结构；但当前新增能力要尽量按长期生产级项目的边界设计。
- 如果历史文档或历史提交引入了偏离当前阶段目标、且不能服务长期生产级边界的实现，优先做正向清理，而不是继续在上面叠功能。
