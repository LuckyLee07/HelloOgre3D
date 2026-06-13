# 参考：MiniGame系沙盒可借鉴架构与实现

> 来源：对成熟生产级沙盒项目 `G:\MiniGame_Dev`（MiniGame系，含 `SandboxEngine` / `SandboxGame` / `MiniStudio` / UGC 编辑器 / Mod 系统）的一次定向深读，结合其自带知识库 `Source/docs/sandbox/module/*.md` 与实际 C++/Lua 源码交叉核对。
>
> 目的：为本项目 `HelloOgre3D` 的目标——**生产级沙盒大世界 + 多生物复杂系统 + AI 系统 + UGC 玩法 + AI 可视化配置 + 生物触发器**——沉淀可直接借鉴的架构决策、实现手法、以及对方已踩过、我们要主动绕开的坑。
>
> 使用方式：做架构决策时先读本文对应章节，再回看本项目现状（`docs/design/architecture-improvement-plan.md` / `docs/design/cpp-object-model-refactor-roadmap.md`）。本文结论涉及对方实现细节处均带证据（文档名或 `文件:行`），但这些证据指向的是 **MiniGame_Dev 仓库**，不是本仓库。
>
> 一句话定调：本项目的四个目标，底层压在两件事上——**(1) 数据驱动的对象/行为定义（CSV 原型 + Lua 表 + 轻量序列化），(2) 一套可序列化、带属性元数据的编辑器节点模型**。MiniGame 把这两件事分成**两套并行体系**来做（见 §1），是值得借鉴的活样本，也暴露了若干债。
>
> 【2026-05-29 实测核实记录】本文初稿由文档 + 代理调研生成，后经**直接通读 MiniGame_Dev 源码逐条核实**。结论：除“反射”一节被显著夸大外（已在 §1 修正），其余主张——参数化事件名（`%s?actorid=%d`）、事件总线四件套（过滤器/跨帧/节流 100·1000/上限 10 万）并喂行为树（`btmanager->OnEvent`）、AI LOD 三件套（`m_tickstep=7` 取模 + 6400·3200 距离降频）、感知 `range>300` 上限（`AIFunctionMgr.cpp:10561`）、`GameRuleOption{ruleid,optionid,cval}` 三元组（`GameModeDef.h:198`）、`addByCopy`、`AbsCsv::onParse`/`onReloadParse` 空默认 + `BlockDefCsv` 实现、CSV 双表头、`TriggerFunction.csv` 可视化积木、dev VM `lua_islightuserdata` 桥、`DevUINode`→FairyGUI 编辑器模型、`LuaFuncCall` 收口——**均与代码一致**。个别数字已订正（`UGCRuntime.h` 实为 37 行，非 77）。

---

## 0. 能力地图对照

| 本项目目标 | MiniGame_Dev 对应实现 | 借鉴优先级 |
|---|---|---|
| 运行时玩法对象（大世界多生物的根） | 经典 Actor 体系：CSV `Def` 原型 + FlatBuffer 序列化 + 组件 + Lua（**不走反射**） | ⭐⭐⭐ 最高 |
| 编辑器场景对象 / 序列化 / 属性面板一致性 | SandboxNode 反射节点树 + flag 位掩码驱动多通道（**仅方块/编辑器场景节点用**） | ⭐⭐（编辑器层才需要） |
| 多生物（零 C++ 子类） | CSV `Def` 只读原型 + 组件组合实例化 | ⭐⭐⭐ |
| 生物触发器 | ECA + 扁平事件总线 + **参数化事件名** | ⭐⭐⭐ |
| AI 可视化配置 | 声明式 Lua 行为树 + 节点工厂注册表 + AI 编辑器 + 运行态回传调试 | ⭐⭐⭐ |
| 大世界 AI 性能 | AI LOD 三件套 + C++ 感知原语库 + 空间查询上限 | ⭐⭐ |
| UGC 玩法规则 | `{ruleid, optionid, value}` 三元组 + 统一 setter facade | ⭐⭐ |
| UGC 内容扩展（自定义生物/方块/玩法） | Mod：拷贝原型 + ID 段隔离 + 进退图还原 | ⭐⭐ |
| 可视化编辑器 | 编辑态模型 = 运行态模型同源 + op-record 单命令栈 | ⭐⭐ |
| 可视化逻辑积木 | CSV 数据驱动节点定义（`TriggerFunction.csv`） | ⭐⭐ |
| AI 辅助生成 UI/布局 | 模型只产中间 JSON + 视觉资产，落地走编辑器既有管线 | ⭐ |

> 证据文档：`docs/sandbox/module/` 下 04/08/09/10/12/14/22/24/25/26/27/28 号；`docs/sandbox/design/02`。

---

## 1. 核心主线：两套并行的对象体系（关键纠偏）

> ⚠️ 本节是全文最重要的纠偏。初稿曾声称“反射是整个对象模型的真正骨架，所有可视化对象都是同一棵反射节点树”——**经源码核实，这是夸大的**。真实情况是 MiniGame 有**两套并行、定位不同**的对象体系：
>
> | 轨道 | 用途 | 数据/序列化机制 | 用反射吗 |
> |---|---|---|---|
> | A. 经典 Actor 体系 | 冒险模式生物/掉落/投射物、UGC 运行时玩法对象 | CSV `Def`(MonsterDef) + FlatBuffer(FBSave/SectionActor) + 组件 + Lua | ❌ 基本不用 |
> | B. SandboxNode 反射场景树 | 方块 + 编辑器/Studio 放置的场景节点（区域/相机/特效/蓝图/灯光/脚本节点） | 反射 `ReflexClassParam` + `SandboxReflexSerialize`/`SandboxLoadSave` | ✅ 重度使用 |
>
> 核实证据：`ClientActor.cpp:256-257` 的反射属性声明**被注释掉**；`ClientMob.cpp:153` 唯一反射属性 `R_MonsterId` 的 flag 是 `REG_LOCAL`（运行时临时，不存档不同步）；actors 目录下 `FBSave/onParse/MonsterDef` 共 232 处。反之 `SandboxLightObject.cpp:87` 等场景节点真实注册反射属性，序列化经 `driver/scene/SandboxLoadSave.cpp`、`SandboxChunkIOMgr.cpp`、`WorldScene.cpp`。
>
> **结论：玩法逻辑（触发器/规则）和冒险生物不走反射；反射真正承载的是方块系统 + 编辑器场景节点树（偏 Studio/可视化编辑那条线）。** 因此“反射”对本项目的价值在**编辑器/可视化配置层**，不是所有运行时对象的统一脊梁。下面 1.1–1.5 仍记录两套体系的实现细节，但请按上表区分各自适用范围。

### 1.1 两层统一根

代码里有两棵对象树，但**同根**：

| 体系 | 根链 | 用途 |
|---|---|---|
| Actor 树（老） | `GameObject → SandboxNode → ActorBase → ClientActor → {ActorLiving/Item/Projectile}` | 方块世界里的生物/掉落/投射物 |
| SandboxNode 场景树（新，Roblox-Studio 式） | `GameObject → SandboxNode → SceneModelObject → SceneActorObject / SandboxLightObject / ...` | 可视化编辑、可序列化、可同步的场景节点（灯光/相机/模型/区域/脚本/特效） |

