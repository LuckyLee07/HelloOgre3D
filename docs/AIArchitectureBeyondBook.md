# AI 架构超越原书设计

> **定位**：本文档是 **AI 能力扩展的设计参考**，覆盖原书《HelloOgre3DX》Ch 7（Knowledge）/ Ch 8（Perception）/ Ch 9（Tactics）三章的概念，但**不做代码迁移**——以当前项目的 C++ runtime + 声明式配置 + 组件化架构为底座，重新设计接口契约、数据结构、扩展点。
>
> **跟其他文档的关系**：
> - [ai-roadmap.md](ai-roadmap.md)：AI 中长期路线与 P0/P1/P2 优先级，本文档是 Ch 7-9 概念落到当前架构时的设计补充。
> - [project-roadmap.md](project-roadmap.md)：项目级路线，本文档只覆盖 AI Knowledge / Perception / Tactics 这条线。
> - [cpp-object-model-refactor-roadmap.md](cpp-object-model-refactor-roadmap.md)：对象模型和组件化边界，本文档新增组件必须遵守它的依赖方向。
> - [SoldierObjectRefactor.md](SoldierObjectRefactor.md)：SoldierObject / AgentObject / AIController 收口事实来源，本文档以后续 AI 能力复用其终态为前提。
> - 原书 chapter 6-9 源码：`E:\Workspace\HelloOgre3DX\src\chapter_[6-9]_*` —— **只作参考阅读，不做代码移植**
>
> **使用规则**：实现 Stage 3-4 新组件时，先查本文档对应章节的接口契约；契约有改动直接更新本文档，不要散在 commit。本文档描述的是架构目标，具体开工前仍要 grep 当前代码确认事实，不凭历史计划推断现状。

---

## 0. 设计哲学：Ch 6 已经验证的"超越原书"模式

原书 Ch 6 是纯 Lua 实现的 FSM/DT/BT；当前项目已经升级到 **C++ runtime 引擎 + Lua 声明式配置 + `IDecisionDriver` 统一抽象**。这次升级建立了一套可复用的模式，**Ch 7-9 沿用之**：

| 维度 | 原书做法 | 本项目升级形态 |
|---|---|---|
| **实现分层** | 纯 Lua | C++ 引擎 + Lua 薄壳 |
| **数据形态** | 命令式代码构造 | 声明式表格配置（Loader 解析）|
| **抽象层级** | 各 paradigm 独立写 | 抽 `IXxx` 接口统一共性 |
| **类型安全** | 字符串 key + Lua table | 强类型 enum + 强类型 struct |
| **可观测性** | print 日志 | Tracy zone + 调试 trace |
| **扩展性** | 改文件加 case | 新加子类 / 新加配置 |
| **业务 vs 引擎** | 混在一处 | 引擎 C++、业务 Lua、组件挂载 |

> **设计准则**：每加一个 Ch 7-9 的能力，都问"这条原书代码体现什么概念？这个概念在我们架构下应该长什么样？"——保留**思想**，丢掉**实现细节**。

### 0.1 当前项目基线

本文档以当前 `HelloOgre3D` 代码为基线，不再假设原书 Ch 7-9 已经整体迁入：

- `AIController` 已经持有稳定的 per-agent `Blackboard`，DT / BT / FSM 通过 `AIController::GetBlackboard()` 共享同一份本地黑板。后续不应再让每个 driver 各自拥有黑板。
- `Blackboard` 当前仍是 typed map（agent / float / int / bool / string / vec3），没有 Team / Global scope，没有 TTL / confidence metadata，也不支持复杂 value。
- `AIScheduler` 已经存在，并已接入错峰 tick、tick budget、Lua 配置和 Tracy counters。后续 Sensor / KnowledgeSource / InfluenceMap / Tactics 更新必须复用它，不新增平行调度器。
- `SandboxEventPayload` 已经提供 `eventType/scope/senderId/targetId/teamId/timeMs/x/y/z` 统一字段，`SandboxEventDispatcher` 已覆盖 Local 事件。后续事件系统优先扩展现有 dispatcher 到 Team / Global / Spatial，而不是直接新建第二套 EventBus。
- Lua 行为配置、BT loader、Lua action 调用约定已经稳定；新增能力要优先保持 `owner, blackboard` 这类调用面兼容。

### 0.2 落地原则

1. **先垂直闭环，再补抽象**：每个阶段至少形成一个 sample 可观察行为，不先堆完整接口矩阵。
2. **先复用现有基础设施**：Blackboard / AIScheduler / SandboxEventDispatcher / RuntimeProfileCounters 已存在，新增能力应扩展它们。
3. **跨对象引用优先用 id / handle**：Blackboard、Memory、Event payload 里不要长期保存裸对象指针；需要行为层访问时再通过 ObjectManager 解析。
4. **Lua 边界不承诺强类型**：C++ 内部可以逐步 enum / typed payload 化，Lua 侧应拿到经过校验的 table 或只读 proxy，而不是假设 Lua 本身强类型。
5. **每个能力都要有验收入口**：新增 Sensor、Event、Memory、TeamBlackboard、InfluenceMap 前，同时写清楚 smoke / selftest / sample 的通过标准。

---

## 1. Ch 7 升级：Multi-Source Blackboard

### 1.1 原书形态

[Blackboard.lua](../../HelloOgre3DX/src/chapter_7_knowledge/script/Blackboard.lua) + [KnowledgeSource.lua](../../HelloOgre3DX/src/chapter_7_knowledge/script/KnowledgeSource.lua)：

```lua
-- 简化伪代码
Blackboard = {
    attributes_ = {},   -- 静态数据
    sources_ = {},      -- 每个 attribute 可有多个 source
}

function Blackboard:Get(attr)
    if self.sources_[attr] then
        -- 多 source 时，按 confidence 选最佳
        return EvaluateSources(self, self.sources_[attr])
    end
    return self.attributes_[attr]
end

KnowledgeSource:Evaluate() -- returns { evaluation, confidence }
```

