# Cycle 01：战术指挥切片

> 周期制说明：本项目不再维护 6–12 个月路线图。**一次只有一个 cycle，4–6 周，三行定义，到期必须结束**——交付了算成功，交不出也要结束并写结论，然后进下一个 cycle。
>
> 候选池见 `backlog.md`；方向定调见 `project-direction.md`。

## 三行定义

| | |
|---|---|
| **问题** | 已经造好的 AI 底座（TeamBlackboard typed fact / InfluenceMap / TacticalQuery / BT），能不能直接变成一种玩法？玩家不开枪，只下指令，好不好玩？ |
| **产出** | 一个**别人能玩 5 分钟**的 build，玩完能说出他刚才在干什么。 |
| **截止** | **2026-08-31**。到期无论什么状态都结束并写结论。 |

## 为什么是这个

- 这些系统今天只有 smoke 日志能证明它们存在，玩家从来没见过。`FocusTarget` / `RetreatPoint` / `FormationSlot` 三个 typed fact 已经在 `TeamBlackboardService` 里跑通、有 TTL 有 priority 有 smoke 检查——**离"能玩"只差输入和表达两层**。
- 成本最低的一条：底座全现成，不需要新 AI system。
- 它用 artifact 回答"做什么品类"，而不是用思辨回答。两年半里这个问题靠想没想出来。
- **它有观众。** 这是项目开张以来第一次把东西交给别人。

## 周排期

| 周 | 日期 | 做什么 | 完成标志 |
|---|---|---|---|
| W1 | 08-04 ~ 08-10 | ~~指令输入层：点选 / 框选单位，三个指令键（集火 / 撤退 / 编队）~~ **已完成 08-04** | 自动化证据已达成，手感待真人验收 |
| W2 | 08-11 ~ 08-17 | **AI 意图可视化**：被指令单位在想什么必须让玩家看见（不是让日志看见） | 旁观者不看代码能说出某个 AI 正在干什么、为什么 |
| W3 | 08-18 ~ 08-24 | 一局的节奏：关卡布局、胜负、重开，让 5 分钟有开局 / 交火 / 收尾 | 自己能连玩三局不觉得空 |
| W4 | 08-25 ~ 08-31 | 找 1–2 个真人玩，记录他们前 5 分钟的行为；写 cycle 总结 + 品类决策 | 有外部反馈，`project-direction.md` §5 落档 |

### W2/W3 实施合同（2026-08-13）

上轮方向讨论中的“小队命令 → AI 可解释 → 一局闭环”按当前 cycle 收敛为以下交付，不新增平行 AI system：

- **意图卡片**：每个存活友军头顶持续显示 `FOCUS / RETREAT / RALLY / ENGAGE / SEARCH / PATROL` 等玩家语义，以及“玩家指令 / 自主感知 / 最后已知位置”等原因。
- **目标表达**：集火目标有红色屏幕标记；撤退/编队目标点有对应颜色的落点标记；雷达 blip 同步采用当前意图颜色。
- **指令生命周期**：TTL 过期、集火目标死亡和重开时清理 `command.*` 与指令专用 `movePos`，HUD 显示活跃指令数量和剩余时间。
- **指挥官身份**：玩家仍可用第三人称移动和观察战场，但改用无武器 `commander_soldier` profile，不能直接开枪；战斗结果必须来自 AI 小队执行。
- **一局节奏**：部署准备 → 三波交火 → 波间整备 → 最终胜负；固定 seed、固定出生模板并记录阶段转换日志，保证同一 build 的基础场景可重复。
- **验证**：在既有 `[Sandbox19CommandSelfTest]` 外补意图和 match-flow 自测，再跑 Release x64 与 `Sandbox19 / Sandbox8 / Sandbox12 / Sandbox17` smoke；视觉可读性仍须真人桌面验收。

上轮同时讨论的通用 `Pause / Resume / Reset`、driver 身份、寻路预算/排队和完整输入录制回放不塞进本应用 cycle；它们已回到 `backlog.md`，等技术 cycle 再按真实压力认领。