证据：
- `ActorBase : public MNSandbox::SandboxNode`（`SandboxEngine/Core/actors/helper/Actor_Base.h:27`）
- `SceneActorObject : public SceneModelObject`（`SandboxEngine/Core/nodes/SandboxActorObject.h:50`）
- `SandboxNode : public GameObject`（`SandboxEngine/Core/driver/scene/SandboxSceneObject.h:59`）

> 注意（已核实纠偏）：两棵树确实都继承 `SandboxNode`（因此生物 Actor 也有 RTTI/节点身份/parent-children/Clone/notify），但**“都是同一棵树上的节点”不等于“都用反射承载数据”**。经核实，Actor 的玩法数据走 CSV+FlatBuffer（轨道 A），只有方块/场景节点真正用反射属性序列化（轨道 B）。即：节点身份是共享的，反射属性序列化不是。

### 1.2 GameObject：组件容器（明确不是 ECS）

- `GameObject`（`SandboxEngine/Core/driver/base/SandboxGameObject.h:22`）：持有 `List<Component*> m_components`，提供 `CreateComponent<T>(name)` / `GetComponent<T>()` / `DestroyComponent` / `AddComponent`，在 `OnPreTick/OnTick` 里驱动 `ComponentPreTick()/ComponentTick()`。
- 组件 update：组件向宿主注册 tick（`RegComponentTick`），由宿主 `OnTick` 统一 fan-out。
- 所有权：宿主持有组件 `List`，生命周期跟随宿主。
- 高频访问优化：`DECLARE_CACHE_COMPONENT(type)` 宏把常用组件指针缓存成成员，避免每帧 `GetComponent<T>` 查找（`docs/.../04-Module:85`）。
- 文档原话：“UGC Core 是一套 GameObject + Component 风格的扩展层，不是 ECS”（`25-Module:200`）。

### 1.3 SandboxNode：节点树语义（Roblox Instance 等价物）

`SandboxNode`（`SandboxSceneObject.h:59-258`）在 GameObject 之上加：

- 树结构：`m_parent + m_children`，`SetParent/GetParent/GetChildByName/GetChildByID/GetAllChildren/GetRoot/IsAncestorOf/IsDescendantOf`，模板 `GetChildT<T>/GetAllDescendantT<T>`。
- 生命周期统一：`InitInstance`（建默认子节点）/ `InitNodeLonely`（只建自己，用于 clone/同步）/ `BeginPlay` / `EndPlay` / `Destroy` / `DestroyAllChildren`。
- 32 个状态位 `FLAGTYPE`：`ENABLE/ACTIVE/SERIALIZABLE/SERIALIZABLE_IN_CHUNK/TICK/DYNAMICLOADABLE/PHYSICSFREEZE/SYNC_ONLYPARENT/CREATE_BY_SYNC...`，把“是否可序列化/是否 tick/是否同步/是否动态加载”做成节点级开关。
- 事件统一：`NODECHANGE` 枚举（ChildAdd/ChildRemove/AttributeChanged/CustomAttrAdd...）+ PhysX 触碰回调 `onPhysXTouch`。
- 服务节点：`GetService()` / `ServiceNode`——单例式服务挂在树上（`SandboxTeamService/TweenService/TeleportService/CollectionService/ContextActionService/CoreScript/ModuleScriptNode`，全是 Roblox 概念对照）。
- 节点工厂：`SandboxNode::CreateNodeByFactory(classtype)` / `CreateNodeByType(classtype)`——按反射类名字符串建任意节点，是编辑器/Lua/存档反序列化通用的实例化入口。

### 1.4 反射框架（重度用于轨道 B：方块 + 场景节点；**不是**玩法 Actor 的数据骨架）

位于 `SandboxEngine/Core/driver/base/reflex/`（36 个文件）。**宏 + 静态对象注册，不是注解扫描。**

- **(a) RTTI 注册**：`DECLARE_REFCLASS(ClassName)` + `IMPLEMENT_REFCLASS`（`SandboxRuntimeClass.h:192,256`）。每类生成静态 `RuntimeClass m_RTTI`，带 `Super` 链、字符串类名、工厂 `NewInstance/NewObjectLonely`，注册进全局 `RuntimeClass::Container`，支持按字符串名和按 int classId 双向查（`GetRTTIById`）。NodeClassId 分段：UGC 用 3000–5999、自定义 20000–29999（`SandboxNodeRegister.h:19-35`）。
- **(b) 属性注册**：每个属性是一个静态 `ReflexClassParam<OwnerClass, ValueType>` 对象：
  ```cpp
  static ReflexClassParam<SandboxLightObject, ColorQuad> R_SetLightColor(
      4, "LightColor", "light", &GetLightColor, &SetLightColor);   // SandboxLightObject.cpp:87
  ```
  属性挂进**继承式 `ReflexContainer`**（`SandboxReflexContainer.h:26`），子类容器构造时 `FillParentReflex()` 自动继承父类全部属性。
- **(c) 一个 flag 位掩码同时驱动全部通道（最精华）**：`ReflexConfig::FLAG`（`SandboxReflexConfig.h:22-68`）用一个 bitmask 表达每个属性在各通道的可见性：
  - `NO_PUBLICREAD/WRITE` → 编辑器属性面板是否显示/可编辑
  - `NO_SCRIPTREAD/WRITE` → Lua 能否读/写
  - `NO_SAVE/NO_LOAD` → 是否进存档
  - `NO_SYNC/SYNC_FORCE/SYNC_ONLYHOST/NO_BROADCAST` → 联机同步策略
  - `COMPRESS`、`NO_PLAYMODE`（仅编辑态）、版本区间 `ReflexVersion`
  并预组合成语义档位：`REG_ONLY_SHOW`（只展示）、`REG_ONLY_SCRIPT`、`REG_ONLY_SAVE`、`REG_ONLY_SYNC`、`REG_LOCAL`（运行时临时，不存档不同步）。容器据此区分各属性的通道归属，序列化经 `SandboxReflexSerialize.cpp` / `driver/scene/SandboxLoadSave.cpp` 落地。
  > 订正：初稿写的 `GetSaveList/GetSyncList` 等方法名在源码中**无任何调用点**，属代理推断，已删除。flag 档位本身真实（`REG_LOCAL` 已由 `ClientMob::R_MonsterId` 实证），但“一个声明自动驱动存档+同步管线”的说法只在轨道 B 的场景节点序列化里成立，且实现入口是 `SandboxReflexSerialize/SandboxLoadSave`，不是那几个 `GetXxxList`。
- **(d) 反射还覆盖**：方法（`SandboxReflexClassMethod.h`）、事件/通知（`DECLARE_SCENEOBJECTCLASS` 自动注册 `ATTRCHANGED`/`DATASYNC` 通知）、枚举（`ReflexEnumDesc<LightType>(...)`，枚举 ID 集中在 `SandboxReflexTypeEnumEx.h`，200+ 个）、引用关系（`ReflexReferenceLinker`）、Lua 类型策略（`ReflexPolicyLua`）。

