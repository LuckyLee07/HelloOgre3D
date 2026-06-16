# chapter3 跑步动画 Parity 对比管线 — 设计

- **日期**：2026-06-15
- **作者**：lizhibao
- **状态**：已批准

---

## 1. 目的与背景（含非目标）

### 目的
对比**新版 Sandbox3** 与**旧版 HelloOgre3DX `chapter_3_animation`** 的 `run_forward` 跑步动画播放，
用来把"移动卡顿"隔离到**动画系统**还是**物理/移动层**。

### 背景
- chapter4 固定步长对比已证明移动逻辑 parity（`maxPosError=0.064`），但**抹不到渲染卡顿**——
  固定步长 `HELLO_SIM_FPS=30` 每帧 `deltaTime` 恒定 33ms，掩盖了真实帧率抖动。
- 卡顿嫌疑：`AgentObject::GetSpeed()` / `AgentLocomotion::GetSpeed()` 近期被改为每帧读 Bullet 刚体瞬时
  水平速度（`sqrt(vx²+vz²)`），替代缓存 `m_speed`，可能在转向反馈环里引入逐帧抖动。
- chapter3 角色**原地播放动画、无物理/转向**，是隔离"动画播放本身"是否卡顿的干净场景：
  若 chapter3 顺滑而 chapter4 卡，则坐实卡顿在移动/物理层。

### 归属
- 北极星 **P2「录制、回放、可复现测试」**。
- `samples` 模块已知 gap「**parity trace 正式化**」（`docs/planning/long-term-iteration-plan.md` §1/§5）。

### 非目标
- 不在本任务修卡顿本身（本任务只建对比工具）。
- 不改 C++ 引擎——新旧两版引擎均已支持所需钩子（见 §3）。
- 只对比 `run_forward`（Num6），不碰武器变形 / fire / death 等其他动作。
- 不引入数据驱动 / 编辑器 / UGC（北极星 §7）。

---

## 2. 设计原则

- **复用 chapter4 已验证设施**：`parity_trace.lua`、`RenderCaptureGate`、`compare_parity_trace.py`、
  `run_chapter4_visual_trace.ps1` 的脚本结构。
- **环境变量门控**（`HELLO_CH3_COMPARE`）：门控关闭时 Sandbox3 行为完全不变，`run_sandbox_smoke` 仍 PASS。
- **手术式 Lua 层接入**：只在 Lua 脚本接 trace + 自动触发，C++ 零改动。
- **两种采集模式并存**（用户已确认）：
  - **模式 A — 固定步长 parity**：锁 `HELLO_SIM_FPS=30`，新旧逐帧对齐，验证动画状态/进度逻辑是否等价。
  - **模式 B — 真实帧率卡顿诊断**：不锁步长，各自跑，逐帧记录 `deltaTime` + `progress` 实际推进量，
    暴露帧间抖动。回答"为什么看起来卡"。

---

## 3. 架构与组件（标层 + 跨仓库）

### 可比动画度量（两版状态机最大公约数）
| 字段 | 含义 | 新版来源 | 旧版来源 |
|---|---|---|---|
| `state` | 当前动画状态名 | `soldierAsm:GetCurrStateName()` | Lua ASM `GetCurrentStateName()` |
| `stateTime` | 当前状态已播时长(s) | C++ ASM 查询 | Lua ASM 状态 time |
| `stateLength` | 当前状态总时长(s) | C++ ASM 查询 | Lua ASM 状态 length |
| `progress` | 归一化进度 `time/length` | 计算 | 计算 |
| `transitioning` | 是否过渡中 | `IsTransitioning()` | Lua ASM 过渡标志 |
| `nextState` | 目标状态名 | `GetNextStateName()` | Lua ASM next |
| `weight` | 混合权重 | C++ ASM 查询（缺则 null） | Lua ASM 权重（缺则 null） |
| `deltaTimeMs` | 本帧步长（仅模式 B） | `Sandbox_Update` 入参 | `Sandbox_Update` 入参 |

> `progress` 是最可靠的公共度量；`weight`/`transition` 语义两版未必完全对应，读不到记 `null`。

### 本仓库（新版 HelloOgre3D，Lua 层 `bin/res/scripts`）
- **扩展 `parity_trace.lua`**：新增 `ParityTrace.AnimSnapshot(asm, extra)`，采集上表度量；
  保持现有 `AgentSnapshot` 不动（向后兼容 chapter4/9）。
- **改 `samples/Sandbox3.lua`**：
  - 门控 `HELLO_CH3_COMPARE` 打开时：`ParityTrace.Start{ sample="Sandbox3", preset="chapter3_compare" }`；
    自动 `soldierAsm:RequestState("run_forward")` 并**保持循环**（门控下跳过"非 idle_aim 强制回 idle"分支）；
    `HELLO_CH3_HIDE_UI` 隐藏 UI。
  - `Sandbox_Update` 内 `ParityTrace.Tick`，快照走 `AnimSnapshot(soldierAsm)`（可选附 `weaponAsm`）；
    模式 B 额外把 `deltaTimeInMillis` 写入快照。

