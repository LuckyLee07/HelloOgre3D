# 可视化编辑器实施方案：AI 行为树 + 触发器（自写 FairyGUI 积木编辑器）

> 定位：这是一份**可执行、可落地**的工程方案，目标是在 `HelloOgre3D` 上从零建出两个可视化编辑器——**AI 行为树编辑器**与**触发器/玩法逻辑编辑器**——二者共用同一套“积木编辑器框架”。
>
> 上位与排期口径：编辑器在北极星（`docs/project-direction.md`）里属 **L3，整体属“缓做”**——项目级第一优先是 L0 地基 + 第一个垂直切片（手写数据跑通生物/触发器/AI），**编辑器是给那套数据加的 GUI 前端，不是项目的第一步**。本文内部的里程碑顺序（M0/M1 先于画布）是**进入编辑器阶段后**的轨道内顺序，不代表要先于地基做编辑器。唯一例外：M0/M1（行为树节点 uuid + 运行态调试回传）也直接服务垂直切片的 AI 调试，可在地基阶段顺带拉早。
>
> 来源与依据：方案蓝本来自对成熟沙盒 `G:\MiniGame_Dev` 的源码核实（详见 `docs/reference/reference-minigame-patterns.md`）。**关键事实已纠正**：MiniGame 的可视化**不是 Google Blockly 库，而是自写的 FairyGUI 积木编辑器**（目录名 `blockly` 只是借用 Scratch 范式的命名）。积木本体是 FGUI 预制件 `UIPackage:createObject(...)`，连线是 FGUI 预制件用 `setRotation+setWidth` 拼，画布是一个 GComponent 根做缩放平移。这对本项目是利好：本项目 FGUI 已接近生产级，**整套做法可直接复制，无需引入任何外部图形化编程库**。
>
> 与其他文档的关系：
> - “为什么这么做 / MiniGame 怎么做的”看 `docs/reference/reference-minigame-patterns.md`（含 file:line 证据，指向 MiniGame 仓库）。
> - 数据驱动/解耦等前置基建看 `docs/design/architecture-improvement-plan.md` 和本文 §4。
> - 本文只写 HelloOgre3D 该怎么建，证据性引用 MiniGame 时标注为外部参考。
>
> 约束（继承 `AGENTS.md`）：小步可验证；C++ 侧热点收口 runtime；FGUI 复杂 UI 走 MVC/AutoGen；Lua 不长期持有底层指针，走 handle/binding id；新增文本文件 CRLF。

---

## 1. 目标与范围

### 1.1 最终形态

- **AI 行为树编辑器**：策划/作者在 FGUI 画布上拖拽“选择/顺序/并行/条件/动作/取值”等积木，拼出一棵行为树，配参数、绑黑板键，保存后生成运行时可加载的行为树数据；运行时能把“当前正在跑/成功/失败”的节点回传到画布高亮，支持暂停/单步。
- **触发器编辑器**：作者拼“事件 → 条件 → 动作（ECA）”积木，配参数，保存后生成可执行逻辑；挂到生物/区域/全局；运行时由统一事件总线驱动。
- **两者共用一套通用积木编辑器框架**（画布、积木、连线、参数、撤销重做、序列化、codegen、调试回传），只在“积木库定义 + 代码生成目标 + 运行时加载”上分化。

### 1.2 范围边界

- 本方案**只做编辑器与其运行时对接**，不重写 AI 内核或事件系统本身；那些是前置依赖（§4）。
- 第一阶段目标是 PC（FGUI 鼠标 + 滚轮），移动端手势后置。
- 不做协同编辑、云存、版本合并；工程为单机本地文件。

---

## 2. 总体判断

走“自写 FGUI 积木编辑器”而不是引第三方，是经过验证的正确路径：

1. **MiniGame 就是自写 FGUI 实现**（已核实），证明该路线在生产级沙盒可行且够用。
2. **本项目 FGUI 已就绪**：MVC/AutoGen、Tree、虚拟列表、拖拽事件、scissor/stencil 裁剪、DebugPanel、各类输入桥都已具备，积木编辑器需要的 UI 能力基本齐了。
3. **可视化只是“前端 GUI”，真正的脊梁是数据 + codegen + 运行时**：积木图 → 中间数据（工程源）→ 生成 Lua → 运行时执行 → uuid 回传高亮。GUI 可以最后做、可以简陋，但数据与运行时格式要先定死。

