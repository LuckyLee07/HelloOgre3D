# HelloOgre3D 结构重构方案

> **定位**：本文档是**结构性重构的参照清单**，跟 [IterationPlan.md](IterationPlan.md) 互补——后者讲"分阶段路线 + 性能 / AI 演化"，本文档讲"目录与代码组织本身的问题与具体重构动作"。重构迭代时**先查本文档对应条目**，按"现状 → 目标 → 落地动作 → 风险 → 工时 → 所属 Stage"对照执行。
>
> **使用规则**：
> 1. 每条问题独立条目，独立 PR，独立可回滚。
> 2. 改动前先看"风险"与"双轨过渡"策略，不允许跨条目混改。
> 3. 每条做完打勾，并在 AGENTS.md 同步规则。
> 4. 新发现的结构问题追加到本文档，不要散落在 commit message。

---

## 0. 问题速查表

| # | 问题 | 严重度 | 所属 Stage | 工时 |
|---|---|---|---|---|
| **目录层次** ||||
| S-01 | 两个 components 目录并存且位置反 EC 思路 | 高 | Stage 1 | 半天-1 天 |
| S-02 | 脚本 / 绑定散在 4 个目录 | 高 | Stage 1 | 半天 |
| S-03 | OpenSteer 三方库适配挂在业务目录下 | 中 | Stage 1 | 1 小时 |
| S-04 | 单文件子目录过度划分（`core/component/`、`core/script/`）| 低 | Stage 1 | 15 分钟 |
| S-05 | `systems/ui/` 命名过宽 + UIService 是空转发壳 | 低 | Stage 1 | 1 小时 |
| S-06 | `systems/service/` 混了 Factory 和 Service | 低 | Stage 2 | 半天 |
| S-07 | `client/` 与 `game/` 边界模糊 | 低 | Stage 4 | — |
| S-08 | `sandbox/` 命名遗留（实质是引擎本体） | 极低 | 不做 | — |
| **横向耦合 / 重复** ||||
| S-09 | `RenderableObject` ↔ `RenderComponent` 重复 | 中 | Stage 3 | 1 天 |
| S-10 | `BaseObject` ↔ `GameObject` wrapper pattern | 中 | Stage 4 | 2-3 天 |
| S-11 | FSM / DT / BT 三套 driver 生命周期重复 | 高 | Stage 2 | 1-2 天 |
| S-12 | `AnimController` / `AnimProfile` 硬绑 SoldierObject | 中 | Stage 3 | 2 天 |
| S-13 | Lua 条件库（SoldierEvaluators ↔ SoldierConditions）重复 | 中 | Stage 2 | 半天 |
| S-14 | 四种 SoldierAgent Lua 入口无共同基类 | 中 | Stage 2 | 半天 |
| **核心类肥大 / 字段冗余** ||||
| S-15 | `SoldierObject` 716 行扛 9+ 职责 | 高 | Stage 1+3 | 多次推进 |
| S-16 | 组件类内部缓存自己的 owner（与 IComponent::m_gameobj 重复）| 低 | Stage 1 | 1 小时 |
| S-17 | `AgentLocomotion::m_owner` 类型绑死 `VehicleObject*` | 中 | Stage 3 | 半天 |

---

## 1. 一级目录层次重组

### S-01 / S-02 / S-03 — 一级目录定位（结合 sandbox 内部统一处理）

#### 一级目录最终定位（写进 AGENTS.md）

```
src/HelloOgre3D/
  client/      应用入口（Application、main、OS 桥接）
  common/     纯工具（FileManager、LogSystem、Singleton、GlobalFuncs、ObfuscatedZip 等）
                ← 注意：Lua VM 不属于这里
  game/       游戏循环顶层协调（ClientManager、GameManager、AppConfig）
                ← 注意：Lua 绑定不属于这里
  runtime/    三方库适配层（Ogre / Tracy / OpenSteer / Bullet 适配）
                ← 凡是封装外部库的 thin wrapper，都进这里
  sandbox/    引擎本体（对象 / 组件 / AI / 系统 / 脚本绑定）
```

#### 判定规则

