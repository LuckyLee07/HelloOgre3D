# chapter3 跑步动画 Parity 对比管线 实施计划

**Goal:** 搭建新版 Sandbox3 与旧版 chapter_3_animation 的 run_forward 动画 ParityTrace 对比管线（固定步长 parity + 真实帧率卡顿诊断两种模式）。
**Architecture:** 全部改动落在 Lua 脚本层（本仓库 `bin/res/scripts` + 旧仓库 `src/chapter_3_animation/script`）与 `tools/`；C++ 引擎零改动（新版 `ObjectASM` 查询方法已绑定、旧版 `RenderCaptureGate`/`HELLO_PARITY_TRACE` 已存在）。无 tolua 步骤。
**Spec:** [docs/dev-design/specs/2026-06-15-chapter3-anim-parity-design.md](../specs/2026-06-15-chapter3-anim-parity-design.md)
**状态:** 已批准，执行中

> **提交约定（本计划特例）**：用户已声明"不要自己提交"。各 Task 的 commit 步骤一律改为「列改动文件 + 起草中文 `[dev]` message 交用户手动提交」，主会话与子代理都不执行 `git commit`。

---

## 文件结构

| 操作 | 路径 | 层 | 职责 |
|---|---|---|---|
| Modify | `bin/res/scripts/samples/parity_trace.lua` | bin/res/scripts | 新增 `ParityTrace.AnimSnapshot`（新版 C++ ASM 动画度量采集） |
| Modify | `bin/res/scripts/samples/Sandbox3.lua` | bin/res/scripts | `HELLO_CH3_COMPARE` 门控：启动 trace + 自动循环 run_forward + 隐藏 UI + 记 deltaTime |
| Create | `E:/Workspace/HelloOgre3DX/src/chapter_3_animation/script/ParityTrace.lua` | 旧仓库 script | 旧引擎版 ParityTrace + `AnimSnapshot`（读 Lua ASM 内部字段） |
| Modify | `E:/Workspace/HelloOgre3DX/src/chapter_3_animation/script/Sandbox.lua` | 旧仓库 script | 同款门控接入 |
| Modify | `tools/compare_parity_trace.py` | tools | 新增 `--anim` 对齐对比 + `--anim-jitter` 抖动统计 |
| Create | `tools/run_chapter3_visual_trace.ps1` | tools | 跑两版 exe + 抓 trace + 截图，`-Mode A/B/Both` |

依赖关系：Task1→Task2，Task3→Task4，Task5 独立，Task6 依赖 1-5（集成验证门）。Task1/Task3/Task5 可并行起步。

---

## Task 1: 新版 parity_trace.lua — AnimSnapshot

**Files:**
- Modify: `bin/res/scripts/samples/parity_trace.lua`（在 `ParityTrace.AgentSnapshot` 函数后追加，约 286 行后）

**层归属:** bin/res/scripts（Lua 工具库）
**验证策略:** `luac -p` 语法检查（无 C++ 编译）；间接由 Task 2 的 Sandbox3 smoke 覆盖。

- [ ] **Step 1：追加 `ParityTrace.AnimSnapshot`**。利用已确认存在的 ObjectASM 绑定方法。新版 ASM 无 weight getter → `weight` 记 `nil`（_EncodeJson 会输出为缺省/null）。

```lua
-- 采集单个 C++ ObjectASM 的动画度量（chapter3 原地动画对比用）。
-- asm 来自 soldier:GetObjectASM()。extra 可附 deltaTimeMs/role 等。
function ParityTrace.AnimSnapshot(asm, index, extra)
    local snapshot = {
        index = index,
        state = tostring(_SafeCall(asm, "GetCurrStateName", "")),
        stateNext = tostring(_SafeCall(asm, "GetNextStateName", "")),
        stateTransitioning = _SafeCall(asm, "IsTransitioning", false) == true,
        stateTime = _Round(_SafeCall(asm, "GetCurrStateTime", 0), 1000),
        stateLength = _Round(_SafeCall(asm, "GetCurrStateLength", 0), 1000),
        progress = _Round(_SafeCall(asm, "GetCurrStateProgress", 0), 1000),
        weight = nil, -- 新版 ASM 未暴露混合权重，记 null 与旧版对齐时跳过
    }
    for key, value in pairs(extra or {}) do
        snapshot[key] = value
    end
    return snapshot
end
```