### W2/W3 代码完成记录（2026-08-13）

W2/W3 已提前完成代码落地：`Sandbox19` 现为无武器 commander 指挥两名 AI 友军，
以头顶 action/reason 卡片、目标标记、雷达意图色和 HUD 指令计数表达 AI 状态；
对局按 `PREPARE → WAVE × 3 → INTERMISSION → VICTORY/DEFEAT` 推进，固定 seed/出生点，波次参数收口在 `sample_presets.lua`。
指令会在 TTL、目标失效、波次结束和重开时清理，且只移除由指令拥有的 `movePos`。

自动化证据：Release x64 构建 0 warning / 0 error；
`[Sandbox19CommandSelfTest] PASS`、`[Sandbox19IntentSelfTest] PASS`、`[Sandbox19MatchSelfTest] PASS`；
`Sandbox19` / `Sandbox8` / `Sandbox12` / `Sandbox17` smoke 均为 `status=PASS`。

W2/W3 的“代码与自动化”已完成，但周完成标志仍不提前勾选：意图卡是否在真实战斗中可读、
三波节奏是否能支撑 5 分钟，以及玩家是否理解自己只能下令，必须由 W4 真人桌面试玩回答。

### W1 完成记录（2026-08-04）

经 `/hello-develop-design` 六阶段流程落地，spec / plan 见
`dev-design/specs/2026-08-04-sandbox19-command-input-design.md`、
`dev-design/plans/2026-08-04-sandbox19-command-input.md`。

W1 当时的操作是 `LMB` 射击 · `RMB` 点选/框选友军 · `F` 集火 · `T` 撤退 · `G` 编队；
W2/W3 已按本 cycle 的原始问题改成无武器 commander，`LMB` 射击不再存在。
（撤退用 `T` 而非原定 `R`——`R` 是 `PlayerController` 的换弹键。）

自动化证据：`[Sandbox19CommandSelfTest] PASS`（指令命中 / 三指令互斥 /
集火对 `blackboard.enemy` 的覆写生效 / TTL 过期后落回自主战斗）+
`Sandbox19` `Sandbox8` `Sandbox12` `Sandbox17` 四个 smoke 全 `status=PASS`。

实施中修掉两个真实缺陷（都是 selftest 抓出来的，不是设计阶段预见的）：
`ObjectManager:getObjectById` 未导出给 Lua；指令 TTL 判定在仿真时间为 0 时失效。

**仍欠**：真人手感验收——headless smoke 驱动不了鼠标键盘，"能下指令、AI 有可观察反应"
这一条 W1 完成标志只有自动化侧证据，手感必须真跑一局才算数。

**W2 是这个 cycle 真正的技术价值**，也是最能搬回生产工作的部分：把 AI 内部状态做成玩家可读的表达。这是所有战术类游戏最难的一环，而这个项目恰好已经有全套可观测数据（`[AIRuntimeDiag]`、BT trace、InfluenceMap debug draw、TeamBlackboard fact）——现在只是把它们从"给我看"改成"给玩家看"。做成了，无论最后做不做游戏，都是能写出来、能讲的东西。

## 完成定义

- [ ] 一个别人能上手玩 5 分钟的 build
- [ ] 至少 1 个人玩过，且行为被记录下来
- [ ] `project-direction.md` §5 写下品类结论（**包括"结论是不做游戏"也算完成**）
- [ ] 本文底部补 cycle 总结

## 本 cycle 明确不做

- 逐帧 parity（chapter8 已证明不可达，见 `memory/chapter8-parity-rootcause.md`）
- P8 / P10 / P11 架构债（不阻塞任何东西；且品类若变，现在还债可能白还）
- 任何新 AI system
- 美术资源升级

## Cycle 总结

（2026-08-31 补。至少回答：好玩吗？哪一刻有意思 / 哪一刻无聊？试玩者卡在哪？下一个 cycle 做什么？）
