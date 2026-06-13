# 触发器系统差异与补强分析（HelloOgre3D vs MiniGame）

> 目的：对比本项目 `HelloOgre3D` 与成熟沙盒 `MiniGame_Dev` 的**触发器/事件系统运行时实现**，找出为支撑“生物带触发器 + UGC 玩法 + 可视化配置”需要补的能力，给出优先级与落点。
>
> 方法：两边均**直接读源码**。本项目证据指向本仓库；MiniGame 证据指向其仓库（详见 `docs/reference/reference-minigame-patterns.md` §3，含 file:line）。本文与 `docs/design/behavior-tree-gap-analysis.md` 同构（BT 那份的姊妹篇）。
>
> 上位：整体优先级以 `docs/project-direction.md`（北极星）为准。本文 T-G1（事件总线）属北极星 L0-③（地基）、T-G2/G3/G5/G6 属 L1-⑥；其中事件总线 + 参数化事件名 + 区域事件源是“第一个垂直切片”的硬前置。
>
> 关联：补强项最终会以新任务并入 `docs/archive/visual-editor-task-breakdown.md` 的 E4（触发器编辑器）；触发器运行时是触发器可视化编辑器的硬前置。

---

## 1. 一句话结论

本项目**几乎没有“触发器系统”**，但事件层有起点（2026-05-29 核对纠正）：`sandbox/core/event/` 已有**全局单例事件总线骨架** `SandboxEventDispatcherManager`（`name→dispatcher`、`Subscribe(name,cb)->Token` / `Unsubscribe` / `Emit(name, SandboxContext)`，payload 已带 `eventType/scope/sender/target/team/timeMs` + 调试统计），目前主要用于 AI 的 `HEALTH_CHANGE/ASM_STATE_CHANGE/ASM_NOTIFY`。**缺的是**：缓冲跨帧分发 + 防递归、每帧节流、队列上限（“四件套”）、参数化事件名 `事件?objid=N`、scope 的实际 Local/Team/Global 路由、区域/碰撞/计时等事件源、“事件→条件→动作(ECA)”、触发器对象库、codegen。即**起点比“从零”高一截，但触发器主体仍需新建**。

要做到“生物带触发器 + UGC 玩法”，**触发器线路基本是从零搭**，但好处是可以直接抄 MiniGame 已验证的形状，且**不必照搬它的历史包袱**（四层并存、dev 子 VM 默认关闭、因子→积木迁移工具等都可跳过）。

> 关键认知（已实测纠正）：MiniGame 触发器**默认跑游戏主 VM**（`ENABLE_DEV_VM=0`），独立子 VM 是可选项、Python 才是真隔离。所以本项目第一版**用主 VM + `loadstring/setfenv` 沙盒即可**，隔离后置。

---

## 2. 现状对比