> **意义（仅限轨道 B：方块/场景节点）：在这些对象上，声明一个属性 = 同时声明了它的存档、Lua 绑定、编辑器面板项、同步规则。** 这对本项目**编辑器/可视化配置层**很有价值（对应 `architecture-improvement-plan.md` P6 杜绝四套定义漂移的思路）。但**切勿据此把反射套到每个运行时玩法对象上**——MiniGame 自己的生物就刻意没这么做（走轨道 A 的 CSV+FlatBuffer）。

### 1.5 Clone = prefab；工厂强制 + facade 隔离

- **Clone 即 prefab**：`SandboxNode::Clone()/CloneTo()/CloneAsync()`——克隆整棵反射子树就是 prefab 实例化，属性随反射自动复制。无需独立 prefab 序列化格式。
- **工厂强制**：`ClientActor::operator new = delete` 从语言层禁止业务裸 new，统一走 `CreateSandboxActor<T>()`（`04-Module:90-103`）。
- **类型转换**：编译期 `SafeCast<T>()` 分派到 `CastToXxx()`，避免 `dynamic_cast`（`04-Module:281-291`）。
- **空间索引**：Actor 用八叉树 `ActorMGT`（MaxDepth=12），范围查询/AOI/碰撞都走它（`04-Module:271-277`）。

### 1.6 → 对本项目的借鉴（按两轨区分，已纠偏）

1. **运行时玩法对象（多生物）走轨道 A，不要上反射**：生物用 CSV `Def` 原型 + 组件组合 + 轻量序列化实例化。本项目当前 `SandboxObject → BaseObject → AgentObject → SoldierObject` 窄继承链 + 组件 map，方向对，缺的是“CSV 原型 + 数据化实例化”，而**不是**“给每个对象加反射”。
2. **仅在编辑器/可视化配置层考虑轨道 B 的反射 + flag 多通道**：当你做属性面板 / 场景对象序列化 / 编辑器节点时，再引入 `Prop(id, name, category, get, set, flags)` 这套（flags 覆盖“编辑器可见可编辑 / Lua 读写 / 是否存档”三通道）。接口形态可照抄，但**范围限定在编辑器节点，别外溢到玩法 Actor**。
3. **prefab 用 Clone 实现**（若采用节点树），不单独造序列化格式。
4. **工厂强制 + facade 隔离**正是本项目要给 `GameManager` 做的事（对应 P3），这里有成熟样板，且与是否用反射无关。

---

## 2. 数据驱动配置：CSV `Def` 原型 + Mod 扩展

### 2.1 CSV 配置体系（怎么做的）

- **自描述“双/三行表头”**（已核对实际数据）：
  - 第 1 行 = 中文注释行（给策划，含字段语义、枚举取值，如 `monster.csv:1` 把 `Type` 注释为 “0-怪物,1-动物,2-稀有动物...10-村民”）。
  - 第 2 行 = 英文字段名行（代码真正用的列名：`ID,Name,Model,Type,Life,Attack,AttackType,DropItem1...`）。
  - 第 3 行起 = 数据。
- `xxxCsv : AbsCsv` 子类的 `onParse(CSVParser&)` 按第 2 行列名把每行翻译成结构体（`12-Module:33-54`）。**schema 内嵌在数据文件里，没有独立 .proto/.fbs，靠列名约定 + C++ 手写 `onParse`。**
- 编码/渠道：统一入口 `csvdef/utf8/{name}.csv`，`ICsvLoadConfig` 按编译宏三选一（国内/海外/PC Debug）。多语言不是单独文件，而是同一行 15+ 个语言列，由 `MultiLanCSVParser` 挑当前语言列。
- 加载：每表单例懒加载（`load()` 模板方法）；`DefManager::loadInLoading(step)` 分 10 桶按依赖顺序预热（基础数值表→玩法周边表）。
- 双端访问：C++ `g_DefMgr.getXxxDef(id)` 或 `XxxCsv::get(id)`；Lua 三层暴露——tolua 自动绑 `DefMgr:getItemDef(id)`、`setUserTypePointer` 把单例注册成 Lua 全局、`PushAllDefTableToLua` 批量导表。
- **类型校验：几乎没有**（只有 CRC、RegionType 屏蔽列、Mod 越界兜底），字段类型靠 `onParse` 手工 `row[col].Str()/Int()`。← **反面教材。**
- **热重载：只有 `BlockDefCsv` 真正实现内存就地替换**（解析到临时 map 再逐字段拷回原 def 对象，保护已持有指针的玩法层），多数表 `onReloadParse` 为空。← **反面教材。**

### 2.2 配置 → 运行时实例化链路

以生物为例：
`monster.csv` 行 → `MonsterCsv::onParse` → `MonsterDef` 结构体（`DefDataTable<MonsterDef>` 红黑树 + `m_MonsterTable` 稀疏数组双容器）→ `MonsterCsv::get(id, takeplace, bUseOne)` 取 def（缺失返回默认怪）→ 玩法层据 `MonsterDef`（`Life/Attack/AttackType/Model/Armors[10]/DropItem*`）实例化 Actor。

> **要点：Def 是只读原型，运行时对象持指向 Def 的指针**——这正是热重载必须“就地替换字段”而非“换指针”的根因。

### 2.3 Mod 插件系统（UGC 内容底座）

- **粒度**：不是 DLL，而是“地图级 UGC 配置+资源扩展包” `ModPack`。类型 `CustomModType = Block/Monster/Item/Recipe/Furnace/Status/Biome/Actor/UI/Rule/...`（`02-Module:37`）。
- **扩展机制**：Mod **不改 CSV 文件**，而是运行时把 JSON 增量写进 `xxxCsv` 单例。`ModPackMgr::ParseModDef(iType, iCfgId, json...)` 按类型分发到 `ParseBlock/ParseCreature/ParseItem/...`。两条铁律：
  - **拷贝写入**：`addByCopy(iCfgId, iCopyId)` 先拷一个原始 def 再覆盖字段。
  - **ID 段隔离**：自定义内容走预留 ID 段（道具 `[10000000,19999999]`、Block 400000、Monster 1000000、Status 60000000...）。
- **加载/沙箱/还原**：进图前 `m_*DefCache` 备份原始 Def；退图 `UnloadWorldMods` 走 `ClearOriginDef/ClearResLibDef/ClearModPacksDef/ClearAllCustomDef` 整体还原。**“进图替换、退图恢复”是核心能力。**
- **ID 稳定性**：`allocatedids.json` 持久化分配进度，已分配 ID 不回收（删 Mod 不重用旧 cfgId），空间换存档稳定。
- **依赖**：不手工维护，而是从 prefab 资源引用反推。
- **已暴露的债**：ID 段重叠（Recipe/Furnace、Biome/World）导致只能启发式反推；`ModPackMgr` 责任过重（加载器+解析器+缓存+ID中心+恢复器一肩挑）；新增类型要同步改 8+ 处链路。← **反面教材。**

