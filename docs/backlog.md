# Backlog（唯一活清单）

> **这是本仓库唯一带 `[ ]` / `[x]` 状态的清单。** 当前阶段的证据与复盘在 cycle 中记录，其它文档不维护任务勾选。
>
> - 方向定调看 `project-direction.md`（北极星，不含状态）。
> - 当前在做什么看 `cycle-01.md`（当前阶段，按证据复盘）。
> - P1–C5 / P8–P11 的**证据与方案**仍在 `design/architecture-improvement-plan.md` §7/§9；本文只记"做不做、什么时候做"，细节不复制。
> - 已归档的历史排期文档在 `archive/`，只作脉络参考，不看旧优先级。
>
> 规则：**候选池不排期**。想做某条时，把它拉进当前 cycle，做完在这里勾掉。不要在这里维护"下一步做什么"的顺序——那是 cycle 的事。

---

## 1. 当前 cycle 已认领

当前范围与验收见 `cycle-01.md`，设计见 `project-direction.md` §5 M1。M1 已按本轮运行证据完成；日志与限制见 cycle 的 M1 实施记录，M2 尚未认领。

- [x] M1：审计单单位身份、感知、记忆、指令、决策、执行的信息来源与可见缺口。
- [x] M1：复用 RuntimeDiag / BT trace / 意图表达，补齐解释行为所需的最小观察链。
- [x] M1：记录自主接敌、集火、目标丢失、指令过期四个过程，验证死亡/重开清理与相关回归。
- [x] 稳定性：修复可复现的重合分离力 NaN、回收 RuntimeDiag probe，补完整对局/重开与合成胜利回归入口；证据见 `stability-2026-09-06.md`。
- [x] 稳定性：限制 Sandbox19 双方自主低血量回避窗口，处理持续回避僵持；普通/probe/低血量压力各三局与重开通过，原失败保留，见 `stalemate-2026-09-06.md`。

## 2. 候选池

### 2.1 玩法 / 个人体验

- [x] Sandbox19 两轮体验优化：选择与指令反馈、结算说明、开阔接敌布局与实际出生点导航验收；见 `playtest-2026-09-06.md`、`playtest-visibility-2026-09-06.md`。通用镜头碰撞及持续移动手感未验收。

- [ ] 指挥策略个人对照实验：不下令、集火、撤退/编队、自由指挥；先固定条件，记录效果与个人体验，不要求指挥必胜。
- [ ] 直接操控实验：当兴趣转向操作与战斗反馈时认领命中、掩体或换弹中的一个问题。
- [ ] 可选外部试玩：有人愿意体验时记录反馈，不作为周期完成条件。

### 2.2 近期实验与可观测性

- [ ] M2：实验元数据、可恢复配置/代码版本、关键事件、成对运行摘要与超时分类；记录/隔离关卡导演干预。
- [ ] M2 按需扩展：语义指令录制回放，含仿真 tick、稳定场景单位映射与格式版本；出现必须重放的问题时认领。
- [ ] M3：Sandbox12 团队信息开启/关闭对照，证明发布、消费、直接视野隔离、事实过期与行为响应。
- [ ] M3 通过后按需接入 Sandbox19，检查玩家指令与自主团队行为的交接。
- [ ] AI driver 运行契约：`Pause / Resume / Reset`、driver/profile 身份；由具体观察、暂停或接管实验触发，不单独造框架。
- [ ] parity 验收口径调整：从逐帧复刻转为行为/统计比较；阈值基于实测分布，保留逐帧 trace 作为诊断。
- [ ] `run_chapter8_parity_gate.ps1` 降级为诊断工具；根因依据见 `memory/chapter8-parity-rootcause.md`。
- [ ] parity trace 正式化、Sandbox14/15 独立化、smoke 入 CI：在重复实验或回归维护需要时认领。

### 2.3 架构债（由具体实验或维护问题触发）