| 维度 | HelloOgre3D（现状） | MiniGame | 差距 |
|---|---|---|---|
| 事件机制 | 已有全局单例 `SandboxEventDispatcherManager`：`name→dispatcher` + Subscribe/Emit/Token 解绑 + 调试统计（**同步直发，无缓冲队列**） | 统一总线 `ObserverEventManager`：注册名表 + 缓冲队列 | 有骨架，**缺缓冲/节流/上限** |
| 事件层级 | 仅 Local（单 agent 内） | Local / Team / Global / AINPC 多级 | 缺 Team/Global |
| 事件源 | 仅 HEALTH_CHANGE / ASM_STATE_CHANGE / ASM_NOTIFY | 区域进出/碰撞/输入/属性变化/计时/触摸/UI… 数十种 | **缺绝大多数** |
| 派发策略 | 直接派发（同步） | 缓冲→下一帧 tick 分发 + 每帧上限(100/1000) + 队列保护 + 客机/裁判过滤 | **缺四件套** |
| 过滤器即订阅 | 无（按事件类型直发） | `m_numfilterevent/m_strfilterevent`，未注册过滤器的事件不进队列 | 缺（性能关键） |
| 参数化事件名 | 无 | `事件?actorid=N` 复用同一监听表实现对象级触发器 | **缺（生物带触发器靠它）** |
| 事件喂 AI | 无（事件不进 BT） | `OnTriggerEvent → BTManager::OnEvent` 同一事件喂触发器 + 行为树 | 缺（反应式接合点） |
| 触发器对象库 | 无 | 5 类可摆放对象(区域/位置/生物/道具/显示板)，C++ Manager + tolua struct + FlatBuffer + UUID 编码 | **缺整套** |
| 区域检测 | 无 | `SceneAreaRect` 包围盒扫描 + tickCount 差分判进出 | 缺 |
| ECA 执行 | 无 | codegen 成 Lua(条件短路/动作协程化/递归保护)，非解释执行 | **缺整套** |
| 编辑态→玩法态生成 | 无 | 编辑摆预览，玩法 `createLivesInWorld→spawnMob` 真实生成 | 缺 |
| 脚本隔离 | 主 VM | 默认主 VM（ENABLE_DEV_VM=0），子 VM/Python 可选 | 对齐（都不必先隔离） |
| API 落地桥 | tolua（GameToLua/SandboxToLua/RuntimeToLua） | scriptsupport.* C 桥(弱类型JSON) + Trigger.X→GameVM.*(tolua) 两道桥 | 本项目有 tolua，缺脚本桥层 |

---

## 3. 补强项（按优先级）

> 判据：对“生物带触发器 + 反应式 AI + 可视化配置”的必要性。**不照搬 MiniGame 全部**——它的因子系统、triggers2blocks 迁移、dev 子 VM、Python 隔离都可跳过。

### P0 —— 触发器线路的承重墙

#### T-G1. 统一事件总线（替代/扩展现有 Local 事件）
- **是什么**：一个 `EventBus`：`map<事件名, 监听者列表>` + 注册/注销(返回 token) + **缓冲→下一帧分发** + 每帧上限 + 队列保护 + **过滤器即订阅**（没人听的事件名不进队列）。
- **为什么 P0**：触发器、AI 反应、玩法规则全部依赖它。现有 Local 事件是它的雏形，但缺总线化、缺分帧、缺过滤器门禁。
- **怎么加（落点）**：在现有 `SandboxEventPayload`/dispatcher 上扩，或新建 `sandbox/events/EventBus.{h,cpp}`。复用现有“返回 token 供解绑、agent 销毁解绑”的好习惯。
- **照抄 MiniGame 的四件套**：缓冲队列(`m_triggerEventListBuffer`→`m_triggerEventList`)、每帧 cap、上限保护、过滤器订阅（见 reference §3.4）。

#### T-G2. 事件层级 + 参数化事件名
- **是什么**：Local / Team / Global 三级；对象级订阅用 `事件?objid=N`（复用同一监听表）。
- **为什么 P0**：**“生物带触发器”就是靠参数化事件名实现的**——挂到某生物的触发器 = 注册 `事件?objid=本生物id`。Team/Global 是 UGC 玩法（胜负/集火/全局指令）的基础。
- **怎么加（落点）**：`EventBus` 分发时按事件名后缀路由；发事件方按 (scope, 关联 id) 拼事件名。`SandboxEventPayload` 已有 `scope/senderId/targetId/teamId` 字段，可直接用。

#### T-G3. 基础事件源：区域进出 + 碰撞 + 计时
- **是什么**：至少补三类事件源——区域(TriggerVolume 进/出)、碰撞(被击/接触)、计时(定时器)。
- **为什么 P0**：这三类覆盖绝大多数“生物触发器”场景（进区域、被打、定时刷新）。
- **怎么加（落点）**：
  - 区域：新建 `sandbox/systems/TriggerVolume`（AABB/球，每帧或低频扫描区域内对象，tickCount 差分判进出 → 发 `Area.In?areaid=N` / `Area.Out`）。本项目已有 Bullet 物理，可用其碰撞查询或简单距离判定起步。
  - 碰撞/被击：把现有 `HEALTH_CHANGE` 这类纳入总线（已是 Local 事件，接入总线即可）。
  - 计时：`Timer` 事件源，到点发 `Timer.Fire?id=N`。