| 代码性质 | 归属 |
|---|---|
| OS / 应用入口 | `client/` |
| 纯工具，跨模块复用，无业务 | `common/` |
| 游戏帧循环 / 顶层 manager | `game/` |
| 直接 include 某个三方库头并做薄封装 | `runtime/<lib>/` |
| 其他所有引擎模块 | `sandbox/` |

---

## 2. sandbox/scripting 收口（S-02）

### 现状

```
common/ScriptLuaVM.{cpp,h}              ← Lua VM 本体
common/LuaInterface.{cpp,h}             ← Lua 工具
sandbox/core/script/LuaClassNameTraits.h
sandbox/scripting/SandboxToLua.{cpp,pkg}, ManualToLua.cpp, LuaPluginMgr.h, SandboxForTypes.pkg, tolua.bat
game/GameToLua.{cpp,pkg}
game/externalToLua.pkg
```

### 目标结构

```
sandbox/scripting/
  vm/
    ScriptLuaVM.{cpp,h}        ← 从 common/ 挪
    LuaInterface.{cpp,h}       ← 从 common/ 挪
  binding/
    SandboxToLua.{cpp,pkg}
    SandboxForTypes.pkg
    ManualToLua.cpp
    GameToLua.{cpp,pkg}        ← 从 game/ 挪
    externalToLua.pkg          ← 从 game/ 挪
  traits/
    LuaClassNameTraits.h       ← 从 core/script/ 挪
  plugin/
    LuaPluginMgr.h
  tolua.bat
```

### 落地动作

1. 新建子目录 `vm/`、`binding/`、`traits/`、`plugin/`。
2. 文件迁移（按上表）。
3. 修复 include 路径，影响面：
   - 所有 `#include "ScriptLuaVM.h"`、`#include "LuaInterface.h"`、`#include "LuaClassNameTraits.h"` 都改路径。
   - GameToLua 在 `GameToLua.pkg` 内有指向源码的相对路径，迁移后要同步改。
4. 改 premake 脚本（[premake/premake.lua](../premake/premake.lua)、[samples.lua](../premake/samples.lua)）通配规则。
5. `core/script/` 目录清空后删除。

### 双轨过渡策略

迁移文件后**保留旧位置的 forwarding header**：
```cpp
// common/ScriptLuaVM.h（旧位置）
#pragma once
#include "scripting/vm/ScriptLuaVM.h"   // forward
```
全部 include 修正后再删除 forwarding header。

### 风险

- tolua 生成代码里有路径注释，可能需要重生成 `SandboxToLua.cpp` / `GameToLua.cpp`。
- premake 通配未更新会导致编译期文件找不到，调整后立刻全量重编。

### 工时：半天

### 所属：Stage 1

---

## 3. sandbox/components 归并（S-01）

> 详细已在 [IterationPlan.md 任务 1.1](IterationPlan.md) 展开，这里仅给重构者用的快速参考。

### 目标结构

```
sandbox/components/
  render/    RenderComponent             ← 让它继承 IComponent
  physics/   PhysicsComponent
  agent/     AgentBody / AgentAttrib / AgentLocomotion / AgentPath
  combat/    WeaponComponent
  (Stage 3+) perception/ skill/ inventory/ stats/
```

### 文件迁移清单（建议按此表逐行 grep + 修 include）

| 旧位置 | 新位置 |
|---|---|
| `sandbox/components/AgentAttrib.{h,cpp}` | `sandbox/components/agent/AgentAttrib.{h,cpp}` |
| `sandbox/components/AgentBody.{h,cpp}` | `sandbox/components/agent/AgentBody.{h,cpp}` |
| `sandbox/components/WeaponComponent.{h,cpp}` | `sandbox/components/combat/WeaponComponent.{h,cpp}` |
| `sandbox/objects/components/RenderComponent.{h,cpp}` | `sandbox/components/render/RenderComponent.{h,cpp}` |
| `sandbox/objects/components/PhysicsComponent.{h,cpp}` | `sandbox/components/physics/PhysicsComponent.{h,cpp}` |
| `sandbox/objects/components/AgentLocomotion.{h,cpp}` | `sandbox/components/agent/AgentLocomotion.{h,cpp}` |
| `sandbox/objects/steer/AgentPath.{h,cpp}` | `sandbox/components/agent/AgentPath.{h,cpp}` |