### 2.4 → 对本项目的借鉴

1. **保留“双行表头自描述 CSV + 模板方法加载基类”这个低成本、策划友好的结构**，但**补上声明式列类型/范围校验**（加一行类型表或第 4 行 schema 行，启动时校验），避免错列/错类型静默带病运行。
2. **Def 只读原型 + 运行时持指针 → 所有 Def 表从第一天就实现“就地替换字段”的 reload**，否则后期补热重载会被“玩法层已持有指针”卡死。这是“大世界多生物快速调参”的刚需。
3. **UGC 自定义内容走“拷贝原型 + ID 段隔离 + 进退图还原”**，但 ID 段从设计起预留充足且**绝不重叠**；把“类型→解析器/清理器/恢复器/ID段”做成**注册表驱动**，新增一种内容只填一行注册，不改 N 处链路。

---

## 3. 事件总线 + 触发器：ECA + 扁平总线 + 参数化事件名

这块对“生物触发器”最直接，有几招可以原样移植。

### 3.1 触发器模型

- **触发器 = 事件→条件→动作（ECA）**，落地形态是“统一事件总线 + 脚本回调 + 代码生成”，不是重型规则树引擎（`26-Module:9`）。
- 三段职责：编辑期把“区域/位置/生物/道具/显示板”沉淀成“触发器对象库”；运行期把世界事件汇聚到事件总线，由作者脚本回调产生反应。
- ⚠️ 关于“dev 子 VM”（已实测纠正，2026-05-29）：源码里确有独立子 VM 设计（`s_DeveloperScriptVM`），但**由编译宏 `ENABLE_DEV_VM` 控制，当前默认构建为 0**（`ClientMacrosConfig.h:32`），即**默认触发器脚本跑在游戏主 VM**（`ClientApp.cpp:1783-1787` 的 `#else` 分支把 `ScriptVM::game()` 当作 “dev” 传入）。所谓 dev/game 隔离在默认 Lua 构建下并不成立——它俩是同一 `lua_State` 上 `__dev_to_game_*` / `__game_to_dev_*` 两组全局函数命名空间。真正进程级隔离的是 **Python**（`PythonScriptWrap` 走外部 CPython dll）；JS（`JsScriptWrap`）是空壳未接线。**对本项目的含义：第一版触发器不必先搞独立子 VM，主 VM + `loadstring/setfenv` 沙盒即可，隔离是后续可选项。**

### 3.2 事件源（枚举 `TriggerEventFilter`，`ObserverEvent.h:173-229`）

- 输入：`Player_InputKeyDown/OnPress/Up/Click`、`Player_GunAction`
- 属性/状态变化：`Player_AttrStateChange`、`Mob_AttrStateChange`、`Actor_ChangeAttr`、`Player_ChangeAttr`
- 区域进出（6 个对象桶各一对 In/Out）：`Player_AreaIn/Out`、`Actor_*`、`Entity_*`、`AreaObj_*`、`DropItem_*`、`Missile_*`
- 计时：`Minitimer_Change`
- 触摸/交互：`Player_TouchBlock`、`Player_TouchObj`、`Player_OpenInnerView/Close`、`Player_PlayAnimFinish`
- UI 事件：`UI_Button_Click` 等整套

### 3.3 对象级触发器靠「参数化事件名」（最妙的一招）

- 全局触发器订阅裸名 `Actor.Death`；挂到某只生物的触发器订阅 `Actor.Death?actorid=456`，**复用同一张监听表**。
- 实现：`SSEventData::OnBlock/OnActor/OnItem` 把全局事件名重写为带实例 id 的事件名再二次分发——`sprintf(eventnameParam, "%s?actorid=%d", eventname, actorid)`（`ScriptSupportEvent.cpp:263-299`）。
- 区域同理：`SceneArea::triggerActorIn/Out` 先查 `GetTriggerEventFilter(Player_AreaIn, m_areaid)`，过滤键就是 **areaid**（`SceneArea.cpp:121-152`）。

> “生物带触发器” = 注册 `事件?objid=本生物id`。**零额外数据模型**，无需为每个对象建独立订阅结构。

### 3.4 事件总线四件套（`ObserverEventManager`，大世界高频不卡死的关键）

1. **过滤器即订阅**：`m_numfilterevent/m_strfilterevent` 按枚举存；**未注册过滤器的事件根本不进队列**（区域只为“作者真正放了触发器的 areaid”开过滤器）。
2. **延迟一帧分发**：`OnTriggerEvent` 先 push 到 `m_triggerEventListBuffer`，`tick()` 再灌入 `m_triggerEventList` 逐个 `OnTrigger`——防递归触发（`ObserverEventManager.cpp:55-122,209-226`）。
3. **每帧节流**：客户端 100 / 服务器 1000。
4. **队列上限保护**：队列 >10 万直接停止。
- 注册表：`m_mapListens: map<string(eventname), ObserverEventListens>`。
- 实例式自动反注册：`ObserverEventRegisterIns/ListenIns` 析构自动 unregister，避免漏注销。
- **接合点**：`OnTriggerEvent` 同时把事件转发给生物行为树 `BTManagerInterface::OnEvent(name, pobevent)`（`ObserverEventManager.cpp:230-234`）——**同一条世界事件既喂触发器脚本，又喂 mob AI 行为树。这就是“生物+触发器”在引擎层的真实接合点。**

### 3.5 触发器数据化 + 跨 VM 桥

- 可摆放对象数据化：五类 ToolData（`TriggerAreaToolData/PositionData/LivingToolData/ItemData/DisplayBoardToolData`）是 PoD struct，`tolua` 暴露给 Lua，持久化用 FlatBuffer（`Save2FlatBuffer/LoadFromFlatBuffer`）。
- ECA 逻辑**不是解释执行，而是代码生成**：`ScriptSupportTrigger:makeTriggerCode` 把因子/积木配置编译成 Lua 文本（`EventMain/ConditionMain/ActionMain`，条件假即短路 return，动作 `pcall` 隔离 + `threadpool:work` 协程化以支持 wait 类动作，`_runCount_>50` 递归保护）。运行链：`SceneAreaManager::tick → SceneArea::updateActor(包围盒扫描+tickCount 差分判进出) → triggerActorIn → 过滤 → ObserverEventManager::OnTriggerEvent → 缓冲→下一帧 tick 分发 → __trigger__(msgid, lightuserdata) → 作者脚本回调 → Trigger.X:Method → 调 C++`。
- 两道桥（关键）：① **`scriptsupport.*` C 函数桥** + `__dev_to_game_*`/`__game_to_dev_*` 全局函数——弱类型、**跨界全 JSON 字符串**、可跨语言/跨 VM，是作者脚本与引擎的唯一受控接口面；② **`Trigger.X:Method → GameVM.*` tolua 静态绑定**——强类型、同 VM、零序列化，ECA 的“动作”从桥①脚本里调桥②落回 C++。事件对象用 **lightuserdata 零拷贝穿透**（默认同一 `lua_State` 下安全，仅同步窗口内有效）。