### 1.2 原书简陋之处

1. **Blackboard 就是 Lua table**，无 scope（Local/Team/Global 都混在一起）
2. **KnowledgeSource 是 closure**，无生命周期、无 onAttach/onDetach
3. **Confidence 是浮点数对比**，无衰减规则、无 TTL
4. **Lua 计算开销大**：每次 Get 遍历所有 source 执行 Evaluate
5. **类型不安全**：Get 返回 generic value，调用方自己 cast

### 1.3 本项目升级形态

#### 1.3.1 Scope 一等概念

```cpp
// sandbox/ai/common/Blackboard.h
class Blackboard {
public:
    enum class Scope { Local, Team, Global };

    // 现有 typed access（已实现，Local blackboard）
    AgentObject* GetAgent(const std::string& key) const;
    Ogre::Vector3 GetVec3(const std::string& key) const;
    // ... 其他类型

    // 新增：带元数据 entry（含 confidence/timestamp/ttl）
    // 注意：VS2017 工程不要默认使用 std::variant，优先使用项目兼容的 tagged value。
    enum class ValueType {
        None,
        AgentId,
        Float,
        Int,
        Bool,
        String,
        Vec3,
    };

    struct Value {
        ValueType type = ValueType::None;
        int objectId = -1;              // 跨对象引用优先存 id，不长期保存裸指针
        float floatValue = 0.0f;
        int intValue = 0;
        bool boolValue = false;
        std::string stringValue;
        Ogre::Vector3 vec3Value = Ogre::Vector3::ZERO;
    };

    struct Entry {
        Value value;
        float confidence = 1.0f;
        int64_t timestampMs = 0;
        int ttlMs = -1;       // -1 = 永久
        const char* source = nullptr;   // debug 用：Vision / Hearing / Team / LuaSource
    };
    void SetEntry(const std::string& key, const Entry& entry);
    bool GetEntry(const std::string& key, Entry& out) const;
};
```

`AIController` 当前已经持有 per-agent `Blackboard`。后续扩展分两层：

1. Local：继续由 `AIController` 拥有，保持 `GetBlackboard()` 指针稳定。
2. Team / Global：由 TeamBlackboardService / GameManager 或后续 SquadController 持有，`AIController` 只持有 view / accessor，不接管其生命周期。

Key 命名需要收敛到命名空间，避免全局字符串膨胀：

| 前缀 | 示例 | 用途 |
|---|---|---|
| `combat.*` | `combat.targetId` | 当前战斗目标、威胁等级 |
| `sense.*` | `sense.visibleEnemyIds` | Sensor 当前输出 |
| `memory.*` | `memory.lastKnownEnemyPos` | 记忆和 TTL 结果 |
| `team.*` | `team.focusTargetId` | TeamBlackboard 共享命令 |
| `debug.*` | `debug.lastDecision` | 调试摘要，不参与行为判断 |

#### 1.3.2 IKnowledgeSource C++ 接口

`IKnowledgeSource` 是后续抽象，不是第一步必做。第一阶段优先让 Sensor / Event / Memory 直接写 Blackboard，等出现多个来源竞争同一个 key 时，再引入 pull-style source 融合。

```cpp
class IKnowledgeSource {
public:
    struct Result {
        Blackboard::Value value;
        float confidence;
        int64_t timestampMs;
    };

    virtual Result Evaluate(BaseObject* owner) = 0;
    virtual int GetUpdateIntervalMs() const = 0;  // 跟原书一致：限频
};

class Blackboard {
public:
    void RegisterSource(const std::string& key, IKnowledgeSource* source);
};
```

Sensor 系（Ch 8）默认不强制继承 `IKnowledgeSource`，避免 Scan(push) 与 Evaluate(pull) 两条路径同时存在。只有在需要"Vision / Hearing / TeamReport / LuaSource 同时给出 targetId，由 Blackboard 按 confidence 选最佳"这类需求时，再让 Sensor 暴露 source adapter：

```cpp
class VisionTargetSource : public IKnowledgeSource {
    Result Evaluate(...) override {  // raycast + FOV
        Result result;
        result.value = MakeAgentIdValue(closestEnemyId);
        result.confidence = 1.0f - distance / maxRange;
        return result;
    }
};
```

多源融合必须有确定性规则，不能只靠"浮点数更大"：

1. 过滤过期 entry。
2. 过滤类型不匹配 entry。
3. 按 source priority 排序。
4. priority 相同时按 confidence 排序。
5. confidence 相同时按 timestamp 较新者胜。
6. 冲突和降级写入 debug trace，便于 AI 面板解释"为什么选这个目标"。

#### 1.3.3 Confidence 衰减规则

```cpp
class Blackboard {
public:
    enum class DecayPolicy {
        None,
        Linear,        // confidence -= rate * deltaT
        Exponential,   // confidence *= pow(decayFactor, deltaT)
    };
    void SetDecayPolicy(const std::string& key, DecayPolicy policy, float rate);
};
```

`Tick()` 时遍历所有 source，按 policy 衰减 confidence。Confidence <= 0 → 自动清理。

#### 1.3.4 TTL 自动过期

`Entry::ttlMs` 设为 N，Tick 时 `timestampMs + ttlMs < currentTime` 自动剔除。原书 [AgentSenses](../../HelloOgre3DX/src/chapter_8_perception/script/AgentSenses.lua) 的 `PruneEvents` 手动遍历过期，这里下沉到 Blackboard 通用机制。

#### 1.3.5 Lua 仍可注册 source（业务灵活）

```cpp
class LuaKnowledgeSource : public IKnowledgeSource {
    int m_luaFuncRef;  // luaL_ref 缓存
    Result Evaluate(BaseObject* owner) override {
        // lua_rawgeti 取函数，调 Evaluate，返回 {value, confidence}
    }
};
```

