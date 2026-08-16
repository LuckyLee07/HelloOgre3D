# Backlog（唯一活清单）

> **这是本仓库唯一带 `[ ]` / `[x]` 状态的清单。** 其它文档一律不再记录进度。
>
> - 方向定调看 `project-direction.md`（北极星，不含状态）。
> - 当前在做什么看 `cycle-01.md`（当前周期，四周一轮）。
> - P1–C5 / P8–P11 的**证据与方案**仍在 `design/architecture-improvement-plan.md` §7/§9；本文只记"做不做、什么时候做"，细节不复制。
> - 已归档的历史排期文档在 `archive/`，只作脉络参考，不看旧优先级。
>
> 规则：**候选池不排期**。想做某条时，把它拉进当前 cycle，做完在这里勾掉。不要在这里维护"下一步做什么"的顺序——那是 cycle 的事。

---

## 1. 当前 cycle 已认领

见 `cycle-01.md`。认领的条目在本文标 `→ cycle-01`。

- [ ] 战术指挥玩法切片（指令输入 + AI 意图可视化 + 一局节奏） → cycle-01
- [ ] 找 1–2 个真人试玩并记录行为 → cycle-01

---

## 2. 候选池

### 2.1 玩法 / 应用接地

- [ ] 直接操控向硬化（命中反馈、掩体、换弹节奏、敌人难度曲线）——cycle-01 若判定指挥向不成立再考虑。
- [ ] 品类决策落档到 `project-direction.md` §5（**cycle-01 结束时必须写，不论结论是什么**）。

### 2.2 可复现 / 观测（我认为下一个技术周期的首选）

- [ ] 输入录制 + 回放（固定 seed + 输入流），让 parity / perf / 偶现 bug 三类问题共用一套复现设施。
- [ ] AI driver 运行契约：`Pause / Resume / Reset`、driver 类型、配置来源和 profile 身份统一进入既有 RuntimeDiag；先由需要暂停/接管 AI 的实际玩法认领，不单独造框架。
- [ ] parity 验收口径从"逐帧复刻"改为**统计等价**（胜率 / 存活时长 / 首次接敌帧 / 路径长度分布落在 legacy 置信区间）。
      理由见 `memory/chapter8-parity-rootcause.md`：保架构下逐帧对齐已被证明不可达，现口径是一个永远红的门禁。
- [ ] `run_chapter8_parity_gate.ps1` 降级为诊断工具（保留 A3DIAG 对比法），不再作为验收门禁。
- [ ] parity trace 正式化、Sandbox14/15 独立化、smoke 入 CI（`modules/samples.md`）。

### 2.3 架构债（不阻塞任何东西，有玩法压力时再还）

- [ ] **P8** component 反向依赖 `ObjectManager` / 具体 `*Object`，收口为 `SandboxServices` 窄接口 + 门禁规则。🔴 三条里最该先做的。
- [ ] **P10** 收窄 `BaseObject` typed getter 的领域污染。
- [ ] **P11** `SandboxEventDispatcherManager` 拆分，字符串 DSL 事件名下线。
- [ ] P2 尾巴：其它对象门面与跨组件语义入口审计；旧 driver/action 的 `u[SoldierObject]` Lua 兼容桥随 sample 迁移收窄。
- [ ] P4 尾巴：其它缓存裸指针继续标注 owning/non-owning。
- [ ] P5 尾巴：Agent profile 外部数据化、SoldierFactory 泛化、更完整非 Soldier 行为场景。

### 2.4 AI 能力（边际价值已低，除非玩法需要，否则不做）

- [x] navigation：按 fixed geometry 自动收紧 navmesh 真实边界（去 2001 固定），并以 config + 变换后几何指纹缓存单 tile Detour 数据。
- [ ] navigation：多层/tiled navmesh、动态障碍。← 这是 AI 链路里剩余的明显薄弱项。
- [ ] navigation 请求治理：同步 `FindPath` 外围增加每帧预算/排队、短时结果复用和 repath 距离/时间阈值；先用战术指挥或 100+ agent 的真实 spike 证明需要，再进入技术 cycle。
- [ ] BT：G2 事件节点（依赖统一事件总线）、G3 参数运行时求值、G6 黑板类型扩展、跨 agent/template 级节点缓存。
- [ ] 感知：更多非视觉 sense、AOI / visibility set 淘汰、空间查询上限调参。
- [ ] 战术：更细粒度增量 rebuild、真实 cover 来源、更细 crowd 模型。
- [ ] 团队：更多按 key / 按角色的 fact 查询从 Lua 兼容缓存迁到 C++ getter。
- [ ] FSM：`RandomMoveState` 补全或弃用（行为变更，基线后做）。

### 2.5 性能

- [ ] `perceptionSystem` 1000 agent 21.8ms/帧——当前 AI 主成本，唯一有明确数字目标的工程题。
      先决条件：录制回放（否则做不了受控 A/B，见 P9 收益低于噪声底的教训）。
      基线：`perf/ai-perf-release-baseline-20260710.md`。

### 2.6 工具 / 运行时

- [ ] profiler UI、FairyGUI element inspector（`modules/runtime.md`）。
- [ ] FGUI：lua_bridge 自动生成、handle 池化、输入路由文档（`fgui/` 专项，非主线）。
- [ ] tolua 导出清单治理、回调生命周期清理流程。
- [ ] `PhysicsFactory` 多形状复合。
- [ ] 补 SMG reload 动画。

---

## 3. 明确不做（本阶段）

写在这里是为了**不再反复重新讨论**：

- 完整数据驱动 Def / CSV / CreatureAssembler
- 触发器运行时 / ECA / 触发器编辑器
- 可视化积木编辑器、UGC / Mod 打包
- 存档、世界 streaming
- 完整 ECS（热点组件走对象侧缓存指针，见 `design/architecture-improvement-plan.md` §9 P9）
- 新的 AI system（感知/团队/战术/BT/调度/缓存/LOD/热重载该有的都有了）
- 6–12 个月长期路线图（个人项目做长排期是负资产；只维护当前 cycle + 本候选池）
