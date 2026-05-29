# 可视化编辑器开发任务清单（可逐条认领）

> 上位与排期口径：编辑器在 `docs/project-direction.md`（北极星）里属 **L3 / 缓做**——项目级第一优先是 L0 地基 + 第一个垂直切片。本文 §关键路径 里“最短见效路径 E0→E1”指的是**编辑器轨道内**的入口顺序；E0/E1（节点 uuid + 调试回传）因为也服务垂直切片的 AI 调试，可在地基阶段顺带拉早，其余 E2–E5 应在地基/切片之后再推进。
>
> 配套文档：设计与里程碑见 `docs/visual-editor-implementation-plan.md`；为什么这么做见 `docs/reference-minigame-patterns.md`。本文把里程碑拆成可认领的细任务，带**真实接口签名**（标注 [已存在]/[新增]/[修改]），签名指向 **HelloOgre3D 本仓库** 现有代码。
>
> 重要前提（已核实本仓库代码）：
> - **数据驱动行为树加载器已存在**：`bin/res/scripts/ai/behavior/BehaviorTreeLoader.lua` 的 `BehaviorTreeLoader.Build(config, agent, driver, blackboard, conditions)` + `BuildNode(cfg, context)` 已能从 `{node=, children=, action=, condition=, ...}` 表建出 C++ 行为树。现成配置 `bin/res/scripts/ai/behavior/config/SoldierBT.lua`。
> - **`BehaviorTreeDriver`（C++）工厂齐全**：`NewTree/NewSequence/NewSelector/NewLuaAction/NewCondition/NewWait/NewInverter/NewForceSuccess/NewForceFailure`、`SetNodeDebugName`、`SetDebugTraceEnabled/SetDebugTracePrintEnabled/GetLastDebugTrace/GetDebugTraceFrame`（`src/HelloOgre3D/sandbox/ai/behavior/BehaviorTreeDriver.h`）。
> - **`BehaviorNode`** 有 `SetDebugName/GetDebugName/Tick/Status`（`STATUS_RUNNING/SUCCESS/FAILURE`），但**没有稳定 nodeid**（`BehaviorNode.h`）。
> - **`BehaviorTrace`/`BehaviorTraceFrame`** 按 (label,status) 逐帧记录，**没有按 nodeid 的运行结果查询**（`BehaviorTrace.h`）。
> - `Blackboard` 强类型 get/set（Agent/Float/Int/Bool/String/Vec3）。
>
> 结论：**M0（数据驱动 BT）≈ 已完成**。可视化编辑器真正要补的是：① 节点 uuid；② 按 uuid 的运行态回传与高亮；③ 整套 FGUI 积木编辑器框架；④（触发器）统一事件总线。下面按 Epic 拆任务，每个任务可独立提交、独立验收。

---

## 任务编号与约定

- `E#` = Epic（对应里程碑）；`T#.#` = 具体任务。
- 每个任务给：**目标 / 改动文件 / 接口签名 / 验收 / 依赖**。
- 签名约定：`[已存在]` 当前代码已有；`[新增]` 需新增；`[修改]` 在现有基础上改。
- C++ 改动若涉及 tolua 导出，记得跑 `src\HelloOgre3D\tolua.bat` 重新生成绑定（`BehaviorNode`/`BehaviorTreeDriver`/`Blackboard` 已是 `//tolua_exports`）。

---

## E0：运行时地基（节点 uuid + 按 uuid 回传）—— 编辑器的最小前置

> 这是整个可视化的承重墙：没有“稳定 uuid + 能按 uuid 查运行态”，画布就无法高亮。其余 GUI 都建立在它上面。

### T0.1 给 BehaviorNode 增加稳定 nodeid `[修改]`
- 目标：每个运行时节点带一个编辑器分配的稳定 id，贯穿 编辑/序列化/codegen/高亮。
- 文件：`src/HelloOgre3D/sandbox/ai/behavior/BehaviorNode.h/.cpp`
- 接口：
  ```cpp
  // BehaviorNode 内（//tolua_begin 区）
  void SetNodeId(const std::string& id);        // [新增]
  const std::string& GetNodeId() const;         // [新增]
  // private: std::string m_nodeId;             // [新增]
  ```
- 验收：节点可设/取 nodeId；不影响现有 Tick；tolua 重新生成后 Lua 可调 `node:SetNodeId(...)`。
- 依赖：无。