`sandbox/objects/components/` 与 `sandbox/objects/steer/` 迁完后整目录删除。

### 配套接口对齐

- `RenderComponent` 改为 `: public IComponent`。
- `AgentAttrib / WeaponComponent / AgentBody` 改为 `: public IComponent`，删除自己重复声明的 `onAttach/onDetach/start/update`。
- `PhysicsComponent` / `AgentLocomotion` 清理冗余 `m_owner`（S-16）。

### 分类契约（写进 AGENTS.md）

- 新建组件**永远**放 `sandbox/components/<域>/`。
- 子目录命名规则：**业务域名词**（render / physics / agent / combat / perception / skill / inventory / stats / ai / network …）。
- 归属判定：看**领域概念**，不看"业务还是引擎"性质。
- 组件**必须**继承 `IComponent`。

### 工时：半天-1 天

### 所属：Stage 1

---

## 4. OpenSteer 适配挪位（S-03）

### 现状

`sandbox/objects/steer/OpenSteerAdapter.{cpp,h}` 是 OpenSteer 三方库的薄适配——跟 `runtime/ogre/`、`runtime/profiling/` 同性质，但位置错了。

### 目标

```
runtime/
  ogre/      OgreCameraController.h
  profiling/ Profile.h
  steer/     OpenSteerAdapter.{cpp,h}   ← 新增
```

注：`AgentPath` 是业务层路径表示（基于 OpenSteer 的 PolylinePathway 但语义业务化），归 `sandbox/components/agent/`，见 S-01。

### 落地动作

1. `mkdir runtime/steer/`。
2. 移 `OpenSteerAdapter.{cpp,h}` → `runtime/steer/`。
3. include 修复：grep `#include.*OpenSteerAdapter`，大约 3-5 处。
4. premake 同步。
5. `sandbox/objects/steer/` 清空后删除。

### 工时：1 小时

### 所属：Stage 1

---

## 5. 单文件子目录扁平化（S-04）

### 现状

`sandbox/core/component/` 只装一个 `IComponent.{cpp,h}`；`sandbox/core/script/` 只装一个 `LuaClassNameTraits.h`。

### 目标

- `core/component/IComponent.{cpp,h}` → `core/IComponent.{cpp,h}`
- `core/script/LuaClassNameTraits.h` → 跟 S-02 一起进 `sandbox/scripting/traits/LuaClassNameTraits.h`

`core/event/`（4 文件）与 `core/object/`（4 文件）保留子目录。

### 落地动作

1. 移文件。
2. 修 include（grep `core/component/IComponent`、`core/script/LuaClassNameTraits`）。
3. 删空子目录。

### 工时：15 分钟

### 所属：Stage 1

---

## 6. systems/ui → systems/hud + 删 UIService（S-05）

### 现状

- `systems/ui/UIManager` + `UIFrame` 实际是 Gorilla 库做的 2D HUD overlay。
- `systems/service/UIService` 是 UIManager 的 thin wrapper，两个方法都是无附加逻辑的转发。

### 目标

```
sandbox/systems/hud/
  UIManager.{cpp,h}
  UIFrame.{cpp,h}
```

`UIService.{cpp,h}` 删除，调用点直接调 UIManager。

### 落地动作

1. `mv systems/ui/ → systems/hud/`。
2. 修 include：~6 处（GameManager / SandboxMgr / SandboxToLua）。
3. 删 `systems/service/UIService.{cpp,h}`。
4. 调用点改：`m_pUIService->CreateUIFrame(i)` → `m_pUIManager->CreateUIFrame(i)`（通过 `GameManager::GetUIManager()` 或类似 accessor）。
5. tolua 绑定改：`UIService` 暴露面挪到 `UIManager`，重生成。
6. AGENTS.md 写明：HUD 模块边界——基于 Gorilla 的 2D 信息叠加层，未来加菜单系统建 `systems/menu/` 平级。

### 风险

- UIService 是否被 Lua 直接引用？grep 确认。如果是，Lua 调用点改为 UIManager 时要同步改。