### 3.6 → 对本项目的借鉴

1. **照搬“参数化事件名”实现对象级触发器**：本项目“生物带触发器”可直接做成“生物身上挂触发器 = 注册 `事件?objid=本生物id`”，复用同一张监听表。
2. **照搬事件总线四件套**（过滤器门禁 / 跨帧分发 / 节流 / 上限保护），把现有只有 Local 的事件系统扩成统一总线。这是大世界多生物高频事件下不卡死的关键。
3. **同一条世界事件同时喂触发器和 AI 行为树**——把事件总线接到 AI driver 的事件入口，生物对环境的“感知-反应”和“触发器反应”走同一条事件流。
4. **触发器逻辑用代码生成 + 两道桥**：可视化/因子配置 codegen 成 Lua 文本（条件短路、动作协程化、递归保护），作者脚本只透过 `scriptsupport.*` 受控桥碰引擎，动作经 `Trigger.X → GameVM.*`(tolua) 落回 C++。隔离 VM 是**可选项**（默认主 VM + 沙盒即可，`ENABLE_DEV_VM` 开关；真隔离走 Python 外部解释器），不是第一版必需。

---

## 4. AI：数据驱动行为树 + 可视化配置 + LOD

本项目已有 FSM/DT/BT + Blackboard + AIScheduler，**缺的不是内核，是数据层和可视化层**。下列各条精准补缺口。

### 4.1 架构（三套并存，新内容首选 BT）

| 体系 | 模型 | 用途 |
|---|---|---|
| BehaviorTreeAI | 行为树 | UGC / AI 编辑器 / 新版村民旅人（主力） |
| AITask | 优先级 + 互斥位状态机 | 老怪物，C++ 写死，170+ `AI*` 子类 |
| AILuaManager | Lua 协程式 | 个别 NPC |

- 无 GOAP。
- 核心类：`AIController`（ClientActor 组件，统一入口，同时持 `AITask` 与 `BTreeIns`，用位掩码启停）、`BTManager`（per-WorldManager）、`BTreeCreator`（工厂 + 对象池 + 按 scriptKey 分桶复用，上限 64/脚本、512/总）、`BTreeIns`（单棵树实例）、`BTBlackboard`、`BTNodeBase + BTNodeFactory`、`BTGetValue`。
- 挂载链路：`ClientMob::OnInit → CreateComponent<AIController> → LoadBTree`（延迟 1 tick，按优先级：存档/Prefab BTreeArray → AI 编辑器本地树 → 怪物 def 的 `AIConfig` UUID → 老 `F<id>_SetAi` → AILua 兜底）。
- **关键省力设计**：`BTManager::Tick()` 是空的，行为树 tick 由每个 actor 自己 `AIController::aiTick() → BTreeIns::Tick()` 驱动，**自然受 AOI 门控**（AOI 不可见 → `needUpdateAI()` false → 整套 AI 停）。

### 4.2 行为树是数据驱动的

- 节点基类 `BTNodeBase`（`BTNode/BTNodeBase.h:41-228`）六段生命周期：`RunBefore → ActivateCondition → OnStart → RunActive(循环) → OnEnd → RunBack`，返回统一 `BTNODERESULT(SUCCESS/FAIL/WAIT/LOGIC_WAIT/NO_RUN)`。
- 节点类型（`BTNodeFactory.cpp`）：
  - 控制流：`Root/Selector/Sequence/Random/Parallel/SimpleParallel/Branch/Priority/Loop/LoopInterval/Condition/Inversion/Success/Fail`
  - 装饰：`Decorator/Single`
  - C++ 任务（性能热点）：`BTNTaskFollow/Attack/MoveTo/Wander/DigBlock/Plant/RangeAttack/Mate/Navigator`
  - **Lua 任务（扩展缝）：`BTNodeTaskLua` / `BTNodeConditionLua`**
  - 事件：`BTNodeEvent` 及 9 种子类
- **行为树结构 = 声明式 Lua 表**（`BT_MOB_<id>.lua`，如渔村村民 `BT_MOB_3214.lua` 2072 行）：
  ```lua
  { node = "BTNodeSelector", children = {
      { node = "BTNTaskMoveTo", param = { bbkey_targetpos = "stollPos", speed = 0.8 } },
      { node = "BTNodeTaskLua", param = { script = "BTTask_commonJudge", subparam = {...} } },
  }}
  ```
  叶子 `BTNTask*` 是 C++ 节点；`BTNodeTaskLua` 通过 `script="BTTask_xxx"` 引用一个 Lua 任务文件。
- **可复用 Lua 任务节点**（`luascript/ai/task/BTTask_*.lua`，数十个），契约固定：`subparam` + `onbeg/onend/run`，run 内只能用 `controller.target / controller.bb.get/set / controller.wait/success/fail / controller.subparam`（`BTTask_FindBlock.lua` 头部就是正式的节点 API 约定）。

### 4.3 黑板 / 感知 / 空间查询

- 黑板 `BTBlackboard`：强类型键值（`BTLuaData` union：number/string/object/vector3/item/area/timer/array），`ToString/InitByString/SetByString` 走 JSON，可存档可同步。`BTGetValue` 把节点参数包成 Lua 字符串，`ExecGetValue()` **运行时求值**（参数可延迟到 tick 时从黑板/表达式计算）。`bbChange()` 黑板键变化时标脏依赖该键的条件节点，跨 tick 复用结果。
- 感知核心在 `AIFunctionMgr`（巨型 C++ AI 原语库，1281 行）：`GetNearestActorPos/GetNearestPlayerPos/findNearestObjIdBySpecifyType/findEnemyAttacked/canSeeInAICache`。
- **空间查询优化，非全场扫描**（`AIFunctionMgr.cpp:10549`）：用 `findNearActorsWithNoType` 基于 block-range 按 chunk/section 局部查；**搜索半径硬上限 300 格**（`if (range>300) range=300`）；平方距离排序免开方；寻路校验 `IsAbleGetPath` 后置；视野判定有缓存 `canSeeInAICache`。

### 4.4 调度与性能：AI LOD 三件套

`BehaviorTreeInstance.cpp:218-284`：
1. **实例化时取模分帧桶**：`m_modetick = s_tickInstanceCount++ % m_tickstep`（`m_tickstep=7`）→ 所有树天然散到 7 帧不同桶，零成本错峰。
2. **距离二级降频**（基于离最近玩家平方距离）：`>6400` 格每 6 tick 一次；`>3200` 格每 2 tick 一次；近处全速。这就是 AI LOD。
3. **寄生 AOI 可见性门控**：不可见直接停整树。
- 另有出生延迟门 `m_mobtick>5`；AITask 侧 `m_TickRate=3` 帧做完整重调度、中间帧只 `continueRun()`。
- 热点 C++ / 灵活 Lua 边界：寻路/攻击/移动/感知/空间查询全 C++；行为编排（树结构）、条件判断、每怪不同的业务逻辑放 Lua。