#### T-G4. 事件同时喂 AI 行为树
- **是什么**：`EventBus` 分发时，除触发器外，同一事件也喂给生物的 BT（对应 BT 补强 `docs/design/behavior-tree-gap-analysis.md` G2 事件节点）。
- **为什么 P0**：这是“生物对环境即时反应”的接合点（reference §3.4 的 `OnTriggerEvent → BTManager::OnEvent`）。
- **怎么加**：`EventBus` 增加“BT 事件入口”监听；BT 侧实现 `BehaviorEventNode`（与 BT G2 同一件事，协同做）。

### P1 —— UGC 玩法 / 可视化触发器需要

#### T-G5. ECA 运行时（先手写 Lua，再接 codegen）
- **是什么**：触发器逻辑承载层——一条触发器 = 注册事件 + 条件函数 + 动作函数。第一版直接手写 Lua（`OnEvent → if cond then action end`），后续接可视化编辑器的 codegen 产物。
- **怎么加（落点）**：`bin/res/scripts/runtime/trigger/TriggerRuntime.lua`：提供 `Trigger.Register{event=, condition=fn, action=fn, scope=, objid=}`，内部注册到事件总线。**照抄 MiniGame 的执行保护**：条件假短路 return、动作 pcall 隔离、（需要 wait 类动作时）协程化、递归保护。
- **沙盒**：UGC 作者脚本用 `loadstring + setfenv(白名单环境)`，**不必先做独立 VM**。

#### T-G6. 触发器动作/条件 API 层（Trigger.X）
- **是什么**：给触发器脚本用的稳定 API 门面（`Trigger.Unit:MoveTo`、`Trigger.World:Spawn`…），内部转调现有 tolua 绑定。
- **为什么**：把“作者能调的玩法操作”收口成稳定 facade，底层实现可换（对应 reference §7 的 `Trigger.X→GameVM.*`）。
- **怎么加**：纯 Lua 门面层 + 一张 `ApiProxy`（块ID→方法名）映射，复用现有 `FairyGuiRuntime`/Sandbox tolua API。

### P2 —— 编辑器/规模化时再补

- T-G7. 触发器对象库（5 类可摆放对象 + FlatBuffer 序列化 + UUID 编码 + 编辑预览/玩法生成）——做**可视化编辑器**时才需要；纯脚本触发器阶段可跳过。本项目当前无 FlatBuffer，可先用 Lua 表/JSON 序列化。
- T-G8. 编辑态→玩法态对象生成链（createLivesInWorld 等）——同上，编辑器阶段。
- T-G9. 生物组（CreatureGroup）批量触发——多生物玩法成熟后。
- T-G10. 脚本隔离升级（独立子 VM / 防作弊）——UGC 对外开放时再做。

---

## 4. 与可视化编辑器路线的衔接

触发器编辑器（`visual-editor-task-breakdown.md` E4）的硬前置就是本文 P0：

| 编辑器能力（E4） | 依赖的触发器补强 |
|---|---|
| 触发器能注册并被触发 | T-G1（事件总线）+ T-G5（ECA 运行时） |
| 对象级触发器（挂到生物） | T-G2（参数化事件名） |
| 事件积木（进区域/被击/计时） | T-G3（事件源） |
| 生物对触发同时驱动 AI | T-G4（事件喂 BT） |
| 动作积木落地 | T-G6（Trigger.X API） |
| 摆放触发器对象（区域/点位/生物） | T-G7/T-G8（对象库，编辑器阶段） |

**顺序建议**：E4 启动前必须先有 **T-G1 + T-G2 + T-G3 + T-G5**；T-G7/G8（对象库）可与编辑器画布并行。

---

## 5. 落地顺序建议（触发器侧）