### 工时：1 小时

### 所属：Stage 1

---

## 7. systems/service 整理（S-06）

### 现状

```
systems/service/
  ObjectFactory      ← Factory
  PhysicsFactory     ← Factory
  SceneFactory       ← Factory
  CameraService      ← Service
  UIService          ← Service（S-05 删除）
```

### 目标（两种方案，二选一）

**方案 A：拆分**
```
systems/factory/   ObjectFactory / PhysicsFactory / SceneFactory
systems/services/  CameraService（+ 未来的 InputService 等）
```

**方案 B：统一为 services**
```
systems/services/  ObjectFactory / PhysicsFactory / SceneFactory / CameraService
```
按使用方角度都是"服务"，工厂也是工厂服务。

### 推荐：方案 B

简单，少一个目录概念。AGENTS.md 写明"工厂也算 service，凡 SandboxMgr 注入的协作类放这里"。

### 落地动作

1. `mv systems/service/ → systems/services/`（S-05 后 UIService 已删）。
2. 修 include。
3. premake 同步。

### 工时：半天

### 所属：Stage 2

---

## 8. AI driver 共性下沉（S-11）

### 现状

`DecisionTreeDriver` / `BehaviorTreeDriver` / `AgentStateController`(FSM) 各自实现：
- 一份 Blackboard（已经在 Stage 1 抽到 AIController）
- 一份生命周期（Init / Tick）
- 一份 debug trace 接口
- 一份节点工厂模式

### 目标

`IDecisionDriver` 基类承担共性，子 driver 只实现"节点结构差异"：

```cpp
class IDecisionDriver {
public:
    virtual void Init();
    virtual void Tick(float dt) = 0;
    void SetDebugTraceEnabled(bool);
    bool IsDebugTraceEnabled() const;
    const std::string& GetLastDebugTrace() const;
    void AttachBlackboard(Blackboard* bb);   // AIController 注入
    // ...
protected:
    Blackboard* m_bb = nullptr;
    bool m_traceEnabled = false;
    std::string m_lastTrace;
};
```

### 落地动作

1. 提取 `IDecisionDriver` 默认实现（cpp 不再 pure header）。
2. 三个子 driver 删除重复字段，调用基类逻辑。
3. AIController 通过 `SetDriver(d)` 后调 `d->AttachBlackboard(GetLocalBb())`。
4. Trace 设施统一，Sandbox6（FSM）也能看 trace。

### 风险

- 三套 driver 的 Init 时序不同，FSM 在构造时已经走过 Init；要保持构造语义。
- BT trace 当前已经在 BehaviorTreeDriver 内做，下沉要保证 trace 输出格式不变。

### 工时：1-2 天

### 所属：Stage 2

---

## 9. AnimController / AnimProfile 接口化（S-12）

### 现状

`SoldierAnimController`、`SoldierAnimProfile`、`SoldierAnimTypes` 全部硬绑 `SOLDIER_STATE` 枚举和 SoldierObject 类型。新对象类型（Monster / NPC）要复用 ASM 必须复制整套。

### 目标

```cpp
class IAnimController {
public:
    virtual void OnBodyStateChanged(int stateId) = 0;
    virtual void OnBodyNotify(const std::string& evt, int stateId, float t) = 0;
    virtual void Update(float dt) = 0;
};

class IAnimProfile {
public:
    virtual int GetStateIdByName(const std::string& name) const = 0;
    virtual std::string GetStateNameById(int stateId) const = 0;
    virtual int MapIntentToState(...) const = 0;
};
```

`SoldierAnimController / Profile` 实现这两个接口；未来 `MonsterAnimController / Profile` 另起。

`AgentObject` 持有 `IAnimController*` 而不是 SoldierAnimController*。

### 文件挪位（接口化完成后）

- 接口：`sandbox/components/anim/IAnimController.h`、`IAnimProfile.h`、`AgentAnimStateMachine.{cpp,h}`（通用 ASM）
- Soldier 实现：留在 `sandbox/objects/animation/`（业务实现）或挪 `sandbox/components/anim/soldier/`（按对象类型分子目录）

### 工时：2 天

### 所属：Stage 3