Lua 侧：

```lua
agent:GetAI():GetBlackboard():RegisterLuaSource("nearestCover", function(owner)
    -- 计算最近的掩体
    return cover, confidence
end, 500)  -- 500ms 更新一次
```

该能力放在 K-04，不作为 Sensor / Memory MVP 的前置。Lua source 必须使用 `luaL_ref` 缓存 callback，owner 销毁或 sample 重载时要解绑，避免悬空 Lua 引用。

### 1.4 保留 / 抛弃 / 新增

| 概念 | 处置 |
|---|---|
| **Get 自动多源融合** | 保留（核心思想）|
| **(value, confidence) 二元组** | 保留 |
| **updateFrequency 限频** | 保留（Tick 内调度）|
| **Blackboard 就是 Lua table** | 抛弃 → C++ Blackboard with scope |
| **KnowledgeSource 是 Lua closure** | 抛弃 → C++ IKnowledgeSource，Lua source 用包装类 |
| **手动 PruneEvents** | 抛弃 → TTL 自动过期机制 |
| **无 scope** | 抛弃 → Local/Team/Global 一等概念 |
| **Confidence 衰减** | **新增**（原书没有）|
| **跟 Sensor 系统的桥接** | **后置新增**（Sensor MVP 先 push，必要时再做 source adapter）|

### 1.5 任务映射

| 本节内容 | 对应阶段 |
|---|---|
| Local Blackboard ownership | 已由 SoldierObjectRefactor T-19 收口到 AIController |
| Blackboard Scope 三层 | Stage 2/3 交界任务：Local 已有，Team / Global 待补 |
| Blackboard metadata value | Stage 3 已有最小通道：typed value + confidence + timestamp + ttl + source；Team / Global scope 待补 |
| TTL / Confidence 衰减 | Stage 3 已有最小通道：`UpdateEntries` 自动剔除过期 entry，并按 linear / exponential policy 衰减 confidence |
| IKnowledgeSource 接口 + multi-source 融合 | Stage 3 后半段（本文档 K-03，等 Sensor MVP 后再做）|
| Lua source 包装 | Stage 3 后半段（本文档 K-04，非首轮 MVP）|

---

## 2. Ch 8 升级：Sensor 体系 + 事件系统规范化

### 2.1 原书形态

[AgentSenses.lua](../../HelloOgre3DX/src/chapter_8_perception/script/AgentSenses.lua) 把所有感知混在一个文件：
- `UpdateVisibility` —— 遍历所有 agent 做 raycast + 45° FOV
- `HandleBulletImpactEvent / HandleBulletShotEvent` —— 子弹事件
- `HandleDeadEnemySightedEvent / HandleEnemySightedEvent` —— 死人通知

[AgentCommunications.lua](../../HelloOgre3DX/src/chapter_8_perception/script/AgentCommunications.lua) 字符串 EventType：
```lua
AgentCommunications.EventType.EnemySighted = "EnemySighted"
AgentCommunications.EventType.BulletImpact = "BulletImpact"
-- 拼写错运行时才发现
```

### 2.2 原书简陋之处

1. **所有感知一个文件**，加新感知类型（听觉、嗅觉、危险）只能往里塞 if-else
2. **视觉算法跟事件处理混杂**（CanSeeAgent + HandleBulletImpact 在一起）
3. **字符串 EventType** —— 编译期无检查，拼写错运行时才崩
4. **事件无空间维度** —— 1000m 外的枪声 1m 处也能收到
5. **Lua 实现性能差** —— 每帧每 agent 都做 raycast，无错峰

### 2.3 本项目升级形态

#### 2.3.1 Sensor 系（组件化扫描）

```cpp
// sandbox/components/ai/ISensor.h
class ISensor : public IComponent {
public:
    virtual void Scan(Blackboard* bb) = 0;
    virtual int GetScanIntervalMs() const = 0;
};

// sandbox/components/ai/VisionSensor.h
class VisionSensor : public ISensor {
public:
    float m_rangeM = 30.0f;
    float m_fovDegrees = 90.0f;
    int m_scanIntervalMs = 200;

    void Scan(Blackboard* bb) override;  // raycast + FOV，写 bb["sense.visibleEnemyIds"]
};

// sandbox/components/ai/HearingSensor.h
class HearingSensor : public ISensor {
public:
    float m_rangeM = 50.0f;
    // 订阅 SandboxEventTypes::BulletShot / Footstep，按距离衰减 confidence 写 bb["sense.heardSounds"]
    void Scan(Blackboard* bb) override;
};

// sandbox/components/ai/DangerSensor.h
class DangerSensor : public ISensor {
public:
    // 检测子弹轨迹、爆炸预警 → bb["sense.incomingThreats"]
};
```

**关键设计**：
- 当前 P-01 MVP 先落在 `sandbox/ai/perception/VisionSensor.h`，由 `AIController` 持有并驱动；独立 `ISensor : IComponent` 挂载留给 Hearing / DangerSensor 一起收口。
- 每个 Sensor 独立组件，挂载方式灵活（不是所有 agent 都要全套感知）
- 每个 Sensor 的 Scan 是独立的 Tracy zone（可定位单个 sensor 慢）
- `AIScheduler` 把 `Sensor::Scan` 也纳入错峰调度，不新增平行调度器
- Sensor 默认是 push 模型：扫描后写 Blackboard / Memory。pull-style `IKnowledgeSource` 是后续 adapter，不是 Sensor MVP 的硬依赖

#### 2.3.2 MemoryComponent（独立组件）

Sensor 写"现在看到什么"，Memory 写"曾经看到过什么"：