> 落地心法（来自 MiniGame 验证）：**先做“数据 + codegen + 运行时加载 + 调试回传”的闭环，再做拖拽 GUI。** 一开始可以用手写 Lua 工程表喂进 codegen 验证整条链路，画布是最后一公里。

---

## 3. 架构总览

```text
                       ┌─────────────────────────────────────────────┐
                       │            通用积木编辑器框架(FGUI)            │
                       │  画布 Canvas / 积木 Block / 连线 Link /        │
                       │  参数 Inspector / 积木库 Palette /             │
                       │  撤销重做 CmdManager / 工程 ProjectManager     │
                       └───────────────┬───────────────┬─────────────┘
                                       │               │
              ┌────────────────────────┘               └─────────────────────────┐
              ▼                                                                    ▼
   ┌──────────────────────┐                                          ┌──────────────────────┐
   │   实例 A：AI 行为树    │                                          │   实例 B：触发器       │
   │  积木库 = BT 节点      │                                          │  积木库 = 事件/条件/动作│
   │  codegen → bt_*.lua    │                                          │  codegen → trigger_*.lua│
   │  运行时 = BT Driver    │                                          │  运行时 = 事件总线+devVM │
   └──────────┬───────────┘                                          └───────────┬──────────┘
              │ uuid                                                              │ uuid
              ▼                                                                   ▼
   ┌──────────────────────┐                                          ┌──────────────────────┐
   │ 运行态调试回传:        │                                          │ 运行态调试回传:        │
   │ 节点 run result→高亮   │                                          │ 触发命中→高亮          │
   └──────────────────────┘                                          └──────────────────────┘

  共同数据底座：CSV 积木定义表 + 工程源(.json/.db) + 生成产物(.lua) 分离
```

每个编辑器 = 通用框架 + 三个可替换件：**(a) 积木库定义（CSV）**、**(b) 代码生成器（codegen）**、**(c) 运行时加载/执行 + 调试回传协议**。

---

## 4. 前置依赖（必须先有，否则编辑器无处落地）

编辑器是“生成数据 + 喂给运行时”，所以运行时必须先能吃数据。下列为硬前置，建议与编辑器并行或先行：

| 前置 | 说明 | 对应文档 |
|---|---|---|
| **数据驱动行为树** | BT 能从 Lua 表 `{node, nodeid, param, children}` 加载实例化，而非只能 C++/命令式搭。现状是命令式（`BehaviorTreeDriver::NewSequence/NewSelector/NewLuaAction`），需补一个“表→树”加载器。 | `reference-minigame-patterns.md` §4 |
| **行为树节点带稳定 uuid** | 每个运行时节点可 `SetNodeId(uuid)`，并能查 run result。本项目已有 `BehaviorTrace`，可扩展。 | 本文 §5.9 |
| **统一事件总线** | 触发器编辑器的运行时基础。现状只有 Local 事件，需扩成 Local/Team/Global + 参数化事件名 + 四件套。 | `reference-minigame-patterns.md` §3 |
| **CSV 配置加载** | 积木定义放 CSV（数据驱动）。本项目当前无 CSV 体系，需建一个最小 CSV 加载器（双行表头：中文注释 + 英文字段名）。 | `reference-minigame-patterns.md` §2 |
| **dev 子 VM（触发器用）** | 作者脚本与内核隔离。第一版可先复用主 VM 的 `loadstring + setfenv` 沙盒，子 VM 后置。 | 本文 §7.4 |

> 若想最快看到东西：先做“数据驱动 BT 加载器 + 节点 uuid + 调试回传”，用**手写** `bt_demo.lua` 工程表验证运行 + 高亮，再回头做画布 GUI。

---

## 5. 通用积木编辑器框架（详细设计）

### 5.1 FGUI 资源与预制件

新建一个 FGUI 包 `blockeditor`（或 AI/触发器各一个包，复用同一组件类型），需要这些组件：

