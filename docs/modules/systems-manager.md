# 系统管理枢纽（alias: systems-manager）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

ObjectManager（update 阶段总编排与战术兼容 facade）、ObjectRegistry（对象注册表与 Agent/Block 二级索引）、ObjectLifecycleSystem（对象生命周期帧循环与延迟 scene node 清理）与 AIUpdateSystem（AI 帧阶段编排）。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/manager/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ObjectManager.{h,cpp}` | 枢纽 | Update 阶段总编排；对象生命周期/update loop 与延迟 scene node 清理已收口到 `ObjectLifecycleSystem`；AI scheduler begin/tick、spatial rebuild、Perception、Team sync 与 AI perf stats 已收口到 `AIUpdateSystem`；tactical Lua 主入口已迁到 `SandboxTactics` / `TacticalService`；navmesh 所有权已迁到 `NavigationService`，ObjectManager 只提供 fixed blocks 来源与兼容 C++ 转发；自有 registry/AI/tactics services 已用 `unique_ptr` 表达所有权；AIScheduler Lua 配置/诊断主入口已迁到 `SandboxAIScheduler`，TeamBlackboard Lua 主入口已迁到 `SandboxTeam`，对应 ObjectManager 旧导出已删除；持 SandboxServices owner；场景访问走 SceneFactory，当前时间由 GameManager 每帧写入 |
| `ObjectLifecycleSystem.{h,cpp}` | 对象生命周期系统 | 承接对象 map 遍历、待删对象移除、对象 update/event flush 和延迟 scene node 清理；AI tick 仅委托 `AIUpdateSystem` |
| `ai/common/AIUpdateSystem.{h,cpp}` | AI 帧系统 | 无状态编排器；承接 scheduler begin、按 scheduler tick 单个 AI、spatial index rebuild、AgentPerceptionSystem update、TeamBlackboardService sync、Tracy/perf stats 写回 |
| `ObjectRegistry.h` | registry | 对象 id 分配、object map、Agent/Block 二级索引、对象查找；non-owning，不负责删除对象 |

## 4. 公开能力要点

- 对象枚举/过滤/计数、AI 调度配置、战术/团队/影响图 Lua facade、各 `build*DebugSummary`；`buildAiRuntimeDebugSummary(maxAgents)` 是 RuntimeDiag 的聚合入口，会把 perception/spatial、memory/blackboard、team facts、tactical influence 与 scheduler stats 放进同一个 `[AIRuntimeDiag]` block。

## 5. 约束与红线

- **C3/P4**：ObjectRegistry 已拆出 registry/id/Agent/Block 二级索引；`ObjectManager::Update` 已拆出 `ObjectLifecycleSystem` 和 `AIUpdateSystem`；`ObjectManager` 自有 registry/AI/tactics services 已用 `unique_ptr` 表达所有权，getter 仍返回 non-owning 裸指针；tactical influence config/layer/source/sample/score/stats、事件与 debug draw Lua 主入口已迁到 [[ai-tactics]] 的 `SandboxTactics` / `TacticalService`，底层分别复用 `TacticalQueryService` 与 `TacticalDebugDrawService`；navmesh map 与销毁责任已迁到 `NavigationService`，并由 `unique_ptr` map 表达所有权；AIScheduler C++ API 已导出为 `SandboxAIScheduler`，TeamBlackboard C++ API 已导出为 `SandboxTeam`，ObjectManager scheduler/team/tactical 旧导出已删除；AI 调度执行、感知系统、TeamBlackboard 每帧 sync 和 AI perf stats 已由 `AIUpdateSystem` 承接。
- **C2**：SandboxMgr 对象创建转发已下沉到 `SandboxObjects`/`ObjectFactory`，Gorilla UI frame/color 转发已下沉到 `SandboxUI`/`UIManager`，相机/profile 查询已下沉到 `SandboxCamera`/`CameraService`，导航配置/构建/查询已下沉到 `SandboxNav`/`NavigationService`，raycast 已下沉到 `SandboxRaycast`/`RaycastService`，场景/light/material 已下沉到 `SandboxScene`/`SceneService`，CallFile 已下沉到 `SandboxScript`/`ScriptService`，CppFSM flag 已下沉到 `SandboxAgentConfig`/`AgentConfigService`；SandboxMgr class/global 已删除。
- ObjectManager 不应 include `GameManager.h` / `ClientManager.h`；需要场景访问走 SceneFactory，需要当前时间走 `SetCurrentTimeMs`。
- 改导出给 Lua 的方法走**手术式** tolua（[[scripting-tolua]]），勿全量重生成。

## 6. 数据流 / 与其他模块关系

`GameManager 填 SandboxServices → ObjectManager`；ObjectManager 每帧编排事件 flush 与 tactics update，生命周期帧循环和 scene cleanup 由 `ObjectLifecycleSystem` 执行；AI scheduler/perception/team sync 由 `AIUpdateSystem` 执行；CppFSM flag 走 `SandboxAgentConfig` / `SandboxServices.agentConfig`；AIScheduler Lua 配置/诊断走 `SandboxAIScheduler`，TeamBlackboard Lua 主入口走 `SandboxTeam`，tactics Lua 主入口走 `SandboxTactics`。关联几乎所有 AI/对象模块。

## 7. 验证策略

- 回归 sample：`Sandbox6`(调度/导航)、`Sandbox12`(team)、`Sandbox18`(影响图)。

## 8. 已知 gap / 相关文档

- 待：ObjectManager 后续只保留阶段编排和对象聚合，新增 tactics API 继续放到 [[ai-tactics]] 的专属 service/global。`docs/design/architecture-improvement-plan.md` C3、`docs/planning/long-term-iteration-plan.md`。