```cpp
class MemoryComponent : public IComponent {
public:
    struct EnemyMemory {
        int agentId = -1;
        Vec3 lastSeenPos;
        int64_t lastSeenAtMs;
        float confidence;  // 随时间衰减
    };

    void OnEnemyObserved(const EnemyMemory& m);  // VisionSensor 调
    void Tick(int deltaMs);                      // 衰减、清理过期

    const EnemyMemory* GetLastKnownEnemy() const;
    bool GetLastKnownPosition(int enemyId, Vec3& out) const;
};
```

**关键设计**：跟 Sensor 解耦——Sensor 写 current state，Memory 提供 historical query。BT 节点"追到最后已知位置"读 Memory 而非临时扫描全场。

Memory 与 Blackboard 不要形成两套真源。推荐落地方式：

- 当前 P-02 MVP 先落在 `sandbox/ai/perception/MemoryStore.h`：它不是独立 `IComponent`，而是 `AIController` 持有的领域接口，底层复用 Blackboard metadata entry / TTL / confidence 衰减。
- `MemoryComponent` 负责领域接口，例如 `GetLastKnownEnemy()`、`GetLastKnownPosition(enemyId)`。
- 底层存储可以复用 Blackboard entry + TTL / confidence，或者由 MemoryComponent 私有存储后同步摘要到 Blackboard。
- 第一阶段只允许一个写入真源；如果选 Blackboard TTL，就不要再在 MemoryComponent 里维护另一份过期逻辑。

#### 2.3.3 事件系统规范化

```cpp
// 已有基础：sandbox/core/event/SandboxEventPayload.h
enum class SandboxEventScope {
    Local,
    Team,
    Global,
    Spatial, // 待补
};

namespace SandboxEventTypes {
    const char* EnemySighted();       // 待补
    const char* BulletShot();         // 待补
    const char* BulletImpact();       // 待补
    const char* SupportRequested();   // 待补
}

namespace SandboxEventFields {
    const char* EventType();
    const char* Scope();
    const char* SenderId();
    const char* TargetId();
    const char* TeamId();
    const char* TimeMs();
    const char* PositionX();
    const char* PositionY();
    const char* PositionZ();
}
```

当前项目已经有 `SandboxEventPayload` 和 `SandboxEventDispatcher`，所以 S-01 不应直接新建第二套 `EventBus`。推荐路线：

1. 扩展 `SandboxEventTypes` 常量，先覆盖 EnemySighted / BulletShot / BulletImpact / SupportRequested。
2. 扩展 `SandboxEventScope`，补 Team / Global 的实际派发路径；Spatial 可以作为 Team / Global 的过滤条件。
3. 保持 dispatcher token unsubscribe 机制，agent 销毁、sample 重载、UI 关闭时必须解绑。
4. C++ 内部可以逐步引入 enum wrapper，但 Lua 侧继续拿 `SandboxContext` / table 字段，字段由 `SandboxEventFields` 统一。
5. 只有当现有 dispatcher 在性能、延迟派发或类型安全上无法承载时，再把它升级或替换成真正 typed EventBus。

`Spatial` scope 配合 `SpatialFilter`（中心点 + 半径）：枪声只在 50m 内 agent 收到。

#### 2.3.4 跟现有 SandboxEventDispatcher 的衔接

[sandbox/core/event/SandboxEventDispatcher.h](../src/HelloOgre3D/sandbox/core/event/SandboxEventDispatcher.h) 当前已有，但用字符串 EventName + `SandboxContext`（也是 string-keyed）。

**升级方向**：
- `SandboxEventDispatcher` 继续承担 Local event（ASM_STATE_CHANGE / HEALTH_CHANGE / ASM_NOTIFY 等）
- 在同一套 dispatcher manager 上补 Team / Global / Spatial 派发，不让感知和战术层依赖另一套全局单例
- 字符串 EventName 暂时保留，但必须集中在 `SandboxEventTypes`，禁止各 Lua 文件散落手写字符串
- 若未来引入 C++ enum，先做薄 wrapper 映射到 `SandboxEventTypes`，不要破坏 Lua 订阅面

#### 2.3.5 Lua 仍可订阅

```cpp
class LuaEventListener {
    int m_luaFuncRef;
};

SandboxEventDispatcher::SubscribeFromLua(SandboxEventTypes::EnemySighted(), lua_func_ref);
```

Lua 侧：
```lua
SandboxEvents:Subscribe("EnemySighted", function(payload)
    -- payload.senderId / targetId / position / timeMs 来自统一字段约定
end)
```

Lua 层不承诺 C++ 意义上的强类型，验收重点是字段稳定、缺字段可诊断、订阅生命周期安全。

### 2.4 保留 / 抛弃 / 新增

| 概念 | 处置 |
|---|---|
| **视觉 raycast + FOV 算法** | 保留（核心思想，C++ 实现）|
| **45° / 90° 视野角** | 保留（变可配置）|
| **bullet impact / shot 通知** | 保留（变 typed event）|
| **dead enemy / friendly 通知** | 保留 |
| **AgentSenses 单文件混杂** | 抛弃 → Sensor 子类多态 |
| **散落字符串 EventType** | 抛弃 → `SandboxEventTypes` 集中定义，后续可包 enum |
| **无空间维度的广播** | 抛弃 → Team / Global / Spatial scope |
| **每帧无错峰 raycast** | 抛弃 → AIScheduler 调度 |
| **MemoryComponent** | **新增**（原书没分离 sensing vs memory）|
| **Sensor 子类（听觉/危险）** | **新增** |
| **跟 IKnowledgeSource 桥接** | **后置新增**（Sensor MVP 先 push，必要时再做 source adapter）|

### 2.5 任务映射