| 预制件 | 作用 | 关键子节点 |
|---|---|---|
| `Block` | 一块积木的本体 | 标题文本、参数槽容器（横向 list）、上接点/下接点锚点、左侧图标、选中描边（用 controller 切多状态：normal/running/success/fail） |
| `LinkLine` | 一段连线 | 一个细长 GImage/GGraph；通过 `setRotation(angle)+setWidth(length)` 对准两端 |
| `ParamSlot` | 参数槽（嵌在 Block 里） | 按类型切换：输入框 `GTextInput` / 下拉 `GComboBox` / 颜色 / 取值积木嵌入位 / 向量三输入 |
| `Canvas`(GComponent) | 画布根 | 三层子容器：`lineRoot`（连线层）、`blockRoot`（积木层）、`overlay`（拖拽预览） |
| `Palette` | 左侧积木库 | 分类 tab + 积木列表（虚拟列表，itemRenderer 用 Block 缩略） |
| `Inspector` | 选中积木的属性面板 | 参数行 list + 枚举 list + 描述编辑 |
| `Toolbar` | 顶部工具栏 | 保存/撤销/重做/缩放/运行调试/工程切换 |

实现要点（照搬 MiniGame 验证做法）：
- **积木 = 预制件，不自绘**：`FairyGuiManager` 创建 `Block` 实例，靠 FGUI 自身的层级、命中、批渲染。
- **连线 = 预制件旋转拉伸**：两端坐标算 `angle = atan2(dy,dx)`、`length = hypot(dx,dy)`，`line:setPosition(start)`、`setPivot(0, 0.5)`、`setRotation(deg)`、`setWidth(length)`。不用贝塞尔（第一版直线即可；后续可换 GGraph 画曲线）。
- **画布缩放/平移**：只对 `Canvas` 根 `setScale(s,s)` 和 `setXY`；缩放范围 0.1~1.5；滚轮缩放、拖空白平移。坐标换算：画布本地坐标 = (屏幕坐标 - canvasXY) / scale。
- **裁剪**：画布外层套一个开 scissor 的容器，超出不渲染（本项目已有 FGUI scissor）。

### 5.2 积木数据模型（运行期内存结构，Lua）

```lua
-- 一块积木实例
BlockInstance = {
    uuid        = "b_0001",     -- 稳定 id，贯穿 编辑/序列化/codegen/运行时高亮
    defId       = 300002,       -- 指向 CSV 积木定义
    blockType   = "Selector",   -- 形状大类（见 §5.3）
    pos         = {x=, y=},      -- 画布坐标
    params      = { [1]={kind="value", value=...},
                    [2]={kind="bbkey", value="enemy_id"},
                    [3]={kind="inner", block=<另一块取值积木>} },
    -- 连接关系
    nextBlock   = <BlockInstance|nil>,  -- 语句序列：下一句
    children    = { <BlockInstance>... },-- 子节点（组合/分支节点用；触发器用 {event=,condition=,action=}）
    decorators  = { <BlockInstance>... },-- 装饰器（AI 用）
}

-- 一张画布/工程
Workspace = {
    id        = "ws_mob_3001",
    topBlocks = { <BlockInstance>... },  -- 顶层根块
    meta      = { name=, author=, version=, target="ai"|"trigger" },
}
```

连接类型（决定能不能拼/嵌）：
- `statement`：能上下拼接（语句流）——用 `nextBlock`。
- `output`：取值积木，能插进参数槽 `params[i].kind="inner"`——用 `check`（返回值类型）做兼容校验。
- `branch`：组合/分支节点的子节点位——用 `children`。

### 5.3 CSV 积木定义 schema（数据驱动的核心）

积木长什么样、有几个参数槽、连哪种线、对应运行时什么——全配在 CSV，代码只做“CSV 行 → blockDef”的纯映射。建三张表：

**`block_def.csv`（积木主表，双行表头）**
```
id,    名称,   形状,      界面分类,  返回类型, 显示模板,                运行时映射,        生成类型
ID,    Name,  BlockType, Category, ReturnType, Desc,                  RuntimeName,      GenType
300002,选择,  branch,    组合,     -,         "选择 执行第一个成功的", BTNodeSelector,   0
410001,取最近敌人,output,取值,     object,    "最近的敌人",            -,                2
210001,移动到, statement,动作,     -,         "移动到 %1 速度 %2",     BTNTaskMoveTo,    1
```
- `BlockType`：决定用哪个形状模板（statement/branch/output/decorator/event/condition）。
- `Desc`：显示模板，用 `%1 %2` 占位标参数槽位置和数量（解析时按 `%` 切分得到槽数）。
- `RuntimeName`：AI 积木 → C++ BT 节点名（`BTNodeSelector`/`BTNTaskMoveTo`…）；触发器积木 → Lua 方法名映射键。
- `GenType`：0=控制流、1=C++ 任务、2=Lua 任务/取值——决定 codegen 走哪条。