> `_SafeCall` 与 `_Round` 已在本文件定义（135、123 行），可直接复用。

- [ ] **Step 2：`luac -p` 校验**：`luac -p bin/res/scripts/samples/parity_trace.lua`（若环境无 luac，用 `tools/run_sandbox_smoke.ps1` 间接验证，见 Task 2）。
- [ ] **Step 3：交付**：列改动文件，起草 message `[dev]parity_trace 新增 AnimSnapshot 采集 C++ ASM 动画度量`，交用户提交。

---

## Task 2: 新版 Sandbox3.lua — 门控接入

**Files:**
- Modify: `bin/res/scripts/samples/Sandbox3.lua`（顶部 require、`Sandbox_Initialize`、`Sandbox_Update`）

**层归属:** bin/res/scripts（samples）
**验证策略:** `tools/run_sandbox_smoke.ps1 -Sample Sandbox3 -StopExisting -NoTail` → 预期 `status=PASS`（门控关，行为不变）。

- [ ] **Step 1：顶部引入 ParityTrace + 门控辅助**（在 `require("res.scripts.agent.SoldierAgent.lua")` 后）：

```lua
local ParityTrace = require("res.scripts.samples.parity_trace")

local _parityTrace = nil
local _ch3Compare = false

local function _IsTruthy(value)
    if value == nil then return false end
    value = string.lower(tostring(value))
    return value == "1" or value == "true" or value == "yes" or value == "on"
end

local function _GetSeed()
    return tonumber(os.getenv("HELLO_PARITY_SEED") or os.getenv("HELLO_SAMPLE_SEED") or "") or 0
end

local function _BuildAnimSnapshot(state)
    local extra = {}
    if state ~= nil and state.lastDeltaMs ~= nil then
        extra.deltaTimeMs = state.lastDeltaMs   -- 模式 B：记录真实帧步长
    end
    return {
        agents = { ParityTrace.AnimSnapshot(soldierAsm, 1, extra) },
    }
end
```

- [ ] **Step 2：`Sandbox_Initialize` 末尾接入门控**（在 `soldierAsm:RequestState("idle_aim")` 之后、函数末尾）：

```lua
    _ch3Compare = _IsTruthy(os.getenv("HELLO_CH3_COMPARE"))
    if _ch3Compare then
        soldierAsm:RequestState("run_forward")   -- 立即切跑步
        _parityTrace = ParityTrace.Start({
            sample = "Sandbox3",
            preset = os.getenv("HELLO_SAMPLE_PRESET") or "chapter3_compare",
            seed = _GetSeed(),
        })
    end
```

> UI 隐藏：`HELLO_CH3_HIDE_UI` 由 ps1 设置；当前 Sandbox3 的 UI 仅 `GUI_CreateSandboxText`，门控开时不影响 trace（截图脚本会传 hide）。若需彻底隐藏，Step 2b 在 `GUI_CreateSandboxText` 调用外包 `if not _ch3Compare then ... end`。

- [ ] **Step 2b：UI 隐藏分支**（包裹 `Sandbox_Initialize` 里两处 GUI 调用）：

```lua
    if not _IsTruthy(os.getenv("HELLO_CH3_HIDE_UI")) then
        GUI_CreateCameraAndProfileInfo()
        GUI_CreateSandboxText(infoText, textSize)
    end
```

- [ ] **Step 3：`Sandbox_Update` 接入 Tick + 关掉门控下的"强制回 idle"**。当前 `Sandbox_Update` 在非 idle_aim 时强制 `RequestState("idle_aim")`，会打断跑步循环；门控开时跳过该分支并保持 run_forward：

```lua
function Sandbox_Update(deltaTimeInMillis)
    if _ch3Compare then
        if _parityTrace ~= nil then
            _parityTrace.lastDeltaMs = math.floor(deltaTimeInMillis + 0.5)
        end
        ParityTrace.Tick(_parityTrace, deltaTimeInMillis, _BuildAnimSnapshot)
        -- 保持跑步循环：run_forward 是 looping 状态，播完会自然循环；
        -- 仅在意外离开 run_forward 时重新请求，不回 idle。
        if soldierAsm:GetCurrStateName() ~= "run_forward" then
            soldierAsm:RequestState("run_forward")
        end
        return
    end

    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()
    if soldierAsm:GetCurrStateName() ~= "idle_aim" then
        soldierAsm:RequestState("idle_aim")
    end
    -- weapon 分支保持原样
    local currStateName = weaponAsm:GetCurrStateName()
    if weaponState == "sniper" then
        if currStateName ~= "sniper_idle" then weaponAsm:RequestState("sniper_idle") end
    elseif weaponState == "smg" then
        if currStateName ~= "smg_idle" then weaponAsm:RequestState("smg_idle") end
    end
end
```