| 本节内容 | 对应阶段 |
|---|---|
| VisionSensor | Stage 3 已有最小通道：定时扫描 + FOV 配置预留 + Blackboard `sense.*` / `memory.*` 写入 |
| MemoryComponent | Stage 3 已有 MemoryStore MVP：lastKnown 查询接口 + Blackboard TTL / confidence 真源 |
| HearingSensor / DangerSensor | Stage 3 后续 |
| SandboxEventTypes + Team/Global 派发 | Stage 3 新增（本文档 S-01）|
| Spatial scope / SpatialFilter | Stage 3 新增（本文档 S-02）|

---

## 3. Ch 9 升级：分层 AI + SquadObject

### 3.1 原书形态

[SoldierTactics.lua](../../HelloOgre3DX/src/chapter_9_tactics/script/SoldierTactics.lua) 把战术混在 agent 个体逻辑里：每个 agent 自己判断"该撤退了吗 / 该求支援了吗"，靠 [AgentCommunications](../../HelloOgre3DX/src/chapter_8_perception/script/AgentCommunications.lua) 广播协调。

### 3.2 原书简陋之处

1. **无小队一等概念**——撤退点选择是个函数被各 agent 调用，谁是"队长"不明确
2. **战术混在个体决策里**——agent 的 BT 既要算战术又要执行动作
3. **协调靠广播 + 个体投票**——没有结构化的指挥链
4. **无队形 / 阵型概念**——多 agent 抱团靠 OpenSteer 自然聚集

### 3.3 本项目升级形态

#### 3.3.1 分层 AI 架构

```
┌─────────────────────────────────────┐
│  Faction / Global 层（未来扩展）       │
│  FactionController：阵营战略           │
│  （进攻/防御/撤退方向）                 │
└──────────────┬──────────────────────┘
               ↓ (broad strategy via Global Blackboard)
┌─────────────────────────────────────┐
│  Squad 层                            │
│  SquadObject + SquadController       │
│  （小队战术：队形/集结/分工）           │
└──────────────┬──────────────────────┘
               ↓ (AICommand via Team Blackboard)
┌─────────────────────────────────────┐
│  Agent 层                            │
│  AIController + BT/DT                │
│  （个体执行 + 本能反应）                │
└─────────────────────────────────────┘
```

#### 3.3.2 SquadObject 一等实体

不要在 TeamBlackboard / Team event 尚未跑通前直接上 SquadObject。推荐先做中间层：

1. TeamBlackboardService：按 teamId 管共享数据，不进入 ObjectManager。
2. Team target sharing：一个 agent 发现敌人后，同队能读到 `team.focusTargetId` / `team.lastKnownEnemyPos`。
3. Team event smoke：SupportRequested / EnemySighted 可以在同队传播，并能正确解绑。
4. 这些闭环稳定后，再把"小队"提升为 `SquadObject`，承载阵型、角色分配和小队 BT。

```cpp
// sandbox/objects/SquadObject.h
class SquadObject : public BaseObject {
public:
    SquadObject(unsigned int teamId);

    void AddMember(AgentObject* agent);
    void RemoveMember(AgentObject* agent);
    const std::vector<AgentObject*>& GetMembers() const;

    // 复用 BaseObject 的组件容器
    // 挂 SquadController（自己的 BT）+ Team Blackboard
};
```

**关键设计**：小队是 BaseObject 子类（或纯 Entity），可独立 tick、可在 ObjectManager 里被遍历、可挂组件、可被 tolua 暴露。

#### 3.3.3 SquadController（小队级 BT）

```cpp
// sandbox/components/ai/SquadController.h
class SquadController : public IComponent {
public:
    void Tick();  // 跑小队 BT

    // 战术接口
    Vec3 SelectRetreatPosition() const;
    void AssignCoverPositions();
    void IssueAttackTarget(AgentObject* enemy);
    Formation GetFormation() const;
    void SetFormation(FormationType type);

private:
    BehaviorTree* m_squadBT;  // 小队自己的 BT 树
};
```

**关键设计**：
- 小队 BT 独立于个体 BT（两棵树）
- 小队 BT 的 Action 节点是 *给成员发命令*，不是直接控制成员
- 小队 BT 配置走 Lua 声明式表（跟 SoldierBT 同格式）

#### 3.3.4 协作 BT 节点

```cpp
// 个体 BT 用的协作节点
class BehaviorCallForBackup : public BehaviorAction {
    // 广播 SandboxEventTypes::SupportRequested 给同队
};

class BehaviorWaitForSquadMate : public BehaviorAction {
    // 读 team bb 的 squad_state, 等其他成员就位
};

class BehaviorMoveToFormationSlot : public BehaviorAction {
    // 读 team bb 的 my_formation_slot, 走到指定位置
};

class BehaviorFollowSquadOrder : public BehaviorAction {
    // 读 team bb 的 latest_order (AICommand)，转给个体 ApplyCommand
};
```

这些节点跟 Sequence/Selector 同级，Loader 自动识别。

#### 3.3.5 Formation 系统

```cpp
enum class FormationType {
    Line,        // 横排
    Wedge,       // 楔形
    Column,      // 纵列
    Circle,      // 圆形（围攻 / 守护中心）
    Scatter,     // 散开（防 AOE）
};

class Formation {
public:
    std::vector<Vec3> ComputeSlots(
        const Vec3& center, 
        const Vec3& forward, 
        int memberCount) const;
};
```

SquadController 每 N 帧调 `Formation::ComputeSlots`，把每个成员的 slot 位置写到对应成员的 local bb；个体 BT 节点 `BehaviorMoveToFormationSlot` 读 bb 走过去。

Slot 分配必须稳定，不能依赖当前 `members` vector 顺序。推荐用 `agentId -> slotId` 映射；成员死亡 / 离队时只重排必要 slot，避免队形每帧抖动。

#### 3.3.6 命令链：从 AICommand 到执行