### 4.5 可视化配置链路

```
[AI 可视化编辑器] ── 拖拽节点 / 配参数 / 绑黑板键
        │ 导出
        ▼
BT_MOB_<id>.lua (声明式树) + BB_MOB_<id>.lua (黑板) + 自定义 UUID 文件
        │ BTManager::GetAiDir(uuid) 三级查找 + BTCommand:LoadAIEditConfig (loadstring+setfenv 沙盒)
        ▼
btscript_init.lua: BehaviorTreeLoadBTree → BTreeCreator::AIEditCreateNode (C++ 建树+建黑板, 走对象池)
        ▼
BTreeIns::Tick() (C++ 内核, 分帧+LOD)
   ├─ BTNTask* 叶子 → 直接调 C++ (AIFunctionMgr 感知/寻路/攻击)
   └─ BTNodeTaskLua 叶子 → script="BTTask_xxx" → 调 Lua run(), Lua 内只能用 controller.* API 回调 C++
        │ 调试回传
        ▼
SendDataToDebug / initSendDebugdata → 编辑器高亮当前运行节点 (支持暂停/单步)
```
- 给策划/玩家配置的部分：整棵树结构、每节点 `param`/`subparam`、黑板键。
- 映射方式：编辑器用节点 UUID + 整数参数键 → `BTNodeBase::SetParam(int key,...)` + `addGetValueNode(key, luastr)` → 运行时 `BTGetValue::ExecGetValue` 求值绑回黑板。
- **双向**：运行态通过 `GetNodeId()` 回推编辑器实时高亮调试。

### 4.6 → 对本项目的借鉴

1. **行为树数据化最小骨架 = 声明式 Lua 表 + C++ 节点工厂 + 名字注册表**。Lua 表本身就是可读、可 diff、可热加载的“序列化格式”。**先做 Lua 表驱动，可视化编辑器只是它的 GUI 前端——不要反过来先做编辑器。**
2. **留“叶子节点 = Lua 函数 + 固定 controller API”扩展缝**（照搬 `BTNodeTaskLua` + `controller.*` 契约）。策划/玩家加行为只写一个 `BTTask_xxx.lua`，不碰 C++、不重编译。
3. **感知收敛进一个 C++ AI 原语库，内建空间查询上限**（对应本项目 Ogre 大世界要有等价的空间分区 query；强制 range 上限、平方距离、寻路后置、视野缓存）。直接对应 `architecture-improvement-plan.md` P7。
4. **AI LOD 三件套**（取模分帧桶 + 距离降频 + AOI 门控）补到现有 AIScheduler 上。
5. **黑板强类型 + JSON 可序列化 + 节点运行态回传调试**——可视化配置若没有运行时可视化调试，策划没法自助调 AI，这条不能省。

---

## 5. UGC 玩法规则（GameMode）：三元组数据驱动

- **规则原子**：`struct GameRuleOption { unsigned short ruleid; unsigned short optionid; float cval; }`（`GameModeDef.h:198-203`）。全部平铺进 `GameMode::m_Options: unordered_map<int, GameRuleOption>`，统一入口 `setGameRule(ruleid, optionid, val)`、`getRuleOptionVal(ruleid)`。
- 规则 enum `GAMEMAKER_RULE` 已膨胀到 80+ 项：时间/天气/重力/视角、队伍/胜负（`GMRULE_ENDTIME/ENDSCORE/LIFE_NUM`）、复活无敌、显示开关、生物刷新（`GMRULE_*GEN`）、毒圈/温度/科技树等。
- 5 个 SceneComponent 聚合：`PlayerSetterComponent / LevelModelSetterComponent / TeamSetterVectorComponent / ReviveComponent / TeamDataComponent`；队伍设置用继承复用（`TeamSetterComponent : PlayerSetterComponent`）。
- **玩家不写代码定义玩法**：C++ 全套 setter 经 tolua 暴露（`IGameMode` 全 virtual），编辑面板是 Lua + UIEditor。
- 规则与触发器的组合点 = 事件脚本 `WEVENT_SCRIPT_TYPE`（`GameModeDef.h:217-234`）：`WES_GAME_START/END/RUN/TIMEOVER/PLAYER_DIE/PLAYER_REVIVE/ENTER_TEAMID/GAME_LOAD/PLAYER_INIT/ON_CHUNKGEN` 等 13 个生命周期钩子，地图绑定的 Lua 脚本由 `callEventScript` 执行。
- 运行驱动：`tickRunMode` 状态机 `PREPARE→COUNTDOWN→SHOWINTROS/SELECTTEAM→RUN→END`；胜负三条独立路径 `onTimeOver/scoreReachTarget/checkLifeNum`。
- 持久化：FlatBuffer `WorldSave.fbs`，老地图用 `fitWithOldRules` 升级。

### → 借鉴

- **玩法规则用 `{ruleid, optionid, value}` 三元组 + 统一 setter facade**：80+ 条规则不建 80 个字段/类，一个 map 平铺。新增规则只加 enum 项、存档 schema 不变、向后兼容极好。这让“作者不写代码配玩法”成立。
- **给玩法开一组生命周期事件钩子**（game start/end/player die/revive/...），地图脚本挂这些钩子——这是玩法规则与触发器/脚本的衔接面。

---

## 6. 可视化编辑器

### 6.1 场景编辑器

- `MiniDeveloper/SceneEditor/`：一组编辑模式单例管理器，把鼠标/触控翻译成对当前 `World` 的方块/Actor/触发器修改，**全部经统一撤销重做**。
- **所见即所得**：靠可复用 3D 交互部件（InteractBox 选区盒 6 面拖拽手柄+gizmo、BlockPreview 半透明预览、ZoneOperate 区域选区、3DUIOperateBtn 悬浮按钮）。**手动种树/挖河复用地形生成的 `EcosysUnit` 基类——手动放置与自动生成走同一套生成代码**（WYSIWYG 一致性关键）。
- **编辑态 vs 运行态**：`PrefabEditModeMgr` 另开独立 `BlockScene`（`PREFAB_EDIT_SCENE`），编辑时相机/天空/灯光/被编辑 Actor 全迁入临时场景，退出迁回——场景隔离。
- **撤销重做用 op-record 增量记录而非快照**：每类对象一个 Cmd 队列（Block/Actor/Area/Pos/Creature/Displayboard/Brush/DropItemPoint），事务 `StartNewCmd → AppendXxxInfo → EndCurrentCmd`，`Execute` 走 `newData`、`Revoke` 走 `oldData`。Actor `opType` 含 `CREATE/DESTROY/MOVE/SCALE/ROTATE/TO_CREATURE/MERGE_ITEM`。
- 序列化：方块改动用 FlatBuffer；笔刷/地形命令存 before/after + zlib 压缩缓冲；联机按 `uin` 分账。
- **反面教材**：三套并存的 Cmd 体系（EditorCmd/EditorBrush/EditorTerrain）**撤销栈互不共享**。

### 6.2 UI 编辑器 + 可视化逻辑

