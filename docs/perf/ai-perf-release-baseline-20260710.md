# ai_perf Release x64 基线（2026-07-10，P9 热点组件缓存后刷新）

> 对应 `docs/design/architecture-improvement-plan.md` §9 P9 验收最后一项「清理后再采集 `ai_perf_100/500/1000` 基线」、`docs/planning/long-term-iteration-plan.md` §8 执行队列 #1。
>
> 前一版基线见 `docs/perf/ai-perf-release-baseline-20260612.md`（口径一致，可直接对照）。

## 运行环境与口径

- 构建：**Release x64**（`bin/HelloOgre3D.exe`，写 `bin/Sandbox.log`），已含 P9 热点组件缓存（`BaseObject` 缓存指针 + AI tick 路径 repoint）。
- 入口：`run_sandbox_smoke.ps1 -Preset ai_perf_{100,500,1000}`（自动选 `Sandbox16`，自动开 `HELLO_PERF_STALL_LOG=1` + `HELLO_PERF_SUMMARY_INTERVAL_MS=3000`；100/500/1000 分别跑 35/90/120s）。
- 压力口径：**scheduler 关、每帧全量感知**（pressure 配置，非生产默认），与 20260612 完全一致。耗时单位 ms，取稳定段 `[FramePerf]` 摘要。
- 机器：host `lizhibao`，20 逻辑核。**spatial candidates 与 rejectTeam 与 20260612 逐值相同**（100→1360、500→23422、1000→58544，rejectTeam 1000→20584），说明 preset / spawn 确定性一致、同机可比。
- ⚠️ 帧时间仍被引擎 `engineGap`（present）主导，判断 AI 成本看 `updateCall` / `[FramePerf] objects` / `perceptionSystem`，非 `cpuFrame`。

## 分项耗时（每帧，稳定段，2026-07-10）

| 项 | 100 agent | 500 agent | 1000 agent |
|---|---|---|---|
| game objects（对象循环总，含 physics/lua） | 3.2 | 15.1 | 30.1 |
| **perceptionSystem**（感知系统，AI 主项） | 2.33 | 10.9 | **21.8** |
| perception memory + vision 写入 | 1.12 + 1.12 | 6.30 + 4.33 | 13.0 + 8.25 |
| spatial queryMs | 0.03 | 0.37 | 0.85 |
| spatial candidates（每帧候选总数） | 1360 | 23422 | 58544 |
| ai ticks total（BT/driver tick） | 0.21 | 1.03 | 2.05 |
| objectUpdate | 0.63 | 3.37 | 6.66 |
| loop（对象遍历） | 0.66 | 3.52 | 6.97 |
| physics（game 行） | 0.11 | 1.25–1.80 | 3.5–3.8 |
| teamBB | 0.07 | 0.30 | 0.52 |
| luaCallbacks / luaCallbackMs | 102 / ~0.7–0.8 | 502 / ~1.3 | 1002 / ~2.2 |

## 帧级时间（stall 帧，cpuFrame vs updateCall）

| | cpuFrame | engineGap | updateCall |
|---|---|---|---|
| 100 agent | ~152 | ~148 | 3.8 |
| 500 agent | ~750–790 | ~732–772 | 17–22 |
| 1000 agent | ~1280–1436 | ~1240–1400 | 34–40 |

## 与 20260612 基线对照

（同机、同 preset，candidates 逐值相同；单位 ms）

| 项 | 100（0612 → 0710） | 500（0612 → 0710） | 1000（0612 → 0710） |
|---|---|---|---|
| perceptionSystem | ~2 → 2.33 | 11.36 → 10.9 | 23.67 → **21.8** |
| perception mem+vision（合计） | 2.29 → 2.24 | 11.22 → 10.6 | 23.42 → 21.3 |
| ai ticks total | 0.26 → 0.21 | 0.95 → 1.03 | 1.95 → 2.05 |
| objectUpdate | — → 0.63 | 2.96 → 3.37 | 6.24 → 6.66 |
| teamBB | — → 0.07 | 0.27 → 0.30 | 0.47 → 0.52 |
| game objects | — → 3.2 | 15.54 → 15.1 | 32.15 → 30.1 |
| updateCall | 4–10 → 3.8 | — → 17–22 | 34–42 → 34–40 |
| spatial queryMs | 0.25 → 0.03 | 3.74 → 0.37 | 8.81 → **0.85** |
| spatial candidates | 1360 → 1360 | 23422 → 23422 | 58544 → 58544 |

## 结论

1. **P9 无回归**：所有 `ai_perf_*` smoke 通过，candidates / rejectTeam 逐值不变（确定性一致），AI 各分项与 20260612 处于同量级或略优。`perceptionSystem` 1000 agent 从 23.67 → 21.8（≈8% 下降），`updateCall` 1000 agent ~34–40ms（与前持平）。
2. **P9（热点组件缓存）的直接收益在聚合帧指标里低于噪声底**：它去掉的是每帧全表 `dynamic_cast` 的常数级组件查找开销，分摊在 `objectUpdate` / `ai ticks` / `perceptionSystem` 里；而这些相对 perception 的 vision/memory 计算（1000 agent 下约 21ms，占 `updateCall` 大头）是小项，单靠这套 pressure 聚合数难以从 refactor 噪声里单独切出。P9 的主要价值是**架构正确性**（entity 热 getter 不再每帧遍历 map）与**消除 `AgentLocomotion` 邻居避让里 `FindComponent` 的超线性风险**（该 pressure preset 未大量走 cohesion/separation 转向，故未在此体现）。
3. **`spatial queryMs` 大幅下降（8.81 → 0.85，≈10×）不是 P9 带来的**：P9 未触及 `AgentSpatialIndexSystem` 查询内部；candidates 逐值相同说明处理量不变、单位耗时变快，来源是 6-12 之后的空间/感知专项 refactor（非本轮）。此处标注避免误归因。
4. **最大 AI 热点仍是 perception system 每帧全量扫描 + memory/vision 写入**（scheduler 关时约占 1000 agent `updateCall` 六成）。这是 pressure 口径刻意逼出的全量路径；生产（scheduler 开 + `PerceptionResultCache`）下不会每帧全量。
5. **帧时间仍 `engineGap` 主导**：1000 agent `cpuFrame≈1300–1400ms` 而 `updateCall` 仅 ~34–40ms，present 是瓶颈，属环境非 AI 计算。cpuFrame 相对 20260612 反而偏高，是 present 抖动，不反映 AI 成本。

## 口径说明与局限

- 本次是**累积刷新**：20260612 → 20260710 之间除 P9 外还有约 40 个 AI 运行时 C++ 化提交（感知缓存、tactical、team、BT runtime 等），对照差值是这批改动 + P9 的**合力**，不是 P9 的受控 A/B。
- 想单独隔离 P9 需对同一构建做 before/after 组件缓存开关对照；当前 P9 改动与未提交的 Sandbox19/PlayerController 改动在同文件交叠，临时 stash 出「无 P9」构建不干净，故未做隔离 A/B。若需要可后续在干净分支上补。
- `memory` 与 `vision` 两列在两版基线间占比互换（0612 vision>memory，0710 memory>vision），是中间 perception 记账口径调整所致，合计基本持平，不影响 AI 总成本判断。