---

## 10. RenderableObject 与 RenderComponent 合并（S-09）

### 现状

两者职责完全重叠——都封装 SceneNode + Entity + 位置 / 旋转操作。`BlockObject` / `AgentObject` 都各持一个 `RenderableObject`，而 `objects/components/RenderComponent` 是新引入的等价物。

### 目标

合并为 `sandbox/components/render/RenderComponent`，`RenderableObject` 删除。所有持有它的对象改成持有 `RenderComponent`（通过 BaseObject 组件容器）。

### 难点

`RenderableObject` 暴露给 Lua 的 API 广泛（位置、朝向、动画、姿态），合并涉及 tolua 绑定大改。

### 落地动作（分两步）

1. **Stage 3 第 1 步**：让 `RenderComponent` 接口对齐 `RenderableObject`，所有 Lua API 都能在 RenderComponent 上调用。
2. **Stage 3 第 2 步**：把 SoldierObject / BlockObject / AgentObject 改为通过 BaseObject 拿 RenderComponent，删除 `m_pAgentBody`、`m_pBody` 等成员。
3. **Stage 3 第 3 步**：删除 `RenderableObject.{cpp,h}`，重生成 tolua。

### 工时：1 天

### 所属：Stage 3

---

## 11. BaseObject ↔ GameObject wrapper（S-10）

### 现状

`BaseObject` 持有 `GameObject* m_pGameObjet`，且 `BaseObject::AddComponent / GetComponent` 是 thin forwarder 到 GameObject。两层包装两套接口，让新人困惑。

> 2026-05-27：S-10 已按方案 B 完成。`GameObject` 类已删除，组件容器并入 `BaseObject`，`IComponent::onAttach` 改为接收 `BaseObject*`。

### 目标

二选一：

**方案 A**：`BaseObject` 直接继承 `GameObject`（合并继承）。

**方案 B**：把组件容器从 GameObject 提到 BaseObject，删除 GameObject。

### 推荐：方案 B

GameObject 在 Unity 里是 entity 概念，但这里的 GameObject 是组件容器实现，名字让人误以为它就是实体。删掉它，让 BaseObject = entity，更直接。

### 落地动作

1. 把 GameObject 的组件 map / Add / Get / Update 全部并入 BaseObject。
2. 删除 GameObject 类。
3. Lua 侧 `BaseObject:GetComponent(...)` 接口保持不变。
4. tolua 重生成。（若 `tolua_begin` 暴露面未变化，可仅做编译和运行验收。）

### 风险

- BaseObject 已经被广泛继承（SandboxObject / AgentObject / SoldierObject / VehicleObject / BlockObject），改动面大。
- 双轨过渡：先在 BaseObject 加一份新接口，确认全部调用方迁完，再删 GameObject。

### 工时：2-3 天

### 所属：Stage 4

---

## 12. SoldierObject 瘦身（S-15）

### 现状

[SoldierObject](../src/HelloOgre3D/sandbox/objects/SoldierObject.cpp) 716 行，承担：阵营、姿态、武器、弹药、敌人、输入、动画控制、3 套 driver、事件分发。新加任何角色相关能力都往这里塞。

### 分阶段瘦身

| Stage | 迁出的职责 | 目标位置 |
|---|---|---|
| Stage 1 | AI 决策（m_driver、敌人、移动目标、bb） | AIController |
| Stage 2 | 输入（m_inputInfo） | InputComponent 或保留 |
| Stage 3 | 武器（m_pWeapon、ammo、shoot 逻辑） | WeaponComponent |
| Stage 3 | 属性（health、stance、maxHealth） | AgentAttrib + StanceComponent |
| Stage 3 | 动画驱动（m_animController） | AnimController 接口化后挂组件 |
| Stage 4 | 整体改为 Entity 装配工厂 | EC 化 |

### 目标行数

| 阶段后 | SoldierObject 行数目标 |
|---|---|
| Stage 1 后 | < 500 |
| Stage 2 后 | < 400 |
| Stage 3 后 | < 200（基本只剩 ApplyCommand 翻译） |
| Stage 4 后 | 删除，改 SoldierFactory 装配 Entity |

