# SoldierObject 继承链重构方案

> **定位**：本文档专注于 **SoldierObject 这条继承链** 的重构，从当前"扛 9+ 职责的肥继承类"演化为"组件装配的薄壳实体"。是 [RefactoringPlan.md](RefactoringPlan.md) 的子专题展开，跟 [IterationPlan.md](IterationPlan.md) 的 Stage 路线对齐。
>
> **核心原则**：**目标可激进，动作必保守**。每一步只在**保留旧 API**的前提下叠加新结构，确认 Sandbox6/7/8 行为稳定后才删字段。绝不允许"先删字段再补组件"。
>
> **使用规则**：每个任务（T-00 ~ T-17）独立可执行、独立 PR、独立可回滚。完成后回填 §8 进度表。新发现问题追加 T-18+，不要散落 commit。

---

## 1. 现状画像

### 1.1 继承链

```
SandboxObject                                       (~50 行, 仅 Event() 接口)
  ↓
BaseObject                                          (id/teamId/objType + GameObject* 组件容器)
  ↓
VehicleObject                                       (mass/height/radius/speed/health
                                                     + m_locomotion: AgentLocomotion*
                                                     + m_physicsComp: PhysicsComponent*
                                                     + OpenSteer Force*)
  ↓
AgentObject : LuaEnvObject (多继承)                 (m_pAgentBody: RenderableObject*
                                                     + agentType + 健康事件)
  ↓
SoldierObject                                        ~716 行
```

### 1.2 SoldierObject 持有 / 继承的"准组件"

| 入口 | 字段 | 类型 | 持有方式 |
|---|---|---|---|
| 直接持有 | `m_pWeapon` | `RenderableObject*` | 不是组件，是个子渲染对象 |
| 直接持有 | `m_inputInfo` | `IPlayerInput*` | new / delete |
| 直接持有 | `m_animController` | `SoldierAnimController*` | 类型硬绑 |
| 直接持有 | `m_driver` | `IDecisionDriver*` | ✓ 已抽象 |
| 直接持有 | `m_enemy / m_movePos / m_hasMovePos` | 散字段 | 应属 AIController/Blackboard |
| 继承自 AgentObject | `m_pAgentBody` | `RenderableObject*` | 走继承字段而不挂组件 |
| 继承自 VehicleObject | `m_locomotion / m_physicsComp` | 组件指针 | ✓ 已组件化 |
| 继承自 BaseObject | `m_pGameObjet` | `GameObject*` | 组件容器（拼写错） |

### 1.3 数值"多份"的字段

| 概念 | 出处 1 | 出处 2 | 出处 3 |
|---|---|---|---|
| mass | `VehicleObject::m_mass` | `AgentLocomotion`（转发回 VehicleObject）| `PhysicsComponent` → `btRigidBody.mass` |
| health | `SoldierObject::m_maxHealth` | `VehicleObject::m_health` | 未来 `AgentAttrib.health` |
| 位置 | `RenderableObject::SetPosition` | `VehicleObject::setPosition` | `PhysicsComponent::SetPosition` |

### 1.4 已存在但容易遗漏的设施（重要！）

**这一节专门记录"实际已实施但与新人直觉相反"的现状，避免后续重构再忽视。**

#### 1.4.1 AIScheduler 已存在