**`block_param.csv`（参数槽能力表）**
```
id,    所属积木, 槽序, 输入方式,  值类型, 枚举id, 默认值,   类型校验
ID,    BlockId, Index, InputType, ValType, EnumId, Default, CheckType
```
- `InputType`：0=直接填入 / 1=只读传参 / 2=枚举下拉 / 3=嵌入取值积木。
- `CheckType`：参数槽接受的值类型（与取值积木的 ReturnType 比对，做连接兼容校验）。

**`block_enum.csv`（下拉枚举值）**：`EnumId, Index, Label, Value`。

> 加一种积木 = 加一行 `block_def` + 若干行 `block_param`（+ 在运行时映射表加一条），**不改编辑器代码**。这是整套系统可长期演进的关键。

`BlockCfgMgr`（Lua）职责：加载三表，`CsvToBlockDef(row)` 产出内存 blockDef（含形状模板、参数模板、显示模板），缓存；并建“按返回值类型反查可用取值积木”的索引，供参数槽下拉填充。

### 5.4 画布与交互

| 交互 | 实现 |
|---|---|
| 从 Palette 拖出新积木 | 拖拽预览跟手指；落到画布算本地坐标，`CmdAddBlock` 创建 BlockInstance + Block UI |
| 拼接（吸附） | 拖动时检测附近接点，命中显示吸附提示；松手按命中类型决定 `nextBlock`/`children`/嵌入参数槽（语义：Be_Next/Be_Child/Be_Param/Be_Delete） |
| 连接类型校验 | 取值积木插参数槽时，比对 `ReturnType` vs 槽 `CheckType`；不兼容拒绝吸附 |
| 移动 | 拖动积木 → 同步它和所有下游/子块的 UI 位置；连线重算 angle/width |
| 缩放/平移 | 只动 Canvas 根；滚轮缩放、空白拖拽平移、双指捏合（移动端后置） |
| 选中 | 点击积木 → 高亮描边 + 打开 Inspector |
| 删除 | `CmdRemoveBlock`，先 `ToTable()` 快照以便撤销 |

### 5.5 撤销重做（命令模式 + 双栈）

```lua
CmdBase   = { Execute(self), Revoke(self) }          -- 每种操作一个子类
CmdManager= { stack={}, curIdx=0, max=50,
              AddAndExecute(cmd), Undo(), Redo() }   -- Undo 调 Revoke, Redo 调 Execute
CmdList   = CmdBase  -- 组合多个 cmd 成一个事务，整体撤销
```
- 命令类：`AddBlock / RemoveBlock / Link / Unlink / MoveBlock / SetParam / AddComment...`
- 复杂操作（如拖一棵子树）用 `CmdList` 打包成一次撤销。
- 新命令入栈时清掉 `curIdx` 之后的已回滚命令；广播剩余可撤销/重做数刷新工具栏按钮。
- 删除类命令保存 `ToTable()` 快照，撤销时整块恢复。

### 5.6 工程序列化（源与产物分离）

每个工程一个目录，**两类文件分离**：

| 文件 | 内容 | 用途 |
|---|---|---|
| `<ws>.proj` | 积木工程源（JSON：blocks + 连接 + 坐标 + 参数 + meta） | 可逆再编辑，是唯一真源 |
| `code/<ws>.lua` | codegen 产物 | 运行时直接 require，**缓存产物** |
| `<ws>.dep` | 资源依赖清单 | 加载/打包用 |

- 序列化走每个对象的白名单 `SerializeMembers`（只存 uuid/defId/pos/params/连接），避免把 UI 状态写进去。
- 产物头打 `-- gen by editor vX, ws=<id>, ts=<…>` 水印；运行时若版本不符，从 `.proj` 重新 codegen（产物始终可丢弃重建）。
- 第一版用明文 JSON；加密/压缩后置。

### 5.7 代码生成框架（codegen）

通用 codegen = 遍历 Workspace 顶层块，递归把每块翻成目标语言片段。框架提供：
- `BlockToCode(block)`：按 `block.defId/blockType` 查“生成函数注册表 `genMethodDB`”分派。
- `ParamToCode(param)`：把参数翻成三态——常量 `val`；取值函数 `func=function(c) return c.bb:get("key") end`；左值 `set`。
- 注册式：`genMethodDB[blockType] = function(block, ctx) ... end`，新增节点类型只注册一个生成函数。

