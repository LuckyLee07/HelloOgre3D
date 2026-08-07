# 项目方向总纲（北极星）

> 这份文档是 `HelloOgre3D` 的最高层方向定调。做路线取舍时先看这里；如果其他 `docs/` 文档与本文冲突，以本文为准。

## 1. 项目身份

`HelloOgre3D` 当前阶段以 **AI 学习与实验沙盒** 为主要推进方式；长期目标是演进为 **生产级游戏项目 / 生产级玩法运行时**。

> 方向说明（2026-07-02，经讨论修订）：长期品类 **暂不锁定**。核心决定是：**让这套 AI / 寻路 / 多 agent 技术落到一个“能上手玩”的应用上，避免只做纯技术深挖而没有应用场景**。至于具体做成什么游戏（直接操控 / 战术指挥 / 中间件……）**不在纸上定，而是先在现有战斗 sample 里加“玩家操控一个单位”的最小可玩切片，用实际手感来定方向**。
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

- 玩家第三人称相机（A/D 平滑转向角色偏航、W/S 沿朝向前后、弹簧跟随后上方，照搬 code-master FollowCamera 数学 + tank 式控制）、朝向、射击与换弹。
- AI 感知、BehaviorTree、团队关系和战斗组件。
- 现有观察相机、HUD、胜负与重置。（小地图 viewport 已于 2026-07-10 移除；2026-07-11 雷达改为 Gorilla 矢量雷达 `UIPolygon`：左上角浅蓝圆盘、敌红友绿圆点 blip、player-up、中心三角箭头。原 FairyGUI 程序化裸对象在本项目不渲染，已弃用。）
- `player_soldier` / `ai_soldier` profile 的 controller 互斥断言。

手感验收已于 2026-07-11 由用户确认通过（A/D 转向、W/S 前后、射击方向、相机跟随、圆盘雷达、重开无残留均正常）。**FPS 相机仍不作为当前优先项**（第三人称 FOLLOW 已作为经确认的方向调整加入、限定 Sandbox19，与 FPS 是两回事）。

### 5.1 品类决策：待 cycle-01 结束时落档

切片跑通了，但"偏直接操控 / 战术指挥 / 继续做 AI 实验场"的结论**至今未做**——2026-07-11 验收后悬空了三周。原因是这个决策程序本身欠定：用一个一天做出来的原型的手感，承载不了"长期做什么"。

因此改为**用 artifact 回答，而不是用思辨回答**：`docs/cycle-01.md` 做战术指挥切片（玩家只下指令、不直接开枪），4 周，交付一个别人能玩 5 分钟的 build，并找真人试玩。**2026-08-31 必须在本节写下结论，包括"结论是不做游戏、继续当技术 lab"也算有效结论。**

### 5.2 项目运作方式（2026-08-04 起）

不再维护 6–12 个月长期路线图（对个人项目是负资产，只会不断提醒还有多少没做完）。改为：

- **北极星**（本文）——方向定调，不含状态。
- **当前 cycle**（`docs/cycle-01.md`）——一次只有一个，4–6 周，三行定义（问题 / 产出 / 截止），到期必须结束并写结论。
- **候选池**（`docs/backlog.md`）——唯一带 `[ ]`/`[x]` 的清单，不排期。

每个 cycle 必须交付下列三者之一，交不出也要结束、写结论、进下一个：① 一个别人能玩 5 分钟的 build；② 一篇能给同事讲 20 分钟的技术总结；③ 一个能搬进真实工程的模块或结论。

#### cycle 类型交替

> **奇数 cycle 做应用（能玩 / 能看 / 能讲），偶数 cycle 做技术（能搬回生产）。**

过去两年半的失衡是**全技术、零应用**——因此永远没有完成感、没有观众，这是 2026-07-11 到 08-04 那三周停摆的真正原因。但纯做应用又会丢掉本项目对真实工作的价值（这套完整可观测的 AI 全链路在生产项目里几乎不可能从头搭）。交替能同时保住两头，且每个 cycle 结束时天然知道下一个是哪一类，不必重新纠结。

允许例外（例如把一个应用切片做完整需要连着两个应用 cycle），但例外要在 cycle 文件里写明理由。

#### 停止与暂停规则

项目烂尾从来不是决定停掉的，是无声滑走的。因此写死：

- **连续两个 cycle 交不出任何 artifact** → 承认它作为活跃项目已经结束，转为只读参考仓库，并在本文写明。这比挂着一个假装在推进的项目健康。
- **暂停必须显式**：不想做时在当前 cycle 文件里写「暂停至 X 日」，而不是让它自然沉默。
- **cycle 到期必须结束**：交不出也要写结论、进下一个。**不允许延期**——延期是这类个人项目最常见的死法。

#### 规划的边界

只规划到**下一个决策点**为止，不做跨决策点的详细排期。当前决策点是 `docs/cycle-01.md` 的品类结论（2026-08-31）；在它落档之前，cycle-02 及以后只以候选形态存在于 `docs/backlog.md`，不写成排期文档。

#### 防止文档再次膨胀

- `cycle-NN.md` 结束后移到 `docs/archive/cycles/`，主目录永远只有当前一个。
- `backlog.md` 只增删条目，**不写状态叙述**（历史进展查 `git log`）。
- 模块文档只在 doc-sync 时更新，不主动扩写。
- **不新建规划文档**；想规划就改 cycle 或 backlog。

## 6. 文档关系

| 文档 | 当前定位 |
|---|---|
| `docs/cycle-01.md` | **当前周期**。现在在做什么、什么时候结束。 |
| `docs/backlog.md` | **唯一活清单**。想做什么；只有这里有 `[ ]`/`[x]`。 |
| `docs/design/architecture-improvement-plan.md` | P1–C5 / P8–P11 的证据与方案（§7/§9）。backlog 只记做不做，细节看这里。 |
| `docs/reference/minigame-ai-production-reference.md` | MiniGame 生产级 AI 性能与架构参考；只吸收热路径系统化、AOI、缓存、调度和 benchmark 思路，不照搬历史包袱。 |
| `docs/design/behavior-tree-gap-analysis.md` | 行为树学习与补强参考。 |
| `docs/archive/ai-roadmap.md` / `long-term-iteration-plan.md` / `ai-technical-iteration-plan.md` / `high-priority-todo.md` / `playable-vertical-slice-todolist.md` | 2026-08-04 归档的历史排期，只作脉络与证据，不看旧优先级。 |
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