> 与 BT 补强、可视化 E0/E1 可并行；与 BT 的 G2（事件节点）共享 T-G1 总线。

1. **T-G1 统一事件总线**（在现有 Local 事件上扩四件套，最基础）。
2. **T-G2 参数化事件名 + Local/Team/Global 分级**（解锁“生物带触发器”）。
3. **T-G3 区域/碰撞/计时三类事件源**（TriggerVolume 先做，覆盖最多场景）。
4. **T-G5 ECA 运行时（手写 Lua + 沙盒）** + **T-G6 Trigger.X API 门面**（让触发器能落地玩法操作）。
5. **T-G4 事件喂 BT**（与 BT G2 事件节点协同）。
6. T-G7/G8 触发器对象库与玩法生成（做可视化编辑器时）。
7. T-G9 生物组、T-G10 隔离升级，按需。

---

## 6. 不照搬 MiniGame 的部分（避免背包袱）

- **因子(factor)系统 + triggers2blocks 迁移**：那是 MiniGame 为兼容历史(经典因子→积木→组件积木三代)留的迁移层。本项目新做，**直接用一套数据格式**，不要因子层。
- **dev 子 VM / Python 隔离**：默认主 VM + loadstring 沙盒即可，隔离后置。
- **四层并存(因子/积木/Trigger.X/services)**：本项目收敛成两层——**触发器脚本(可手写或编辑器生成) + Trigger.X API 门面**，别一上来分四层。
- **FlatBuffer 序列化**：本项目无此依赖，触发器/对象库先用 Lua 表/JSON 存，够用且零依赖。

---

## 7. 验证

- 事件总线：单测式 sample——注册一个 `Area.In?objid=N` 监听，触发后回调命中、agent 销毁后不再回调、未注册事件不进队列。
- 区域触发：在 `Sandbox` 放一个 TriggerVolume，agent 进/出时各触发一次。
- 生物带触发器：给某生物挂“被击→反击”触发器，验证只有该生物响应（参数化事件名隔离）。
- 事件喂 BT：生物巡逻中被击，BT 事件节点收到并切到战斗（与 BT G2 联调）。
- 不破坏现有 AI Local 事件链路（回归 `Sandbox6/7/8`）。

---

## 8. 关键证据索引

**HelloOgre3D（本仓库）**：
- `sandbox/...` 现有 Local 事件（`SandboxEventPayload` + dispatcher，覆盖 HEALTH_CHANGE/ASM_STATE_CHANGE/ASM_NOTIFY，返回 token 解绑）——见 `docs/planning/ai-roadmap.md` §5 当前进展记录。
- 现状：有同步 dispatcher 骨架（`SandboxEventDispatcherManager`），但**无生产级跨对象事件总线**（缺缓冲/节流/上限/scope 路由/参数化事件名）/ 无 Team-Global 路由 / 无区域-碰撞-计时事件源 / 无 ECA / 无触发器对象库。

**MiniGame（外部参考，见 `reference-minigame-patterns.md` §3）**：
- `ObserverEventManager.cpp`（总线四件套 :55-122,137-155,209-243；喂 BT :230-234）
- `ScriptSupportEvent.cpp`（参数化事件名 `%s?actorid=%d` :263-299）
- `SceneArea.cpp`（区域包围盒扫描 + tickCount 差分判进出 :437-455,62-111；areaid 过滤 :113-329）
- `TriggerScriptMgr.cpp`（scriptsupport.* C 桥 :408-485；两道桥）
- `ScriptSupportTrigger.lua`（ECA codegen：EventMain/ConditionMain/ActionMain、条件短路、协程化、递归保护 :54-231,593-636）
- `TriggerLivesToolMgr.cpp` / `TriggerObjLibManager.cpp`（触发器对象库、UUID 编码、编辑预览→玩法 createLivesInWorld :1347-1453）
- `ClientMacrosConfig.h:32` + `ClientApp.cpp:1783-1787`（ENABLE_DEV_VM=0，默认跑主 VM）
