# HelloOgre3D 演化路线图

> 目标：让代码库能稳定承接 **大量对象类型 + 复杂行为** 的扩展，而不是每加一个新东西都要全局改一遍。
>
> 范围：覆盖 `src/HelloOgre3D/` C++ 全部子系统与 `bin/res/scripts/` Lua 脚本层。
>
> 节奏：分四个阶段，每阶段独立可交付、可回退。不允许"大爆炸式重构"。

---

## 0. 现状画像（一页式总览）

### 已经做对的事

- `IDecisionDriver` 抽象把 FSM / DecisionTree / BehaviorTree 统一在同一驱动接口下，对象只看到 `m_driver->Tick(...)`。
- 行为树已经走通了 **声明式 Lua 配置 + Loader 构造 + C++ 执行** 这条路（[BehaviorTreeLoader.lua](../bin/res/scripts/ai/behavior/BehaviorTreeLoader.lua) + [config/SoldierBT.lua](../bin/res/scripts/ai/behavior/config/SoldierBT.lua)），跟成熟项目同形态，且 Condition 用 Lua 闭包比字符串协议更类型安全。
- `Blackboard` 已经是分类型 map（agent / float / int / bool / string / vec3），避免了 `std::any` 在 debug 构建的开销。
- ClientManager 已经在 30Hz 节流并透传固定 dt 给 GameManager，相当于隐式做了"Update / Tick 分层"的一半。
- Tracy 埋点（[Profile.h](../src/HelloOgre3D/runtime/profiling/Profile.h)）已经覆盖了关键路径，性能问题可观测。
- 已经存在 `sandbox/objects/components/`（PhysicsComponent / AgentLocomotion / RenderComponent）这条组件化萌芽。

### 走偏 / 缺失的事

