# Project Snapshot

<!-- AI_README_PROMPT: BEGIN -->

# AI 阅读建议提示块 (v7)

> 你现在拿到的是一个项目快照目录（snapshot/_ai_snapshot/）。
> 请严格按“推荐顺序”读取这些文件，并按要求产出分析结果。
> 如果信息不足，请明确说“不足”，并指出需要哪类补充（而不是凭空猜测）。

---

## 0. 推荐阅读顺序（必须按这个来）

1) `PROJECT_SNAPSHOT.md`
   - 获取：项目概览、核心候选入口、Top 模块、CORE 子图热点、第三方依赖概览

2) `entry_candidates.md`
   - 获取：Entry / Tick / Script 的候选文件清单（CORE 优先），以及提示词命中情况

3) `index.jsonl`
   - 获取：文件清单（lines/size/module2/module4/类别） + 过滤后的 symbols（类/函数/宏/namespace）
   - 任务：推断模块边界、找“最可能是系统骨架”的目录/文件

4) `edges.jsonl`
   - 获取：依赖关系（C++ include、Lua require/dofile、C++->Lua hint）
   - 任务：构建“入口->关键模块->脚本/AI系统”的依赖路径

5) `dir_map.jsonl`
   - 获取：全量目录树与文件类型分布（不读内容）
   - 任务：补齐宏观结构，确认有哪些系统/资源/脚本分布

6) `snapshot_meta.json`
   - 获取：快照的过滤规则、核心目录规则、模块粒度、阈值
   - 任务：避免误解（比如 ThirdParty 被折叠/跳过、读取大小限制等）

---

## 1. 第一轮输出（读完 1~2 之后就要给）

请输出以下结构（必须包含小标题）：

### A) 项目一句话总结
- 用一句话描述项目“是什么 + 核心循环大概在哪 + 用了哪些脚本/AI机制”。

### B) 入口链路猜测（Top 3）
- 每条链路形如：`入口文件 -> Tick/Loop -> AI/状态机/行为树 -> Lua脚本/资源`
- 每条链路说明：为什么这么推断（来自 candidates/提示词命中）

### C) 模块分区（module2 维度）
- 列出 Top 8 module2，并说明它们的可能职责
- 标注：哪些属于 CORE（按 CORE_PATH_PREFIXES）

---

## 2. 第二轮输出（读完 3~6 后再给）

### D) 依赖图关键点（基于 edges.jsonl）
- Top 10 “依赖最重的文件”（outgoing edges 多）
- Top 5 “依赖最重的 module2”
- 说明：哪些是“中心模块”、哪些是“叶子模块”

### E) AI/脚本相关路径（如果存在）
- 输出 Lua require 链路（3 条代表性链路）
- 输出 C++ 调 Lua 的证据（cpp_to_lua_hint）

### F) 风险与建议（面向工程推进）
- 3 个结构风险（比如：耦合、循环依赖、入口不清晰、脚本散落）
- 3 个可落地的下一步建议（具体到目录/文件）

---

## 3. 输出约束（必须遵守）

- 不要凭空猜测不存在的系统；只能基于 snapshot 文件中的证据推断。
- 如果遇到“未索引/太大跳过”的文件，要明确指出，并建议用更小的抽样/单独快照补齐。
- 任何结论都尽量指向：具体路径（文件/目录/module2）。

---

## 4. 你最终要交付的内容（必须给）

1) “入口链路 Top3” + 证据
2) “核心模块 Top8（module2）” + 职责猜测
3) “依赖中心 Top10 文件/Top5 模块”
4) “下一步建议（3 条可执行）”

<!-- AI_README_PROMPT: END -->

---

- Generated at: `2026-02-14 22:50:35`
- Project root: `C:/Workspace/HelloOgre3D`
- Total files: `2694`
- Indexed files: `849`

> Note: Engine layer is infrastructure; for gameplay logic, focus on `src/HelloOgre3D/` (CORE).

## Quick Entry Points (Top)
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnim.cpp`
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnim.h`
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnimState.cpp`
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnimState.h`
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnimStateMachine.cpp`
- `src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp`
- `src/HelloOgre3D/client/DemoHelloWorld.cpp`
- `src/HelloOgre3D/client/DemoHelloWorld.h`
- `src/HelloOgre3D/client/main.cpp`
- `src/HelloOgre3D/game/ClientManager.cpp`
- `src/HelloOgre3D/game/ClientManager.h`
- `src/HelloOgre3D/sandbox/ai/decision_tree/DecisionTree.cpp`
- `src/HelloOgre3D/sandbox/ai/decision_tree/DTActionBase.cpp`
- `src/HelloOgre3D/sandbox/ai/decision_tree/DTActionBase.h`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.h`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentStateFactory.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentStateFactory.h`
- `src/HelloOgre3D/sandbox/ai/steer/AgentPath.cpp`
- `src/HelloOgre3D/sandbox/core/object/BaseObject.h`

## Tick / Update / AI Loop (Top)
- `src/HelloOgre3D/sandbox/objects/SoldierObject.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentFSM.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentFSM.h`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp`
- `src/HelloOgre3D/sandbox/objects/AgentObject.cpp`
- `src/HelloOgre3D/sandbox/objects/RenderableObject.cpp`
- `src/HelloOgre3D/sandbox/objects/SoldierObject.h`
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnimStateMachine.cpp`
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnimStateMachine.h`
- `src/HelloOgre3D/sandbox/objects/components/RenderComponent.cpp`
- `src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp`
- `src/HelloOgre3D/sandbox/ai/decision_tree/DTActionBase.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.h`
- `src/HelloOgre3D/sandbox/ai/fsm/states/AgentLuaState.h`
- `src/HelloOgre3D/sandbox/ai/fsm/states/IdleState.cpp`
- `src/HelloOgre3D/sandbox/core/object/BaseObject.h`
- `src/HelloOgre3D/sandbox/objects/AgentObject.h`
- `src/HelloOgre3D/sandbox/objects/BlockObject.cpp`
- `src/HelloOgre3D/sandbox/objects/BlockObject.h`
- `src/HelloOgre3D/sandbox/objects/RenderableObject.h`