- **核心架构（最值得参考）**：UIEditor 是“编辑期数据模型 → 运行期 FairyGUI 真实控件”的桥接，**两端共用同一份 `DevUINode 树 + UIComponent 组件列表` 模型和同一份 jsonxx 序列化**：
  - 编辑期：`DevUINode`（transform/锚点/可见/关系链/控制器 gear）+ 18 种语义组件（Button/Label/Texture/Input/Grid/Spine/Beacon/Progress...）。
  - 运行期：`FactoryCreate*` 工厂族把每个 node 翻译成 `fairygui::GObject`，`GObjectFactoryCreator` 一对一派发。node id ↔ GObject id 一一对应。
- 拖拽配置：Lua 上层 IDE（79 个 lua 文件）走 MVC + Command 模式，画板节点与 C++ `DevUINode` 双向引用，撤销重做在 Lua 侧 `UIEditorCmdManager`（栈上限 20）。
- **可视化逻辑积木用 CSV 数据驱动定义**：`TriggerFunction.csv` 每行定义一个可视化触发器函数（`ID,Name,ReturnType,Param1..10,DefaultParam*,Display`），参数类型用 `资源类型,数量` 编码，描述带 `@1/@2` 占位符做模板填空。**UI 按 CSV 渲染参数槽——加积木不改编辑器代码。** 另有 `CodeBlockConfig.csv / CodeBlockDynamicValue.csv / TriggerParam/Enum/Item.csv`。
- Gear 控制器系统（可视化状态切换）：每个节点为每个控制器页每种 gear（DISPLAY/XY/SIZE/COLOR/TEXT/ICON...）存一份属性差分，换页时 apply，实现 normal/pressed/disabled 多状态可视化配置。

### 6.3 AI 辅助生成（design/02）

核心结论：**“分而治之，绝不让模型直接写最终工程文件”**：
1. 图像模型只生成视觉资产（panel/button/icon），**不绑定布局、不烤文字进图**（文字仍走 Label/RichText）。
2. AI 图必须走既有资源链路：`UploadTexture → 平台压缩(Win DXT5 / 移动 ASTC_6x6) → 云上传 → 回填节点 sandboxAssetId`。
3. 布局：模型只输出**中间布局 JSON**（project/assets/nodes，含 component/x/y/w/h/scale9），本地转换器再调 `UIEditorManager::CreateNode` + 命令栈落地——**复用编辑器既有 CRUD/撤销/保存/运行时装载，不旁路。**

### 6.4 → 对本项目的借鉴

1. **编辑态模型 = 运行态模型同源**：本项目 FGUI 已是运行态控件层，可直接做“编辑期 `DevUINode` → `FactoryCreate*` → FairyGUI GObject”的桥接。**这验证了本项目 FGUI 的重投入不是过度——它正是可视化编辑 UI 的基座。**
2. **op-record 增量撤销 + 单一命令栈**：从一开始定义单一 `ICommand` 接口和单一命令栈，所有编辑动作（方块/Actor/触发器/UI/地形）统一入栈，避开它“三套撤销栈不共享”的坑。
3. **可视化逻辑积木用 CSV 数据驱动定义**：触发器/行为节点的“可用函数、参数槽、默认值、显示模板”全配在 CSV，UI 按数据渲染——对“生物触发器”“AI 可视化配置”极契合，直接照搬这个模式。
4. **WYSIWYG 一致性**：手动放置与自动生成共用同一套生成代码。
5. **AI 辅助生成守边界**：模型只产中间结构化 JSON + 视觉资产，落地一律走编辑器既有 CreateNode/命令栈/保存管线。

---

## 7. C++ / Lua 边界（贯穿全项目的铁律）

| 放 C++ | 放 Lua |
|---|---|
| 每帧热点：Actor tick / 物理 / AI 内核 / 渲染 / 空间查询 | 玩法规则、配置、UI 路由、触发器业务逻辑 |
| 生命周期 fan-out 顺序（`UGCManager::OnEnterWorld` 扇出 20+ 子系统） | 事件回调里的具体玩法 |
| 对象/组件/容器/方块的存储与同步 | 新容器/新行为类型（虚函数转事件让 Lua 实现） |

- **反向调用统一收口**：C++→Lua 全走 `LuaCallInterface::LuaFuncCall(<id>, ...)`，事件回调风格——**C++ 控制生命周期顺序，Lua 永远只是被回调，从不反控生命周期。**
- **对象同时暴露给 Lua 和编辑器**：同一份反射属性表导出两份列表——`GetScriptList`（Lua 绑定）与 `GetPublicList`（编辑器面板）。一次声明，两端共用。
- **稳定 facade**：Lua 看到薄壳 `UGCRuntime`（约 37 行转发，已核实），业务实现 `UGCManager` 不对 Lua 暴露（`.pkg` 只 `$cfile UGCRuntime.h`）。换实现不动 Lua 表。
- **触发器作者脚本经 `scriptsupport.*` 受控桥**与引擎通信（默认跑主 VM；`ENABLE_DEV_VM` 开后可切独立子 VM，Python 为进程级隔离）——隔离是可配置项，非默认。

> 借鉴：本项目沿用“热点 C++ / 玩法配置 Lua”原则即可；**新增的关键是“同一份反射属性表导出 Lua 列表 + 编辑器列表”**，以及把 `GameManager` 收敛成稳定 facade（对应 `architecture-improvement-plan.md` P3）。

---

## 8. 它踩过、本项目要主动绕开的债（对方自陈）

新项目最大优势是没有历史包袱，下列债从设计第一天就能避免：

1. **继承爆炸**：老 Actor 树 `miniActor/` 178 文件、40+ 子类，多为行为微调。→ 本项目“多生物”全程组件 + 数据配置，不要每种生物一个 C++ 子类。
2. **三套 AI 体系并存**（BT/AITask/AILua）无迁移表。→ 直接 all-in 数据驱动 BT 一套。
3. **CSV 无类型校验、热重载只有一张表能用**。→ 加载层加声明式类型/范围校验；所有 Def 表从一开始实现“就地替换”reload。
4. **ModPackMgr 上帝类 + ID 段重叠**。→ 用注册表驱动“类型→解析器/清理器/ID段”，ID 段预留充足且绝不重叠。
5. **三套互不共享的撤销栈**。→ 单一 `ICommand` 接口 + 单一命令栈。
6. **单 `m_pWorld` 裸指针、假设只有一个主世界**（RunTime 层不按 mapid 路由）。→ “大世界”若分块/多实例并行，这层从一开始按 worldId 路由。
7. **容器 `MAX_PACKINDEX_TYPE=150` 硬编码、`BackPack` 构造手写 27 行 `new`**。→ 用 `(index, factory)` 配置表驱动。

---

## 9. 对照 HelloOgre3D 现状：差距与拼图

把对方的成熟形态对齐到本项目当前结构（`SandboxObject → BaseObject → AgentObject → SoldierObject` 窄继承链 + 组件 map + 全局单例耦合 + FGUI 已重投入 + AI 有 FSM/DT/BT + AIScheduler）：