> 注：`_parityTrace.lastDeltaMs` 复用 ParityTrace.state 表（开放表，可挂字段）；`_BuildAnimSnapshot` 从 state 读它。

- [ ] **Step 4：跑 smoke（门控关回归）** `tools\run_sandbox_smoke.ps1 -Sample Sandbox3 -StopExisting -NoTail` → 预期 `status=PASS`。
- [ ] **Step 5：交付**：列改动文件，message `[dev]Sandbox3 接入 chapter3_compare 门控与动画 parity trace`，交用户提交。

---

## Task 3: 旧版 ParityTrace.lua — 新建 + AnimSnapshot

**Files:**
- Create: `E:/Workspace/HelloOgre3DX/src/chapter_3_animation/script/ParityTrace.lua`

**层归属:** 旧仓库 chapter_3_animation/script（旧引擎 Lua）
**验证策略:** 由 Task 6 集成脚本跑通旧版 exe 产出 `legacy_trace.jsonl` 验证（旧仓库无独立 smoke）。

- [ ] **Step 1：复制旧版 chapter4 的 ParityTrace.lua 为模板**：以 `E:/Workspace/HelloOgre3DX/src/chapter_4_mind_body/script/ParityTrace.lua` 全文为基底（含 Start/Tick/Record/PrintRecord/EncodeValue），删除 `AgentSnapshot`/`SetAgentExtra`/`AgentExtras`/`VectorSnapshot` 等移动 agent 专用部分（chapter3 无 agent）。保留 `Round`/`EncodeValue`/`PrintRecord`/`Start`/`Tick`。

- [ ] **Step 2：追加 `ParityTrace.AnimSnapshot`**。旧版 asm 是纯 Lua `AnimationStateMachine` 表，直读内部字段 + `Animation.*`：

```lua
-- 旧引擎：从 Lua AnimationStateMachine 表采集动画度量。
function ParityTrace.AnimSnapshot(asm, index, extra)
    local snapshot = {
        index = index,
        state = asm:GetCurrentStateName() or "",
        stateNext = (asm.nextState_ ~= nil) and asm.nextState_.name_ or "",
        stateTransitioning = asm.currentTransition_ ~= nil,
        stateTime = 0,
        stateLength = 0,
        progress = 0,
        weight = nil,
    }
    if (asm.currentState_ ~= nil and asm.currentState_.animation_ ~= nil) then
        local anim = asm.currentState_.animation_
        local t = Animation.GetTime(anim) or 0
        local len = Animation.GetLength(anim) or 0
        snapshot.stateTime = Round(t)
        snapshot.stateLength = Round(len)
        snapshot.progress = (len > 0) and Round(t / len) or 0
        snapshot.weight = Round(Animation.GetWeight(anim))
    end
    if (extra ~= nil) then
        for key, value in pairs(extra) do snapshot[key] = value end
    end
    return snapshot
end
```

> 字段名（`state/stateNext/stateTransitioning/stateTime/stateLength/progress/weight/deltaTimeMs`）与新版 Task 1 **逐字一致**，保证 compare 可对齐。`Round` 已在模板中定义。`Animation.GetTime/GetLength/GetWeight` 是旧引擎已有 API（见 AnimationStateMachine.lua 56-59、206 行用法）。

- [ ] **Step 3：交付**：列新建文件，message `[dev]chapter3 旧版新增 ParityTrace.AnimSnapshot 动画度量采集`，交用户提交。

---

## Task 4: 旧版 chapter_3_animation/Sandbox.lua — 门控接入

**Files:**
- Modify: `E:/Workspace/HelloOgre3DX/src/chapter_3_animation/script/Sandbox.lua`（require、`Sandbox_Initialize`、`Sandbox_Update`、`CreateSandboxText`）

