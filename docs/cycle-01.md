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

### W1 完成记录（2026-08-04）

经 `/hello-develop-design` 六阶段流程落地，spec / plan 见
`dev-design/specs/2026-08-04-sandbox19-command-input-design.md`、
`dev-design/plans/2026-08-04-sandbox19-command-input.md`。

操作：`LMB` 射击（不变）· `RMB` 点选/框选友军 · `F` 集火 · `T` 撤退 · `G` 编队。
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
