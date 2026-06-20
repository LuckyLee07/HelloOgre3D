# AI 战术层（alias: ai-tactics）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

多层影响图战术评分系统：把威胁/支援/目标组合评分，输出最优移动点；服务 Chapter9 战术语义。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/tactics/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `InfluenceMapSystem.{h,cpp}` | 系统 | 2D flat（`Configure`）与 navmesh 体素化 3D（`BuildFromNavMesh`）；layer/source/spread/sample/score；source 写入 dirty bounds，spread 按 dirty region 外扩 passCount 限域；best position 支持 candidate 上限统计；`CollectDebugCells` 2D 拍平到 `m_minY` |
| `TacticalQueryService.{h,cpp}` | 服务 | 持 InfluenceMapSystem；事件订阅/TTL/Publish；influence config/layer/source/sample/score 包装；layer interval / dirty-only update policy；query candidate limit；`Rebuild{Danger,Team,Objective}Layer`；查询 API `FindBestSupportPosition`/`FindLowThreatPosition`/`ScoreQueryPosition`/`FindBestQueryPosition` |
| `TacticalDebugDrawService.{h,cpp}` | 调试可视化 | 承接 `DebugDrawer` 临时绘制与 Ogre `ManualObject` 持久 debug visual 生命周期；记录 per-layer y/threshold/maxCells/neutral/projectToNav/nav/order debug config 并输出 summary |
| `TacticalService.{h,cpp}` | Lua facade | Lua 全局 `SandboxTactics`；保留现有 tactics Lua 方法名，内部转发到 `TacticalQueryService` / `TacticalDebugDrawService`，并通过 ObjectManager 读取 agent 列表与 navmesh |

## 4. 公开能力要点

- danger/team/objective 三层主线已落地；`TacticalQueryService` 已支持 layer 级 interval / dirty-only rebuild policy 与 tactical query candidate limit；`InfluenceMapSystem` 已支持 cell/region dirty bounds 第一切片；support/cover/crowd schema 第一切片已接入 query 评分，`team` rebuild 会派生 `support`、软 `cover` 和 `crowd` 层；`TacticalDebugDrawService` 已支持 per-layer debug config 第一切片；debug summary 输出 `policies/dirtyLayers/skippedBuilds/intervalSkips/dirtySkips/candidateLimit/capped/dirtyRegions/dirtyCells/spreadRegionCells/spreadLimited/schema/supportCells/coverCells/crowdCells` 和 `[TacticalDebugDraw] configs/order/projectToNav`。
- 影响图绘制 2D 拍平贴地（每列取最低 used cell）。

## 5. 约束与红线

- **TacticalQueryService 已是查询/层事实来源**，`configureTacticalInfluenceFromNavMesh` 的三角提取与 `BuildFromNavMesh` 编排已下沉到 service；`TacticalDebugDrawService` 已承接 `rebuildTacticalInfluenceLayerDebugVisual` 的 Ogre 绘制生命周期；Lua 主入口是 `SandboxTactics` / `TacticalService`，不再通过 ObjectManager tactical 旧 facade。
- 已有 layer 级 interval / dirty-only 更新策略、cell/region dirty bounds 第一切片、support/cover/crowd schema 第一切片、per-layer debug config 第一切片和 query candidate 上限；仍缺更细粒度增量 rebuild、真实 cover 来源、更细 crowd 模型和更完整的 layer debug 配置面；Lua 绑定签名中 `projectToNav/maxProjectionDistance/navMeshName` 仍是兼容保留的死参数。
- tolua 绑定**手术式**改、勿全量重生成（Sandbox18 教训）。
- 影响图 3D 来自 navmesh，受 [[ai-navigation]] 固定 2001×2001 影响。

## 6. 数据流 / 与其他模块关系

`Lua SandboxTactics → TacticalService → TacticalQueryService → InfluenceMapSystem`；`Lua/事件 → TacticalService.PublishEvent → TacticalQueryService.PublishEvent → Rebuild*Layer → InfluenceMapSystem`；`AIController/BT → FindBest* → Vector3`；`SandboxTactics.draw* → TacticalService → TacticalDebugDrawService → CollectDebugCells → DebugDrawer/ManualObject`。

## 7. 验证策略

- 回归 sample：`Sandbox13`（2D 教学）、`Sandbox17`（Lua-first）、`Sandbox18`（C++，`[Chapter9TacticsCppSmoke] PASS`）；`chapter9_tactics_cpp_pressure` 额外检查 `[TacticalCandidateLimitSmoke] PASS`、`[TacticalDirtyRegionSmoke] PASS`、`[TacticalLayerSchemaSmoke] PASS` 和 `[TacticalDebugConfigSmoke] PASS`。
- gate：`run_chapter9_parity_gate.ps1`、`run_chapter9_visual_capture.ps1`。

## 8. 已知 gap / 相关文档

- 待：更细粒度增量 rebuild、真实 cover 来源、更细 crowd 模型、更完整的 layer debug 配置面。
- `docs/planning/ai-technical-iteration-plan.md` §6、`docs/design/chapter9-parity-architecture-notes.md`、`docs/design/architecture-improvement-plan.md` C3。