**层归属:** 旧仓库 chapter_3_animation/script
**验证策略:** 由 Task 6 集成脚本验证旧版 exe 门控开产 trace + 截图。

- [ ] **Step 1：顶部 require + 门控状态**（在 `require "GUI"` 后）：

```lua
require "ParityTrace";

local _parityTrace;
local _ch3Compare = false;

local function _IsTruthy(value)
    if (value == nil) then return false; end
    value = string.lower(tostring(value));
    return value == "1" or value == "true" or value == "yes" or value == "on";
end

local function _GetSeed()
    return tonumber(os.getenv("HELLO_PARITY_SEED") or os.getenv("HELLO_SAMPLE_SEED") or "") or 0;
end

local function _BuildAnimSnapshot(state)
    local extra = {};
    if (state ~= nil and state.lastDeltaMs ~= nil) then
        extra.deltaTimeMs = state.lastDeltaMs;
    end
    return { agents = { ParityTrace.AnimSnapshot(soldierAsm, 1, extra) } };
end
```

> 旧版 `soldierAsm` 是文件顶部 `local soldierAsm`（29 行）；`_BuildAnimSnapshot` 需能访问它——把 `soldierAsm` 的引用确保在 upvalue 作用域内（它已是文件级 local，OK）。

- [ ] **Step 2：`Sandbox_Initialize` 末尾接入**（在 `soldierAsm:RequestState("idle_aim")` 232 行之后）：

```lua
    _ch3Compare = _IsTruthy(os.getenv("HELLO_CH3_COMPARE"));
    if (_ch3Compare) then
        soldierAsm:RequestState("run_forward");
        _parityTrace = ParityTrace.Start({
            sample = "chapter_3_animation",
            preset = os.getenv("HELLO_SAMPLE_PRESET") or "chapter3_compare",
            seed = _GetSeed(),
        });
    end
```

- [ ] **Step 2b：UI 隐藏**（包裹 `CreateSandboxText(sandbox)` 调用，130 行）：

```lua
    if (not _IsTruthy(os.getenv("HELLO_CH3_HIDE_UI"))) then
        CreateSandboxText(sandbox);
    end
```

- [ ] **Step 3：`Sandbox_Update` 接入 Tick + 跳过强制回 idle**（替换 245-268 行函数体的回-idle 逻辑）：

```lua
function Sandbox_Update(sandbox, deltaTimeInMillis)
    if (_ch3Compare) then
        if (_parityTrace ~= nil) then
            _parityTrace.lastDeltaMs = math.floor(deltaTimeInMillis + 0.5);
        end
        soldierAsm:Update(deltaTimeInMillis, Sandbox.GetTimeInMillis(sandbox));
        weaponAsm:Update(deltaTimeInMillis, Sandbox.GetTimeInMillis(sandbox));
        ParityTrace.Tick(_parityTrace, deltaTimeInMillis, _BuildAnimSnapshot);
        if (soldierAsm:GetCurrentStateName() ~= "run_forward") then
            soldierAsm:RequestState("run_forward");
        end
        return;
    end

    GUI_UpdateUI(sandbox);
    soldierAsm:Update(deltaTimeInMillis, Sandbox.GetTimeInMillis(sandbox));
    weaponAsm:Update(deltaTimeInMillis, Sandbox.GetTimeInMillis(sandbox));
    if (soldierAsm:GetCurrentStateName() ~= "idle_aim") then
        soldierAsm:RequestState("idle_aim");
    end
    if (weaponState == "sniper") then
        if (weaponAsm:GetCurrentStateName() ~= "sniper_idle") then weaponAsm:RequestState("sniper_idle"); end
    elseif (weaponState == "smg") then
        if (weaponAsm:GetCurrentStateName() ~= "smg_idle") then weaponAsm:RequestState("smg_idle"); end
    end
end
```

> 关键：旧版必须**保留** `soldierAsm:Update(...)` 调用（动画步进靠它），门控分支里也要调，否则动画不前进、progress 不变。

- [ ] **Step 4：交付**：列改动文件，message `[dev]chapter3 旧版 Sandbox 接入 chapter3_compare 门控与动画 trace`，交用户提交。

---

## Task 5: compare_parity_trace.py — anim 对比 + 抖动统计

