# 可玩纵切片 TODOLIST

> 上位：`docs/project-direction.md`、`docs/planning/long-term-iteration-plan.md`。
>
> 当前唯一主线：**先完成“玩家操控一个单位”的最小可玩纵切片**。FPS 相机、小地图 viewport、V 键切换视角都归为这条主线的支撑能力；P8-P11 架构 review 归为后续债务，不作为当前实现主线。

## 1. 主线边界

目标不是现在决定最终游戏品类，也不是继续横向堆更多系统，而是在现有战斗 sample 中做出一个能上手验证手感的闭环：

```text
玩家输入
  -> PlayerController / human driver
  -> 复用现有 SoldierObject 组件
  -> 移动 / 朝向 / 射击 / 换弹
  -> AI 单位继续运行
  -> FPS / 小地图辅助观察
```

验收口径：

- 玩家能在一个现有战斗 sample 中控制一个单位。
- AI 单位仍按原 sample 逻辑感知、决策、移动和攻击。
- 相机与小地图只服务观察和操作，不反向决定最终玩法品类。
- 纵切片跑通后，再用实际手感决定后续偏战术指挥、FPS、AI sandbox 还是其它方向。

## 2. 当前未提交改动归类

| 改动 | 归类 | 收口方式 |
|---|---|---|
| `docs/project-direction.md` / `docs/planning/long-term-iteration-plan.md` 改为“先做可玩纵切片” | 主线定义 | 保留，作为当前阶段方向 |
| `CS_FPS`、`HELLO_CAMERA_FPS`、V 键切换视角 | 支撑能力 | 保留为纵切片调试/操作视角 |
| 小地图 viewport | 支撑能力 | 保留为战场观察能力，后续决定默认开关 |
| weapon view model 调试参数 | 支撑能力 | 暂作为 FPS 视角可用性验证 |
| `docs/design/architecture-improvement-plan.md` P8-P11 | 后续债务 | 不进入当前可玩切片的完成标准 |
| Release x64 build / Sandbox8 smoke 验证记录 | 当前基线 | 保留为本轮改动已可运行的证据 |

## 3. P0 - 已完成基线

- [x] 明确近期主线：在现有战斗 sample 里先做“玩家操控一个单位”。
- [x] 新增 FPS 相机状态 `CS_FPS`，支持 `HELLO_CAMERA_FPS` 环境变量切入。
- [x] 新增 V 键在 `CS_FPS` 和 `CS_FREELOOK` 之间切换。
- [x] 新增小地图 camera / viewport，用于俯视观察战场。
- [x] FPS 视角增加 weapon view model 和 `HELLO_FPS_WPN_*` 调试参数。
- [x] 更新 runtime / project direction / long-term planning 文档，使方向与当前代码改动一致。
- [x] 通过 `git diff --check`、架构静态检查、Release x64 构建、`Sandbox8` smoke。

## 4. P1 - 可玩纵切片

- [x] 新建 `Sandbox19` 作为第一个承载 sample。
  - 一个玩家 `SoldierObject`、两个友方 AI、四个敌方 AI，覆盖移动、射击、死亡、胜负与重置。
- [x] 新增 `PlayerController` / human driver。
  - 输入映射到玩家单位意图，而不是直接绕过组件改 scene node。
  - 复用 `SoldierObject`、`AgentLocomotion`、`WeaponComponent`、`AnimComponent`、`AgentAttrib` 等现有组件。
  - 不给 `SoldierObject` 继续新增大量单组件 facade。
- [x] 建立互斥 controller profile。
  - `player_soldier` 只装 `PlayerController`，`ai_soldier` 只装 `AIController`；不新增 `PlayerObject`。
  - `Sandbox19` 启动时对 `player` / `ai` 组件互斥关系做可执行断言。
- [x] 接入基础操作。
  - 移动：前后左右或目标点移动，先追求闭环，不做复杂手感。
  - 朝向：跟随相机或鼠标方向，先保证射击方向可解释。
  - 射击：复用 `WeaponComponent` 现有发射链路。
  - 换弹/冷却：优先复用已有武器状态，避免单独造一套玩家武器逻辑。
- [x] 处理 AI 与玩家关系。
  - AI 能把玩家单位当作合法目标。
  - 玩家单位不破坏原 sample 的 team / perception / blackboard 逻辑。
  - 死亡、重置或 sample reload 不留下悬挂输入目标。
- [x] 增加最小可观测信息。
  - HUD 显示玩家 HP、弹药、存活友军/敌军和胜负状态，日志输出 player id 与组件清单。
- [x] 自动验证。
  - Lua 语法、sandbox 架构门禁、Release x64 构建、`Sandbox19` smoke、`Sandbox8` AI 回归均通过。
- [ ] 手动手感验收。
  - 手动运行目标 sample：能控制一个单位移动和射击。
  - AI 单位仍能感知、攻击或执行原 sample 行为。
  - FPS / free-look 切换不导致输入目标丢失或相机崩溃。

## 5. P2 - FPS 与小地图硬化

这些任务只有在 P1 跑通后再做，避免相机功能变成新的主线。

- [ ] 确认 FPS 相机默认策略。
  - `HELLO_CAMERA_FPS=1` 可作为调试入口。
  - 是否默认 FPS，需要等玩家控制切片手感验证后再决定。
- [ ] weapon view model 资源硬化。
  - 缺资源时只记录一次清晰日志，不产生每帧噪声。
  - 调整默认 offset / rotation / scale，使近景可用。
- [ ] 小地图 viewport 策略。
  - 决定是否增加环境变量开关。
  - 处理窗口 resize、aspect 和 UI overlay 的层级关系。
- [ ] 补一个 FPS 模式 smoke 或手动验证记录。
  - 至少覆盖 `HELLO_CAMERA_FPS=1` 启动和 V 键切换。

## 6. P3 - 纵切片后再处理的技术债

以下内容不挡住 P1，但会影响后续性能基线和长期维护。

- [ ] P9：清理 `AgentObject` update throttling 死代码。
  - 移除或解释 `if (true || ...)`。
  - 移除或解释 `forceUpdate = true`。
  - 再刷新 Release x64 `ai_perf_100/500/1000` 干净基线。
- [ ] P9：缓存热点组件查询。
  - 优先处理高频 `FindComponent<T>` 扫描点。
  - 保持组件 attach/detach 生命周期清楚。
- [ ] P9：删除或收口 `USE_CPP_FSM` 死宏。
- [ ] P8：处理 component 对 `ObjectManager` / 具体对象类型的反向依赖。
- [ ] P10：收窄 `BaseObject` typed getter 继续扩散的问题。
- [ ] P11：收口 event dispatcher manager 过度设计问题。

## 7. 非目标

- [ ] 不在本轮决定最终品类。
- [ ] 不在本轮引入完整 ECS。
- [ ] 不在本轮铺开完整 Def / CSV / CreatureAssembler。
- [ ] 不在本轮做触发器编辑器、UGC、存档或世界 streaming。
- [ ] 不为了玩家控制复制一套平行的玩家对象层级。

## 8. 推荐提交边界

如果后续需要拆提交，建议按以下边界拆：

1. 方向与 TODO 文档：把“先做可玩纵切片”确认为唯一主线。
2. 相机观察基础：FPS 相机、小地图 viewport、V 键切换。
3. 玩家控制纵切片：`PlayerController` / human driver 与目标 sample 接入。
4. 纵切片后的债务：P9 性能债，再进入 P8/P10/P11 架构清理。