[ObjectManager.cpp:103](../src/HelloOgre3D/sandbox/systems/manager/ObjectManager.cpp#L103) 构造时 `m_aiScheduler = new AIScheduler()`，[L132](../src/HelloOgre3D/sandbox/systems/manager/ObjectManager.cpp#L132) `BeginFrame` 已经在驱动 AI 错峰调度。位置在 [sandbox/ai/common/AIScheduler.{h,cpp}](../src/HelloOgre3D/sandbox/ai/common/AIScheduler.h)。

**含义**：AI 低频调度的基础设施**已经有了**。任何 "GameManager 增加 AI 累加器" 的方案都是重复造轮子。正确做法是 **ObjectManager 调度从 `dynamic_cast<SoldierObject*>` 改为查 AIController 接口，复用现有 AIScheduler**。

#### 1.4.2 UseDecisionTreeDriver / UseBehaviorTreeDriver 是有意为之的 Lua 入口

[SoldierObject.cpp:659](../src/HelloOgre3D/sandbox/objects/SoldierObject.cpp#L659) `UseDecisionTreeDriver` 和 [L679](../src/HelloOgre3D/sandbox/objects/SoldierObject.cpp#L679) `UseBehaviorTreeDriver` 还在用。它们是为了**规避 tolua 直接 new C++ Driver 的所有权问题**——Lua 不能直接 `DecisionTreeDriver:new()`，因为 driver 持有节点和 blackboard 的所有权，跨语言所有权很难处理。

**含义**：第一步抽 AIController 时**不能删这两个接口**。"让 Lua 改用 SetDriver(...) 注入"是要先解决 tolua 所有权坑之后才能做的，属于中期工作。

#### 1.4.3 AgentLocomotion::m_owner 是反向转发结构，不是字段冗余

grep [AgentLocomotion.cpp](../src/HelloOgre3D/sandbox/objects/components/AgentLocomotion.cpp) 几乎每个 setter/getter 都是：
```cpp
void AgentLocomotion::SetMass(Ogre::Real mass) { m_owner->SetMass(mass); }
Ogre::Real AgentLocomotion::GetMass() const { return m_owner->GetMass(); }
Ogre::Vector3 AgentLocomotion::GetPosition() const { return m_owner->GetPosition(); }
```
组件持有 VehicleObject*，把所有调用反传回去。

**含义**：清理 `AgentLocomotion::m_owner` ≠ 字段重命名。它是**解耦 VehicleObject 的前置工程**，必须等 HealthComponent / AgentAttrib 真正承接数据后才能动。先动它会让 AgentLocomotion 一大半实现失效。`PhysicsComponent::m_owner` 是纯字段冗余级别，可以单独清理。

#### 1.4.4 RenderableObject 不是单纯 wrapper，是 ASM/动画 Lua 入口

[RenderableObject.h](../src/HelloOgre3D/sandbox/objects/RenderableObject.h) 持有 `m_pAnimateStateMachine` 和 `m_animations` map，对外暴露 `GetAnimation / GetObjectASM / InitAsmWithOwner` 等。**BlockObject 也用它**。

**含义**：直接删除 RenderableObject 会同时推翻动画 + 渲染两条路径。正确做法是**先让 RenderComponent : IComponent，再让 RenderableObject 改 facade 化保留 Lua 调用面**，删除放最后阶段。

---

## 2. 问题清单（按严重度分组）

### 🔴 P0 根源性

#### P-01：VehicleObject 名字完全误导
里面是 `ForceToAvoidAgents`、`ForceToFollowPath`、`ForceToWander`——是 **agent + steering**，不是车辆。`SoldierObject is-a VehicleObject` 在语义上荒谬。

#### P-02：位置 / 朝向数据有 3 个真源
`RenderableObject::SetPosition`（SceneNode）、`PhysicsComponent::SetPosition`（btRigidBody）、`VehicleObject::setPosition`（透传）。没人能一眼说出哪个权威。

#### P-03：同一数值在多处各持一份 + 循环转发
`VehicleObject` 有 `m_mass` 字段，`AgentLocomotion::SetMass` 又转回 `m_owner->SetMass(mass)`。**改一处不知道传到哪里**。

### 🟠 P1 架构性

#### P-04：`m_pAgentBody` 走继承字段而不挂组件
AgentObject 持有 `RenderableObject* m_pAgentBody`，但 BaseObject 已有组件容器。

#### P-05：`m_pWeapon` 是 RenderableObject 不是 WeaponComponent
武器属性（弹药、射程、伤害、CD）散在 SoldierObject。换武器只能换 mesh。

#### P-06：AgentObject 多继承 LuaEnvObject 把"能挂 Lua"绑死继承链
"能不能挂脚本"应是组件能力，不是继承属性。

#### P-07：m_driver 已抽象但 AI 数据散在 SoldierObject
`m_driver` 是 `IDecisionDriver*`，但 `m_enemy / m_hasMovePos / m_movePos` 还在 SoldierObject 字段里。

### 🟡 P2 质量性

#### P-08：方法签名大小写混乱
`Update` (U) / `SetPosition` (S) / `setPosition` (s) / `initWeapon` (i) / `changeStanceType` (c) 混杂。

#### P-09：构造里硬编码 FSM 驱动
[SoldierObject.cpp 构造](../src/HelloOgre3D/sandbox/objects/SoldierObject.cpp) 里 `if (GetUseCppFSM()) { new AgentStateController; m_driver = fsm; }`。

#### P-10：`m_pGameObjet` 拼写错误
"Objet" 不是 "Object"，多年遗留。

#### P-11：m_animController 直接是 SoldierAnimController\* 类型
跟接口化方向反。

#### P-12：多个 event token 字段管理散乱
ASM_STATE_CHANGE / ASM_NOTIFY / HEALTH_CHANGE 的 token 分散在 SoldierObject 和 AgentObject。

#### P-13：`m_aiTickInUpdateEnabled` 临时标志
GameManager 调度后这个 flag 没意义。

---

## 3. 目标形态（终态）

### 3.1 SoldierObject 类定义

```cpp
// ~150 行，本质是 EntityFactory + 命令翻译器
class SoldierObject : public BaseObject {
public:
    SoldierObject();
    virtual ~SoldierObject();

    virtual void Init() override;
    virtual void Update(int deltaMs) override;     // 仅协调组件更新顺序

    // 翻译抽象 AICommand 为具体动作
    void ApplyCommand(const AICommand& cmd);

    // 高级动画意图保留
    void EnterIdleAnim();
    void EnterMoveAnim();
    void EnterShootAnim();
};
```

### 3.2 组件挂载结构

```
SoldierObject (BaseObject)
├── RenderComponent           ← 替代 m_pAgentBody（P-04）
├── PhysicsComponent          ← 位置真源（P-02，含条件）
├── AnimComponent             ← 持 IAnimController*（P-11）
├── AgentAttrib               ← health / maxHealth / stance（P-03，teamId 留 BaseObject）
├── WeaponComponent           ← m_pWeapon + ammo + 射击逻辑（P-05）
├── InputComponent (可选)     ← m_inputInfo
├── AIController              ← m_driver + m_blackboard + enemy/movePos（P-07）
├── SensorComponent (S3)      ← 敌人扫描
├── MemoryComponent (S3)      ← 记忆
├── SkillComponent (S3)       ← 技能
└── LuaScriptComponent (S4)   ← 替代 LuaEnvObject 多继承（P-06）
```

### 3.3 继承链终态

```
BaseObject
  ↓
AgentObject (无 LuaEnvObject 多继承；含 agentType + 死亡处理)
  ↓
SoldierObject (薄壳，仅 ApplyCommand 翻译)
```

`VehicleObject` 消失或改名 `SteeringAgentObject` 跟 `AgentObject` 平级（取决于 Stage 4 决定）。

---

## 4. 分阶段重构任务（17 项，已按"保守原则"重排）

> **每个 Task 通用约束**：
> - 必须保留 SoldierObject 旧 Lua API 为 thin forwarder，不破坏 Sandbox6/7/8 调用面。
> - Sandbox6/7/8 视觉与行为表现作为回归基线，每个 PR 完成都要手测一遍。
> - 删字段 / 删旧接口 必须在"组件已就绪 + 双轨运行验证 + Lua 调用面迁完"之后。
> - 每个任务都标注 **[Stage X]** 对应 [IterationPlan.md](IterationPlan.md)。

### Stage 1：地基（最保守，不动业务行为）

#### T-00 组件目录统一 + 领域骨架接入 IComponent
- **[Stage 1]** [详见 [RefactoringPlan.md S-01](RefactoringPlan.md)]
- **现状**：两个 components 目录并存，领域骨架（AgentAttrib / AgentBody / WeaponComponent）未接 IComponent。
- **目标**：
  - 统一到 `sandbox/components/<域>/`。
  - **领域骨架接入 IComponent**：AgentAttrib / AgentBody / WeaponComponent。
  - **已接入 IComponent 的组件（PhysicsComponent / AgentLocomotion）保持原状**，只迁文件位置。
  - **RenderComponent 不在这步接 IComponent**，留给 T-06 一并 facade 化处理。
- **关键约束**：**只动文件位置和接口对齐，不动任何业务逻辑**。SoldierObject 引用组件的代码原封不动（include 路径改一下而已）。
- **工时**：半天到 1 天。

#### T-01a 新建 AIController 包住现有 driver / AI 字段
- **[Stage 1]** [P-07]
- **现状**：`m_driver / m_enemy / m_enemyId / m_hasMovePos / m_movePos` 在 SoldierObject 字段；`UseDecisionTreeDriver / UseBehaviorTreeDriver` 是 Lua 入口；**Blackboard 分别由 DecisionTreeDriver / BehaviorTreeDriver / AgentStateController 各自持有**（不是 AIController）。
- **目标**：新建 `sandbox/components/ai/AIController.{h,cpp}`，**继承 `IComponent`**（让 T-01b 的 `GetComponent<AIController>()` 查询路径成立）。
- **关键约束**（**保守原则核心**）：
  - **SoldierObject 全部旧 API 保留为 thin forwarder**：`GetEnemy() / HasMovePosition() / SetMovePosition() / ClearMovePosition() / IsTargetReached() / UseDecisionTreeDriver() / UseBehaviorTreeDriver() / GetDecisionTreeDriver() / GetBehaviorTreeDriver() / GetFsmController() / RequestState()` 等全部保留，内部转发到 `m_ai->...`。
  - **不动 Lua 调用面**：Sandbox6/7/8 的 Lua 脚本一行不改照常运行。
  - **不删 UseDecisionTreeDriver / UseBehaviorTreeDriver**（这是 tolua 所有权背景下有意为之的入口，见 §1.4.2）。
  - **构造里仍创建默认 driver**（不要在这步改成 Lua 注入）。
  - **AIController 不聚合 Blackboard 字段**：driver 仍各自持有自己的 Blackboard。`AIController::GetBlackboard()` 暂时**转发到当前 active driver 的 blackboard**。一上来就改 driver 跟 bb 的关系会破坏 Lua action 取 bb 的路径，留作 T-01a 双轨收尾（见下方"已知后续动作"）。
- **落地动作**：
  1. 新建 `AIController : public IComponent`，字段含 `IDecisionDriver* m_driver`、`AgentObject* m_enemy`、`int m_enemyId`、`Vector3 m_movePos`、`bool m_hasMovePos`。**不含 m_blackboard**。
  2. `AIController::GetBlackboard()` 实现为 `return m_driver ? m_driver->GetBlackboard() : nullptr;`。
  3. SoldierObject 构造里 `AddComponent("ai", new AIController(this))`，并把原本对 FSM driver 的构造挪进 AIController（保持 `if (GetUseCppFSM())` 逻辑）。
  4. SoldierObject 删上述字段，所有 getter/setter 改为 forwarder。
  5. tolua 暴露 AIController（让 Lua 能拿到 blackboard），但**不强制**通过它访问（旧路径仍可用）。
- **已知后续动作（T-01a 双轨收尾）**：
  - 当 T-01a / T-01b 稳定运行 ≥ 1 周、Sandbox6/7/8 无回归后，**让 AIController 真正拥有 Blackboard 字段**，drivers 改为读 AIController 注入的 bb 而不再各自 own。
  - 这一步不另开任务编号，作为 T-01a 的延伸 PR 执行；执行前在 §8 进度表 T-01a 备注一行注明 "bb ownership migrated"。
  - 这一步是 Stage 3 SensorComponent / MemoryComponent 共享 bb 的前置基础设施。
- **风险**：tolua 绑定改动；Lua 拿到的 enemy / blackboard 指针要保持有效；driver 切换时 GetBlackboard 返回值会跟着切（这是当前事实，T-01a 不引入新行为）。
- **工时**：1-2 天（不含后续动作）。
- **验收**：Sandbox6/7/8 行为表现与改动前完全一致。

#### T-01b ObjectManager 调度面向 AIController（复用 AIScheduler）
- **[Stage 1]**
- **现状**：[ObjectManager.cpp](../src/HelloOgre3D/sandbox/systems/manager/ObjectManager.cpp) 已有 AIScheduler，但当前调度路径靠 `dynamic_cast<SoldierObject*>` 判断"是否有 AI"。
- **目标**：把判断条件改为"是否有 AIController 组件"，让 AIScheduler 驱动的对象不再限于 SoldierObject。
- **关键约束**：**不引入新调度，不动 AIScheduler 实现**，只改 ObjectManager 内查询逻辑。
- **落地动作**：
  1. AIController 提供 `GetAgentId() / TickAI(int dt)` 接口。
  2. ObjectManager 遍历改为：对每个 BaseObject 取 `GetComponent<AIController>()`（或类似），有就纳入 AIScheduler 调度。
  3. SoldierObject 的 `TickAi(...)` 和 `SetAiTickInUpdateEnabled(...)` 逻辑保留 forwarder，内部走 AIController。
- **风险**：组件容器查询性能（频繁 GetComponent 调用）。一帧只查一次缓存到本地变量。
- **工时**：半天。
- **验收**：tracy 跑出来 AIController 调度行为跟当前 SoldierObject 调度一致；Sandbox6/7/8 行为不变。

#### T-02 PhysicsComponent owner 字段清理（仅这一个）
- **[Stage 1]** [P-04 衍生] [对应 [RefactoringPlan.md S-16](RefactoringPlan.md)]
- **现状**：PhysicsComponent 内部 `BaseObject* m_owner` 跟基类 `IComponent::m_gameobj` 重复。
- **目标**：删 m_owner，统一用基类字段。
- **关键约束**：**AgentLocomotion::m_owner 不在这一步动**（它是反向转发结构，见 §1.4.3）。
- **落地动作**：grep PhysicsComponent::m_owner 引用点 → 改 `m_gameobj->getOwner()`。
- **风险**：低。
- **工时**：1 小时。

#### Stage 1 验收

- [ ] T-00 ~ T-02 全部 merge，SoldierObject 旧 Lua API 一行不改。
- [ ] Sandbox6/7/8 行为视觉一致。
- [ ] tracy 复测 AIScheduler 调度行为正常。
- [ ] SoldierObject 行数从 ~716 减少到 ~580（AI 字段迁出）。

### Stage 2：业务数据组件化（开始动 SoldierObject 业务字段）

#### T-03 AgentAttrib：承接 health / maxHealth / stance
- **[Stage 2]** [P-03 部分]
- **现状**：health/maxHealth 在 VehicleObject + SoldierObject 两份；stance 在 SoldierObject。
- **目标**：AgentAttrib（IComponent 已在 T-00 接入）承接 health / maxHealth / stance 等**通用 agent 属性**。
- **关键约束**：
  - **teamId 留在 BaseObject** —— 那是所有对象通用 API（BlockObject、子弹也需要），等后续真正做 faction component 再迁。
  - **ammo / maxAmmo 不在这步**：弹药跟武器绑（§6.5 决策），由 T-04 WeaponComponent 承接，不进 AgentAttrib。
  - **SoldierObject 旧 getter/setter 全部保留为 forwarder**：`GetMaxHealth / SetMaxHealth / changeStanceType / getStanceType` 等不动。
  - **VehicleObject::m_health 暂保留**（先双轨：AgentAttrib 优先，VehicleObject 后写）。
- **落地动作**：
  1. AgentAttrib 加字段 health / maxHealth / stance + 对应接口（IComponent 接入 T-00 已完成）。
  2. SoldierObject 构造里 `AddComponent("attrib", new AgentAttrib(...))`。
  3. SoldierObject 各 getter/setter 改为内部走 `GetComponent<AgentAttrib>()`。
  4. SoldierObject 的 `m_maxHealth / m_stanceType / m_pendingStanceType` 字段**暂时保留**作为冗余备份（验证组件路径稳定后由 T-05 删）。
- **风险**：HEALTH_CHANGE 事件触发时机要保持一致。
- **工时**：1-2 天。

#### T-04 WeaponComponent：承接 m_pWeapon / ammo / shoot 逻辑
- **[Stage 2]** [P-05] [§6.5 决策：ammo 归武器]
- **现状**：m_pWeapon 是 RenderableObject；ammo / maxAmmo / ShootBullet / DoShootBullet / SyncWeaponToHandBone / HasAmmo / ConsumeAmmo / RestoreAmmo 全在 SoldierObject。
- **目标**：WeaponComponent（IComponent 已在 T-00 接入）承接**武器挂载 + 射击 + 弹药管理**——弹药跟武器绑，不跟兵绑（§6.5 决策）。
- **关键约束**：
  - **SoldierObject 全部相关旧 API 保留为 forwarder**：`ShootBullet / DoShootBullet / getWeapon / initWeapon / GetAmmo / SetAmmo / GetMaxAmmo / SetMaxAmmo / HasAmmo / ConsumeAmmo / RestoreAmmo` 一个都不删，内部转发到 `GetComponent<WeaponComponent>()`。
  - **武器渲染挂载先用 RenderableObject 子对象**（不在这步改 RenderComponent，那是 T-06 工作）。
- **落地动作**：
  1. WeaponComponent 加字段：weaponBody / ammo / maxAmmo / handOffsetPos / handOffsetOrientation（IComponent 接入 T-00 已完成）。
  2. 接口：`Shoot() / CanShoot() / Reload() / SyncToHandBone(...) / Init(meshFile) / GetAmmo / SetAmmo / ConsumeAmmo(n) / RestoreAmmo / HasAmmo`。
  3. SoldierObject 各 weapon/ammo 方法改为内部走 `GetComponent<WeaponComponent>()`。
  4. SoldierObject 的 `m_pWeapon / m_ammo / m_maxAmmo / m_weaponHandOffsetPos / m_weaponHandOffsetOrientation` 字段**暂时保留**作为冗余备份（验证组件路径稳定后由 T-05 删）。
- **风险**：weapon 跟 anim 的事件交互（射击动画触发 ShootBullet）要保持；ammo 旧字段跟组件字段同步阶段要确保两边一致。
- **工时**：1-2 天。

#### T-05 删除 SoldierObject 中已组件化字段的冗余备份
- **[Stage 2]** [对应 T-03/T-04 收尾]
- **现状**：T-03/T-04 暂时保留了字段冗余备份。
- **目标**：删除 SoldierObject 的 m_maxHealth / m_ammo / m_maxAmmo / m_stanceType / m_pendingStanceType / m_pWeapon 等字段。
- **关键约束**：原计划要求 T-03/T-04 稳定运行至少 1 周；当前项目仍处于沙盒重构期，本轮改为以 Sandbox1-8 全量 smoke 通过作为放行条件。
- **落地动作**：删字段，所有 getter/setter 仍保留为 forwarder（只是不再有本地缓存）。
- **风险**：低（前置任务已经验证）。
- **工时**：半天。

#### T-06 RenderComponent : IComponent + RenderableObject facade 化
- **[Stage 2]** [P-04] [对应 [RefactoringPlan.md S-09](RefactoringPlan.md)]
- **现状**：RenderComponent 不继承 IComponent，跟 RenderableObject 各做一份 SceneNode 封装。
- **目标**：
  - RenderComponent 改为 `: public IComponent`，承接渲染挂载逻辑（位置 / 朝向 / scenenode / 材质）。
  - **RenderableObject 不删除**，改为 facade：内部委托给 RenderComponent，保留 ASM / 动画相关接口（`InitAsmWithOwner / GetAnimation / GetObjectASM`）。
- **关键约束**：
  - **RenderableObject 不删除**（见 §1.4.4，BlockObject 也用，删除是终态工作）。
  - **AgentObject::m_pAgentBody 保留**，类型保持 `RenderableObject*`，调用面不变。
- **落地动作**：
  1. RenderComponent : IComponent。
  2. RenderableObject 持有 RenderComponent 实例（或 RenderComponent 是 RenderableObject 的内部实现细节）。
  3. RenderableObject 对外 API 不变。
- **风险**：ASM 事件订阅时机别错位。
- **工时**：1 天。

#### Stage 2 验收

- [ ] T-03 ~ T-06 全部 merge。
- [ ] SoldierObject 行数从 ~580 降到 ~400。
- [ ] AgentAttrib / WeaponComponent / RenderComponent 三个组件都挂在 SoldierObject 上正常工作。
- [ ] Sandbox6/7/8 + BlockObject 表现一致。

### Stage 3：AI 接口面演化 + 动画组件化

#### T-07 driver 注入方式重构（解决 tolua 所有权坑）
- **[Stage 3]** [P-09]
- **现状**：`UseDecisionTreeDriver / UseBehaviorTreeDriver` 是 SoldierObject 内构造 Driver 的工厂入口（避开 tolua 所有权）。
- **目标**：让 Lua 能通过 AIController 直接安装外部构造的 driver。
- **前置**：解决 tolua 对 driver 所有权的处理（候选方案：让 AIController 暴露 `SetDriverByType(string)` 工厂方法，Lua 传字符串而非指针）。
- **关键约束**：
  - **UseDecisionTreeDriver / UseBehaviorTreeDriver 暂保留为 forwarder**（内部走 AIController::SetDriverByType）。
  - Stage 4 终态再删 forwarder。
- **落地动作**：
  1. AIController 增 `SetDriverByType(const char* type)`，type ∈ {"fsm", "dt", "bt"}。
  2. SoldierObject::UseDecisionTreeDriver 改为 forwarder 到 `m_ai->SetDriverByType("dt")`。
  3. Lua 侧可以选择继续用旧 API，也可以用新 API。
- **风险**：driver 切换时旧节点 / blackboard 的清理时机。
- **工时**：1-2 天。

#### T-08 AnimComponent + IAnimController 接口化
- **[Stage 3]** [P-11] [对应 [RefactoringPlan.md S-12](RefactoringPlan.md)]
- **现状**：SoldierObject 直接持 `SoldierAnimController*`，类型硬绑。
- **目标**：抽 `IAnimController` 接口；SoldierAnimController 实现；用 `AnimComponent` 挂组件容器。
- **关键约束**：SoldierObject::GetAnimController / HasNextAnim / IsAnimReadyForMove 等保留为 forwarder。
- **落地动作**：
  1. 新增 `sandbox/components/anim/IAnimController.h` + `IAnimProfile.h`。
  2. SoldierAnimController : IAnimController；SoldierAnimProfile : IAnimProfile。
  3. AnimComponent 持 `IAnimController*` 挂到组件容器。
  4. SoldierObject 删 `m_animController`，所有调用改 forwarder。
- **风险**：动画事件订阅时机要保持。
- **工时**：2 天。

#### T-09 AgentLocomotion::m_owner 泛化为 AgentObject\*
- **[Stage 3]** [P-03] [对应 [RefactoringPlan.md S-17](RefactoringPlan.md)]
- **现状**：`AgentLocomotion::m_owner` 类型 `VehicleObject*`，反向转发结构。
- **目标**：类型泛化为 `AgentObject*`（不解构 VehicleObject，那是 Stage 4），让 SoldierObject 也能直接用。
- **关键约束**：
  - **暂不消除反向转发**（VehicleObject 的字段还在）。
  - 只改类型 + AgentObject 提供必要的 SetMass / GetMass 等 forwarder（实际仍走 VehicleObject）。
- **落地动作**：类型替换 + cpp 引用点改。
- **风险**：低，类型替换。
- **工时**：半天。

#### T-10 事件 token 挂到组件 onAttach
- **[Stage 3]** [P-12]
- **现状**：`m_asmStateChangeEventToken / m_asmNotifyEventToken` 在 SoldierObject，`m_healthChangeEventToken` 在 AgentObject。
- **目标**：事件订阅在各组件的 onAttach 完成、onDetach 退订，token 由组件自己管。
- **关键约束**：订阅时机必须保证依赖组件已就绪（用 onAttach 的执行顺序保证或显式注入）。
- **落地动作**：
  1. AnimComponent::onAttach 订阅 ASM_STATE_CHANGE / ASM_NOTIFY。
  2. AgentAttrib::onAttach 订阅 HEALTH_CHANGE。
  3. SoldierObject / AgentObject 删 token 字段和 CreateEventDispatcher 里订阅代码。
- **风险**：onAttach 顺序错位导致漏订阅。
- **工时**：半天（跟 T-08 顺手）。

#### Stage 3 验收

- [ ] T-07 ~ T-10 全部 merge。
- [ ] SoldierObject 行数从 ~400 降到 ~250。
- [ ] Lua 侧既能用 `UseDecisionTreeDriver()` 老 API，也能用 `GetAI():SetDriverByType("dt")` 新 API。
- [ ] AnimController 接口化，理论上能挂在 Monster/NPC 上。

### Stage 4：终态收尾（最大改动，最后做）

#### T-11 VehicleObject 解构
- **[Stage 4]** [P-01]
- **目标**：steering 能力下沉到 AgentLocomotion 组件，删除 VehicleObject 类；AgentObject 直接继承 BaseObject。
- **关键约束**：分多个 PR 推进，先消除反向转发，再删字段，最后删类。
- **工时**：2-3 天。

#### T-12 LuaEnvObject 改 LuaScriptComponent
- **[Stage 4]** [P-06]
- **目标**：拆出 AgentObject 的多继承，做成组件。
- **工时**：1-2 天。

#### T-13 BaseObject ↔ GameObject 合并 + 拼写纠正
- **[Stage 4]** [P-10] [对应 [RefactoringPlan.md S-10](RefactoringPlan.md)]
- **目标**：BaseObject 直接持组件容器，删除 GameObject 类，顺手改对 `m_pGameObjet` 拼写。
- **工时**：2-3 天。

#### T-14 位置真源统一（含条件规则）
- **[Stage 4]** [P-02]
- **目标**：明确"**有 PhysicsComponent 时 physics 权威，无 physics 时 RenderComponent 自身权威**"，写进 AGENTS.md。
- **关键约束**：BlockObject 部分实例没刚体，UI/特效/调试对象也不该挂假刚体凑数。
- **落地动作**：
  1. 写 AGENTS.md "位置真源" 规则。
  2. RenderComponent::Update 检测：有 PhysicsComponent 时从它 sync，无则保持自身 transform。
  3. 直接改 RenderComponent 位置的代码（root motion）走特殊接口 `SetVisualOffset(...)`。
- **风险**：root motion / 物理 push 同时发生的情况测试。
- **工时**：1 天。

#### T-15 删除 RenderableObject
- **[Stage 4]** [P-04 收尾]
- **目标**：RenderableObject facade 删除，全部走 RenderComponent + AnimComponent。
- **关键约束**：BlockObject 的渲染路径要先改完。
- **工时**：1-2 天。

#### T-16 删除 driver Lua forwarder
- **[Stage 4]** [P-09 收尾]
- **目标**：删除 SoldierObject::UseDecisionTreeDriver / UseBehaviorTreeDriver，全部走 AIController::SetDriverByType。
- **关键约束**：Lua 侧所有调用点先改完。
- **工时**：半天。

#### T-17 SoldierObject 改 SoldierFactory（终态）
- **[Stage 4]** [P-04/P-05/P-07 收尾]
- **目标**：SoldierObject 类缩成"动作命令翻译器"或删除，实体创建走 SoldierFactory。
- **工时**：3-5 天。

#### Stage 4 验收

- [ ] SoldierObject 行数从 ~250 降到 0 或 ~50（薄壳）。
- [ ] VehicleObject 删除或重命名。
- [ ] 至少一个新对象类型（如 Monster）能通过 SoldierFactory 模式装配。

---

## 5. 行数 / 复杂度阶段目标

| 阶段 | SoldierObject 行数 | VehicleObject 行数 | 备注 |
|---|---|---|---|
| 基线 | 716 | ~250 | 当前 |
| Stage 1 后（T-00~T-02）| ~580 | ~250 | AI 抽走，旧 API 全 forwarder |
| Stage 2 后（T-03~T-06）| ~400 | ~200 | 属性/武器组件化 + RenderComponent 接口化 |
| Stage 3 后（T-07~T-10）| ~250 | ~150 | driver/anim 接口化 |
| Stage 4 后（T-11~T-17）| 0 或 ~50 | 删除 | Factory 取代 |

---

## 6. 跨任务的决策点

### 6.1 位置真源谁权威（含条件规则）

**决策**：**有 PhysicsComponent 时它权威；无 physics 时 RenderComponent 自身 transform 是权威**。

理由：BlockObject 部分实例没刚体，未来 UI/特效/调试 helper 对象也不该挂假刚体。规则写进 AGENTS.md。

**决策时机**：T-14 之前确认。

### 6.2 VehicleObject 留与不留

候选：
- **A**：steering 下沉到 AgentLocomotion，删除 VehicleObject（T-11 推荐路径）
- **B**：改名 `SteeringAgentObject` 跟 AgentObject 平级

**决策时机**：T-11 之前。倾向 A。

### 6.3 SoldierObject 最终留不留

候选：
- **A**：删除，全走 SoldierFactory + BaseObject + 组件
- **B**：保留为薄壳含 ApplyCommand 翻译

**决策时机**：T-17 之前。倾向 B（ApplyCommand 是个有意义的接口面，让 AICommand 跟具体角色解耦）。

### 6.4 teamId 是否迁组件

**决策**：**不迁，留在 BaseObject**。teamId 是所有对象通用属性（BlockObject、子弹也需要）。等真正需要 faction component（多阵营关系、外交、敌我图）再单独做。

### 6.5 ammo 归 AgentAttrib 还是 WeaponComponent

**决策**：**WeaponComponent**。弹药跟武器绑，不跟兵绑——换枪应该换弹药容量。

---

## 7. 风险与回退

### 7.1 保守原则（核心心法）

> **目标可激进，动作必保守**。

具体执行约束：

1. **加组件 + 旧接口 forward** 是 Stage 1-2 的默认形态，不允许"先删字段后补组件"。
2. **删字段 / 删旧接口** 必须在以下条件全部满足后执行：
   - 新组件已就绪并 merge 至少 1 周
   - 双轨运行验证通过（Sandbox6/7/8 行为一致）
   - 所有 Lua 调用点已迁移或已验证 forwarder 正确
3. **每个 PR** 必须能独立回滚，不依赖前置 PR 的隐式状态。

### 7.2 已知教训

| 教训 | 如何避免 |
|---|---|
| 凭印象推断现状写计划 | 改文档前必须 grep 验证关键论点（如 AIScheduler、UseXxxDriver、AgentLocomotion::m_owner）|
| 把"字段冗余"和"反向转发结构"混为一谈 | 看 cpp 实现，不只看 .h 字段声明 |
| 激进动作和保守动作放进同一个 Task | 一个 Task 一个动作；删字段 ≠ 加组件，分两个 PR |
| 忽视 "通用 API" 跟 "Soldier 特化" 的边界 | 像 teamId 这种全对象通用的字段，不要塞进 Soldier-only 组件 |

### 7.3 通用风险表

| 风险 | 缓解 |
|---|---|
| tolua 绑定改动量大 | 每个 Task 内 tolua 重新生成在最后一步统一做；分支 PR 隔离 |
| Lua 调用点广泛 | 关键改动加 thin forwarder 保持旧 API 双轨运行 |
| 组件依赖顺序问题 | onAttach 时显式取依赖组件，依赖不在时 assert + 错误日志 |
| 位置同步时机 bug | T-14 加 H3D_PROFILE 埋点对比 render/physics 位置差 |
| Sandbox 回归 | 每个 Task 完成跑 Sandbox6/7/8 一遍，行为表现作为回归基线 |
| Driver 切换时的所有权 | T-07 用 SetDriverByType(string) 而非传 driver 指针，避开 tolua 所有权 |

---

## 8. 进度追踪

| # | 任务 | Stage | 状态 | 完成日期 | Commit |
|---|---|---|---|---|---|
| T-00 | 组件目录统一 + IComponent 契约 | 1 | ☑ | 2026-05-26 | |
| T-01a | 新建 AIController 包住 driver / AI 字段 | 1 | ☑ | 2026-05-26 | |
| T-01b | ObjectManager 调度面向 AIController | 1 | ☑ | 2026-05-26 | |
| T-02 | PhysicsComponent owner 字段清理 | 1 | ☑ | 2026-05-26 | |
| T-03 | HealthComponent/AgentAttrib 承接属性 | 2 | ☑ | 2026-05-26 | |
| T-04 | WeaponComponent 承接武器 | 2 | ☑ | 2026-05-26 | |
| T-05 | 删除组件化字段冗余备份 | 2 | ☑ | 2026-05-26 | |
| T-06 | RenderComponent : IComponent + facade 化 | 2 | ☑ | 2026-05-26 | |
| T-07 | driver 注入方式重构 | 3 | ☐ | | |
| T-08 | AnimComponent + IAnimController | 3 | ☐ | | |
| T-09 | AgentLocomotion::m_owner 泛化 | 3 | ☐ | | |
| T-10 | 事件 token 挂组件 onAttach | 3 | ☐ | | |
| T-11 | VehicleObject 解构 | 4 | ☐ | | |
| T-12 | LuaEnvObject 改 LuaScriptComponent | 4 | ☐ | | |
| T-13 | BaseObject ↔ GameObject 合并 | 4 | ☐ | | |
| T-14 | 位置真源统一 | 4 | ☐ | | |
| T-15 | 删除 RenderableObject | 4 | ☐ | | |
| T-16 | 删除 driver Lua forwarder | 4 | ☐ | | |
| T-17 | SoldierObject 改 SoldierFactory | 4 | ☐ | | |

---

## 9. 与其他文档的关系

- 上层路线 / 性能 / AI 演化：[IterationPlan.md](IterationPlan.md)
- 全工程结构重构总账：[RefactoringPlan.md](RefactoringPlan.md)（本文档 T-00/T-02/T-06/T-08/T-09/T-13 跟其中 S-xx 一一映射）
- 项目协作约定 / 命名规则：AGENTS.md（每 Task 完成同步更新）

> **维护**：完成 Task 在 §8 表格回填日期和 commit。新发现问题追加为 T-18+ 并补 §2 问题清单。决策点（§6）有变更直接更新本文档，不要散在 commit。