两个实例各自实现自己的 `genMethodDB`（见 §6、§7）。

### 5.8 运行时加载与实例化

- AI：`code/<ws>.lua` 返回一张 `{node, nodeid, param, children}` 树表；运行时加载器读表，对每个节点调 BT 工厂建 C++ 节点并 `SetNodeId(nodeid)`，叶子 LuaTask 挂 `param.script` + `subparam`。
- 触发器：`code/<ws>.lua` 是可执行 Lua（注册事件监听 + 条件/动作函数）；加载即 `require`/`loadstring` 执行，把监听注册到事件总线。

### 5.9 运行时调试回传（可视化调试，最有价值的一环）

闭环四步（AI 与触发器同构）：

1. **节点带 uuid**：加载时 `SetNodeId(uuid)`（uuid 由编辑器生成、写进工程源、随 codegen 落到 `nodeid`/动作调试标记）。
2. **运行态产出结果**：C++ 节点每次 Run 结束记 `run result`（SUCCESS/FAIL/RUNNING/WAIT）。本项目已有 `BehaviorTrace`，扩成“按 uuid 存最近一帧结果 + 是否 active”。
3. **回传给编辑器**：两种方式择一/结合——
   - 推：节点激活时 C++ 调一个 Lua 回调 `Editor_OnNodeRun(treeId, nodeId, result, paramSnapshot)`。
   - 拉：编辑器开调试时按帧轮询 C++ 树，读每个 uuid 的 `GetRunResult()/IsActive()`。
   （MiniGame 两者都用：参数值用推，运行/成功/失败高亮用轮询。）
4. **画布高亮**：编辑器按 uuid 找到 Block UI，用 controller 切状态（running/success/fail/wait 四态描边），并可显示该节点本帧的参数实时值。

调试控制：
- **暂停/单步**：C++ 加一个 `DebugMode{ paused, runOneFrame }`，`runOneFrame` 时放行一帧后复位——“单步 = 跑一帧”。
- 选中目标：在场景里点一个生物 → 设当前调试 treeId/objId → 首帧推一次当前 active 节点。

C++/Lua 协议（建议签名）：
```text
C++ → Lua（推）:  Editor_OnNodeRun(treeId:int, nodeId:string, result:int, snapshotJson:string)
Lua → C++（拉）:  bt:GetNodeRunResult(nodeId) -> int ; bt:IsNodeActive(nodeId) -> bool
调试开关:         Debug:SetMode(on) / Debug:Pause(b) / Debug:Step()
```

---

## 6. 实例 A：AI 行为树编辑器

### 6.1 积木库（block_def.csv 内容）
- 控制流：Selector / Sequence / Parallel / SimpleParallel / Random / Loop / Condition / Inverter / ForceSuccess / ForceFail（对应本项目 `sandbox/ai/behavior` 已有的 `BehaviorComposite/BehaviorDecorator`）。
- C++ 任务：MoveTo / Attack / Wander / Follow / Wait…（`GenType=1`，`RuntimeName=BTNTask*`）。
- Lua 任务：`BTNodeTaskLua`（`GenType=2`，`param.script="BTTask_xxx"`）——扩展缝，新行为只写一个 Lua 文件。
- 取值积木：GetNearestEnemy / GetSelfHp / BBGet…（`output`，返回类型用于参数槽兼容）。
- 变量：黑板读/写积木。

### 6.2 codegen 目标格式（与数据驱动 BT 一致）
```lua
return {
  node="BTNodeSelector", nodeid="b_0001", children={
    { node="BTNTaskMoveTo", nodeid="b_0002",
      param={ bbkey_targetpos="stollPos", speed=0.8 } },
    { node="BTNodeTaskLua", nodeid="b_0003",
      param={ script="BTTask_commonJudge", subparam={...} } },
  }
}
```
- 黑板取值 codegen 成 `param.x = function(c) return c.bb:get("enemy_id") end`。
- 每节点带 `nodeid`（= 积木 uuid）供调试高亮。