**本项目已有、可直接接的资产：**
- **FGUI = 可视化编辑器 UI 基座**（对方 UI 编辑器就跑在 FairyGUI 上）。
- **BT 内核已有 = 只差数据驱动层**（Lua 表 + 节点工厂注册表 + Lua 叶子节点契约）。
- **AIScheduler 已有 = 只差距离 LOD 与 AOI 门控**。

**本项目缺的核心拼图（按前置依赖排序，已纠偏）：**
1. **CSV / 数据驱动的 `Def` 只读原型体系**（生物先数据化，对应轨道 A——这才是多生物的真脊梁）。
2. **统一事件总线（含参数化事件名 + 四件套）**，并把事件流接到 AI driver 入口。
3. **空间分区查询**（大世界多生物 AI 与感知的前置；感知设 `range` 上限）。
4. **行为树数据化**（Lua 表 + `CreateNode(字符串)` 工厂 + Lua 叶子节点契约）。
5. **编辑器节点模型 + flag 多通道属性（轨道 B）**——**仅在做可视化配置/编辑器时引入**，不外溢到玩法对象。

> 这次调研印证了 `docs/design/architecture-improvement-plan.md` 的方向（解耦 + 组件化 + 数据驱动），但**纠正一处**：不要把“带反射的统一节点”当成所有对象的脊梁。多生物运行时走 CSV 原型（轨道 A）；反射 + flag 多通道留给编辑器/可视化配置层（轨道 B）。

---

## 10. 建议采纳顺序

1. **先立地基**：完成 `architecture-improvement-plan.md` 的 P1（解耦/去全局单例）+ CSV `Def` 原型体系（轨道 A），让生物可数据化实例化。**本阶段不做反射节点树**——它属于后期编辑器层（第 7 步）。
2. **统一事件总线 + 参数化事件名**（四件套全抄），把现有 Local 事件扩成统一总线，并把事件流接到 AI driver 入口。
3. **CSV / 数据驱动 Def 原型**，生物先数据化（组件组合，零 C++ 子类）。所有 Def 表从一开始支持“就地替换”reload。
4. **行为树数据化**（Lua 表 + 节点工厂 + Lua 叶子节点契约），AI LOD 补距离降频 + AOI 门控；感知收敛进 C++ 原语库 + 空间查询上限。
5. **触发器 ECA**（挂在统一事件总线 + 反射节点上，对象级用参数化事件名）。
6. **UGC 玩法规则**（`{ruleid, optionid, value}` 三元组 + 生命周期事件钩子）。
7. **可视化编辑器**（编辑态=运行态同源 + 单命令栈 + CSV 驱动积木定义 + 运行态回传调试），最后做；**此阶段才引入轨道 B 的反射节点 + flag 多通道属性**（编辑器节点序列化/属性面板）；AI 辅助生成守“中间 JSON + 不旁路编辑器”边界。

---

## 11. 证据文件索引（均位于 MiniGame_Dev 仓库）

**知识库文档**（`G:\MiniGame_Dev\Source\docs\sandbox\`）：
- `module/04-Module-ClientActor系统模块.md`、`08-Module-沙盒UGC整体架构总览.md`、`09-Module-Container容器系统.md`、`10-Module-AI状态机与行为树.md`、`12-Module-CSV表格配置体系.md`、`14-Module-沙盒玩法业务族.md`、`22-Module-UGCGameMode玩法规则.md`、`24-Module-UGCRunTime运行时.md`、`25-Module-UGCCore对象组件框架.md`、`26-Module-UGCTriggers触发器.md`、`27-Module-UGCSceneEditor场景编辑器.md`、`28-Module-UGCUIEditorUI编辑器.md`、`02-Module-Mod插件系统模块.md`
- `design/02-Design-UGCUIEditorAI图像与布局生成方案.md`

**核心代码**（`G:\MiniGame_Dev\Source\`）：
- 对象/反射：`SandboxEngine/Core/driver/scene/SandboxSceneObject.h`（SandboxNode，:59, 1003-1048）、`SandboxEngine/Core/driver/base/SandboxGameObject.h`（GameObject，:22）、`SandboxEngine/Core/driver/base/SandboxRuntimeClass.h`（RTTI，:192,256）、`SandboxEngine/Core/driver/base/reflex/`（反射框架 36 文件；`SandboxReflexConfig.h:22-68`、`SandboxReflexClassParam.h`、`SandboxReflexContainer.h:75-158`）、`SandboxEngine/Core/nodes/SandboxLightObject.cpp`（属性注册实例，:45,87-138）、`SandboxEngine/Core/base/SandboxNodeRegister.h`
- 事件/触发器：`SandboxEngine/Core/interact/eventObserver/ObserverEvent.h:173-229`、`ObserverEventManager.cpp:55-146,209-236`、`MiniGame/MiniModule/MiniDeveloper/Triggers/SceneArea/SceneArea.cpp:113-293`、`SandboxEngine/Service/scriptSupport/ScriptSupportEvent.cpp:201-299`、`MiniGame/MiniModule/MiniDeveloper/Triggers/TriggerScriptMgr.cpp:92-107,426-451`、`TriggerLivesToolMgr.cpp:137,506-579`
- AI：`SandboxGame/Framework/GamePlay/Components/AIController.h/.cpp`、`BehaviorTreeManager.*`、`BehaviorTreeCreator.cpp`、`BehaviorTreeInstance.cpp:218-284`、`BTNode/BTNodeBase.h:41-228`、`BTNodeFactory.cpp`、`SandboxGame/Play/gameplay/mgr/AIFunctionMgr.cpp:10549`；Lua：`AssetRuntime/CommonResource/Script/luascript/ai/btree/BT_MOB_*.lua`、`.../ai/bb/BB_MOB_*.lua`、`.../ai/task/BTTask_*.lua`、`.../ai/btscript_init.lua`
- 玩法规则：`SandboxEngine/Play/gamemode/GameModeDef.h:198-234`、`GameMode.cpp:2311`
- 配置/Mod：`MiniGame/MiniModule/CsvLoader/`（`AbsCsv` / `defmanager` / `MonsterCsv` / `BlockDefCsv`）、`SandboxGame/Modules/SandboxGame/Mods/ModPackMgr.cpp`；数据样本 `AssetRuntime/CommonResource/Script/csvdef/autogen/{monster,buffdef,aidef}.csv`、`csvdef/TriggerFunction.csv`
- 编辑器：`MiniGame/MiniModule/MiniDeveloper/SceneEditor/`（EditorCmd/EditorScene/EditorPrefab）、`MiniGame/MiniModule/MiniDeveloper/UIEditor/`（`GObjectFactoryCreator` / `UIEditorManager`）
- 运行时 facade：`SandboxEngine/.../UGCRuntime.h`（Lua 薄壳）、`UGCManager`（实现）

> 维护提示：本文是基于某一时点的外部项目快照所做的提炼，MiniGame_Dev 后续可能演进。落地任一条时，应回该仓库以当前代码复核，再结合本项目实际取舍。
