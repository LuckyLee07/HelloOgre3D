# 运行时切面（alias: runtime）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

引擎/中间件适配层：性能计数、资源诊断、相机控制、FairyGUI 系统集成。

## 2. 源码位置

- `src/HelloOgre3D/runtime/`（FairyGUI 详见 [[fgui]]）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `profiling/RuntimeProfileCounters.{h,cpp}` | 性能 | `Plot{AiScheduler,AgentPerception,HearingDangerSense,TeamBlackboard,FairyGuiService}Stats` + 帧时序 / Lua callback 计数；Tracy 关闭时是 no-op |
| `profiling/Profile.h` | 采样 | 时序宏 |
| `diagnostics/RuntimeResourceDiagnostics.{h,cpp}` | 诊断 | `BuildResourceDump` texture/mesh/buffer 清单 |
| `ogre/OgreCameraController.h` | 相机 | FREELOOK/ORBIT/MANUAL 相机控制；当前不扩展 FPS 模式 |
| `ui/fairygui/FairyGuiSystem.*` | UI | cocoslite 内嵌，见 [[fgui]] |
| `RuntimeToLua.{cpp,pkg}` | 绑定 | runtime 层 tolua |

## 4. 公开能力要点

- 性能上报（AI/UI/帧分项/Lua callback count）、资源快照、既有相机模式、FairyGUI 栈；FGUI `AiDebugPanel` 可读取统一 `[AIRuntimeDiag]` 并按 `focusAgentId` / `filterText` 参数化筛选。

## 5. 约束与红线

- 引擎/中间件耦合逻辑收口在 runtime（AGENTS.md 依赖流）。
- **判断 AI 成本看 `updateCall`/`perceptionSystem`，非 `cpuFrame`**（VM 上帧时间被渲染 engineGap 主导，见基线报告）。
- RuntimeProfileCounters 是 static，调用方控频；FairyGUI lua_bridge 是手工 glue（非 tolua），见 [[fgui]]。

## 6. 数据流 / 与其他模块关系

`ObjectManager/各 AI 系统 → RuntimeProfileCounters.Plot*`；`FairyGuiSystem ← ScriptLuaVM(lua_bridge)`；相机服务 [[systems-service]] CameraService 包装。

## 7. 验证策略

- 回归 sample：`Sandbox16`(性能采样)；gate：`run_fgui_production_gate.ps1`、`ai_perf_1000`。

## 8. 已知 gap / 相关文档

- 待：profiler UI、FairyGUI element inspector。`docs/planning/long-term-iteration-plan.md` §5、`docs/perf/ai-perf-release-baseline-20260612.md`。