**Files:**
- Modify: `tools/compare_parity_trace.py`（新增 `--anim` / `--anim-jitter` 分支，不破坏现有 pos 对比）

**层归属:** tools（Python）
**验证策略:** 用 Task 6 产出的 trace 自测；或先用现有 chapter4 trace 确认旧路径未回归（`--anim` 不传时行为不变）。

- [ ] **Step 1：加参数**（在 `add_argument` 区）：

```python
    parser.add_argument("--anim", action="store_true", help="动画 parity 模式：逐帧对齐 state/progress")
    parser.add_argument("--anim-jitter", action="store_true", help="模式 B：各自输出 progress/deltaTime 抖动统计，不判 PASS/FAIL")
    parser.add_argument("--progress-tolerance", type=float, default=0.05)
```

- [ ] **Step 2：加 `compare_anim` 函数**（逐帧对齐 state + progress 容差）：

```python
def compare_anim(old_records, new_records, args):
    failures = []
    n = min(len(old_records), len(new_records))
    max_prog_err = 0.0
    for i in range(n):
        oa = (old_records[i].get("agents") or [{}])[0]
        na = (new_records[i].get("agents") or [{}])[0]
        label = f"sample#{i+1}"
        if oa.get("state") != na.get("state"):
            failures.append(f"{label}: state old={oa.get('state')} new={na.get('state')}")
        op, npv = oa.get("progress"), na.get("progress")
        if op is not None and npv is not None:
            err = abs(float(op) - float(npv))
            max_prog_err = max(max_prog_err, err)
            if err > args.progress_tolerance:
                failures.append(f"{label}: progress err {err:.3f} > {args.progress_tolerance:.3f}")
    status = "PASS" if not failures else "FAIL"
    msgs = [f"[ParityCompare] ANIM {status} samples={n} maxProgressError={max_prog_err:.3f}"]
    msgs += [f"[ParityCompare] failure: {f}" for f in failures[:args.max_failures]]
    return not failures, msgs
```

- [ ] **Step 3：加 `report_jitter` 函数**（模式 B，单文件抖动统计；处理 progress 循环 wrap）：

```python
def _progress_deltas(records):
    deltas, prev = [], None
    for r in records:
        a = (r.get("agents") or [{}])[0]
        p = a.get("progress")
        if p is None: continue
        p = float(p)
        if prev is not None:
            d = p - prev
            if d < 0: d += 1.0   # 循环 wrap
            deltas.append(d)
        prev = p
    return deltas

def _delta_ms(records):
    out = []
    for r in records:
        a = (r.get("agents") or [{}])[0]
        if a.get("deltaTimeMs") is not None:
            out.append(float(a["deltaTimeMs"]))
    return out

def report_jitter(label, records):
    pds = _progress_deltas(records)
    dms = _delta_ms(records)
    def stats(xs):
        if not xs: return (0,0,0,0)
        m = sum(xs)/len(xs)
        var = sum((x-m)**2 for x in xs)/len(xs)
        return (m, var**0.5, min(xs), max(xs))
    pm, psd, pmin, pmax = stats(pds)
    dm, dsd, dmin, dmax = stats(dms)
    stalls = sum(1 for d in pds if d < 1e-4)  # 进度几乎不动的帧
    return [
        f"[Jitter] {label} frames={len(records)} "
        f"progDelta(mean={pm:.4f} std={psd:.4f} min={pmin:.4f} max={pmax:.4f} stalls={stalls}) "
        f"deltaMs(mean={dm:.2f} std={dsd:.2f} min={dmin:.2f} max={dmax:.2f})"
    ]
```

- [ ] **Step 4：`main` 分派**：`--anim-jitter` 时对 old/new 各调 `report_jitter` 并打印（return 0）；`--anim` 时调 `compare_anim`；否则走原 `compare`。保持默认路径不变。
- [ ] **Step 5：交付**：列改动文件，message `[dev]compare_parity_trace 增加动画 parity 对齐与帧抖动统计`，交用户提交。

---

## Task 6: run_chapter3_visual_trace.ps1 — 集成脚本（验证门）

**Files:**
- Create: `tools/run_chapter3_visual_trace.ps1`

**层归属:** tools（PowerShell）
**验证策略（本任务即集成验证门）:** 跑 `-Mode Both`：模式 A 产 `legacy_trace.jsonl`+`modern_trace.jsonl`+截图、`compare_parity_trace.py --anim` 输出 `ANIM PASS`；模式 B 产 realtime trace + `--anim-jitter` 抖动报告 + 截图。

