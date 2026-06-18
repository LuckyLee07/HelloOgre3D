# 系统管理枢纽（alias: systems-manager）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

ObjectManager（对象生命周期/update/AI 调度/导航/战术 facade）、ObjectRegistry（对象注册表与 Agent/Block 二级索引）与 SandboxMgr（场景/nav 门面）。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/manager/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ObjectManager.{h,cpp}` | 枢纽 | 生命周期 + Update 阶段编排 + AIScheduler/Perception/Team/Tactical facade + navmesh + 战术调试绘制；对象生命周期/update loop 已收口到 `UpdateManagedObjects`，延迟 scene node 清理已收口到 `CleanupRemovedSceneNodes`；持 SandboxServices owner；场景访问走 SceneFactory，当前时间由 GameManager 每帧写入 |
| `ObjectRegistry.h` | registry | 对象 id 分配、object map、Agent/Block 二级索引、对象查找；non-owning，不负责删除对象 |
| `SandboxMgr.{h,cpp}` | 门面 | 光照/场景/CallFile/nav/raycast；对象创建已移到 `SandboxObjects`/`ObjectFactory`，Gorilla UI frame/color 已移到 `SandboxUI`/`UIManager`，相机/profile 查询已移到 `SandboxCamera`/`CameraService`；RayCastObjectId 通过 SandboxServices.physics 取物理世界 |

## 4. 公开能力要点

- 对象枚举/过滤/计数、AI 调度配置、战术/团队/影响图 Lua facade、各 `build*DebugSummary`。

## 5. 约束与红线

- **C3**：ObjectRegistry 已拆出 registry/id/Agent/Block 二级索引；`ObjectManager::Update` 已拆出对象 update loop 和 scene node cleanup 两个 stage helper；ObjectManager 仍承担生命周期编排、AI调度、感知、navmesh/tactical debug draw，待继续拆薄服务。
- 战术调试绘制（`rebuildTacticalInfluenceLayerDebugVisual`/`configureTacticalInfluenceFromNavMesh`）逻辑待下沉 [[ai-tactics]] service。
- **C2**：SandboxMgr 对象创建转发已下沉到 `SandboxObjects`/`ObjectFactory`，Gorilla UI frame/color 转发已下沉到 `SandboxUI`/`UIManager`，相机/profile 查询已下沉到 `SandboxCamera`/`CameraService`；剩余场景/nav 门面边界继续收口；raycast 已不再通过 `g_GameManager` 获取 PhysicsWorld。
- ObjectManager 不应 include `GameManager.h` / `ClientManager.h`；需要场景访问走 SceneFactory，需要当前时间走 `SetCurrentTimeMs`。
- 改导出给 Lua 的方法走**手术式** tolua（[[scripting-tolua]]），勿全量重生成。

## 6. 数据流 / 与其他模块关系

`GameManager 填 SandboxServices → ObjectManager`；每帧驱动 AIScheduler/Perception/Team/Influence；Lua 经 tolua 调其 facade。关联几乎所有 AI/对象模块。

## 7. 验证策略

- 回归 sample：`Sandbox6`(调度/导航)、`Sandbox12`(team)、`Sandbox18`(影响图)。

## 8. 已知 gap / 相关文档

- 待：C3 继续把 update stage helper 提升为独立 UpdateSystem、拆 TacticalDebugDraw/更薄 tactical facade，C2 剩余场景/nav 门面减肥。`docs/design/architecture-improvement-plan.md` C2/C3、`docs/planning/long-term-iteration-plan.md`。