## Script / Lua / Binding (Top)
- `src/HelloOgre3D/game/GameManager.cpp`
- `src/HelloOgre3D/common/ScriptLuaVM.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/states/AgentLuaState.cpp`
- `src/HelloOgre3D/sandbox/ai/fsm/states/AgentLuaState.h`
- `src/HelloOgre3D/sandbox/ai/fsm/states/MoveState.cpp`
- `src/HelloOgre3D/sandbox/core/object/BaseObject.h`
- `src/HelloOgre3D/sandbox/core/object/LuaEnvObject.cpp`
- `src/HelloOgre3D/sandbox/core/object/LuaEnvObject.h`
- `src/HelloOgre3D/sandbox/objects/AgentObject.h`
- `src/HelloOgre3D/sandbox/objects/VehicleObject.cpp`
- `src/HelloOgre3D/sandbox/scripting/LuaPluginMgr.h`
- `src/HelloOgre3D/sandbox/scripting/ManualToLua.cpp`
- `src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp`
- `src/HelloOgre3D/sandbox/systems/manager/ObjectManager.cpp`
- `src/HelloOgre3D/sandbox/systems/manager/ObjectManager.h`
- `src/HelloOgre3D/sandbox/systems/manager/SandboxMgr.cpp`
- `src/HelloOgre3D/sandbox/systems/service/ObjectFactory.cpp`
- `bin/res/scripts/samples/game_init.lua`
- `premake/premake.lua`

## Top Modules (by symbol count)
- `src/Engine` symbols=1982
- `src/HelloOgre3D` symbols=711
- `bin/res` symbols=68
- `tools/tolua++` symbols=13
- `premake/samples.lua` symbols=1

## CORE Subgraph Summary
### CORE Modules (by symbol count)
- `src/HelloOgre3D` symbols=711
- `bin/res` symbols=68
- `premake/samples.lua` symbols=1

## Top Edge Hotspots (Outgoing)
- `src/Engine/ogre3d/include/Ogre.h` out_edges=95
- `src/Engine/ogre3d/src/OgreRoot.cpp` out_edges=48
- `src/Engine/ogre3d/src/OgreSceneManager.cpp` out_edges=45
- `src/Engine/ogre3d/include/OgreStdHeaders.h` out_edges=42
- `src/Engine/ogre3d/include/OgreStableHeaders.h` out_edges=37
- `src/Engine/ogre3d/src/OgreEntity.cpp` out_edges=27
- `src/Engine/ogre3d_direct3d9/src/OgreD3D9RenderSystem.cpp` out_edges=27
- `src/Engine/ogre3d/include/OgreSceneManager.h` out_edges=26
- `src/Engine/ogre3d/src/compile_OgreMain_0.cpp` out_edges=26
- `src/Engine/ogre3d/src/compile_OgreMain_1.cpp` out_edges=26

## Top Incoming Dependency Targets
- `src/Engine/ogre3d/include/OgrePrerequisites.h` in_edges=224
- `src/Engine/ogre3d/include/OgreStableHeaders.h` in_edges=201
- `src/Engine/ogre3d/include/OgreHeaderPrefix.h` in_edges=150
- `src/Engine/ogre3d/include/OgreHeaderSuffix.h` in_edges=148
- `src/Engine/ogre3d/include/OgreException.h` in_edges=125
- `src/Engine/ogre3d/include/OgreLogManager.h` in_edges=81
- `src/Engine/ogre3d/include/OgreRoot.h` in_edges=73
- `src/Engine/ogre3d/include/OgreStringConverter.h` in_edges=71
- `src/Engine/ogre3d/include/OgreVector3.h` in_edges=62
- `src/Engine/ogre3d/include/OgreString.h` in_edges=57

## Top Incoming Modules (module2)
- `src/Engine` in_edges=3704
- `src/HelloOgre3D` in_edges=313
- `src/External` in_edges=46

## Example Control-Flow Hypotheses (heuristic)
- `src/HelloOgre3D/client/main.cpp` -> `src/HelloOgre3D/game/ClientManager.cpp` -> `src/HelloOgre3D/common/ScriptLuaVM.cpp` (evidence: entry_hint=main.cpp, tick_hint=ClientManager, script_hint=Lua)
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnim.h` -> `src/HelloOgre3D/sandbox/ai/fsm/AgentFSM.cpp` -> `src/HelloOgre3D/common/ScriptLuaVM.cpp` (evidence: candidates rank≈2)
- `src/HelloOgre3D/sandbox/objects/animation/AgentAnimState.cpp` -> `src/HelloOgre3D/sandbox/ai/fsm/AgentFSM.h` -> `src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp` (evidence: candidates rank≈3)

## Key Output Files
- `PROJECT_SNAPSHOT.md`: front page (with AI prompt + hotspots + hypotheses)
- `entry_candidates.md`: CORE-first entry/tick/script candidates
- `index.jsonl`: file records + filtered symbols (module2/module4)
- `edges.jsonl`: include/include_resolved + Lua edges + C++->Lua hints
- `dir_map.jsonl`: full directory map (lightweight)
- `snapshot_meta.json`: config/rules/stats

Tip: If you need deeper analysis on a submodule, generate an extra focused snapshot on that sub-tree only.