### T0.2 BehaviorTreeLoader 读取并写入 nodeid `[修改]`
- 目标：让现有加载器把 `cfg.nodeid` 设到 C++ 节点上（codegen 产物里每节点带 `nodeid`）。
- 文件：`bin/res/scripts/ai/behavior/BehaviorTreeLoader.lua`
- 改动：在 `BuildNode` 每个分支建出 `node` 后，若 `cfg.nodeid` 非空则 `node:SetNodeId(cfg.nodeid)`（可在 `_SetDebugName` 旁加一个 `_SetNodeId(node, cfg)` helper 统一处理）。
- 验收：给 `SoldierBT.lua` 任意节点加 `nodeid="t1"`，运行后能从 C++ 侧读到。
- 依赖：T0.1。

### T0.3 按 uuid 的运行态结果记录与查询 `[修改/新增]`
- 目标：编辑器能按 nodeId 查“本帧/最近一帧的状态 + 是否 active”。
- 文件：`src/HelloOgre3D/sandbox/ai/behavior/BehaviorTrace.h/.cpp`、`BehaviorTreeDriver.h/.cpp`
- 接口：
  ```cpp
  // 方案：在 driver 上维护 nodeId -> 最近状态 的 map，Tick 时写入
  // BehaviorTreeDriver（//tolua_begin 区）
  int  GetNodeStatus(const std::string& nodeId) const;  // [新增] 返回 BehaviorNode::Status，缺省 STATUS_INVALID
  bool IsNodeActive(const std::string& nodeId) const;    // [新增] 本帧是否被 Tick 到
  int  GetTraceFrameIndex() const;                       // [已存在等价] GetDebugTraceFrame()
  // private: std::unordered_map<std::string,int> m_nodeStatus; // [新增]
  ```
  实现：复用现有 `BehaviorTrace::Record`/`TraceStatus` 链路——`BehaviorNode::TraceStatus` 已在每次 Tick 末被调用（见 `BehaviorNode.h:42`），在其落点处同时把 `(GetNodeId(), status)` 写进 driver 的 map。`BehaviorTraceFrame::Begin` 时清空 active 标记。
- 验收：开 debugTrace 跑 `SoldierBT`，能按 nodeid 查到每节点最近状态，未命中节点返回 INVALID。
- 依赖：T0.1、T0.2。

### T0.4 调试开关：暂停 / 单步 `[新增]`
- 目标：运行态可暂停、单步（“单步 = 放行一帧”）。
- 文件：新增 `src/HelloOgre3D/sandbox/ai/behavior/BTDebugControl.h/.cpp`（或挂到 `AIScheduler`）；接入 AI tick 调用点（`ObjectManager` AI tick / `AIController::TickAI`）。
- 接口：
  ```cpp
  class BTDebugControl {                          // [新增]，进程级单例或挂 SandboxContext
  public:
    static BTDebugControl& Get();
    void SetEnabled(bool on);  bool IsEnabled() const;
    void Pause(bool b);        bool IsPaused() const;
    void Step();               // 置 runOneFrame=true
    bool ConsumeRunOneFrame(); // tick 前调用：paused 且无 runOneFrame 时返回 false（跳过本帧）
  };
  ```
  接入：AI tick 入口处 `if (BTDebugControl::Get().IsEnabled() && !BTDebugControl::Get().ConsumeRunOneFrame()) return;`
- 验收：暂停后生物 AI 不再推进；单步一次只推进一帧。
- 依赖：无（与 T0.1–0.3 并行）。

---

## E1：调试回传闭环 + 只读高亮视图（先证明闭环，GUI 从简）

### T1.1 C++→Lua 调试数据桥 `[新增]`
- 目标：把“当前调试中的树 + 每节点状态”暴露给 Lua 编辑器。
- 文件：`runtime/RuntimeToLua.pkg`（或 `SandboxToLua.pkg`）+ 一个薄封装。
- 接口（Lua 可见）：
  ```text
  AIDebug:SetTargetAgent(objId)            -- 选定要调试的生物
  AIDebug:GetTreeNodeIds() -> {id,...}     -- 当前树所有 nodeid（供首帧建视图）
  AIDebug:GetNodeStatus(nodeId) -> int     -- 转发 driver:GetNodeStatus
  AIDebug:IsNodeActive(nodeId) -> bool
  AIDebug:Pause(b) / AIDebug:Step() / AIDebug:SetEnabled(b)
  ```