### 工时：分散在多个 Stage

### 所属：跨 Stage 1-4

---

## 13. Lua 重复（S-13 / S-14）

### S-13 条件库合并

#### 现状

[SoldierEvaluators.lua](../bin/res/scripts/ai/decision/SoldierEvaluators.lua)（DT，返 1/2）和 [SoldierConditions.lua](../bin/res/scripts/ai/behavior/SoldierConditions.lua)（BT，返 bool）逻辑完全相同。

#### 目标

```
bin/res/scripts/ai/common/AgentConditions.lua   ← 唯一真源，返 bool
bin/res/scripts/ai/decision/SoldierEvaluators.lua  ← thin wrapper：cond and 1 or 2
bin/res/scripts/ai/behavior/SoldierConditions.lua  ← thin wrapper：直接转发
```

#### 工时：半天

#### 所属：Stage 2

---

### S-14 SoldierAgent 共享基类

#### 现状

四个 agent 入口脚本各自重复动画 ASM 初始化、武器初始化、属性配置：
- [SoldierAgent.lua](../bin/res/scripts/agent/SoldierAgent.lua)
- [BehaviorSoldierAgent.lua](../bin/res/scripts/agent/BehaviorSoldierAgent.lua)
- [DecisionSoldierAgent.lua](../bin/res/scripts/agent/DecisionSoldierAgent.lua)
- [IndirectSoldierAgent.lua](../bin/res/scripts/agent/IndirectSoldierAgent.lua)

#### 目标

```lua
-- agent/SoldierAgentBase.lua
-- 提供共享方法：SetupAnimation / SetupWeapon / SetupAttributes / Agent_Update 默认实现
SoldierAgentBase = class.Class("SoldierAgentBase")
function SoldierAgentBase:SetupAnimation(...) ... end
function SoldierAgentBase:SetupWeapon(...) ... end
function SoldierAgentBase:SetupAttributes(...) ... end

-- 四种变体只覆写 SetupDriver
BehaviorSoldierAgent = class.Class("BehaviorSoldierAgent", SoldierAgentBase)
function BehaviorSoldierAgent:SetupDriver() ... end
```

#### 工时：半天

#### 所属：Stage 2

---

## 14. 组件 owner 字段冗余（S-16 / S-17）

### S-16 与基类重复

#### 现状

```cpp
class AgentLocomotion : public IComponent {
    // IComponent 已有 BaseObject* m_owner
    AgentObject* m_owner = nullptr;  ← 重复 + 类型仍偏窄
};
```

#### 目标

删除子类的 `m_owner`，统一用 `IComponent::getOwner()` 取得宿主 `BaseObject`；需要更窄类型时在局部 `dynamic_cast` 或由组件初始化时缓存，但不再重复维护通用 owner 字段。

#### 工时：1 小时

#### 所属：Stage 1（跟 S-01 components 归并一起做）

---

### S-17 AgentLocomotion::m_owner 类型绑死

#### 现状

`AgentLocomotion::m_owner` 是 `VehicleObject*`——隐含"Locomotion 只能给 Vehicle 用"，但 Soldier 也用 Locomotion。

#### 目标

类型泛化到 `AgentObject*` 或更通用，把 `VehicleObject` 特有的调用挪到外层或通过接口。

#### 工时：半天

#### 所属：Stage 3

---

## 15. 不做的事（明确拒绝）

| 项 | 为什么不做 |
|---|---|
| `sandbox/` 改名 `engine/` / `core/` | 名义改动，tolua 命名引用面太大，收益小。AGENTS.md 注释历史遗留即可 |
| `client/` + `game/` 立即合并 | 当前能工作，合并的迁移成本高于结构收益；Stage 4 EC 化时再做 |
| `common/` 内部分子目录 | 9 个文件分组也就 3-4 类，分了反而让 #include 变长；纯工具保持扁平 |
| `ClientManager` 改名 | 跟 Lua 引用、tolua 命名绑死，改名牵连巨大 |
| 引入完整 ECS（archetype / SoA / SystemScheduler） | 当前规模用不到，引入复杂度远超收益 |
| 用 LuaJIT 替换 Lua 5.1 | 性能瓶颈在 C↔Lua 边界次数，不在 Lua 执行 |
| 重写 AgentAnimStateMachine | 通用 ASM 是工作的，只补接口化即可 |
| 网络同步层 | 当前单机沙盒，无需求 |