| 类别 | 现状 | 妨碍什么 |
|---|---|---|
| **对象类太肥** | [SoldierObject.cpp](../src/HelloOgre3D/sandbox/objects/SoldierObject.cpp) 716 行，扛了 9+ 职责（武器、姿态、敌人、输入、动画、3 套 driver） | 加坐骑/技能/buff 时分支爆炸；新对象类型只能复制粘贴 |
| **组件目录位置反 EC 思路** | 当前有两个 components 目录：[sandbox/components/](../src/HelloOgre3D/sandbox/components/)（领域层骨架，未接 IComponent）+ [sandbox/objects/components/](../src/HelloOgre3D/sandbox/objects/components/)（真在用，但挂在 objects/ 下）。EC 思路里"对象由组件组装"，组件应跟 objects/ 平级而不是挂在它下面；且 RenderComponent 没继承 IComponent、AgentLocomotion 含 steering 业务行为，分层不齐 | 新人不知道新组件该放哪；位置语义跟 EC 化方向冲突 |
| **三套 AI 驱动各写各的** | FSM / DT / BT 在生命周期、Blackboard 集成、Lua 桥接上完全重复实现 | 加一个全局能力（如感知）要写三遍 |
| **Lua 调用每次字符串查找** | [scriptluavm.cpp:346](../src/HelloOgre3D/common/scriptluavm.cpp#L346) `callFunctionV1` 每次 `lua_getfield(funcname)` | tracy 显示这是第 1 热点（387ms / 32s） |
| **Blackboard 每 agent 一份且不共享** | 三个 driver 各 copy，agent 间无共享 | 无法实现"看见敌人"、"小队协作" |
| **缺感知 / 记忆 / 技能基础设施** | BT/DT 节点临时算敌人列表、距离判断 | BT 树越改越乱，不可维护 |
| **动画状态机硬绑 SoldierObject** | [SoldierAnimController](../src/HelloOgre3D/sandbox/objects/animation/SoldierAnimController.h)、[SoldierAnimProfile](../src/HelloOgre3D/sandbox/objects/animation/SoldierAnimProfile.h)、`SOLDIER_STATE` 枚举耦合死 | 加怪物 / NPC 必须复制一整套 |
| **Lua 条件库重复实现** | [SoldierEvaluators.lua](../bin/res/scripts/ai/decision/SoldierEvaluators.lua)（DT，返回 1/2）与 [SoldierConditions.lua](../bin/res/scripts/ai/behavior/SoldierConditions.lua)（BT，返回 bool）逻辑相同 | 修一个条件要改两个文件 |
| **四种 SoldierAgent 入口无共同基类** | [SoldierAgent](../bin/res/scripts/agent/SoldierAgent.lua) / [BehaviorSoldierAgent](../bin/res/scripts/agent/BehaviorSoldierAgent.lua) / [DecisionSoldierAgent](../bin/res/scripts/agent/DecisionSoldierAgent.lua) / [IndirectSoldierAgent](../bin/res/scripts/agent/IndirectSoldierAgent.lua) | 加第 5 种驱动复制粘贴 |
| **无 AI 调度** | 所有 agent 同一帧 tick；NavigationMesh 寻路同步、无缓存 | 多 agent 后周期性卡顿、寻路 spike |
| **无数据驱动配置** | 角色属性、武器、技能写在 C++ 或散落的 Lua 中 | 无热更新、策划改不了 |

---

## 1. 总体目标与设计原则

### 终态目标

代码库能够支持下述场景而不需要架构性改动：

- 同时存在 **5+ 种 AI 对象类型**（士兵、怪物、车辆、NPC、坐骑），共享一套 AI / 动画 / 物理基础设施。
- 单场景 **50+ 个 agent** 同时活动，无周期性卡顿。
- 新增一种行为（例如"投掷手雷"）从需求到上线只需 **改 1 个配置 + 加 1 个 C++ 节点 / Lua action**，不动核心代码。
- 策划改数值（速度、血量、技能 CD）**不重编**，热更 Lua 配置即可。

### 设计原则

1. **组合优于继承**：对象 = 核心实体 + 组件。不要再往 `SoldierObject` 加字段。
2. **驱动 / 决策 / 执行分离**：AI 决定做什么（AIController）→ 翻译成抽象指令（AICommand）→ 实体翻译为具体动作（Soldier 开枪 / Vehicle 发射）。
3. **数据驱动**：行为树结构、角色属性、武器、技能全部走 Lua 配置表，C++ 只跑执行引擎。
4. **共享底座，类型特化**：感知 / 记忆 / 寻路 / 动画 ASM 一套通用实现，各对象类型只注入自己的 Profile。
5. **不做投机性抽象**：感知系统等真正撞到需求再做，而不是先搭脚手架。

---

## 2. 阶段路线图

| 阶段 | 重心 | 持续时间 | 准入条件 | 风险 |
|---|---|---|---|---|
| **Stage 1** | 清死代码、抽 AIController、AI 独立低频 tick | 1 周 | 现在就做 | 低 |
| **Stage 2** | 三套 AI driver 收敛 + Lua 调用提速 + Blackboard 统一 | 1-2 周 | Stage 1 完成 | 低 |
| **Stage 3** | 引入 Sensor / Memory / Skill 组件 + AICommand 指令层 | 2-3 周 | 实际有第二种 AI 对象类型需求时 | 中 |
| **Stage 4** | 完全 EC 化、数据驱动配置、AI 调度器、感知世界 | 4 周+ | 对象组合维度真的爆了 | 高 |

> **关键判断**：Stage 1-2 是无后悔决定，立刻能做；Stage 3 要等真实需求驱动；Stage 4 多数项目不会做到。

---

## 3. Stage 1 — 清地基与 AIController 抽离

> **目标**：让 SoldierObject 瘦下来，AI 跟表现层解耦，AI tick 频率可独立调。
>
> **预期收益**：tracy 中 `callFunctionV1` 总耗从 ~387ms 降到 ~150ms；SoldierObject 行数减 30%+；BT/DT/FSM 切换不再耦合在对象构造里。

### 任务 1.1 统一 components 目录到单根 + 按域子分类 + IComponent 体系对齐

#### 目标结构

把现有的两个 components 目录合并为一个，跟 `objects/`、`systems/`、`ai/` 平级，符合 EC "对象由组件组装" 的思路。子目录按**业务域**分（不是按业务/引擎分层），新加组件直接对号入座：

```text
sandbox/components/
    render/
        RenderComponent.{h,cpp}      ← 让它继承 IComponent
    physics/
        PhysicsComponent.{h,cpp}     ← 已继承，迁移即可
    agent/
        AgentBody.{h,cpp}            ← 从 sandbox/components/ 移入
        AgentAttrib.{h,cpp}          ← 从 sandbox/components/ 移入，接入 IComponent
        AgentLocomotion.{h,cpp}      ← 从 sandbox/objects/components/ 上移
    combat/
        WeaponComponent.{h,cpp}      ← 从 sandbox/components/ 移入，接入 IComponent
    (Stage 3 起按需新增)
    perception/   SensorComponent / MemoryComponent
    skill/        SkillComponent
    inventory/    InventoryComponent
    stats/        HealthComponent / BuffComponent
```

`sandbox/core/component/IComponent.h` 保持在 core/ 不动（它是接口契约，不是具体组件）。

#### 分类契约（写进 AGENTS.md）

- 新建组件**永远**放 `sandbox/components/<域>/`，不准再回 `sandbox/objects/`。
- 子目录命名规则：**业务域名词**（render / physics / agent / combat / perception / skill / inventory / stats / ai / network …）。
- 一个组件归哪个子目录：看它表达的**领域概念**，不看它的"业务还是引擎"性质。例：`AgentLocomotion` 虽然内部跑 steering 算法（引擎层风格），但表达的是 agent 移动能力 → 归 `agent/`。
- 组件**必须**继承 `IComponent`，不允许出现"看着像组件但不继承"的混合体（如当前的 `RenderComponent`）。

#### 落地动作

1. **新建目录** `sandbox/components/{render,physics,agent,combat}/`。
2. **迁移文件**：
    - `sandbox/components/AgentAttrib.{h,cpp}`           → `sandbox/components/agent/AgentAttrib.{h,cpp}`
    - `sandbox/components/AgentBody.{h,cpp}`             → `sandbox/components/agent/AgentBody.{h,cpp}`
    - `sandbox/components/WeaponComponent.{h,cpp}`       → `sandbox/components/combat/WeaponComponent.{h,cpp}`
    - `sandbox/objects/components/RenderComponent.{h,cpp}`   → `sandbox/components/render/RenderComponent.{h,cpp}`
    - `sandbox/objects/components/PhysicsComponent.{h,cpp}`  → `sandbox/components/physics/PhysicsComponent.{h,cpp}`
    - `sandbox/objects/components/AgentLocomotion.{h,cpp}`   → `sandbox/components/agent/AgentLocomotion.{h,cpp}`
3. **删除空目录** `sandbox/objects/components/`。
4. **IComponent 接口对齐**：
    - `RenderComponent` 改为 `: public IComponent`，实现 `onAttach/onDetach` 缓存 GameObject。
    - `AgentAttrib`、`WeaponComponent`、`AgentBody` 改为 `: public IComponent`，删掉自己重复声明的 `onAttach/onDetach/start/update`，cpp 给出最小实现。
    - `AgentLocomotion` / `PhysicsComponent` 已继承 IComponent，清理它们内部冗余的 `m_owner`（基类已有 `m_gameobj`），cpp 引用点同步改。
5. **修复 include 路径**：
    - 影响面（grep 出来的）：[BlockObject](../src/HelloOgre3D/sandbox/objects/BlockObject.cpp)、[VehicleObject](../src/HelloOgre3D/sandbox/objects/VehicleObject.cpp)、[RenderableObject](../src/HelloOgre3D/sandbox/objects/RenderableObject.cpp)、[ObjectManager](../src/HelloOgre3D/sandbox/systems/manager/ObjectManager.cpp) 等 ~10 处。
    - 配套改 premake 脚本里 `sandbox/components/**` 的通配收集（[premake/premake.lua](../premake/premake.lua) / [samples.lua](../premake/samples.lua)）。
6. **暂不动业务逻辑**：
    - 不强制 SoldierObject 改用 WeaponComponent —— 那是 Stage 3 的工作（跟 SkillComponent / InventoryComponent 一起做）。
    - 不合并 RenderableObject 跟 RenderComponent —— 它俩职责重复要清理，但放 Stage 3 EC 化时一并处理。
    - AgentLocomotion 里 `VehicleObject* m_owner` 这种业务绑定先保留（基类已经有 GameObject*，业务字段缓存先不动），Stage 3 再泛化。

#### 不做的事

- 不删除 `sandbox/components/` 下的领域组件骨架（用户已确认这些是 EC 化时要保留的）。
- 不引入 archetype / SoA / SystemScheduler 等"完美 ECS"机制 —— 那是 Stage 4 才考虑且多半不做。
- 不调整 `sandbox/core/component/IComponent.h` 的接口签名 —— 保持稳定。

#### 风险

- **链接错误**：迁移后 include 路径错漏会大面积爆。缓解：分两步提交，先新增不删旧（保留 forwarding header），验证通过后再删原文件。
- **premake 通配未更新**：cmake/premake 收集不到新位置文件。缓解：调整后立刻全量重编一次。
- **基类字段冗余**：PhysicsComponent / AgentLocomotion 内部都缓存了自己的 owner（跟 IComponent::m_gameobj 重复），迁移时顺手清理，但要小心 cpp 引用点。

#### 工时

- 文件迁移 + include 修复：3-4 小时
- IComponent 接口对齐 + 最小 cpp 实现：2 小时
- premake 调整与全量编译验证：1 小时
- 合计：**半天到 1 天**

### 任务 1.2 抽出 `AIController`

- **位置**：新建 `src/HelloOgre3D/sandbox/ai/common/AIController.{h,cpp}`。
- **职责**：聚合 `m_driver`（IDecisionDriver*）、`m_blackboard`（从 driver 内提到 controller 持有）、AI 相关字段（`m_enemy`、`m_hasMovePos`、`m_movePos`、`m_target` —— 从 SoldierObject 迁过来）、AI tick 调度状态（`m_aiTickAccumMs`、`m_strideBucket`）。
- **接口**：
  ```cpp
  class AIController {
  public:
      explicit AIController(AgentObject* owner);
      ~AIController();

      void  SetDriver(IDecisionDriver* d);  // 取所有权
      Blackboard* GetBlackboard() { return &m_blackboard; }
      AgentObject* GetOwner() const { return m_owner; }

      // 由 GameManager / Sandbox 调度，不再由对象 Update 触发
      void TickAI(float fixedDtMs);
  };
  ```
- **SoldierObject 改动**：删除 `m_driver`、AI 字段；持有 `AIController* m_ai`；构造里创建 AIController 并塞入 FSM/DT/BT 驱动；析构里 delete。
- **tolua 绑定**：AIController 暴露为 Lua 类，Lua 侧通过 `soldier:GetAI():GetBlackboard()` 访问。
- **风险**：tolua 绑定要重生成；Lua 侧调用点要改。

### 任务 1.3 AI 独立低频 tick

- **现状**：[SoldierObject::Update](../src/HelloOgre3D/sandbox/objects/SoldierObject.cpp#L139) 里 `m_driver->Tick(deltaMilisec)` 跟动画、武器、locomotion 同频跑。
- **动作**：
  1. SoldierObject::Update 不再 tick AI，只跑表现层（动画、武器、locomotion）。
  2. GameManager 增加一个 AI 累加器，达到阈值（默认 100ms = 10Hz）才遍历所有 AI 对象调 `ai->TickAI(...)`。
  3. AI tick 频率走配置：`GameManager::SetAITickIntervalMs(int)`，Lua 可调。
- **代码骨架**：
  ```cpp
  // GameManager::Update
  m_aiAccumMs += deltaMs;
  if (m_aiAccumMs >= m_aiTickIntervalMs) {
      for (auto* obj : m_objectMgr->GetAllAIObjects()) {
          if (auto* ai = obj->GetAI()) ai->TickAI((float)m_aiAccumMs);
      }
      m_aiAccumMs = 0;
  }
  ```
- **副作用警告**：AI 反应延迟从 33ms 变成 100ms。对当前 sandbox 完全可接受。Move 动作的"路径前进"留在表现层 30Hz，决策"是否换目标"才走 AI 10Hz。
- **预期收益**：`LuaBehaviorAction::OnUpdate` 调用次数砍 2/3，`callFunctionV1` 387ms → ~150ms。

### 任务 1.4 修掉 Viewport 每帧重设

- **现状**：[ClientManager.cpp:482-491](../src/HelloOgre3D/game/ClientManager.cpp#L482-L491) 每渲染帧把 viewport 重设为 `(0,0,1,1)` 并更新 aspect ratio。1650 帧 / 32s = 23ms 浪费。
- **动作**：挪到 `windowResized` 回调里，只在窗口尺寸变化时触发。
- **工时**：30 分钟。

### Stage 1 验收

- [ ] `sandbox/objects/components/` 已删除；所有组件统一在 `sandbox/components/<域>/` 下；所有组件均继承 `IComponent`；AGENTS.md 写明子目录命名规则与归属判定。
- [ ] 全工程编译通过，Sandbox6/7/8 行为表现跟改动前一致。
- [ ] `AIController` 类已存在，所有 AI 字段从 SoldierObject 迁走，SoldierObject 行数 < 500。
- [ ] AI tick 跟对象 Update 解耦，可通过配置改频率。
- [ ] 重跑 tracy，`callFunctionV1` 总耗 ≤ 200ms。

---

## 4. Stage 2 — AI driver 收敛 + Lua 调用提速 + Blackboard 统一

> **目标**：消除三套 driver 的代码重复；Lua 调用单次成本降一半；Blackboard 支持跨 agent 共享。
>
> **预期收益**：`callFunctionV1` 单次成本降 ~40%；维护一处条件库就够；agent 协作成为可能。

### 任务 2.1 把生命周期 / Blackboard 提到 IDecisionDriver 基类

- **现状**：`DecisionTreeDriver`、`BehaviorTreeDriver`、`AgentStateController`(FSM) 都各自 own 一份 Blackboard、各自实现节点工厂、各自处理 Init/Tick。
- **动作**：把 `Blackboard m_blackboard`（已经移到 AIController 了，这里就让基类 view）、`Init()`、调试 trace 接口（`SetDebugTraceEnabled`）下沉到 `IDecisionDriver` 默认实现。
- **保留差异**：节点种类、Tick 内部算法，各 driver 自己写。
- **副效果**：FSM/DT/BT 共享 trace 设施 → Sandbox6 也能看 trace。

### 任务 2.2 合并重复的 Lua 条件库

- **现状**：[SoldierEvaluators.lua](../bin/res/scripts/ai/decision/SoldierEvaluators.lua)（DT，返 1/2）和 [SoldierConditions.lua](../bin/res/scripts/ai/behavior/SoldierConditions.lua)（BT，返 bool）逻辑完全相同。
- **动作**：
  1. 新建 `bin/res/scripts/ai/common/AgentConditions.lua`，所有判定写成返回 bool 的函数。
  2. SoldierEvaluators.lua 改为 thin wrapper：`function HasEnemyBranch(...) return AgentConditions.HasEnemy(...) and 1 or 2 end`。
  3. SoldierConditions.lua 直接转发 `return AgentConditions.HasEnemy(...)`。
- **后续**：随着 Stage 3 引入 Sensor，这些条件大部分会被 `bb:GetBool("hasEnemy")` 替代，条件库会进一步精简。

### 任务 2.3 Lua 函数 ref 缓存（最大性能收益）

- **现状**：[ScriptLuaVM::callFunctionV1](../src/HelloOgre3D/common/scriptluavm.cpp#L346) 每次 `lua_getfield(-1, funcname)` 做字符串哈希查找。LuaBehaviorAction/LuaDecisionAction 每帧每 agent 调 3 次（OnInitialize/OnUpdate/OnCleanUp）。
- **动作**：
  1. 给 `LuaEnvObject` 加 `int CacheFunctionRef(const char* name)` —— 一次性查 Lua 函数并 `luaL_ref(L, LUA_REGISTRYINDEX)` 保留 ref。
  2. 给 `ScriptLuaVM` 加 `bool callFunctionByRef(int ref, const char* format, ...)`，用 `lua_rawgeti(L, LUA_REGISTRYINDEX, ref)` 直接拿函数。
  3. LuaBehaviorAction / LuaDecisionAction / AgentLuaState 在 `BindToScript` 完成时缓存 OnInitialize/OnUpdate/OnCleanUp 三个 ref，运行时直接调 ref 版本。
- **副效果**：funcname 字符串也不必再写进 profile zone 文本（[scriptluavm.cpp:350-352](../src/HelloOgre3D/common/scriptluavm.cpp#L350-L352)），profile 自身开销也降。
- **预期**：`callFunctionV1` 单次成本 37µs → ~20µs。

### 任务 2.4 收敛四种 SoldierAgent 入口为一个基类

- **现状**：[SoldierAgent.lua](../bin/res/scripts/agent/SoldierAgent.lua)、[BehaviorSoldierAgent.lua](../bin/res/scripts/agent/BehaviorSoldierAgent.lua)、[DecisionSoldierAgent.lua](../bin/res/scripts/agent/DecisionSoldierAgent.lua)、[IndirectSoldierAgent.lua](../bin/res/scripts/agent/IndirectSoldierAgent.lua) 每个文件都有重复的动画 ASM 初始化、武器初始化、血量/弹药设置。
- **动作**：
  1. 提取 `agent/SoldierAgentBase.lua`：共享的 `SetupAnimation / SetupWeapon / SetupAttributes / Agent_Update` 默认实现。
  2. 四种变体只覆写 `SetupDriver`：分别注入 FSM / DT / BT / nil。
- **基础设施**：依赖 [base/class.lua](../bin/res/scripts/base/class.lua) 的单继承。

### 任务 2.5 Blackboard 扩展：支持 table 存储与全局/团队层

- **现状**：[Blackboard.h](../src/HelloOgre3D/sandbox/ai/common/Blackboard.h) 不支持存 table、array、复杂结构。每 agent 一份不共享。
- **动作**：
  1. 增加 `BlackboardScope`：`Local`（每 agent，现状）/ `Team`（同队共享）/ `Global`（场景级）。
  2. `AIController` 持有指针：`m_localBb`（own）、`m_teamBb`（GameManager 注入）、`m_globalBb`（GameManager 注入）。
  3. 暂不引入 table 存储（用 Lua registry ref + key 组合可临时解决，等真撞到需求再加）。

### Stage 2 验收

- [ ] `IDecisionDriver` 基类承担 Init / Blackboard 集成 / trace，三个子 driver 删除重复实现。
- [ ] `AgentConditions.lua` 上线，原两个文件成为薄包装。
- [ ] Lua 函数 ref 缓存生效，tracy 中 `callFunctionV1` 单次 mean ≤ 25µs。
- [ ] 四种 SoldierAgent 共享 `SoldierAgentBase`。
- [ ] Blackboard 支持 Local / Team / Global 三层，至少有一个测试场景使用 Team Blackboard。

---

## 5. Stage 3 — Sensor / Memory / Skill 组件 + AICommand 指令层

> **目标**：把 AI 决策依赖的"输入"（看见什么、记得什么、能用什么技能）下沉到组件；让一棵 BT 树能驱动不同实体类型。
>
> **触发条件**：实际需要第二种 AI 对象（怪物 / NPC / 车辆 AI）时，或 BT 节点里出现"临时算敌人列表"代码时。

### 任务 3.1 SensorComponent

- **位置**：`src/HelloOgre3D/sandbox/ai/perception/SensorComponent.{h,cpp}`。
- **职责**：周期性扫描视野范围内的对象（敌人 / 友军 / 资源 / 危险），按距离 / 威胁度排序，结果写到 `AIController::GetLocalBlackboard()`。
- **可配置**：视野角度、距离、扫描频率（默认与 AI tick 同频）。
- **副作用**：BT/DT 节点只需 `bb:GetAgent("enemy")` 读结果，不再临时算。
- **节点改动**：`AgentConditions.HasEnemy` 改为读 bb，原始扫描逻辑搬到 SensorComponent。

### 任务 3.2 MemoryComponent

- **位置**：`src/HelloOgre3D/sandbox/ai/perception/MemoryComponent.{h,cpp}`。
- **职责**：记录"上次看到敌人的位置 + 时间戳"，敌人离开视野后保留 N 秒，便于"追到最后已知位置"行为。
- **依赖**：SensorComponent 输出。
- **节点改动**：新增 BT 条件 `HasLastKnownEnemyPos` / Action `MoveToLastKnown`。

### 任务 3.3 SkillComponent

- **位置**：`src/HelloOgre3D/sandbox/ai/skill/SkillComponent.{h,cpp}` + `Skill.{h,cpp}`。
- **职责**：管理技能列表（每个技能有 cd / cost / range / type）；提供 `CanCast(skillId)` / `Cast(skillId, target)`。
- **数据**：技能配置走 Lua 表（`bin/res/scripts/data/skills.lua`），ConfigLoader 加载。
- **节点改动**：BT Action `UseSkill(skillId)` 通用化，不再每个技能写一个 Action。

### 任务 3.4 引入 AICommand 指令层

- **现状**：BT/DT 的 LuaAction 直接调 `owner:ShootBullet()` / `owner:SetMoveDir(...)`，跟 SoldierObject API 绑死。
- **动作**：
  1. 定义 `struct AICommand { enum Kind {MoveTo, Attack, UseSkill, Interact, Idle, Stop}; ... }`。
  2. `AIController::IssueCommand(const AICommand&)`，缓存当前命令。
  3. `AgentObject::ApplyCommand(const AICommand&)` 虚函数，SoldierObject / 未来的 MonsterObject 各自翻译为具体动作。
  4. BT/DT/FSM action 改成下发命令，不直接调对象方法。
- **收益**：一棵 BT 配置可以驱动 Soldier / Monster / Vehicle，只要它们实现 `ApplyCommand`。

### 任务 3.5 AnimController / AnimProfile 接口化

- **现状**：[SoldierAnimController](../src/HelloOgre3D/sandbox/objects/animation/SoldierAnimController.h)、[SoldierAnimProfile](../src/HelloOgre3D/sandbox/objects/animation/SoldierAnimProfile.h) 硬绑 `SOLDIER_STATE` 枚举。
- **动作**：
  1. 抽 `IAnimController` 接口：`OnBodyStateChanged(int stateId)` / `OnBodyNotify(...)` / `Update(float dt)`。
  2. 抽 `IAnimProfile` 接口：`GetStateIdByName(...)` / `GetStateNameById(...)` / `MapIntentToState(...)`。
  3. SoldierAnimController / Profile 实现这两个接口，未来 MonsterAnimController / Profile 可另起一套。
  4. AgentObject 持有 `IAnimController*` 而非 SoldierAnimController*。

### 任务 3.6 NavigationMesh 查询缓存与限频

- **现状**：[NavigationMesh::FindPath](../src/HelloOgre3D/sandbox/ai/navigation/NavigationMesh.cpp#L278) 每次新建 polyRef 数组，无缓存。tracy 显示 max 44ms 的尖峰。
- **动作**：
  1. 给路径查询加 LRU 缓存（key = startCell + endCell + agentRadius，value = 路径），TTL 1 秒。
  2. 限制每帧寻路次数（默认 5），超出的请求挂到队列下一帧处理。
  3. `FindClosestPoint` / `RandomPoint` 同样限频。
- **预期**：寻路 spike 消失，多 agent 寻路平均成本下降 50%+。

### Stage 3 验收

- [ ] SensorComponent / MemoryComponent / SkillComponent 上线，至少有一个 sandbox 场景使用。
- [ ] AICommand 指令层落地，至少一棵 BT 配置同时驱动两种对象类型（Soldier + 新增的 Monster 或 NPC）。
- [ ] AnimController / Profile 走接口，新对象类型的动画接入成本可量化（< 1 周）。
- [ ] NavigationMesh 单帧寻路 spike ≤ 5ms。
- [ ] 现有 Sandbox6/7/8 行为表现一致。

---

## 6. Stage 4 — 完全 EC 化 + 数据驱动 + AI 调度器

> **目标**：对象组合自由化，能处理"会飞的弓手"、"骑马的步兵"这类对角组合；策划可独立调参。
>
> **触发条件**：对象类型 ≥ 5 且组合维度爆炸时；策划反复要求"加配置不用程序"时。多数项目永远到不了。

### 任务 4.1 Entity-Component 化对象层

- **动作**：把 BaseObject / SandboxObject / GameObject / RenderableObject / AgentObject / SoldierObject 拆成：
  - `Entity`：纯 id + 组件管理器，无业务逻辑。
  - 各 Component：RenderComponent、PhysicsComponent、AIControllerComponent、AnimControllerComponent、WeaponComponent、HealthComponent、InventoryComponent 等。
- **过渡**：保留 SoldierObject 作为"预制 Entity 工厂"，内部调 `entity->AddComponent(...)` 装配。Lua 侧 `agent:GetComponent<WeaponComponent>()` 取组件。
- **风险**：tolua 绑定大改；Lua API 全部要适配；建议先在新增对象类型上试点，老代码暂保留。

### 任务 4.2 ConfigLoader + 数据驱动配置表

- **位置**：`bin/res/scripts/data/`，下分 `weapons.lua` / `skills.lua` / `agents.lua` / `enemies.lua`。
- **C++ 端**：`ConfigManager` 单例，加载时把配置读进 C++ 缓存（避免 BT 节点每次回 Lua 查）。
- **热更**：`ConfigManager::Reload()` Lua 可调，开发时改配置立即生效。

### 任务 4.3 AI 调度器

- **位置**：`src/HelloOgre3D/sandbox/ai/common/AIScheduler.{h,cpp}`。
- **职责**：
  - 错峰分桶：N 个 agent 分 K 桶，每帧只 tick 一桶。`bucket = (frameIndex + agentId) % stride`。
  - 优先级：玩家可见的 agent 优先 tick，远处 agent 可降频。
  - 预算控制：单帧 AI 总耗时超过阈值时跳过低优先级 agent。
- **触发条件**：单场景 agent 数 ≥ 20 时启用。
- **接口**：`AIScheduler::Tick(float dt)`，由 GameManager 调用替代当前 for-loop。

### 任务 4.4 全局感知 / 团队协作

- **依赖**：Stage 3 的 Sensor / Memory + Stage 2 的 Team Blackboard。
- **动作**：实现"团队级感知共享"——一个 agent 看到敌人，团队所有成员的 Memory 都得到这个信息（可配延迟）。
- **节点**：新增 BT Action `CallForBackup` / Condition `TeamSpottedEnemy`。

### 任务 4.5 BehaviorTree 节点补全

- **现状**：BT 工厂只有 Sequence / Selector / Inverter / ForceSuccess / ForceFailure / Wait / LuaAction / LuaCondition。
- **补足**：`BehaviorLoop`（巡逻 / 重试）、`BehaviorRandom`（按权重随机分支）、`BehaviorSimpleParallel`（主任务 + 副任务）、节点级 `timeover`（超时保护）。
- **优先级**：先做 Loop 和 Random，SimpleParallel 等真有需求再做。
- **不做**：BTNodeCondition（用 Sequence(Condition, ...) 已覆盖）、BTNodeBranch（语义反直觉）。

### Stage 4 验收

- [ ] 至少一个新对象类型（Monster 或 NPC）完全用 EC 装配，无新继承类。
- [ ] 角色 / 武器 / 技能数值改 Lua 即生效，不重编。
- [ ] 50+ agent 单场景，单帧 AI 总耗 ≤ 5ms。
- [ ] 团队协作 sample 上线。

---

## 7. 不做什么（避免过度工程）

- ❌ 不做"完美 ECS"（archetype、SoA 内存布局、System Scheduler）。当前规模用不到，引入复杂度远超收益。
- ❌ 不做 GOAP / Utility AI / HTN。BT + DT 够用，多 paradigm 是负债。
- ❌ 不引入 LuaJIT 替换 Lua 5.1。性能瓶颈在 C↔Lua 边界次数，不是 Lua 执行速度。
- ❌ 不写网络同步层。当前单机沙盒，没有需求。
- ❌ 不重写动画 ASM。现有 [AgentAnimStateMachine](../src/HelloOgre3D/sandbox/objects/animation/AgentAnimStateMachine.h) 是工作的，只补接口化即可。
- ❌ 不在 Stage 1-2 引入新的设计模式（Observer / Strategy / Visitor 等）。先把现有抽象用好。

---

## 8. 性能目标与可观测性

### 基线（hello1.tracy，2026-05-15）

| 指标 | 当前值 |
|---|---|
| 渲染帧率 | ~52 FPS（1650 帧 / 32s） |
| 逻辑 tick 率 | 20 Hz |
| `callFunctionV1` 总耗 | 387 ms / 32s |
| `callFunctionV1` 单次 mean | 37.6 µs |
| `ClientManager::InputCapture` max | 16.76 ms（单帧 spike） |
| `NavigationMesh::FindPath` max | 44 ms |
| `BehaviorTreeDriver::Tick` 次数 | 4571（≈ 7 agent × 20Hz × 32s） |

### 阶段性目标

| 指标 | Stage 1 后 | Stage 2 后 | Stage 3 后 | Stage 4 后 |
|---|---|---|---|---|
| `callFunctionV1` 总耗 | ≤ 200 ms | ≤ 120 ms | ≤ 100 ms | ≤ 50 ms |
| `callFunctionV1` 单次 mean | ~37 µs | ≤ 25 µs | ≤ 20 µs | ≤ 15 µs |
| InputCapture max | < 5 ms | < 2 ms | — | — |
| FindPath max | — | — | ≤ 5 ms | ≤ 3 ms |
| 单场景 agent 上限 | 10 | 20 | 30 | 50+ |

### 持续可观测性

- 每个 Stage 完成后跑一次 tracy，结果归档到 `docs/perf/stageN.tracy`。
- 新增 zone 埋点：Sensor::Scan、AIScheduler::Tick、SkillComponent::Cast、AICommand::Apply。
- 关键路径加 `H3D_PROFILE_PLOT` 追踪长期趋势（agent 数、AI 总耗 / 帧）。

---

## 9. 风险与回退

| 风险 | 缓解 |
|---|---|
| tolua 绑定改动量大 | 每个 Stage 内的 tolua 重新生成在最后一步统一做；分支 PR 隔离 |
| AI 反应延迟（10Hz tick）感知差 | Stage 1 把频率改成可配置，sandbox 测过再固化默认值 |
| Lua ref 缓存导致 GC 行为变 | 加 `WeakRef` 支持；BindToScript 失败时 ref 必须释放，测覆盖 |
| 三 driver 收敛破坏现有 sample | Sandbox6/7/8 作为回归测试基线，每次 merge 前手测一次 |
| EC 化导致 Lua API 大变 | Stage 4 在新对象类型上试点，老对象保留旧 API 双轨运行 |

---

## 10. 行动清单：本周可做

按优先级排（每项独立 PR）：

1. **修 Viewport 每帧重设** —— 30 分钟，立省 23ms。
2. **components 目录归并到 `sandbox/components/<域>/` + IComponent 体系对齐** —— 半天到 1 天，把组件基础设施一次性理顺到 EC 路径上。
3. **抽 AIController** —— 1-2 天，最大架构收益。
4. **AI 独立低频 tick** —— 半天，最大性能收益。
5. **Lua 函数 ref 缓存（核心 3 处）** —— 1 天，单次 Lua 调用降 30%+。

这五件做完，Stage 1 + Stage 2 的第一个性能任务就落地了，可以再跑一次 tracy 验证。

---

> **维护**：本文档随每个 Stage 完成更新。已完成的任务勾掉，未触发的 Stage 推迟。新发现的痛点先记到"现状画像"，再决定是否进入下个 Stage。
