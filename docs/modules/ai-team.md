# AI 团队黑板服务（alias: ai-team）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

把团队共享信息（敌人目击等 fact）从 Lua 全局表迁到 C++ service，提供 TTL、优先级、confidence。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/team/`（Lua facade 在 `bin/res/scripts/ai/team/TeamBlackboard.lua`，见 [[ai-scripts]]）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `TeamBlackboardService.{h,cpp}` | 服务 | EnemySighting fact + typed team fact（factType/teamId/source/target/pos/confidence/priority/ttl）；`RememberEnemySighting`/`RememberFact`/`GetBestEnemyFact`/`GetBestFact`/`SyncFromAgents`；TTL 自动清理；Lua 全局 `SandboxTeam` 直接访问 |
| `bin/res/scripts/ai/team/TeamBlackboard.lua` | Lua facade | 只走 `SandboxTeam`；`SupportRequested` / `SupportResponded` / `FocusTarget` / `RetreatPoint` / `FormationSlot` 已同步为 C++ typed facts；Lua 侧仅保留 support/focus/retreat 的 typed 兼容缓存，不再保存任意 key/value |

## 4. 公开能力要点

- 第一版 `EnemySighted`：记录/查询团队最佳敌情、写回 agent blackboard、stats（factCount/reportCount/expired）。
- 二期 typed fact：`SupportRequested` / `SupportResponded` / `FocusTarget` / `RetreatPoint` / `FormationSlot` 已通过 `rememberTeamFact(...)` 进入 C++ service，stats 输出 `enemyFacts/typedFacts/typedReports`，`writeBestTeamFactToBlackboard(...)` 可把指定 factType 写回 agent blackboard。
- `FocusTarget` 已从可见敌人和 formation focus 写入，并由 TeamBlackboard / InfluenceMap / Formation sample 写回 blackboard；`RetreatPoint` 已由 `AgentPerceptionSystem` Hearing/Danger C++ sense 写入并写回 blackboard。
- `TeamBlackboardService` 已导出为 Lua 全局 `SandboxTeam`；Lua `TeamBlackboard.lua` 只使用它，避免团队 AI 主路径继续挂在 `ObjectManager` facade 上。
- 优先级 = f(confidence, reportCount, ageMs)。

## 5. 约束与红线

- `SetValue/GetValue` 只兼容 `backupRequest` / `focusTarget` / `retreat:*` 这类已知 typed legacy key；不要再把任意团队状态塞进 Lua 表。
- Lua `TeamBlackboard.lua` 保留薄 facade，C++ typed fact 是主记录面，Lua 表只承担旧读路径的最小兼容缓存。
- position 真源须来自感知（[[ai-perception]] 的 PerceptionResultCache / agent.GetPosition），不信 Lua 坐标。
- `SyncFromAgents` 须在感知后调用。

## 6. 数据流 / 与其他模块关系

`AIController.TickPerception → PerceptionResultCache → (Sync)TeamBlackboardService.RememberEnemySighting`；`BT/Lua → TeamBlackboard.lua → SandboxTeam → GetBestEnemyFact/writeBestTeamEnemyFactToBlackboard/writeBestTeamFactToBlackboard`；`AgentPerceptionSystem Hearing/Danger → TeamBlackboardService.RememberFact(RetreatPoint)`；支援、队形、焦点目标、危险撤退类事件同步为 typed fact。关联 [[ai-controller]] [[ai-perception]] [[ai-scripts]]。

## 7. 验证策略

- 回归 sample：`Sandbox12`（一人发现敌人队友响应，`TeamBlackboardSmoke` 覆盖 cppFacts/cppReports/cppTypedFacts/cppTypedReports/cppFocusApplies）；`Sandbox14` Hearing/Danger smoke 覆盖 C++ sense 的 `RetreatPoint` 写回；`Sandbox15` Formation smoke 覆盖 `FormationSlot` typed facts 与 formation `FocusTarget` 写回。
- gate：`run_chapter9_parity_gate.ps1`。

## 8. 已知 gap / 相关文档

- 待：更多团队 fact 查询接口可继续从 Lua 兼容缓存迁到 C++ getter；更多非视觉输入见 [[ai-perception]]。
- `docs/planning/ai-technical-iteration-plan.md` §5、`docs/design/chapter9-parity-architecture-notes.md` §4。
