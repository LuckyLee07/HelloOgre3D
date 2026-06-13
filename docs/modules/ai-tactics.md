# AI 战术层（alias: ai-tactics）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

多层影响图战术评分系统：把威胁/支援/目标组合评分，输出最优移动点；服务 Chapter9 战术语义。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/tactics/`（影响图调试可视化逻辑当前仍在 `systems/manager/ObjectManager.cpp`）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `InfluenceMapSystem.{h,cpp}` | 系统 | 2D flat（`Configure`）与 navmesh 体素化 3D（`BuildFromNavMesh`）；layer/source/spread/sample/score；`CollectDebugCells` 2D 拍平到 `m_minY` |
| `TacticalQueryService.{h,cpp}` | 服务 | 持 InfluenceMapSystem；事件订阅/TTL/Publish；`Rebuild{Danger,Team,Objective}Layer`；查询 API `FindBestSupportPosition`/`FindLowThreatPosition`/`ScoreQueryPosition`/`FindBestQueryPosition` |
| `ObjectManager.cpp`（facade） | 转发 | `configureTacticalInfluence*`/`score*`/`findBest*`/`rebuildTacticalInfluenceLayerDebugVisual`（Lua 入口，待下沉） |

## 4. 公开能力要点

- 三层（danger/team/objective）已落地；组合评分 `objective+support+cover-threat-crowd`（cover/crowd 未做）。
- 影响图绘制 2D 拍平贴地（每列取最低 used cell）。

## 5. 约束与红线

- **TacticalQueryService 已是查询/层事实来源**，但 ObjectManager 仍持 `rebuildTacticalInfluenceLayerDebugVisual` / `configureTacticalInfluenceFromNavMesh` 逻辑 → 待下沉 service / TacticalDebugDrawService（ObjectManager 只留薄转发）。
- 缺 dirty region / interval 更新、cover/crowd 层。
- tolua 绑定**手术式**改、勿全量重生成（Sandbox18 教训）。
- 影响图 3D 来自 navmesh，受 [[ai-navigation]] 固定 2001×2001 影响。

## 6. 数据流 / 与其他模块关系

`Lua/事件 → TacticalQueryService.PublishEvent → Rebuild*Layer → InfluenceMapSystem`；`AIController/BT → FindBest* → Vector3`；`ObjectManager.draw* → CollectDebugCells → ManualObject`。

## 7. 验证策略

- 回归 sample：`Sandbox13`（2D 教学）、`Sandbox17`（Lua-first）、`Sandbox18`（C++，`[Chapter9TacticsCppSmoke] PASS`）。
- gate：`run_chapter9_parity_gate.ps1`、`run_chapter9_visual_capture.ps1`。

## 8. 已知 gap / 相关文档

- 待：service 收口（debug 绘制下沉）、dirty/interval、cover/crowd。
- `docs/planning/ai-technical-iteration-plan.md` §6、`docs/design/chapter9-parity-architecture-notes.md`、`docs/design/architecture-improvement-plan.md` C3。