---

## 16. 重构操作的统一原则

### 16.1 双轨过渡

涉及文件迁移的重构，**先新增不删旧**：旧位置保留 forwarding header，确认全部调用方迁移完再删原文件。

```cpp
// 旧位置 sandbox/objects/components/RenderComponent.h
#pragma once
#include "components/render/RenderComponent.h"  // forward to new location
```

### 16.2 premake 同步

每次目录改动后立刻：
1. 改 [premake/premake.lua](../premake/premake.lua) 和 [samples.lua](../premake/samples.lua) 的 `files` 通配。
2. 全量重新生成工程 (`vs2022.bat` 或对应平台)。
3. 全量重编验证。

### 16.3 include 路径修复策略

1. **grep 出所有引用**：`grep -rn "#include.*<旧文件名>" src/ bin/`。
2. **批量改**：用编辑器多文件替换。
3. **编译验证**：每一类文件迁完编一次，错误及早暴露。

### 16.4 tolua 重生成

涉及 tolua 绑定改动（类位置变 / 接口变）的重构：
1. 改 `.pkg` 文件里的 `$cfile` 路径。
2. 跑 `tolua.bat` 重新生成 `SandboxToLua.cpp` / `GameToLua.cpp`。
3. 检查生成结果，确认 `tolua_pushusertype(..., "ClassName")` 里类名没漏。

### 16.5 AGENTS.md 同步

每完成一项结构重构，更新 [AGENTS.md](../AGENTS.md)：
- 目录新增 / 改名 / 删除 → 更新目录树和职责说明。
- 命名规则变更 → 更新规则段。
- 引入的新接口（如 IAnimController）→ 加到接口契约段。

### 16.6 验收清单

每项重构 PR 必须满足：
- [ ] 全量编译通过（所有 sandbox 含 Sandbox1-8）
- [ ] Sandbox6/7/8 行为表现跟改动前一致（回归基线）
- [ ] AGENTS.md 同步更新
- [ ] 本文档对应条目打勾
- [ ] forwarding header 在确认调用方迁完后删除

---

## 17. 与 IterationPlan 的映射关系

| 本文档条目 | 对应 IterationPlan 任务 | 状态 |
|---|---|---|
| S-01 components 归并 | Stage 1 任务 1.1 | 已在 IterationPlan 展开 |
| S-02 / S-03 / S-04 / S-05 / S-16 | Stage 1 任务 1.1b（建议追加）| 待加入 IterationPlan |
| S-06 | Stage 2 顺手做 | 待加入 IterationPlan |
| S-11 | Stage 2 任务 2.1 | 已在 IterationPlan |
| S-12 | Stage 3 任务 3.5 | 已在 IterationPlan |
| S-09 | Stage 3 EC 化时一并做 | 已在 IterationPlan |
| S-10 / S-15 收尾 | Stage 4 任务 4.1 | 已在 IterationPlan |
| S-13 | Stage 2 任务 2.2 | 已在 IterationPlan |
| S-14 | Stage 2 任务 2.4 | 已在 IterationPlan |
| S-17 | Stage 3 任务 3.x | 待补充 |

---

## 18. 进度追踪

完成的条目在表格末尾标注完成日期与 commit。

| # | 完成日期 | Commit | 备注 |
|---|---|---|---|
| S-01 | | | |
| S-02 | | | |
| S-03 | | | |
| S-04 | | | |
| S-05 | | | |
| S-06 | | | |
| S-09 | | | |
| S-10 | | | |
| S-11 | | | |
| S-12 | | | |
| S-13 | | | |
| S-14 | | | |
| S-15 | | | |
| S-16 | | | |
| S-17 | | | |

---

> **维护**：本文档随每条重构完成更新表格 18，并把对应条目打勾。新发现的结构问题追加为 S-18 / S-19 …，并在表 0 顶端的速查表加一行。改动方案有调整时直接修对应条目，不要新建。