- [ ] **Step 1：以 `tools/run_chapter4_visual_trace.ps1` 为骨架**复制，改动如下：
  - `$LegacyExe = chapter_3_animation.exe`（路径 `E:\Workspace\HelloOgre3DX\bin\x32\release\chapter_3_animation.exe`）。
  - 新增 `-Mode` 参数（`A`/`B`/`Both`，默认 `Both`）。
  - 公共环境去掉 chapter4 的 `HELLO_CH4_*`，改用 `HELLO_CH3_COMPARE=1`、`HELLO_CH3_HIDE_UI=1`、`HELLO_SAMPLE_PRESET=chapter3_compare`。
  - 新版仍设 `HELLO_SANDBOX_SAMPLE=Sandbox3`、`HELLO_SANDBOX_SMOKE_TEST=1`。

- [ ] **Step 2：模式 A 段**（固定步长 parity）：设 `HELLO_SIM_FPS=30`、`HELLO_PARITY_TRACE_INTERVAL_MS=33`、`HELLO_PARITY_TRACE_MAX_SAMPLES=140`、`HELLO_PARITY_TRACE_DELAY_MS=0`。跑两版 → `$OutputDir/modeA/{legacy,modern}_trace.jsonl` + 截图。对比：

```powershell
& $python (Join-Path $ScriptRoot "compare_parity_trace.py") $legacyA $modernA --anim --progress-tolerance 0.05 --time-tolerance-ms 250
```

- [ ] **Step 3：模式 B 段**（真实帧率诊断）：**不设** `HELLO_SIM_FPS`，设 `HELLO_PARITY_TRACE_INTERVAL_MS=0`（每帧采）、`HELLO_PARITY_TRACE_MAX_SAMPLES=600`、`HELLO_PARITY_TRACE_DELAY_MS=0`。跑两版 → `$OutputDir/modeB/{legacy,modern}_realtime.jsonl` + 截图。报告：

```powershell
& $python (Join-Path $ScriptRoot "compare_parity_trace.py") $legacyB $modernB --anim-jitter
```

- [ ] **Step 4：`-Mode` 控制跑哪些段**（`A`/`B`/`Both`）；复用 chapter4 脚本的 `Invoke-CaptureRun`/`Wait-CaptureDone`/`Set-ProcessEnvironment` 等函数。截图时刻 `-CaptureMs` 默认 `2000`。
- [ ] **Step 5：跑通验证** `tools\run_chapter3_visual_trace.ps1 -Mode Both`：确认两模式 trace、截图、compare/jitter 输出齐全；模式 A 期望 `ANIM PASS`（若 FAIL，记录 state/progress 差异供分析——这本身就是有价值的对比结果）。
- [ ] **Step 6：交付**：列新建文件，message `[dev]新增 chapter3 跑步动画 parity 视觉对比脚本`，交用户提交。

---

## 自审

- ① spec 每条需求可指向 task：两模式→Task6 Step2/3；动画度量→Task1/3；门控不侵入→Task2 Step4 smoke；跨仓库→Task3/4；对比/抖动→Task5。✓
- ② 占位扫描：无 TBD/TODO，关键代码均给代码块。✓
- ③ 类型/命名一致：动画字段名 `state/stateNext/stateTransitioning/stateTime/stateLength/progress/weight/deltaTimeMs` 新旧两版逐字一致。✓
- ④ 每 task 标层归属 + 验证策略；无导出 C++ API 改动 → 无 tolua 步骤（已在 Architecture 注明）。✓

## 已知风险

- 模式 A 两版触发 run_forward 的 idle→run 过渡时机若不一致，前若干帧 state/progress 会差异 → compare 会 FAIL 并指出具体帧；这是诊断信息而非脚本缺陷，必要时用 `HELLO_PARITY_TRACE_DELAY_MS` 错开过渡窗口后再比稳定循环段。
- 旧版 `Animation.GetWeight/GetTime/GetLength` 若对裸 mesh 动画返回异常 → AnimSnapshot 已用默认值兜底（len>0 判定）。
- 旧版 `luac` 可能不可用 → 旧版 Lua 仅靠 Task6 运行期验证。
