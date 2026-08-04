# Sandbox19 战术指挥切片 W1：指令输入层设计

- **日期**：2026-08-04
- **作者**：lizhibao
- **状态**：已批准

> 上位：`docs/project-direction.md`（北极星）、`docs/cycle-01.md`（当前周期，截止 2026-08-31）。
> 注入模块：`docs/modules/ai-team.md`、`docs/modules/components.md`、`docs/modules/samples.md`。

## 1. 目的与背景

`docs/cycle-01.md` 的问题是：已经造好的 AI 底座（TeamBlackboard typed fact / InfluenceMap / TacticalQuery / BT）能不能直接变成一种玩法。W1 交付其中的**指令输入层**——玩家作为小队长，能选中友军并下达集火 / 撤退 / 编队三个指令，AI 有可观察反应。

这些 typed fact（`FocusTarget` / `RetreatPoint` / `FormationSlot`）在 `TeamBlackboardService` 里已经跑通、有 TTL / priority / smoke 检查，但至今只有日志见过它们。W1 把它们接到玩家输入上。

### 非目标

- AI 意图可视化（W2 的事，本轮只保证"有可观察反应"，不做解释性表达）
- 关卡节奏、胜负调整（W3）
- 改共享 `SoldierBT.lua` / `SoldierConditions.lua`
- 新增 AI system（北极星：边际价值已低）
- 指挥模式切换 UI / 模式状态机
- 持续命令队列、编队阵型编辑、指令历史

## 2. 设计原则

1. **玩法语义在 Lua，C++ 只补能力**。指令输入不是热路径，符合北极星"Lua 负责讲清楚行为、配置 sample；C++ 承载高频热路径"。
2. **共享 BT 零改动**。新建 Sandbox19 专用 BT 并用 `Subtree` 引用现有 combat 分支，`SoldierBT.lua` / `SoldierConditions.lua` 一行不动 → `Sandbox8` / `Sandbox17` / `Sandbox18` / chapter9 parity gate 零回归风险。
3. **玩家指令优先级高于 AI 自主决策**。指令走独立 `command.*` blackboard 命名空间 + 顶层最高优先级分支；不复用 `sense.*` / `formation.*`（前者被 `HearingDangerSense` 每帧覆盖，后者在 Selector 末位，交火时玩家会觉得"下了令不听"）。
4. **无模式切换**。`LMB` 射击（不变）、`RMB` 选择、`F/R/G` 下令。已验证 `OgreCameraController::injectMouseDown` 在 `CS_ORBIT` 之外直接 return，FOLLOW 模式下 RMB 空闲，省掉整套模式状态机。
5. **不新增拾取基础设施的重型版本**。有 `WorldToScreen` 后，点选=遍历友军投影取最近者（命中阈值 **48 屏幕像素**，超出视为空点选并清空选择集），框选=同一投影判矩形（拖拽超过 **8 像素**才判定为框选，否则按点选处理）；对 2–6 个单位比射线更准（不受胶囊碰撞体形状影响），且两个功能共用一个 C++ 方法。

## 3. 架构与组件

依赖流遵循 `AGENTS.md`：`runtime → sandbox → game`，Lua 在最上层。

| 层 | 文件 | 改动 |
|---|---|---|
| game | `src/HelloOgre3D/game/GameManager.cpp` | 鼠标事件派发 `EventHandle_Mouse(ctype, x, y, button)` 到 Lua。Lua 侧 stub 已存在但 C++ 从未调用过；与既有 `EventHandle_Keyboard` 派发对称，属该文件既有职责。 |
| sandbox/systems/service | `CameraService.{h,cpp}` | 新增 `Ogre::Vector2 WorldToScreen(const Ogre::Vector3&)`、`Ogre::Vector3 ScreenToGroundPoint(Real x, Real y, Real groundY)`，tolua **手术式**补绑定（禁 `tolua.bat` 全量重生成）。 |
| Lua sample | `bin/res/scripts/samples/Sandbox19.lua` | 选择集管理、RMB 点选 / 框选、F/R/G 指令语义、写 fact + blackboard、选中高亮与框选矩形绘制、HUD 提示、reload 清理。 |
| Lua BT 配置 | `bin/res/scripts/ai/behavior/config/Sandbox19CommandBT.lua`（新） | 顶层 `Selector = [commandBranch(新), Subtree → SoldierBT.combat]`。 |
| Lua BT 条件 | `bin/res/scripts/ai/behavior/Sandbox19CommandConditions.lua`（新） | `HasCommandFocus` / `HasCommandRetreat` / `HasCommandRally`，读 `command.*` 键。 |
| Lua agent | `bin/res/scripts/agent/BehaviorSoldierAgent.lua` | BT 模块 / 全局名 / 条件表改为从 preset 读，缺省仍是 `SoldierBT` + `SoldierConditions`（照搬 2026-07-12 落库的 `chapter8Config.agentScript or 默认值` 写法）。 |
| Lua preset | `bin/res/scripts/config/sample_presets.lua` | `Sandbox19` preset 指定 BT 模块为 `Sandbox19CommandBT`。 |

UI 全部复用现有 `UIFrame`（已有 `setPosition` / `setDimension` / `setBackgroundColor` / `setVisible`），不需要新 Gorilla 图元；`UIPolygon`（2026-07-12 落库）已被雷达占用，不在本轮扩展。

