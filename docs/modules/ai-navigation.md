# AI 导航（alias: ai-navigation）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

Recast/Detour 寻路：从场景几何构建导航网格，提供路径查询、最近点投影、随机点、可走三角导出。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/navigation/`（第三方在 `src/External/recast`、`detour`）
- `src/HelloOgre3D/sandbox/systems/service/NavigationService.{h,cpp}`（Lua/AI 使用的导航门面）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `NavigationMesh.{h,cpp}` | 包装 | dtNavMesh/dtNavMeshQuery 托管；`FindPath`/`FindClosestPoint`/`RandomPoint`/`GetWalkableTriangles`/debug visual |
| `NavBuilder.{h,cpp}` | 编译 | Recast heightfield→poly mesh→detour navmesh；`Build`/`BuildDetour*` |
| `NavigationService.{h,cpp}` | 门面 / owner | 默认 Recast config、agent 设置覆盖、navmesh 构建，按 name 以 `unique_ptr` 持有 navmesh map，以及查询 `RandomPoint`/`FindClosestPoint`/`FindPath`；Lua 全局 `SandboxNav` |
| `ObjectManager`（兼容） | fixed blocks 来源 | `NavigationService.CreateNavigationMesh` 通过 `ObjectManager.getFixedObjects()` 读取构建输入；`ObjectManager.getNavigationMesh/addNavigationMesh` 仅保留 C++ 兼容转发 |

## 4. 公开能力要点

- `SandboxNav` 提供路径查询、最近点投影、随机点与 navmesh 构建；`NavigationMesh::GetWalkableTriangles` 供 [[ai-tactics]] 影响图 3D 体素化。

## 5. 约束与红线

- **NavMesh 当前固定 2001×2001**（`NavigationMesh.cpp:20` TODO，未按真实边界重算）。
- 无运行时动态障碍（编译后固定）。
- 影响图 3D 体素化只用单个 navmesh；多层地形未支持。
- debug visual 涉及 ManualObject 增删，仅在 navmesh 变更时重建，勿每帧；场景访问走 SceneFactory，不再 include ClientManager。
- Lua 和 AI/FSM 代码应通过 `SandboxNav` / `SandboxServices.navigation` 访问导航查询，不再把新导航 API 挂回 `SandboxMgr`。
- `AddNavigationMesh(name, rawPtr)` 是历史兼容入口；返回 true 后 rawPtr 所有权立即转入 `NavigationService::m_navMeshes` 的 `unique_ptr`。

## 6. 数据流 / 与其他模块关系

`Lua SandboxNav.CreateNavigationMesh → NavigationService → ObjectManager.getFixedObjects → NavBuilder.Build → NavigationService.AddNavigationMesh`；`AIController/FSM/Lua → SandboxServices.navigation 或 SandboxNav → NavigationMesh.FindPath/RandomPoint/FindClosestPoint`；`InfluenceMapSystem.BuildFromNavMesh → GetWalkableTriangles`。

## 7. 验证策略

- 回归 sample：`Sandbox6`（自主导航/FSM）、`Sandbox5`（navmesh 可视化）、`Sandbox13`（战术导航查询）；所有移动 sample 依赖。

## 8. 已知 gap / 相关文档

- 待：navmesh 真实边界（去 2001 固定）、多层 navmesh、动态障碍。
- `docs/design/chapter9-parity-architecture-notes.md` §6、`docs/design/architecture-improvement-plan.md` P1。
