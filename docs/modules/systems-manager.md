# 系统管理枢纽（alias: systems-manager）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

ObjectManager（update 阶段总编排与战术兼容 facade）、ObjectRegistry（对象注册表与 Agent/Block 二级索引）、ObjectLifecycleSystem（对象生命周期帧循环与延迟 scene node 清理）、AIUpdateSystem（AI 帧阶段编排）与 SandboxMgr（兼容门面）。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/manager/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ObjectManager.{h,cpp}` | 枢纽 | Update 阶段总编排 + Tactical compat facade；对象生命周期/update loop 与延迟 scene node 清理已收口到 `ObjectLifecycleSystem`；AI scheduler begin/tick、spatial rebuild、Perception、Team sync 与 AI perf stats 已收口到 `AIUpdateSystem`；tactical influence config/layer/query 已薄转发到 `TacticalQueryService`，tactical debug draw 已薄转发到 `TacticalDebugDrawService`；navmesh 所有权已迁到 `NavigationService`，ObjectManager 只提供 fixed blocks 来源与兼容转发；AIScheduler Lua 配置/诊断主入口已迁到 `SandboxAIScheduler`，TeamBlackboard Lua 主入口已迁到 `SandboxTeam`，ObjectManager 仅保留兼容转发；持 SandboxServices owner；场景访问走 SceneFactory，当前时间由 GameManager 每帧写入 |
| `ObjectLifecycleSystem.{h,cpp}` | 对象生命周期系统 | 承接对象 map 遍历、待删对象移除、对象 update/event flush 和延迟 scene node 清理；AI tick 仅委托 `AIUpdateSystem` |
| `ai/common/AIUpdateSystem.{h,cpp}` | AI 帧系统 | 无状态编排器；承接 scheduler begin、按 scheduler tick 单个 AI、spatial index rebuild、AgentPerceptionSystem update、TeamBlackboardService sync、Tracy/perf stats 写回 |
| `ObjectRegistry.h` | registry | 对象 id 分配、object map、Agent/Block 二级索引、对象查找；non-owning，不负责删除对象 |
| `SandboxMgr.{h,cpp}` | 兼容门面 | 保留 `SetUseCppFsmFlag` / `GetUseCppFsmFlag` 与旧 Lua 兼容转发；对象创建已移到 `SandboxObjects`/`ObjectFactory`，Gorilla UI frame/color 已移到 `SandboxUI`/`UIManager`，相机/profile 查询已移到 `SandboxCamera`/`CameraService`，导航配置/构建/查询已移到 `SandboxNav`/`NavigationService`，raycast 已移到 `SandboxRaycast`/`RaycastService`，场景/light/material 已移到 `SandboxScene`/`SceneService`，CallFile 已移到 `SandboxScript`/`ScriptService` |

## 4. 公开能力要点

- 对象枚举/过滤/计数、AI 调度配置、战术/团队/影响图 Lua facade、各 `build*DebugSummary`。

## 5. 约束与红线

- **C3**：ObjectRegistry 已拆出 registry/id/Agent/Block 二级索引；`ObjectManager::Update` 已拆出 `ObjectLifecycleSystem` 和 `AIUpdateSystem`；ObjectManager 的 influence config/layer/source/sample/score/stats 与 navmesh 建图编排已薄转发到 `TacticalQueryService`，战术调试绘制（`rebuildTacticalInfluenceLayerDebugVisual` / ManualObject 生命周期）已薄转发到 [[ai-tactics]] 的 `TacticalDebugDrawService`；navmesh map 与销毁责任已迁到 `NavigationService`；AIScheduler C++ API 已导出为 `SandboxAIScheduler`，TeamBlackboard C++ API 已导出为 `SandboxTeam`，ObjectManager 只保留旧 Lua 兼容转发；AI 调度执行、感知系统、TeamBlackboard 每帧 sync 和 AI perf stats 已由 `AIUpdateSystem` 承接。
- **C2**：SandboxMgr 对象创建转发已下沉到 `SandboxObjects`/`ObjectFactory`，Gorilla UI frame/color 转发已下沉到 `SandboxUI`/`UIManager`，相机/profile 查询已下沉到 `SandboxCamera`/`CameraService`，导航配置/构建/查询已下沉到 `SandboxNav`/`NavigationService`，raycast 已下沉到 `SandboxRaycast`/`RaycastService`，场景/light/material 已下沉到 `SandboxScene`/`SceneService`，CallFile 已下沉到 `SandboxScript`/`ScriptService`；SandboxMgr 当前仅保留 CppFSM flag 与旧 Lua 兼容转发。
- ObjectManager 不应 include `GameManager.h` / `ClientManager.h`；需要场景访问走 SceneFactory，需要当前时间走 `SetCurrentTimeMs`。
- 改导出给 Lua 的方法走**手术式** tolua（[[scripting-tolua]]），勿全量重生成。

## 6. 数据流 / 与其他模块关系

`GameManager 填 SandboxServices → ObjectManager`；ObjectManager 每帧编排事件 flush 与 tactics，生命周期帧循环和 scene cleanup 由 `ObjectLifecycleSystem` 执行；AI scheduler/perception/team sync 由 `AIUpdateSystem` 执行；AIScheduler Lua 配置/诊断走 `SandboxAIScheduler`，TeamBlackboard Lua 主入口走 `SandboxTeam`，tactical/历史入口仍经 tolua 兼容 facade。关联几乎所有 AI/对象模块。

## 7. 验证策略

- 回归 sample：`Sandbox6`(调度/导航)、`Sandbox12`(team)、`Sandbox18`(影响图)。

## 8. 已知 gap / 相关文档

- 待：C3 后续主要是把 ObjectManager 的 tactical/team/scheduler 旧 Lua 兼容 facade 按 sample 节奏删除；C2 后续可在 Lua 旧入口完全清空后删除 SandboxMgr 兼容 scene/script/raycast 转发，只保留或另迁 CppFSM flag。`docs/design/architecture-improvement-plan.md` C2/C3、`docs/planning/long-term-iteration-plan.md`。
