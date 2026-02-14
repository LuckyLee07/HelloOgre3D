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

- Generated at: `2026-02-14 16:10:27`
- Project root: `C:/Workspace/HelloOgre3D`
- Total files: `2694`
- Indexed files: `849`

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

## Key Output Files (Lite 6)
- `dir_map.jsonl`: full project map (lightweight)
- `index.jsonl`: file manifest + filtered symbols (jsonl)
- `edges.jsonl`: unified edges (include/require/hint) (jsonl)
- `entry_candidates.md`: entry/tick/script candidates (annotated)
- `snapshot_meta.json`: snapshot meta/config/rules

## Top Third-Party Libraries (summary)
- `src/External` top libs:
  - `src/External/bullet_collision` total=217 code=215 config=0 doc=2
  - `src/External/lua` total=137 code=135 config=0 doc=0
  - `src/External/ois` total=84 code=82 config=0 doc=2
  - `src/External/bullet_dynamics` total=52 code=50 config=0 doc=2
  - `src/External/bullet_linearmath` total=36 code=34 config=0 doc=2
  - `src/External/detour` total=16 code=14 config=0 doc=2
  - `src/External/opensteer` total=15 code=13 config=0 doc=2
  - `src/External/recast` total=15 code=13 config=0 doc=2
  - `src/External/ogre3d_gorilla` total=4 code=2 config=0 doc=2
- `src/Engine/ThirdParty` top libs:
  - `src/Engine/ThirdParty/freetype` total=526 code=453 config=0 doc=3
  - `src/Engine/ThirdParty/openexr` total=196 code=193 config=0 doc=3
  - `src/Engine/ThirdParty/freeimage` total=101 code=99 config=0 doc=2
  - `src/Engine/ThirdParty/libjpeg` total=61 code=59 config=0 doc=2
  - `src/Engine/ThirdParty/libtiff4` total=56 code=54 config=0 doc=2
  - `src/Engine/ThirdParty/libopenjpeg` total=51 code=49 config=0 doc=2
  - `src/Engine/ThirdParty/zlib` total=28 code=26 config=0 doc=2
  - `src/Engine/ThirdParty/zzip` total=27 code=25 config=0 doc=2
  - `src/Engine/ThirdParty/libpng` total=24 code=22 config=0 doc=2
  - `src/Engine/ThirdParty/libraw` total=21 code=18 config=0 doc=1

## Top Modules (by symbol count)
- `src/Engine` symbols=1982
- `src/HelloOgre3D` symbols=711
- `bin/res` symbols=68
- `tools/tolua++` symbols=13
- `premake/samples.lua` symbols=1

## Top Edge Hotspots (most dependencies)
- `src/Engine/ogre3d/include/Ogre.h` out_edges=190 lines=129 module2=`src/Engine`
- `src/Engine/ogre3d/src/OgreRoot.cpp` out_edges=97 lines=1611 module2=`src/Engine`
- `src/Engine/ogre3d/src/OgreSceneManager.cpp` out_edges=91 lines=7356 module2=`src/Engine`
- `src/Engine/ogre3d/include/OgreStdHeaders.h` out_edges=84 lines=123 module2=`src/Engine`
- `src/Engine/ogre3d/include/OgreStableHeaders.h` out_edges=75 lines=82 module2=`src/Engine`
- `src/Engine/ogre3d/src/OgreEntity.cpp` out_edges=55 lines=2568 module2=`src/Engine`
- `src/Engine/ogre3d_direct3d9/src/OgreD3D9RenderSystem.cpp` out_edges=55 lines=4459 module2=`src/Engine`
- `src/Engine/ogre3d/include/OgreSceneManager.h` out_edges=53 lines=3675 module2=`src/Engine`
- `src/Engine/ogre3d/src/compile_OgreMain_6.cpp` out_edges=53 lines=27 module2=`src/Engine`
- `src/Engine/ogre3d/src/compile_OgreMain_0.cpp` out_edges=52 lines=27 module2=`src/Engine`
- `src/Engine/ogre3d/src/compile_OgreMain_1.cpp` out_edges=52 lines=27 module2=`src/Engine`
- `src/Engine/ogre3d/src/compile_OgreMain_2.cpp` out_edges=52 lines=27 module2=`src/Engine`
- `src/Engine/ogre3d/src/compile_OgreMain_3.cpp` out_edges=52 lines=27 module2=`src/Engine`
- `src/Engine/ogre3d/src/compile_OgreMain_4.cpp` out_edges=52 lines=27 module2=`src/Engine`
- `src/Engine/ogre3d/src/compile_OgreMain_5.cpp` out_edges=52 lines=27 module2=`src/Engine`
- `src/Engine/ogre3d/src/OgreCompositorInstance.cpp` out_edges=47 lines=1303 module2=`src/Engine`
- `src/Engine/ogre3d/src/OgreScriptTranslator.cpp` out_edges=45 lines=7027 module2=`src/Engine`
- `src/Engine/ogre3d_procedural/include/Procedural.h` out_edges=44 lines=54 module2=`src/Engine`
- `src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp` out_edges=43 lines=5916 module2=`src/HelloOgre3D`
- `src/Engine/ogre3d/src/OgreMesh.cpp` out_edges=39 lines=2476 module2=`src/Engine`