```
SquadController BT          产出 AICommand{kind=MoveTo, target=Vec3}
   ↓ (通过 Team Blackboard 或直接 AgentObject::ApplyCommand)
Agent 收到 AICommand
   ↓
SoldierObject::ApplyCommand 翻译为具体动作（朝目标走/朝敌人开枪）
```

这复用 [SoldierObjectRefactor.md](SoldierObjectRefactor.md) 终态的 AICommand 体系。

#### 3.3.7 InfluenceMap 不应缺席

Ch 9 的 Tactics 不能只覆盖 Squad / Formation。`ai-roadmap.md` 已把 InfluenceMap 多层升级列为 P1，后续战术移动、撤退点、掩体偏好、危险规避都应能读取 influence 结果。

推荐拆成独立能力，不强塞进 SquadObject：

```cpp
enum class InfluenceLayer {
    EnemyThreat,
    TeamControl,
    CoverValue,
    SightLine,
    CrowdDensity,
    ObjectiveAttraction,
};

class InfluenceMap {
public:
    void UpdateLayer(InfluenceLayer layer, int budgetCells);
    float Sample(InfluenceLayer layer, const Vec3& pos) const;
    float EvaluateTacticScore(const Vec3& pos, const InfluenceQuery& query) const;
};
```

第一阶段只要求至少一个 sample 展示"AI 避开危险区域或偏好掩体区域"，并接入 Tracy / debug 显示。Formation 和 Squad BT 可以读取 InfluenceMap，但不拥有它。

### 3.4 保留 / 抛弃 / 新增

| 概念 | 处置 |
|---|---|
| **撤退点选择算法** | 保留（C++ 实现）|
| **小队协作思想** | 保留（升级为结构化分层）|
| **agent 自己当队长** | 抛弃 → SquadController 独立指挥 |
| **战术混在个体 BT** | 抛弃 → 双层 BT（小队 BT + 个体 BT）|
| **协调靠广播投票** | 抛弃 → SquadController 直接下命令 |
| **SquadObject 一等实体** | **新增** |
| **Formation 系统** | **新增** |
| **分层 AI（Faction → Squad → Agent）** | **新增** |
| **协作 BT 节点（CallForBackup 等）** | **新增** |
| **InfluenceMap 多层战术评分** | **新增**（独立系统，供 Squad / Agent 读取）|

### 3.5 任务映射

| 本节内容 | 对应阶段 |
|---|---|
| TeamBlackboardService + team target sharing | Stage 3/4 交界（本文档 T-00） |
| SquadObject + SquadController | Stage 4 后续，TeamBlackboard 验收后再做 |
| 协作 BT 节点 | Stage 4 后续（节点补全，本文档 T-02） |
| Formation 系统 | Stage 4 新增（本文档 T-01） |
| 分层 AI 架构 | Stage 4 整体目标 |
| InfluenceMap 多层升级 | Stage 4 新增（本文档 T-03） |

---

## 4. 设计上的整体一致性

### 4.1 三个新组件的共同基础

| 组件 | 继承 | 依赖 |
|---|---|---|
| `VisionSensor / HearingSensor / DangerSensor` | `ISensor : IComponent` | Blackboard、SandboxEventDispatcher、AIScheduler |
| `MemoryComponent` | `IComponent` | Blackboard 或私有 memory store、SandboxEventDispatcher（订阅 EnemySighted）|
| `TeamBlackboardService` | service | teamId -> Blackboard，GameManager / ObjectManager 持有 |
| `SquadController` | `IComponent` | Team Blackboard、SandboxEventDispatcher、BT runtime |
| `InfluenceMap` | system/service | Navigation、ObjectManager、debug draw、AIScheduler |
| `SandboxEventDispatcher` 扩展 | 现有事件系统 | `SandboxEventPayload` + `SandboxEventTypes` + scope routing |

### 4.2 跟现有架构的衔接

- 全部继承 `IComponent`（[sandbox/core/component/IComponent.h](../src/HelloOgre3D/sandbox/core/component/IComponent.h)），符合 EC 化方向
- Blackboard scope 复用当前 `AIController` local bb，后续补 Team / Global accessor
- BT 节点扩展用现有 `BehaviorAction` 体系
- Lua 桥接用现有 tolua 绑定 + LuaPluginMgr 模式

### 4.3 数据流图

```
                                     ┌───────────────────┐
                                     │ SandboxEvent      │
                                     │ Dispatcher        │
                                     │ (payload + scope) │
                                     └────┬──────────────┘
                                          │ Emit / Subscribe
                                          ↓
┌───────────────┐   write    ┌───────────────────────┐   read   ┌─────────────────┐
│ VisionSensor  │ ─────────→ │  Local Blackboard     │ ←──────── │  Agent BT/DT    │
│ HearingSensor │            │  (visible / heard /   │           │  (个体决策)      │
│ DangerSensor  │            │   threats / memory)   │           └─────────────────┘
└───────────────┘            └───────────────────────┘
                                          ↑
                                          │ sync 部分到
                                          ↓
┌───────────────┐   write    ┌───────────────────────┐   read   ┌─────────────────┐
│ SquadCtrl     │ ─────────→ │  Team Blackboard      │ ←──────── │  Squad BT       │
│ (team-level   │            │  (squad state /       │           │  (战术决策)      │
│  perception)  │            │   formation / orders) │           └─────────────────┘
└───────────────┘            └───────────────────────┘
                                          ↑
                                          │ Faction / Influence (Stage 4+)
                                          ↓
                              ┌───────────────────────┐
                              │  Global Blackboard    │
                              └───────────────────────┘
```

### 4.4 性能与可观测性

每个新组件都要满足：
- Tracy zone 埋点（Sensor::Scan / SquadController::Tick / SandboxEventDispatcher::Dispatch / InfluenceMap::UpdateLayer）
- 错峰调度纳入 [AIScheduler](../src/HelloOgre3D/sandbox/ai/common/AIScheduler.h)
- 配置 update interval（不是每帧都跑）
- Lua 调用走 ref 缓存（`luaL_ref` + `lua_rawgeti`），不走 `lua_getfield(funcname)`