- [ ] **P8** component 反向依赖 `ObjectManager` / 具体 `*Object`，收口为 `SandboxServices` 窄接口 + 门禁规则。🔴 三条里最该先做的。
- [ ] **P10** 收窄 `BaseObject` typed getter 的领域污染。
- [ ] **P11** `SandboxEventDispatcherManager` 拆分，字符串 DSL 事件名下线。
- [ ] P2 尾巴：其它对象门面与跨组件语义入口审计；旧 driver/action 的 `u[SoldierObject]` Lua 兼容桥随 sample 迁移收窄。
- [ ] P4 尾巴：其它缓存裸指针继续标注 owning/non-owning。
- [ ] P5 尾巴：Agent profile 外部数据化、SoldierFactory 泛化、更完整非 Soldier 行为场景。

### 2.4 AI 专题（由具体问题或个人学习兴趣认领）

- [x] navigation：按 fixed geometry 自动收紧 navmesh 真实边界（去 2001 固定），并以 config + 变换后几何指纹缓存单 tile Detour 数据。
- [ ] navigation：多层/tiled navmesh、动态障碍。← 这是 AI 链路里剩余的明显薄弱项。
- [ ] navigation 请求治理：同步 `FindPath` 外围增加每帧预算/排队、短时结果复用和 repath 距离/时间阈值；先用战术指挥或 100+ agent 的真实 spike 证明需要，再进入技术 cycle。
- [ ] BT：G2 事件节点（依赖统一事件总线）、G3 参数运行时求值、G6 黑板类型扩展、跨 agent/template 级节点缓存。
- [ ] 感知：更多非视觉 sense、AOI / visibility set 淘汰、空间查询上限调参。
- [ ] 战术：更细粒度增量 rebuild、真实 cover 来源、更细 crowd 模型。
- [ ] 团队：更多按 key / 按角色的 fact 查询从 Lua 兼容缓存迁到 C++ getter。
- [ ] FSM：`RandomMoveState` 补全或弃用（行为变更，基线后做）。

### 2.5 性能

- [ ] `perceptionSystem` 1000 agent 21.8ms/帧——历史基线中的主要成本，固定新环境后再确定优化目标。
      先决条件：可比较的场景、版本、配置和重复测量；涉及输入时按需回放，不要求先完成通用录制系统。该数字仅为历史 Windows 基线，新平台先重测。
      基线：`perf/ai-perf-release-baseline-20260710.md`。

### 2.6 工具 / 运行时

- [ ] 定位 macOS 第二波的 Ogre NaN 位置断言：2026-09-05 改动前基线启用 RuntimeDiag 自测/probe 后出现，尚未证明普通模式必现，本轮已修复独立可复现的分离力 0/0，但尚未证明与历史崩溃同源；证据见 `stability-2026-09-06.md`。

- [ ] profiler UI、FairyGUI element inspector（`modules/runtime.md`）。
- [ ] FGUI：lua_bridge 自动生成、handle 池化、输入路由文档（`fgui/` 专项，非主线）。
- [ ] tolua 导出清单治理、回调生命周期清理流程。
- [ ] `PhysicsFactory` 多形状复合。
- [ ] 补 SMG reload 动画。

---

## 3. 默认不展开（明确专题需要时再激活）

下列方向不作为默认长期终点；需要时收敛为一个可验证的小实验，再移入候选池：

- 完整数据驱动 Def / CSV / CreatureAssembler
- 触发器运行时 / ECA / 触发器编辑器
- 可视化积木编辑器、UGC / Mod 打包
- 存档、世界 streaming
- 完整 ECS（热点组件走对象侧缓存指针，见 `design/architecture-improvement-plan.md` §9 P9）
- 无具体实验问题的平行 AI system；优先复用既有感知、团队、战术、BT 和调度
- 跨阶段详细日历排期；长期能力地图在方向文档维护，具体执行只认领当前问题