### 6.3 运行时对接
- 写一个加载器 `BTLoadFromTable(tbl, driver)`：递归读表，调本项目 `BehaviorTreeDriver` 的 `NewSequence/NewSelector/NewLuaAction/...` 建树，并 `node:SetNodeId(nodeid)`。
- 黑板键经 `Blackboard` 读写；Lua 叶子节点契约：`controller.target / controller.bb / controller.wait/success/fail / controller.subparam`（照搬 MiniGame 的 `BTTask_*` 约定）。
- 调试回传扩 `BehaviorTrace`。

---

## 7. 实例 B：触发器/玩法逻辑编辑器

### 7.1 积木库（ECA）
- 事件积木（`BlockType=event`）：玩家进区域 / 生物死亡 / 计时到 / 属性变化 / 自定义广播…，`RuntimeName` = 事件名。
- 条件积木（`condition`，`output` 布尔）：距离判断 / 属性比较 / 队伍判断 / 与或非。
- 动作积木（`statement`）：移动 / 播动画 / 改属性 / 发广播 / 生成对象 / UI 操作…，`RuntimeName` = Lua 方法映射键。
- 取值/变量积木：同 AI。

### 7.2 块 ID → 运行时方法的间接映射表
建一张 `ApiProxy`（Lua 表）：`[310003]="Trigger.Unit:MoveTo"`、`[1120009]="UI.Button.Click"`。codegen 只拼 `actionName(args...)`，底层 API 改名不动积木数据。

### 7.3 codegen 目标格式（生成可执行 Lua 文本）
```lua
-- gen by editor, ws=trigger_001
local EventFunc = {}
function EventFunc.Main_1(ctx)
  if not ( ctx.unit:distanceTo(ctx.player) < 5 ) then return end   -- 条件
  Trigger.Unit:MoveTo(ctx.unit, ctx.player:pos())                  -- 动作
end
EventBus:register("Actor.AreaIn?objid="..selfId, EventFunc.Main_1) -- 事件
```
- 对象级触发器用**参数化事件名** `事件?objid=<自身id>`（照搬 MiniGame 验证做法，复用同一张监听表）。
- 调试模式额外注入 `Trigger.Debug:Mark(nodeId)`，命中时回传高亮。

### 7.4 运行时对接
- 生成的 Lua 经加载器 `loadstring + setfenv(沙盒环境)` 执行（第一版）；后续升级为独立 dev 子 VM，主 VM 通过一组白名单 C 函数桥通信。
- 事件监听注册到**统一事件总线**（前置 §4）；触发链 = 世界事件 → 总线 → 注册的 EventFunc。
- 与 AI 衔接：同一条世界事件可同时喂触发器与生物行为树（总线 `OnEvent` 同时分发两路）。

---

## 8. 分阶段里程碑

> 原则：**先闭环后 GUI**。每阶段可独立验收、可回退。

### M0：运行时数据格式与加载器（无 GUI）
- [ ] 定义 AI 行为树 Lua 表格式 + 写 `BTLoadFromTable`，手写 `bt_demo.lua` 能在 `Sandbox6/7/8` 跑通一只生物。
- [ ] 节点 `SetNodeId(uuid)` + `BehaviorTrace` 按 uuid 存 run result。
- 验收：手写工程表驱动一棵行为树正常运行；能 dump 每节点最近结果。

### M1：调试回传 + 最小高亮（GUI 雏形）
- [ ] C++ Debug 协议（推/拉 + 暂停/单步）。
- [ ] 一个**只读**的 FGUI 树视图（先用现有 FGUI Tree 控件展示节点 + 三态着色），验证回传闭环。
- 验收：运行时能在 UI 上看到当前 active/成功/失败节点；能暂停/单步。

### M2：通用积木编辑器框架（可拖拽）
- [ ] FGUI 预制件（Block/LinkLine/Canvas/Palette/Inspector/Toolbar）。
- [ ] CSV 三表 + `BlockCfgMgr`；画布拖拽/连接/吸附/缩放/平移；Inspector 改参数；CmdManager 撤销重做；ProjectManager 存 `.proj`。
- 验收：能纯靠拖拽拼一张图、配参数、撤销重做、保存重开还原。

### M3：AI 编辑器打通（编辑 → 生成 → 运行 → 调试）
- [ ] AI 积木库 CSV + codegen → `code/<ws>.lua` + 运行时加载 + 调试高亮接到 M2 画布。
- 验收：在编辑器里拼一棵行为树 → 保存生成 → 生物按它行动 → 画布实时高亮当前节点。