- 验收：Lua 能选一个生物、按帧拉到各节点状态。
- 依赖：T0.3、T0.4。

### T1.2 只读树视图（复用现有 FGUI Tree 控件）`[新增 Lua]`
- 目标：用本项目已有的 FGUI `Tree/TreeNode` 封装，把行为树画成可折叠树，按状态四态着色（running/success/fail/idle）。
- 文件：`bin/res/scripts/editor/ai/AIDebugTreeView.lua`（走现有 `FairyGuiManager` MVC）。
- 机制：开调试 → `AIDebug:GetTreeNodeIds` 建树 → 每帧 `RegisterTick` 轮询 `GetNodeStatus/IsNodeActive` → 设节点 controller 状态着色。
- 验收：跑 `Sandbox8`，开调试面板能实时看到当前生物行为树的运行/成功/失败节点，暂停/单步可用。
- 依赖：T1.1；本项目 FGUI Tree（已具备，见 `HELLO_FGUI_TREE_SELF_TEST`）。

> ✅ E1 完成即拿到“可视化调试”的核心价值，此时还没有拖拽编辑器。

---

## E2：通用积木编辑器框架（FGUI，可拖拽）

### T2.1 FGUI 资源包 blockeditor `[新增 资源]`
- 目标：FairyGUI 包含 `Block / LinkLine / ParamSlot / Canvas / Palette / Inspector / Toolbar` 组件（见 plan §5.1）。
- 文件：`bin/res/assets/...`（FGUI 工程）→ 导出 → `bin/res/fuires/blockeditor.*` + manifest/AutoGen（走现有 `tools/fgui_autogen`）。
- 验收：包能加载，`FairyGuiManager:Open` 能开出空画布 + 工具栏 + 积木库 + Inspector 四区。
- 依赖：无（FGUI 既有能力）。

### T2.2 CSV 加载器（双行表头）`[新增]`
- 目标：最小 CSV 加载（第 1 行中文注释、第 2 行英文字段名、第 3 行起数据），C++/Lua 可读。第一版可纯 Lua 实现以省事。
- 文件：`bin/res/scripts/common/CsvLoader.lua`（纯 Lua 版）。
- 接口：`CsvLoader.Load(path) -> { rows = {{field=value,...}...}, byId = {[id]=row} }`
- 验收：能读 `block_def_ai.csv` 并按 ID 取行。
- 依赖：无。

### T2.3 积木定义三表 + BlockCfgMgr `[新增]`
- 目标：积木数据驱动（plan §5.3）。
- 文件：`bin/res/csv/block_def_ai.csv`、`block_param_ai.csv`、`block_enum_ai.csv`；`bin/res/scripts/editor/blockeditor/BlockCfgMgr.lua`
- 接口：
  ```text
  BlockCfgMgr:Load(defCsv, paramCsv, enumCsv)
  BlockCfgMgr:GetBlockDef(defId) -> { id, name, blockType, category, returnType,
                                      descTemplate, runtimeName, genType, params={...} }
  BlockCfgMgr:GetBlocksByCategory(cat) -> {def,...}        -- 填充 Palette
  BlockCfgMgr:GetOutputBlocksByReturnType(t) -> {def,...}  -- 取值积木下拉
  ```
- 验收：改 CSV 即可让 Palette 多/少一个积木，无需改代码。
- 依赖：T2.2。

### T2.4 积木数据模型 + 画布渲染 `[新增]`
- 目标：BlockInstance/Workspace 内存结构（plan §5.2）+ 把模型渲染成 FGUI 积木与连线。
- 文件：`editor/blockeditor/Block.lua`、`LinkLine.lua`、`Canvas.lua`
- 关键：连线 `setRotation(angle)+setWidth(length)`；画布缩放只 `canvasRoot:setScale`；坐标换算 `local=(screen-canvasXY)/scale`。
- 验收：给定一个手写 Workspace 表，能正确画出积木树和连线，可缩放平移。
- 依赖：T2.1、T2.3。

### T2.5 交互：拖拽 / 连接 / 吸附 / 选中 `[新增]`
- 目标：从 Palette 拖出积木、拼接吸附（语句/子节点/参数槽）、连接类型校验、选中打开 Inspector（plan §5.4）。
- 文件：`editor/blockeditor/Canvas.lua`（交互）、`Inspector.lua`
- 验收：纯拖拽能拼出一棵图，取值积木只能插入兼容参数槽。
- 依赖：T2.4。

