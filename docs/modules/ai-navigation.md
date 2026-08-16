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
| `NavigationMesh.{h,cpp}` | 包装 | dtNavMesh/dtNavMeshQuery 托管；按输入指纹加载/保存本地缓存；`FindPath`/`FindClosestPoint`/`RandomPoint`/`GetWalkableTriangles`/debug visual |
| `NavBuilder.{h,cpp}` | 编译 | 从变换后的 fixed geometry 收紧 Recast bounds、计算稳定输入指纹、构建 heightfield→poly mesh→detour navmesh |
| `NavigationService.{h,cpp}` | 门面 / owner | 默认 Recast config、agent 设置覆盖、navmesh 构建，按 name 以 `unique_ptr` 持有 navmesh map，以及查询 `RandomPoint`/`FindClosestPoint`/`FindPath`；Lua 全局 `SandboxNav` |
| `ObjectManager`（兼容） | fixed blocks 来源 | `NavigationService.CreateNavigationMesh` 通过 `ObjectManager.getFixedObjects()` 读取构建输入；`ObjectManager.getNavigationMesh/addNavigationMesh` 仅保留 C++ 兼容转发 |

## 4. 公开能力要点

- `SandboxNav` 提供路径查询、最近点投影、随机点与 navmesh 构建；`NavigationMesh::GetWalkableTriangles` 供 [[ai-tactics]] 影响图 3D 体素化。
- 构建前会按 `ObjectManager.getFixedObjects()` 的变换后顶点自动收紧 bounds，并保留 agent radius/cell 的安全边距；没有有效几何时回退调用方原始 bounds。
- 单 tile Detour 数据默认缓存到 `../tmp/navmesh-cache`。缓存键覆盖 Recast config 与变换后几何，Detour/cache 版本或输入变化会自动 miss；`HELLO_NAVMESH_CACHE=0` 可禁用，`HELLO_NAVMESH_CACHE_DIR` 可覆盖目录。

## 5. 约束与红线

- 无运行时动态障碍（编译后固定）。
- 影响图 3D 体素化只用单个 navmesh；多层地形未支持。
- 当前磁盘缓存只接受单 tile navmesh；未来切到 tiled navmesh 时必须先升级缓存格式，不能只保存首 tile。
- debug visual 涉及 ManualObject 增删，仅在 navmesh 变更时重建，勿每帧；场景访问走 SceneFactory，不再 include ClientManager。
- Lua 和 AI/FSM 代码应通过 `SandboxNav` / `SandboxServices.navigation` 访问导航查询，不再把新导航 API 挂回 `SandboxMgr`。
- `AddNavigationMesh(name, rawPtr)` 是历史兼容入口；返回 true 后 rawPtr 所有权立即转入 `NavigationService::m_navMeshes` 的 `unique_ptr`。

## 6. 数据流 / 与其他模块关系

`Lua SandboxNav.CreateNavigationMesh → NavigationService → ObjectManager.getFixedObjects → NavBuilder.FitBoundsToObjects / ComputeInputFingerprint → 磁盘缓存命中或 NavBuilder.Build → NavigationService.AddNavigationMesh`；`AIController/FSM/Lua → SandboxServices.navigation 或 SandboxNav → NavigationMesh.FindPath/RandomPoint/FindClosestPoint`；`InfluenceMapSystem.BuildFromNavMesh → GetWalkableTriangles`。

## 7. 验证策略

- 回归 sample：`Sandbox6`（自主导航/FSM）、`Sandbox5`（navmesh 可视化）、`Sandbox13`（战术导航查询）；所有移动 sample 依赖。
- 缓存改动至少验证一次 fresh cache 的 `cache=miss saved=true`、第二次相同输入的 `cache=hit`，以及 `HELLO_NAVMESH_CACHE=0` 的同步构建回退；日志同时记录旧/新 grid、fingerprint 与 load/build/total 毫秒数。

## 8. 已知 gap / 相关文档

- 待：多层/tiled navmesh、动态障碍。
- `docs/design/chapter9-parity-architecture-notes.md` §6、`docs/design/architecture-improvement-plan.md` P1。