### 旧版仓库（HelloOgre3DX，`src/chapter_3_animation/script/`）
- **复制一份 `ParityTrace.lua`**（旧引擎 Lua API：`Sandbox.xxx`、`Animation.xxx`），加同款 `AnimSnapshot`，
  从纯 Lua `AnimationStateMachine` 读状态名 + 状态 time/length（沿用 chapter4/9 旧版 `ParityTrace.lua` 模式）。
- **改 `chapter_3_animation/script/Sandbox.lua`**：同款门控接入（自动触发 run_forward 循环 + trace + 隐藏 UI）。

### 工具层（本仓库 `tools/`）
- **新增 `run_chapter3_visual_trace.ps1`**：仿 `run_chapter4_visual_trace.ps1`，
  跑两版 exe + 抓 trace jsonl + headless 截图；`-Mode A|B|Both` 参数切换采集模式与是否锁 `HELLO_SIM_FPS`。
- **对比脚本**：
  - 模式 A：复用/扩展 `compare_parity_trace.py`，比 `state` 序列对齐 + `progress` 容差，输出 `maxProgressError` + PASS/FAIL。
  - 模式 B：新增抖动统计（可并入同一 py 加 `--anim-jitter` 子模式），各自算 progress 增量方差、最大连续停顿帧数、deltaTime 分布；**并排报告，不判 PASS/FAIL**。

### 层归属与依赖流
- 全部改动落在 **Lua 脚本层**（本仓库 `bin/res/scripts` + 旧仓库 `script/`）与 **tools/**；
- C++（`src/HelloOgre3D`、旧版 `src/demo_framework`）**不改**，遵循 `AGENTS.md` 依赖流；不触 `src/Engine`/`src/External`。

---

## 4. 数据流

```
模式 A（固定步长 parity）:
  新版 exe (HELLO_SIM_FPS=30, HELLO_CH3_COMPARE=1) → AnimSnapshot 逐帧 → modern_trace.jsonl
  旧版 exe (同环境)                                 → AnimSnapshot 逐帧 → legacy_trace.jsonl
  compare_parity_trace.py 逐帧对齐 state/progress → maxProgressError + PASS/FAIL

模式 B（真实帧率卡顿诊断）:
  新版 exe (不锁 SIM_FPS) → 逐帧 deltaTime+progress 增量 → modern_realtime.jsonl
  旧版 exe (不锁 SIM_FPS) → 逐帧 deltaTime+progress 增量 → legacy_realtime.jsonl
  抖动统计 → 并排报告（新版抖动 vs 旧版抖动）
```

---

## 5. 错误处理与失败语义

- 旧版 exe / 资源缺失 → 脚本 `throw` 报错退出（同 chapter4）。
- 某度量旧版读不到（如 `weight`）→ 记 `null`，compare 跳过缺失字段（同现有 compare 只比公共字段思路）。
- **模式 B 不判 PASS/FAIL**（无逐帧对齐基准），只产诊断数据；脚本退出码恒为 0（仅模式 A 影响门）。
- 门控关闭时任何环境异常都不应影响 Sandbox3 正常运行（默认路径零侵入）。

---

## 6. 与现有系统 / sample / 规则的关系

- **影响 sample**：仅 Sandbox3（新增门控分支，默认行为不变）。
- **回归面**：`tools/run_sandbox_smoke.ps1 -Sample Sandbox3 -StopExisting -NoTail` 应仍 `status=PASS`（门控关）。
- **复用模块**：`objects-anim`（动画状态机为采集源）、`samples`。
- **跨仓库改造**：需改旧版 HelloOgre3DX `chapter_3_animation` 脚本；chapter4/chapter9 已有同样先例，沿用该模式（旧版为另一项目的 Lua 脚本，非本仓库 vendored 代码）。
- **tolua**：本任务不改导出给 Lua 的 C++ API，无 tolua 步骤。

---

## 7. 已知局限

- 模式 B 两版真实帧率**不可逐帧对齐**（引擎/渲染负载不同），只能比各自抖动特征。
- 两版状态机实现不同（C++ `AgentAnimStateMachine` vs 旧版纯 Lua `AnimationStateMachine`），
  `weight`/`transition` 语义未必对应，`progress` 为最可靠公共度量。
- chapter3 原地不动，只验证"动画播放本身"；移动+物理叠加的卡顿仍需回 chapter4 / 真实移动场景复现。
- 旧版截图/trace 依赖旧引擎 `RenderCaptureGate` 与 `HELLO_PARITY_TRACE` 设施（chapter4/9 已验证存在）。

---

## 8. 验证策略（移交阶段 4 plan 细化）

- 每个改动 Lua 任务：`luac -p` 语法检查 + `git diff --check`。
- Sandbox3 门控关回归：`run_sandbox_smoke.ps1 -Sample Sandbox3`（`status=PASS`，行为不变）。
- 门控开冒烟：新增 `run_chapter3_visual_trace.ps1` 跑通，模式 A 产出双 trace + compare、模式 B 产出抖动报告 + 截图。
- 旧版改造：旧版 exe 在门控开时能产出 `legacy_trace.jsonl` + 截图。