默认预算先按保守值起步，后续由 smoke 和 Tracy 调整：

| 能力 | 默认频率 / 预算 | 说明 |
|---|---|---|
| VisionSensor | 200ms / agent，按 agentId 错峰 | raycast 和 FOV 不每帧全量跑 |
| HearingSensor | 事件驱动，100ms 合并窗口 | 高频声音事件先合并再写 bb |
| Memory decay | 500ms 或 1000ms | 只处理活跃 memory entry |
| TeamBlackboard sync | 200ms | 共享目标不需要逐帧同步 |
| InfluenceMap layer | 500ms+，每帧 cell budget | 大地图必须分片更新 |
| SandboxEvent dispatch | 每帧最大事件数可配置 | 超预算延迟到下一帧，避免回调里改遍历中列表 |

---

## 5. 推荐实现顺序

**前提**：[SoldierObjectRefactor.md](SoldierObjectRefactor.md) 当前 AIController / Blackboard / AIScheduler 收口状态保持稳定。

| 顺序 | 任务 | 引入能力 |
|---|---|---|
| 1 | **K-01** Blackboard metadata / safe value | 为 TTL、confidence、debug trace 打基础 |
| 2 | **S-01** 扩展 `SandboxEventPayload` / `SandboxEventTypes` | 统一 AI 事件字段，补 Team / Global 路线 |
| 3 | **S-02** SpatialFilter / 延迟派发 | 枪声、冲击、发现敌人按范围传播 |
| 4 | VisionSensor MVP | 视觉感知写 `sense.*` / `memory.*`，固定 sample 可观察 |
| 5 | MemoryComponent / MemoryStore MVP | 最后已知位置、TTL、confidence 衰减 |
| 6 | **K-02** Blackboard TTL / Confidence 收口 | 让 Memory 和 Sensor 不各自造过期机制 |
| 7 | TeamBlackboardService | 同队共享发现敌人 / 集火目标 |
| 8 | HearingSensor / DangerSensor | 多感知类型复用事件和 memory |
| 9 | **K-03** IKnowledgeSource adapter | 多源融合，只在有真实冲突需求后引入 |
| 10 | **K-04** LuaKnowledgeSource | Lua 业务 source 注册面，非 MVP 前置 |
| 11 | SquadObject + SquadController | 小队一等实体和小队 BT |
| 12 | **T-01** Formation 系统 | 队形数据驱动、稳定 slot 分配 |
| 13 | 协作 BT 节点 | CallForBackup / WaitForSquadMate / MoveToFormationSlot |
| 14 | **T-03** InfluenceMap 多层 | 威胁、掩体、控制区、拥挤度战术评分 |
| 15 | 分层 AI（Faction 层，按需）| 阵营战略 |

---

## 6. 待定决策点

实现前需要拍板的设计选择。各任务执行时跟用户确认。

### 6.1 EventBus 跟 SandboxEventDispatcher 的关系

候选：
- **A**：并存。SandboxEventDispatcher 管对象内部事件（ASM_STATE_CHANGE 等），EventBus 管世界级游戏事件
- **B**：合并到统一事件总线，对象级事件作为 self-scope 特例
- **C**：让 SandboxEventDispatcher 改为 typed 接口（不再字符串 key），实现等价 EventBus

**当前倾向 C 的渐进版**：优先扩展现有 `SandboxEventDispatcher` / `SandboxEventPayload`，补 `SandboxEventTypes`、Team / Global / Spatial 路由和延迟派发；如果后续 typed enum 价值明确，再在外层加 enum wrapper，不直接新建第二套 EventBus。**决策时机**：S-01 实现前。

### 6.2 IKnowledgeSource 跟 ISensor 是否合并

候选：
- **A**：Sensor 同时实现 ISensor 和 IKnowledgeSource 两个接口（多继承）
- **B**：Sensor 内部聚合 KnowledgeSource，组合不继承
- **C**：合并为一个 ISensor 接口，里面包含 Evaluate 方法

**倾向 B**：Sensor 先保持 push-only；需要多源融合时，用 adapter 把 Sensor 输出包装成 `IKnowledgeSource`。这样可以先完成 Vision / Memory 垂直闭环，再抽象 source。**决策时机**：Sensor MVP 后、K-03 实现前。

### 6.3 Squad 是 BaseObject 子类还是纯 manager

候选：
- **A**：SquadObject : BaseObject —— 像兵一样在场景里有 id、可被 ObjectManager 遍历
- **B**：Squad 是 GameManager 持有的 manager，不进对象列表
- **C**：复合：Squad 数据放 BaseObject，逻辑在 SquadController 组件

**倾向 A**：跟 EC 化方向一致，可挂组件，可被 tolua 暴露。**决策时机**：Squad 实现前。

**落地顺序**：先做 TeamBlackboardService 和 team target sharing；只有当需要小队 BT、阵型、角色分配、可视化调试时，再引入 `SquadObject`。

### 6.4 Formation 数据驱动还是代码硬编码

候选：
- **A**：FormationType enum + C++ 算法计算每个 slot 位置
- **B**：Lua 配置文件描述每种阵型（slot 偏移 + 朝向）
- **C**：策划用编辑器拖出阵型，导出 JSON

**倾向 B**：跟 SoldierBT 一样走声明式配置，策划可改。**决策时机**：T-01 实现前。

### 6.5 Memory 衰减跟 Blackboard TTL 是否合并

候选：
- **A**：Memory 自己管衰减（独立 MemoryComponent::Tick）
- **B**：Memory 数据存进 bb，用 bb 的 TTL/Decay 机制自动衰减
- **C**：两者并存（不同类型数据走不同路径）

