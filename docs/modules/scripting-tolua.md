# Lua 绑定（alias: scripting-tolua）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

C++↔Lua 绑定：tolua++ 生成导出、手工钩子捕获 Lua 回调、对象级脚本环境隔离。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/scripting/`、生成脚本 `src/HelloOgre3D/tolua.bat`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `SandboxToLua.pkg` | 输入 | `$cfile` 列举导出头（ObjectManager/SandboxMgr/AgentObject/AIController/DecisionTree/BehaviorTree…） |
| `SandboxToLua.cpp` | 生成物 | tolua++ 产物（**勿手编、除非手术式改单个绑定函数**） |
| `ManualToLua.cpp` | 手工钩子 | `setPluginEnv`/`SetEvaluator`/`SetCondition` 捕获 Lua function ref |
| `LuaPluginMgr.h` | 插件 | `BindLuaPluginByFile` 对象级隔离 env |

## 4. 公开能力要点

- tolua 自动绑定 + 手工钩子（Lua closure 捕获）+ 动态脚本加载与环境隔离。

## 5. 约束与红线

- **改导出 API：优先手术式改 `SandboxToLua.cpp` 对应绑定函数 + 头文件签名；禁 `tolua.bat` 全量重生成**——曾致 Sandbox18 崩溃（0xC0000409 栈溢出）。
- 改绑定必须跑用到它的 sample（影响图绑定改动跑 `Sandbox17` **和** `Sandbox18`；DT/BT 回调跑 `Sandbox7/8`）。
- **P2/P5 导出收窄（2026-06-20）**：`SoldierObject` 不再向 Lua 导出 `getWeapon`/AI/maxHealth/ammo/敌人查询/移动目标/射击/Enter*Anim 纯组件转发，`AgentObject:getBody/GetAnimation/GetObjectASM` 也已撤出 Lua 导出；Soldier 这批 C++ 兼容转发实现也已删除；`AgentLocomotion` 已进入 `SandboxToLua.pkg`，`BaseObject:GetLocomotionComponent()` 可供 Lua 直取；`AgentObject` 不再向 Lua 导出 `SetTarget/GetTarget/SetTargetRadius/GetTargetRadius`、`SetMaxForce/GetMaxForce/SetMaxSpeed/GetMaxSpeed`、`SetPath/GetPath/HasPath`、路径采样、`ForceTo*`、`ApplyForce`、`GetMass/GetHeight/GetRadius` 等 pure Locomotion facade，相关 Lua 调用已改走 `AgentComponentAccess.lua` 或 `AgentLocomotion`；同批 target/path/maxForce/maxSpeed/steering force/shape getter/`ApplyForce`/`GetLocomotion`/`GetAdapter` 的 `AgentObject` C++ facade 也已删除，生成绑定中不再有 `AgentObject_*` 旧函数；`AIController`、`DecisionTreeDriver`、`BehaviorTreeDriver`、`LuaDecisionAction`、`LuaBehaviorAction` 向 Lua 暴露泛化 `GetAgentOwner()` 而非 Soldier `GetOwner()`；DT/BT Lua action 生命周期回调已统一传 `u[AgentObject]`；脚本改走 `GetAIComponent()` / `GetAttribComponent()` / `GetWeaponComponent()` / `GetAnimComponent()` / `GetLocomotionComponent()` 或 `AgentComponentAccess.lua`，避免新脚本继续依赖对象层样板。
- **P5 装配入口（2026-06-20）**：`ObjectFactory` 已向 Lua 导出 `CreateAgentWithProfile(agentType, profileName, filepath)`，运行时通过 `SandboxObjects:CreateAgentWithProfile(...)` 创建命名 profile 的普通 `AgentObject`；RuntimeDiag 使用 `component_probe` profile 验证新绑定，并使用 `animated_probe` + `NonSoldierAnimProbeAgent.lua` 验证非 Soldier animated mesh / body ASM 状态请求链路。
- **AI RuntimeDiag 聚合入口（2026-06-20）**：`ObjectManager:buildAiRuntimeDebugSummary(maxAgents)` / `buildAiRuntimeDebugSummaryForAgent(maxAgents, focusAgentId)` 已导出给 Lua，`runtime_diagnostics.lua` 优先打印 `[AIRuntimeDiag]` 聚合块，并可通过 `HELLO_RUNTIME_DIAGNOSTIC_FOCUS_AGENT_ID` 聚焦单个 agent；改动该入口后至少跑 `Sandbox6 -RuntimeDiag` 或相关 AI sample smoke。
- **BT trace sampling / tick LOD / distance LOD / frame budget / condition cache（2026-06-20/21）**：`BehaviorTreeDriver:SetDebugTraceSampleInterval(interval)` / `GetDebugTraceSampleInterval()` 已导出给 Lua；默认也可用环境变量 `HELLO_BT_TRACE_SAMPLE_INTERVAL` 控制。`SetRuntimeTickIntervalMs(intervalMs)` / `GetRuntimeTickIntervalMs()` 与 `SetRuntimeTickStaggerEnabled(enabled)` / `IsRuntimeTickStaggerEnabled()` 已导出；`SetRuntimeDistanceLod(nearDistance, farDistance, maxIntervalMultiplier)` / `GetRuntimeDistanceLodNearDistance()` / `GetRuntimeDistanceLodFarDistance()` / `GetRuntimeDistanceLodMaxIntervalMultiplier()` / `GetRuntimeDistanceLodLastMultiplier()` 已手术式补入绑定；`SetRuntimeMaxTreeTicksPerFrame(maxTicksPerFrame)` / `GetRuntimeMaxTreeTicksPerFrame()` 已导出；`ConfigManager` 可从 preset `behaviorTree.tickIntervalMs` / `tickStagger` / `distanceLodNear` / `distanceLodFar` / `distanceLodMaxMultiplier` / `maxTreeTicksPerFrame` 或 `HELLO_BT_TICK_INTERVAL_MS` / `HELLO_BT_TICK_STAGGER` / `HELLO_BT_DISTANCE_LOD_NEAR` / `HELLO_BT_DISTANCE_LOD_FAR` / `HELLO_BT_DISTANCE_LOD_MAX_MULTIPLIER` / `HELLO_BT_MAX_TREE_TICKS_PER_FRAME` 配置 BT runtime tick/distance LOD 与每帧树 tick budget。`Blackboard:GetRevision()` / `GetKeyRevision(key)` 与 `LuaCondition:SetResultCacheEnabled(...)` / `SetResultCacheTtlMs(...)` / `AddResultCacheDependencyKey(...)` / cache stats 已导出，用于 `BehaviorTreeLoader` 配置黑板依赖条件缓存。
- **AgentPerceptionSystem Hearing/Danger（2026-06-20）**：`AgentPerceptionSystem` 已导出为 Lua 全局 `SandboxPerception`，提供 `configureHearingDangerSense(...)` / `clearHearingDangerSense()` / `publishHearingDangerEvent(...)` 与 `getHearingDanger*Count()` 统计；`Sandbox14` 用它让 C++ sense 接管听觉/危险扫描和 `RetreatPoint` 写回。
- **TeamBlackboard typed facts（2026-06-20/21）**：`TeamBlackboardService:rememberTeamFact(...)` / `writeBestTeamFactToBlackboard(...)` / `getTeamBlackboardTypedFactCount()` / `getTeamBlackboardTypedReportCount()` 已导出给 Lua；`hasBestTeamFact(...)` 与 `getBestTeamFact*` 字段级 getter 已手术式补入绑定，`TeamBlackboard.lua:GetBestCppFact(...)` 可直接读取最佳 typed fact；`TeamBlackboard.lua` 用它同步 `SupportRequested` / `SupportResponded` / `FocusTarget` / `RetreatPoint` / `FormationSlot`。
- **Tactics layer update policy（2026-06-20）**：`TacticalService:configureTacticalInfluenceLayerUpdate(layerName, intervalMs, dirtyOnly)` / `markTacticalInfluenceLayerDirty(layerName)` 已导出给 Lua；`Sandbox18` 用它配置 danger/team/objective 的 interval 与 dirty-only 更新策略。
- **Tactics query candidate limit（2026-06-20）**：`TacticalService:configureTacticalQueryCandidateLimit(maxCandidates)` / `getTacticalQueryCandidateLimit()` 已导出给 Lua；`chapter9_tactics_cpp_pressure` 用它固定战术查询候选点预算并在 smoke 中检查 `candidateLimit/capped`。
- **Tactics debug config（2026-06-21）**：`TacticalService:configureTacticalInfluenceLayerDebug(layerName, yOffset, threshold, maxCells, drawNeutralCells, projectToNav, maxProjectionDistance, navMeshName, drawOrder)` 已手术式补入绑定；`setTacticalInfluenceLayerDebugOrder(layerName, drawOrder)` 保留为兼容排序入口。现有 `drawTacticalInfluenceLayer` / `rebuildTacticalInfluenceLayerDebugVisual` 会在 C++ `TacticalDebugDrawService` 记录 per-layer y/threshold/maxCells/drawNeutral/projectToNav/nav/order 配置，并在 `buildTacticalInfluenceDebugSummary()` 输出 `[TacticalDebugDraw]`。
- Lua function ref（evaluator/condition）须在对象销毁/reload 前清理，防悬空。

## 6. 数据流 / 与其他模块关系

编译期 `.pkg →(tolua++)→ .cpp`；运行期 `tolua_*_open` 注册 + `ManualToLua` 钩子。导出几乎所有 [[systems-manager]] [[objects]] [[ai-*]] 类给 [[ai-scripts]] [[samples]]。

## 7. 验证策略

- 改 .pkg/绑定后：编译 + 跑用到该绑定的 sample（DT/BT 跑 `Sandbox7/8`；影响图跑 `Sandbox17/18`；`SandboxPerception` 跑 `hearing_danger`；`SandboxTeam` typed getter 跑 `team_blackboard`）。

## 8. 已知 gap / 相关文档

- 待：导出清单治理、回调生命周期清理流程。`docs/design/architecture-improvement-plan.md` P1、`AGENTS.md`（tolua.bat 说明）。