### T2.6 撤销重做（命令模式 + 双栈）`[新增]`
- 目标：plan §5.5。
- 文件：`editor/blockeditor/CmdManager.lua` + `cmds/*`（AddBlock/RemoveBlock/Link/Unlink/MoveBlock/SetParam）
- 接口：`CmdManager:AddAndExecute(cmd)`、`:Undo()`、`:Redo()`；`CmdBase:Execute()/:Revoke()`；删除类命令存 `ToTable()` 快照。
- 验收：所有编辑操作可撤销/重做；工具栏按钮按剩余数启用/禁用。
- 依赖：T2.5。

### T2.7 工程序列化（源/产物分离）`[新增]`
- 目标：plan §5.6。`<ws>.proj`（JSON 源）+ `code/<ws>.lua`（产物）。
- 文件：`editor/blockeditor/ProjectManager.lua`、`Serialize.lua`（白名单 `SerializeMembers`）
- 接口：`ProjectManager:Save(ws)`、`:Load(id) -> ws`、`:New(meta)`
- 验收：保存重开后图完整还原（含坐标/参数/连接）。
- 依赖：T2.6。

---

## E3：AI 编辑器打通（编辑 → 生成 → 运行 → 高亮）

### T3.1 codegen 框架 + AI 生成器 `[新增]`
- 目标：Workspace → 行为树 Lua 表（**就是 `BehaviorTreeLoader` 吃的格式**，每节点带 `nodeid`）。
- 文件：`editor/blockeditor/Codegen.lua`（框架，`genMethodDB` 注册式）+ `editor/ai/AICodegen.lua`
- 产物示例（与现有 `config/SoldierBT.lua` 同构 + nodeid）：
  ```lua
  return {
    actionDir = "res/scripts/ai/decision/actions/",
    actions = { shoot="ShootAction.lua", ... },
    tree = {
      node="Selector", nodeid="b_0001", children={
        { node="Sequence", nodeid="b_0002", children={
            { node="Condition", nodeid="b_0003", condition="HasEnemy" },
            { node="Action",    nodeid="b_0004", action="shoot" },
        }},
      }
    }
  }
  ```
- 验收：编辑器产物能被现有 `BehaviorTreeLoader.Build` 直接加载（**零改动加载器，除 T0.2 的 nodeid 支持**）。
- 依赖：T2.7、T0.2。

### T3.2 AI 积木库 CSV 落地 `[新增 数据]`
- 目标：把现有 BT 能力填进 `block_def_ai.csv`：Selector/Sequence/Condition/Action/Wait/Inverter/ForceSuccess/ForceFailure + 取值/黑板积木。
- 关键：`runtimeName` 对应 loader 的 `node` 字段值（"Selector"/"Sequence"/...）；Condition/Action 的下拉来自现有 `SoldierConditions.lua` 注册的条件名与 `actions` 表。
- 验收：Palette 显示全部 BT 积木，参数槽正确。
- 依赖：T2.3、T3.1。

### T3.3 运行时高亮接入画布 `[新增 Lua]`
- 目标：把 E1 的调试回传接到 E2 画布（不再是只读树视图，而是真画布积木高亮）。
- 文件：`editor/ai/AIDebugBinding.lua`
- 机制：调试时按 nodeId 找画布 Block UI，`GetNodeStatus/IsNodeActive` → 四态着色 + 显示本帧参数值。
- 验收：编辑器拼树 → 保存生成 → 生物按它行动 → 画布实时高亮当前节点；暂停/单步可用。
- 依赖：T3.1、T1.1。

> ✅ E3 完成 = AI 可视化配置全链路打通（编辑→生成→运行→可视化调试）。

---

## E4：触发器编辑器打通（依赖统一事件总线）

> 前置硬依赖：统一事件总线（Local/Team/Global + 参数化事件名 + 四件套），见 `reference-minigame-patterns.md` §3 与 `architecture-improvement-plan.md`。未就绪前 E4 不启动。