### M4：触发器编辑器打通
- [ ] 前置统一事件总线就绪；触发器积木库 + ApiProxy + codegen → 可执行 Lua + 沙盒加载 + 事件总线注册。
- [ ] 对象级触发器参数化事件名；触发命中回传高亮。
- 验收：拼“玩家进区域 → 生物攻击”，运行时正确触发，命中积木高亮。

### M5：打磨与工程化
- [ ] 工程库管理（多工程、复制、删除、uuid 权限）；资源依赖清单；版本水印与重生成；积木搜索/注释/分组；移动端手势。
- 验收：作者可独立完成“建工程 → 拼逻辑 → 调试 → 复用”的完整工作流。

---

## 9. 要绕开的坑（MiniGame 自陈 + 本项目约束）

1. **不要先做 GUI 再补运行时**：会反复返工。先 M0/M1 闭环。
2. **codegen 产物必须可丢弃重建**：`.proj` 是唯一真源，`.lua` 是缓存；版本不符就重生成。别让人去手改生成产物。
3. **积木定义彻底进 CSV**：别把积木形状/参数硬编码进 Lua。加积木=改表。
4. **块 ID → 运行时方法用间接映射表**：底层 API 改名不动积木数据。
5. **单一命令栈**：AI 与触发器各自一个 CmdManager 即可，但**一个编辑器内只有一套撤销栈**（MiniGame 吃过三套栈不共享的亏）。
6. **uuid 全程稳定**：编辑/序列化/codegen/运行时高亮共用同一 uuid，是调试回传成立的前提。
7. **调试回传不能省**：没有运行态可视化，策划无法自助调 AI/触发器——这是“可视化配置”能交付给非程序员的关键，不是可选项。
8. **触发器作者脚本要沙盒**：第一版 `loadstring+setfenv`，量大后上独立子 VM，避免作者逻辑崩溃/作弊污染内核。

---

## 10. 目录与文件布局建议

```text
bin/res/scripts/editor/                 # 编辑器 Lua（仅开发/编辑态加载）
  blockeditor/                          # 通用积木编辑器框架
    BlockCfgMgr.lua  Canvas.lua  Block.lua  LinkLine.lua
    Inspector.lua  Palette.lua  CmdManager.lua  ProjectManager.lua  Codegen.lua
  ai/                                   # AI 编辑器实例
    AIBlockLibrary.lua  AICodegen.lua
  trigger/                              # 触发器编辑器实例
    TriggerBlockLibrary.lua  TriggerCodegen.lua  ApiProxy.lua
bin/res/scripts/runtime/
  ai/BTLoadFromTable.lua                # 行为树表→运行时
  trigger/TriggerRuntime.lua            # 触发器加载/沙盒执行
bin/res/csv/                            # 积木定义表（双行表头）
  block_def_ai.csv  block_param_ai.csv  block_enum_ai.csv
  block_def_trigger.csv  block_param_trigger.csv  block_enum_trigger.csv
bin/res/assets/blockeditor.fairypackage # FGUI 预制件包（Block/LinkLine/Canvas/...）
data/projects/<ws>/                     # 工程：<ws>.proj(源) + code/<ws>.lua(产物) + <ws>.dep
src/HelloOgre3D/sandbox/ai/behavior/    # 扩 BehaviorTrace + SetNodeId + Debug 协议
```

---

## 11. 跟踪清单

- [ ] M0 行为树 Lua 表格式 + `BTLoadFromTable` + 节点 uuid + trace 存 result
- [ ] M1 C++ 调试协议（推/拉/暂停/单步）+ 只读树视图高亮
- [ ] M2 通用积木框架（FGUI 预制件 + CSV 三表 + 画布 + Inspector + 撤销重做 + 工程序列化）
- [ ] M3 AI 编辑器闭环（编辑→codegen→运行→高亮）
- [ ] M4 触发器编辑器闭环（事件总线前置 + ECA 积木 + ApiProxy + 沙盒执行 + 高亮）
- [ ] M5 工程库管理 / 依赖清单 / 版本重生成 / 搜索注释 / 移动端手势

前置依赖跟踪（见 §4）：
- [ ] 数据驱动行为树加载器
- [ ] 统一事件总线（Local/Team/Global + 参数化事件名 + 四件套）
- [ ] 最小 CSV 加载器（双行表头）
- [ ] dev 子 VM（触发器，可后置，先用 loadstring+setfenv）
