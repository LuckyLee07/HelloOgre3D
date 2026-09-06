# Sample 场景（alias: samples）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

Sandbox1-19 是 AI 学习章节 + 回归面：每个隔离场景演示一个 AI 概念/工程特性，是 smoke 验证入口；Sandbox19 首次把已有 AI/对象/战斗能力收成可操作纵切片。

## 2. 源码位置

- `bin/res/scripts/samples/`（agent 脚本在 `bin/res/scripts/agent/`，见 [[ai-scripts]]）

## 3. Sandbox 清单

| Sandbox | 演示 | | Sandbox | 演示 |
|---|---|---|---|---|
| 1 | 物理/射击基础 | | 10 | 感知/记忆/lastKnown |
| 2 | 转向行为 | | 11 | 多单位感知/通信 |
| 3 | 骨骼动画状态机 | | 12 | TeamBlackboard(C++) |
| 4 | FSM 直接控制 | | 13 | 影响图(Lua 教学) |
| 5 | 路径/navmesh 可视化 | | 14 | **= Sandbox13 别名空壳** |
| 6 | 间接控制 FSM + 导航 | | 15 | **= Sandbox13 别名空壳** |
| 7 | 决策树(DT) | | 16 | 感知压力(ai_perf) |
| 8 | 行为树(BT) | | 17 | Chapter9 战术(Lua-first) |
| 9 | Chapter7 知识源 | | 18 | Chapter9 战术(C++) |
| 19 | 战术指挥遭遇战（无武器 commander + BT AI 小队 + 玩家可读意图 + 三波闭环） | | | |

入口：`game_init.lua`(选 HELLO_SANDBOX_SAMPLE，默认 Sandbox19)、`fgui_init.lua`、`parity_trace.lua`、`runtime_diagnostics.lua`。

## 4. 公开能力要点

- 各 sample `Sandbox_Initialize` + 键盘/鼠标事件；是 AI 行为回归基线。

## 5. 约束与红线

- **Sandbox14/15 是 Sandbox13 的别名空壳**（4 行 require，无独立实现）；要独立需新建 preset+入口（行为变更，基线后做）。
- sample 是回归面：改 AI/对象/组件/绑定必须回归对应 sample（smoke `status=PASS`）。
- Sandbox17/18 是 Chapter9 对照面，受 `run_chapter9_parity_gate`/`visual_capture` 守。
- Sandbox19 的 `commander_soldier` / `ai_soldier` profile 必须保持 controller 互斥；commander 只装 `PlayerController`、不装 `AIController` / `WeaponComponent`，启动脚本会直接断言该约束。
- Sandbox19 第三人称相机由 `PlayerController` 在 `onSandboxServicesChanged` 进 FOLLOW、`onDetach` 退回 FREELOOK（防污染其它 sample），控制为 tank 式（A/D 转向、W/S 沿朝向前后、无横移无鼠标转向）；圆盘雷达经 `SandboxUI:CreatePolygon`（`UIPolygon` 封装 `Gorilla::Polygon`）建浅蓝圆盘 + 静止朝上三角箭头 + 复用红/绿圆点 blip 池，每帧按玩家朝向投影（player-up）。原 FairyGUI 程序化裸对象在本项目不渲染（仅 `.fui` 包视图才渲染），已弃用。
- Sandbox19 指令层（2026-08-04，cycle-01 W1；2026-08-13 W2/W3 收口）：RMB 点选/框选友军、Tab 全选存活友军、F 集火 / T 撤退 / G 编队。
  指令写 agent blackboard 的 `command.*` 命名空间（三指令互斥，撤退/编队复用 MoveAction 的 `movePos`）+ 一份 TeamBlackboard typed fact；
  TTL 过期、集火目标失效、波次结束和重开都会清理命令及命令拥有的 `movePos`。当前 commander 无武器，LMB 不再射击。
  AI 侧走 preset 指定的 `Sandbox19CommandBT` + `Sandbox19CommandConditions`，共享 `SoldierBT` 零改动，见 [[ai-behavior]]。
  `[Sandbox19CommandSelfTest]` / `[Sandbox19IntentSelfTest]`（`HELLO_SANDBOX_SMOKE_MODE` 门控）覆盖指令命中、互斥、enemy 覆写、TTL 过期和生命周期清理。
- Sandbox19 意图表达（cycle-01 W2）：友军头顶持续显示 `FOCUS / RETREAT / RALLY / HOLD / ENGAGE / SEARCH / INVESTIGATE / MOVE / PATROL` 及原因；
  指令目标用屏幕标记表达，雷达友军 blip 使用意图颜色，集火敌人高亮。点选诊断默认关闭，只能用 `HELLO_SANDBOX19_INPUT_DIAG=1` 显式开启。
- Sandbox19 对局节奏（cycle-01 W3）：`PREPARE → WAVE × 3 → INTERMISSION → VICTORY/DEFEAT`，固定 seed/出生点、波间清命令与小队恢复；
  `sample_presets.lua` 的 `commanderMatch` 配置准备/波间时长、友军数和每波敌人数，阶段转换输出 `[Sandbox19Match]`，smoke 用 `[Sandbox19MatchSelfTest]` 验证基础装配与第一波。
- **`ObjectManager:getObjectById` 未导出给 Lua**（`.h` 有声明但不在 tolua 块内）：Lua 侧按 id 找对象只能扫
  `getAllAgents()`；顺带避免跨帧持有 agent userdata 的悬垂风险。
- sample reload 须清理上轮 agent/UI/debug draw；Sandbox19 还须清选择集/拖拽/集火标记并 `TeamBlackboard:Reset()`。

- M1 单单位观察（2026-09-05）：Sandbox19 右侧观察面板默认隐藏（2026-09-06 试玩调整），按 I 开启，以最小选中 id 为观察对象；`I` 开关、`O` 导出最后 snapshot 与 BT trace。`ai_observer.lua` 只读采样（200ms），只在选中对象上临时开启 trace，切换/关闭恢复原设置；只保留 id 与标量/字符串，不跨帧保留对象 userdata。
- 指令有效性由只读 `Observer.Command` 读取，清理由 sample `_MaintainCommands` 执行；`_BuildIntentForAgent` 不再承担指令清理。死亡、过期和重开清理应保持独立于面板开关。
- `HELLO_SANDBOX19_OBSERVATION_SELF_TEST=1` 显式开启真实多帧 M1 自测；入口 `python3 tools/run_m1_smoke.py`，详见 `docs/cycle-01.md` 的审计、用例与验证边界。

## 6. 数据流 / 与其他模块关系

`game_init → require SandboxX → Sandbox_Initialize`（创建 agent、影响图、team）；驱动 [[ai-scripts]] [[ai-controller]] 等。

## 7. 验证策略

- `tools/run_sandbox_smoke.ps1 -Sample SandboxX -StopExisting -NoTail`；Chapter9 跑 parity_gate；战术指挥切片跑 `Sandbox19` smoke 后，再手动验收移动/选择/下令、意图可读性、三波节奏和重开清理。

## 8. 已知 gap / 相关文档

- 待：Sandbox14/15 独立化、parity trace 正式化、smoke 入 CI。`docs/archive/long-term-iteration-plan.md` §1/§5。

- [2026-09-06 Computer Use 试玩与改进](../playtest-2026-09-06.md)：选择恢复、命令倒计时、可读 HUD、结算说明；含实际操作证据与未验证边界。
