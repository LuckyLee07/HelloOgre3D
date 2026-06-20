# AI 团队黑板服务（alias: ai-team）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

把团队共享信息（敌人目击等 fact）从 Lua 全局表迁到 C++ service，提供 TTL、优先级、confidence。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/team/`（Lua facade 在 `bin/res/scripts/ai/team/TeamBlackboard.lua`，见 [[ai-scripts]]）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `TeamBlackboardService.{h,cpp}` | 服务 | EnemySighting fact（teamId/targetId/reporterId/pos/confidence/priority/lastSeenMs）；`RememberEnemySighting`/`GetBestEnemyFact(teamId[,ignoredReporter])`/`SyncFromAgents`；TTL 自动清理；Lua 全局 `SandboxTeam` 直接访问 |
| `bin/res/scripts/ai/team/TeamBlackboard.lua` | Lua facade | 只走 `SandboxTeam`；仍承担 SupportResponse/泛型值 |
| `ObjectManager.cpp`（compat） | 转发 | `configureTeamBlackboard`/`rememberTeamEnemyFact`/`writeBestTeamEnemyFactToBlackboard`/`getTeamBlackboard*Count` 仅薄转发到 service |

## 4. 公开能力要点

- 第一版只 `EnemySighted`：记录/查询团队最佳敌情、写回 agent blackboard、stats（factCount/reportCount/expired）。
- `TeamBlackboardService` 已导出为 Lua 全局 `SandboxTeam`；Lua `TeamBlackboard.lua` 只使用它，避免团队 AI 主路径继续挂在 `ObjectManager` facade 上。
- 优先级 = f(confidence, reportCount, ageMs)。

## 5. 约束与红线

- 仅 `EnemySighted` fact；二期待迁 `SupportRequested/SupportResponded/FocusTarget/RetreatPoint/FormationSlot`。
- Lua `TeamBlackboard.lua` 保留薄 facade，仍承担 SupportResponse/泛型值（待迁）。
- position 真源须来自感知（[[ai-perception]] 的 PerceptionResultCache / agent.GetPosition），不信 Lua 坐标。
- `SyncFromAgents` 须在感知后调用。

## 6. 数据流 / 与其他模块关系

`AIController.TickPerception → PerceptionResultCache → (Sync)TeamBlackboardService.RememberEnemySighting`；`BT/Lua → TeamBlackboard.lua → SandboxTeam → GetBestEnemyFact/writeBestTeamEnemyFactToBlackboard`。关联 [[ai-controller]] [[ai-perception]] [[ai-scripts]]。

## 7. 验证策略

- 回归 sample：`Sandbox12`（一人发现敌人队友响应，`TeamBlackboardSmoke` 覆盖 cppFacts/cppReports/cppApplies）。
- gate：`run_chapter9_parity_gate.ps1`。

## 8. 已知 gap / 相关文档

- 待：fact schema 二期、Lua SupportResponse/泛型值迁移；旧 `ObjectManager` TeamBlackboard Lua 导出最终删除。
- `docs/planning/ai-technical-iteration-plan.md` §5、`docs/design/chapter9-parity-architecture-notes.md` §4。