## 4. 数据流

```
RMB 按下/拖拽/抬起
  → GameManager::EventHandle_Mouse(ctype, x, y, button)
    → Sandbox19.lua 选择集（点选：WorldToScreen 投影取最近友军；框选：投影判矩形）

F / R / G 按下
  → EventHandle_Keyboard（既有通路）
    → Sandbox19.lua 指令语义
      ├→ TeamBlackboard:RememberFocusTarget / RememberRetreatPoint / RememberFormationSlot
      │    （C++ typed fact：团队级、TTL、priority、可被 RuntimeDiag 观测）
      └→ 逐个选中 agent 写 blackboard：
           command.focusTargetId / command.retreatPos / command.rallyPos / command.issuedMs
             → Sandbox19CommandBT.commandBranch 条件命中
               → 复用现有 action：shoot / pursue / move / moveToFormationSlot
```

**为什么两处都写**：typed fact 面向团队语义、TTL 与可观测性；agent blackboard 面向 BT 条件（BT 条件读 blackboard，不读 service）。这与 `Sandbox13.lua` 现有的 `RememberFocusTarget` + `WriteBestCppFactToBlackboard` 写法一致，不是新发明。

## 5. 指令语义

| 键 | 指令 | blackboard 键 | 目标来源 |
|---|---|---|---|
| `F` | 集火 | `command.focusTargetId` | RMB 点中的敌人；未点选则兜底取**玩家朝向 ±45° 扇形内最近的存活敌人**，扇形内无敌人则指令不下达并 HUD 提示 |
| `R` | 撤退 | `command.retreatPos` | 玩家当前位置（"到我这来"） |
| `G` | 编队 | `command.rallyPos` | 玩家周围按 slot 排列，复用 `Sandbox13.lua` `_FormationSlotOffset` 的偏移思路 |

三个指令互斥：新指令覆盖同一 agent 的旧指令（写入前清空另两个键）。

## 6. 错误处理与失败语义

- **TTL**：指令写 `command.issuedMs`，默认存活 8000ms；超时后条件返回 false，agent 自然落回 combat subtree。不做显式清理定时器。
- **目标失效**：集火目标死亡或 id 查不到 → `HasCommandFocus` 返回 false。
- **单位死亡**：从选择集移除；其 `command.*` 随对象销毁失效。
- **空选择下令**：无操作，HUD 提示一行（不静默）。
- **reload / restart**：清选择集、清所有选中 agent 的 `command.*`、调 `TeamBlackboard:Reset()`，与现有 `_RestartEncounter` 清理路径合并。
- **`ScreenToGroundPoint` 射线与地面平行**：返回玩家位置兜底，不返回 NaN。
- **窗口尺寸变化**：`EventHandle_WindowResized` 已有通路，框选矩形与高亮重新布局。

## 7. 与现有系统 / sample / 规则的关系

**遵守的红线**（来自注入的模块文档）：
- `components.md`：Controller 互斥——不新增 `PlayerObject`，不给 `SoldierObject` 加 facade。本设计不改 `PlayerController` 的控制语义（RMB 此前未被 PlayerController 使用）。
- `ai-team.md`：position 真源须来自感知 / `agent.GetPosition`，不信 Lua 坐标——指令位置取自 `agent:GetPosition()` 与玩家实时位置。`SetValue/GetValue` 不塞任意 key——本设计只用 typed fact 接口。
- `samples.md`：sample reload 须清理上轮 agent/UI/debug draw；`player_soldier`/`ai_soldier` controller 互斥断言保持。

**回归面**：

| sample / gate | 守什么 |
|---|---|
| `Sandbox19` | 主验证面：指令下达、AI 反应、reload 无残留 |
| `Sandbox8` | 共享 `SoldierBT` 未被改动的证据 |
| `Sandbox12` / `Sandbox13` | TeamBlackboard typed fact 链路未退化 |
| `Sandbox17` | 相机未被 Sandbox19 的 FOLLOW 污染 |

不碰 `SoldierBT.lua` / `SoldierConditions.lua`，`run_chapter9_parity_gate.ps1` 不受影响。

## 8. 已知局限

- 框选用单点投影判定，不做包围盒；单位密集重叠时可能选不全（2–6 单位场景够用）。
- 指令是一次性下达，不是持续命令队列；不支持"取消指令"键（W2 视手感再定）。
- `ScreenToGroundPoint` 与水平面求交，不考虑地形起伏（Sandbox19 是平地，`groundY` 由调用方传）。
- 集火不改变 AI 的射程 / 掩体判断，只改变目标选择；若目标在射程外，agent 会 pursue 过去。
- 本轮不给指令做视觉反馈以外的音效 / 动画表现。

## 9. 验证策略

按 `docs/skills/verify.md`：Release x64 编译 + `tools/run_sandbox_smoke.ps1`。

- 每 task 结束：Release x64 编译 0 错。
- C++ 面（GameManager / CameraService）改动后：`Sandbox19` + `Sandbox17` smoke `status=PASS`。
- Lua BT / 指令面改动后：`Sandbox19` + `Sandbox8` + `Sandbox12` smoke `status=PASS`。
- 全部完成后：手动跑 `Sandbox19` 确认"能下指令、AI 有可观察反应"（cycle-01 W1 完成标志），headless smoke 无法覆盖手感。