**倾向 B，但保留 MemoryComponent 领域接口**：MemoryComponent 对外提供 `GetLastKnownEnemy()` 这类查询；底层存储和过期策略优先交给 Blackboard TTL / Decay，避免两套状态源。**决策时机**：MemoryComponent / K-02 实现前。

### 6.6 Blackboard value 表达方式

候选：
- **A**：使用 C++17 `std::variant`
- **B**：使用项目兼容的 tagged value（`ValueType + fields`）
- **C**：继续只扩展现有 typed map，不新增统一 value

**倾向 B**：项目当前 VS2017 构建不应默认依赖 C++17 `std::variant`；tagged value 更容易暴露给 tolua，也方便 debug dump。**决策时机**：K-01 实现前。

### 6.7 Blackboard / Memory / Event 里的对象引用

候选：
- **A**：直接保存 `AgentObject*`
- **B**：保存 object id，使用时通过 ObjectManager 解析
- **C**：保存 handle + generation，解析时校验 generation

**倾向 B 起步，C 作为增强**：避免对象销毁后黑板、记忆或事件 payload 里残留悬挂指针。**决策时机**：K-01 / Memory MVP / S-01 实现前。

---

## 7. 验收与门禁

AI 能力容易出现"编译通过但表现漂移"，每个阶段必须有最小可复现验收。建议先补脚本门禁，再逐步接入完整 sample。

| 门禁 | 建议入口 | 验收标准 |
|---|---|---|
| Blackboard metadata selftest | `HELLO_AI_BLACKBOARD_SELF_TEST=1` 或专用 smoke | typed value / ttl / confidence / debug dump 行为稳定 |
| Event scope selftest | `HELLO_AI_EVENT_SELF_TEST=1` | Local / Team / Global / Spatial 派发路径正确，销毁后无残留 callback |
| Vision smoke | 固定 seed sample | 视野距离、FOV、遮挡、目标 id 写入符合预期 |
| Memory smoke | 固定 seed sample | 敌人离开视野后，AI 能移动到最后已知位置，TTL 到期后遗忘 |
| TeamBlackboard smoke | 固定 seed sample | 一个 agent 发现敌人后，同队 agent 行为改变 |
| Influence smoke | 固定 seed sample | AI 避开危险区域或偏好掩体区域，layer 更新成本可见 |
| Performance smoke | 复用 `ai_perf_smoke` | 20+ agent 下 AI tick / Sensor / Event / Influence 无固定帧 spike |

最低要求：

- 新增 C++ 文件后跑 VS2017 Debug x64 构建。
- 涉及 Lua 绑定时跑 `src\HelloOgre3D\tolua.bat` 并检查 Lua 调用点。
- 涉及行为表现时固定随机种子，记录 sample、入口参数和预期日志。
- 涉及调度 / 感知 / InfluenceMap 时接入 Tracy 或 `RuntimeProfileCounters` 摘要。

## 8. 待办与进度

实现一项打勾一项。新发现的设计点追加进 §6。

| # | 任务 | 类别 | 状态 | 阶段 |
|---|---|---|---|---|
| K-01 | Blackboard metadata / safe value | Knowledge | ☑ | Stage 3 |
| K-02 | Blackboard TTL + Confidence 衰减 | Knowledge | ☑ | Stage 3 |
| K-03 | IKnowledgeSource 接口 + multi-source 融合 | Knowledge | ☐ | Stage 3 |
| K-04 | LuaKnowledgeSource 包装 | Knowledge | ☐ | Stage 3 |
| S-01 | SandboxEventTypes + Team/Global 派发 | Sensor/Event | ☐ | Stage 3 |
| S-02 | Spatial scope 过滤 | Sensor/Event | ☐ | Stage 3 |
| S-03 | HearingSensor | Sensor | ☐ | Stage 3 |
| S-04 | DangerSensor | Sensor | ☐ | Stage 3 |
| P-01 | VisionSensor MVP | Perception | ☑ | Stage 3 |
| P-02 | MemoryComponent / MemoryStore MVP | Perception | ☑ | Stage 3 |
| P-03 | BT lastKnown investigation branch | Perception/BT | ☑ | Stage 3 |
| T-00 | TeamBlackboardService + team target sharing | Tactic | ☐ | Stage 3/4 |
| T-01 | Formation 系统 + Lua 配置 | Tactic | ☐ | Stage 4 |
| T-02 | 协作 BT 节点（CallForBackup/WaitForSquadMate/MoveToFormationSlot/FollowSquadOrder）| Tactic | ☐ | Stage 4 |
| T-03 | InfluenceMap 多层升级 | Tactic | ☐ | Stage 4 |
| T-04 | 分层 AI Faction 层 | Tactic | ☐ | Stage 4+ |

SquadObject / SquadController 是否进入下一阶段，取决于 TeamBlackboardService 与 team target sharing 的验收结果。

---

## 9. 阅读原书代码的边界

允许查阅 `E:\Workspace\HelloOgre3DX\src\chapter_[6-9]_*` 作为：
- **算法参考**（视觉 raycast + FOV 数值、bullet 事件类型清单等）
- **概念命名参考**（attribute 命名 visibleAgents / bulletShots / deadEnemies 等）
- **业务范围确认**（确认原书覆盖了哪些 use case）

**禁止**：
- 直接 copy Lua 代码到本项目
- 沿用原书的字符串 EventType、Lua-only Blackboard、IndirectSoldierAgent 全局模式
- 不经审视移植原书的 `Sandbox.GetAgents()` / `Animation.GetBonePosition()` 等 API 依赖

---

> **维护**：实现 §8 任务时回填进度。设计点（§6）拍板后直接更新本文档，不要散在 commit。新发现的设计差异 / 跟原书的取舍追加到对应 §1.4 / §2.4 / §3.4 "保留/抛弃/新增" 表。