### T4.1 统一事件总线（前置）`[新增/扩展]`
- 目标：把现有 Local 事件扩成统一总线：`map<事件名, 监听者>` + 过滤器即订阅 + 延迟一帧分发 + 节流 + 上限保护；支持 `事件?objid=N` 参数化事件名。
- 文件：`src/HelloOgre3D/sandbox/...`（在现有 `SandboxEventPayload`/dispatcher 上扩）。
- 验收：能注册/分发全局与对象级事件，回调在 agent 销毁后自动失效。
- 依赖：见架构方案 P1（解耦）。

### T4.2 触发器积木库 + ApiProxy `[新增]`
- 目标：事件/条件/动作积木（CSV）+ 块ID→Lua 方法名映射表。
- 文件：`bin/res/csv/block_def_trigger.csv` 等；`editor/trigger/ApiProxy.lua`（`[id]="Trigger.Unit:MoveTo"`）。
- 验收：Palette 显示 ECA 积木；动作积木能配参数。
- 依赖：T2.3、T4.1。

### T4.3 触发器 codegen（生成可执行 Lua）`[新增]`
- 目标：Workspace → 可执行 Lua 文本（注册事件监听 + 条件 `if not ret then return end` + 动作顺序拼接 + 对象级用 `事件?objid=`）。
- 文件：`editor/trigger/TriggerCodegen.lua`
- 验收：产物可加载执行，触发链正确。
- 依赖：T4.2、T3.1（共用 codegen 框架）。

### T4.4 触发器运行时加载 + 沙盒 `[新增]`
- 目标：`loadstring + setfenv(沙盒环境)` 执行生成 Lua，把监听注册到事件总线；命中回传高亮。
- 文件：`bin/res/scripts/runtime/trigger/TriggerRuntime.lua`
- 验收：拼“玩家进区域→生物攻击”，运行正确触发，命中积木高亮。
- 依赖：T4.3、T4.1。

---

## E5：工程化与打磨

- T5.1 工程库管理（多工程/复制/删除/uuid 生成与权限）`[新增]`
- T5.2 资源依赖清单 `<ws>.dep` + 产物版本水印与自动重生成 `[新增]`
- T5.3 积木搜索 / 注释 / 分组框 `[新增]`
- T5.4 移动端手势（双指捏合缩放）`[新增]`
- T5.5 dev 子 VM（触发器作者脚本与内核隔离，替换 T4.4 的 loadstring 沙盒）`[新增]`

---

## 关键路径与并行建议

```text
E0(地基) ──► E1(调试闭环) ──────────────┐
   │                                     ├──► E3(AI 编辑器打通) ──► E5(打磨)
   └──► E2(积木框架) ───────────────────┘
                                  事件总线(前置) ──► E4(触发器编辑器)
```

- **最短见效路径**：E0 → E1（拿到“可视化调试”，无需画布）。
- **AI 编辑器路径**：E0 + E2 → E3。
- **触发器路径**：需先补统一事件总线，再 E4。
- E2 与 E0/E1 可并行（不同人/不同层）。

## 跨任务约束（避免返工）

1. **nodeid 全程一致**：编辑器生成、`.proj` 存、codegen 写 `nodeid`、运行时 `SetNodeId`、高亮按 nodeId 查——同一个 id 贯穿。
2. **codegen 产物必须能被现有 `BehaviorTreeLoader` 直接吃**：AI 生成器的输出格式以 `BehaviorTreeLoader.BuildNode` 支持的字段为准（`node/children/action/condition/params/wait/...`），新增字段要先在 loader 加支持。
3. **产物可丢弃重建**：`.proj` 是唯一真源，`code/*.lua` 随时可由 codegen 重生成。
4. **单一命令栈**：一个编辑器实例只有一套 `CmdManager`。
5. **tolua 改动走生成链**：改 `BehaviorNode/Driver/Blackboard` 的导出后跑 `tolua.bat`，勿手改生成 cpp。
6. **调试回传不能省**：E1 是“可视化配置能交付非程序员”的关键，优先级高于画布美化。

## 验证入口

- 行为树/AI 改动：`Sandbox6/7/8`（移动、射击、状态切换、寻路）+ `ObjectManager:buildObjectDebugSummary`。
- FGUI 编辑器改动：对应 `HELLO_FGUI_*_SELF_TEST`，复杂改动 `HELLO_FGUI_SELF_TEST_ALL=1` + `tools/run_fgui_production_gate.ps1`。
- Lua 绑定改动：同步检查 `.pkg`、生成 cpp、Lua 调用点。