## CORE Subgraph Summary (quick map)
- CORE_PATH_PREFIXES=['src/HelloOgre3D/', 'bin/res/', 'premake/', 'scripts/']
- Edge types (CORE-only):
  - cpp_include_raw: 538
  - cpp_include_resolved_or_unresolved: 538
  - lua_require: 29
  - cpp_to_lua_hint: 23
  - lua_dofile: 1
- Top CORE edge hotspots (from-files with most outgoing edges):
  - `src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp` out_edges=43 lines=5916 module2=`src/HelloOgre3D` hints(entry=2,tick=3,script=2)
  - `src/HelloOgre3D/sandbox/objects/BlockObject.cpp` out_edges=34 lines=259 module2=`src/HelloOgre3D` hints(entry=1,tick=2,script=0)
  - `src/HelloOgre3D/common/FileManager.cpp` out_edges=32 lines=266 module2=`src/HelloOgre3D` hints(entry=1,tick=0,script=0)
  - `src/HelloOgre3D/game/GameManager.cpp` out_edges=31 lines=189 module2=`src/HelloOgre3D` hints(entry=1,tick=2,script=5)
  - `src/HelloOgre3D/sandbox/objects/SoldierObject.cpp` out_edges=28 lines=303 module2=`src/HelloOgre3D` hints(entry=1,tick=4,script=0)
  - `src/HelloOgre3D/sandbox/systems/service/ObjectFactory.cpp` out_edges=27 lines=121 module2=`src/HelloOgre3D` hints(entry=0,tick=0,script=2)
  - `src/HelloOgre3D/game/GameToLua.cpp` out_edges=25 lines=3482 module2=`src/HelloOgre3D` hints(entry=0,tick=1,script=2)
  - `src/HelloOgre3D/sandbox/objects/VehicleObject.cpp` out_edges=24 lines=361 module2=`src/HelloOgre3D` hints(entry=1,tick=1,script=2)
  - `src/HelloOgre3D/game/ClientManager.cpp` out_edges=23 lines=321 module2=`src/HelloOgre3D` hints(entry=2,tick=3,script=0)
  - `src/HelloOgre3D/sandbox/objects/AgentObject.cpp` out_edges=22 lines=243 module2=`src/HelloOgre3D` hints(entry=1,tick=3,script=0)
  - `src/HelloOgre3D/sandbox/objects/RenderableObject.cpp` out_edges=22 lines=164 module2=`src/HelloOgre3D` hints(entry=1,tick=3,script=0)
  - `src/HelloOgre3D/common/ScriptLuaVM.cpp` out_edges=21 lines=628 module2=`src/HelloOgre3D` hints(entry=1,tick=0,script=4)
  - `src/HelloOgre3D/sandbox/objects/components/RenderComponent.cpp` out_edges=20 lines=165 module2=`src/HelloOgre3D` hints(entry=0,tick=3,script=0)
  - `src/HelloOgre3D/sandbox/systems/manager/ObjectManager.cpp` out_edges=20 lines=238 module2=`src/HelloOgre3D` hints(entry=1,tick=1,script=2)
  - `src/HelloOgre3D/sandbox/systems/manager/SandboxMgr.cpp` out_edges=20 lines=204 module2=`src/HelloOgre3D` hints(entry=0,tick=0,script=2)
  - `src/HelloOgre3D/sandbox/objects/animation/AgentAnimStateMachine.cpp` out_edges=18 lines=291 module2=`src/HelloOgre3D` hints(entry=2,tick=3,script=0)
  - `src/HelloOgre3D/sandbox/objects/components/AgentLocomotion.cpp` out_edges=18 lines=343 module2=`src/HelloOgre3D` hints(entry=0,tick=1,script=0)
  - `src/HelloOgre3D/sandbox/GameFunction.h` out_edges=16 lines=92 module2=`src/HelloOgre3D` hints(entry=0,tick=1,script=0)
  - `src/HelloOgre3D/sandbox/objects/components/PhysicsComponent.cpp` out_edges=16 lines=258 module2=`src/HelloOgre3D` hints(entry=0,tick=0,script=0)
  - `src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp` out_edges=15 lines=100 module2=`src/HelloOgre3D` hints(entry=1,tick=3,script=2)
- Top CORE modules (module2) by file count & hint strength:
  - `src/HelloOgre3D` files=137 hint_strength=240
  - `bin/res` files=23 hint_strength=83
  - `premake/premake.lua` files=1 hint_strength=6
  - `premake/samples.lua` files=1 hint_strength=3
