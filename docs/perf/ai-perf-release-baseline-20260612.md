# ai_perf Release x64 基线（2026-06-12）

> 对应 `docs/long-term-iteration-plan.md` §8 执行队列 #1、`docs/high-priority-todo.md` 的 `ai_perf baseline 二期`（Release x64 部分）。
>
> 配套 Debug x64 基线见 `docs/perf/ai-perception-baseline-20260602.md`；spatial filter 复测见 `docs/perf/ai-spatial-filter-retest-20260602.md`。

## 运行环境与口径

- 构建：**Release x64**（`bin/HelloOgre3D.exe` ~8.8MB，写 `bin/Sandbox.log`）。
- 入口：`run_sandbox_smoke.ps1 -Preset ai_perf_{100,500,1000}`（自动选 `Sandbox16`、自动开 `HELLO_PERF_STALL_LOG=1` + `HELLO_PERF_SUMMARY_INTERVAL_MS=3000`）。
- 压力口径：**scheduler 关、每帧全量感知**（pressure 配置，非生产默认）。耗时单位 ms，取稳定段 `[FramePerf]` 摘要。
- ⚠️ 机器是 **Parallels VM**（`ogre.cfg` 渲染设备为 Parallels Display Adapter）。帧时间被 VM 渲染主导，见下。

## 分项耗时（每帧，稳定段）

| 项 | 100 agent | 500 agent | 1000 agent |
|---|---|---|---|
| game objects（对象循环总） | — | 15.54 | 32.15 |
| **perceptionSystem**（感知系统，AI 主项） | ~2 | 11.36 | **23.67** |
| perception memory + vision 写入 | 0.89 + 1.40 | 4.32 + 6.90 | 8.68 + 14.74 |
| spatial queryMs | 0.25 | 3.74 | 8.81 |
| spatial candidates（每帧候选总数） | 1360 | 23422 | 58544 |
| ai ticks total（BT/driver tick） | 0.26 | 0.95 | 1.95 |
| objectUpdate | — | 2.96 | 6.24 |
| physics | — | 0.92 | 2.02 |
| teamBB | — | 0.27 | 0.47 |

## 帧级时间（stall 帧，cpuFrame vs updateCall）

| | cpuFrame | engineGap | updateCall |
|---|---|---|---|
| 100 agent | ~180–204 | ~170–198 | 4–10 |
| 1000 agent | ~1000–1050 | ~960–1007 | 34–42 |

## 结论

1. **帧时间由 `engineGap`（VM 渲染/present）主导，不是 AI**。1000 agent 下 `cpuFrame≈1000ms` 而 `updateCall` 仅 ~40ms，`engineGap≈1000ms`。在 Parallels VM 上渲染 present 是瓶颈；这解释了"肉眼卡"的主因来自环境，而非项目 AI 计算。判断 AI 真实成本应看 `updateCall` / `[FramePerf] objects` / `perceptionSystem`，而非 `cpuFrame`。
2. **AI 真实 CPU 开销有界且近似线性**：`perceptionSystem` 2 → 11.36 → 23.67ms（100/500/1000），spatial query 0.25 → 3.74 → 8.81ms，perception memory+vision 2.3 → 11.2 → 23.4ms。1000 agent 全量每帧感知约占 `updateCall` 的一多半。
3. **最大 AI 热点是 perception system 的每帧全量扫描 + memory/vision 写入**（scheduler 关时）。这正是 `PerceptionResultCache`（缓存目标/lastKnown/置信度/候选/耗时，dirty/间隔更新）要削减的目标——预计能把 1000 agent 的 perceptionSystem 从 ~24ms 显著下降。
4. spatial 候选过滤有效但未做空间分区淘汰：rejectTeam 占候选大头（1000 下 20584/58544），`filtered/results=0` 说明当前查询只统计未真正裁剪结果集——后续 spatial 可加近邻上限淘汰。

## 与原项目（HelloOgre3DX）性能差异的定位

新项目相对旧版多出的真实 AI 开销集中在 **C++ perception/spatial/teamBB 批量系统**（旧版 Chapter9 是 6 agent 小场景、无这些批量系统）。在小场景（Sandbox17/18，6 agent）这部分 <1ms，帧时间同样被 VM 渲染主导，AI 成本可忽略；规模化（百/千 agent）时 perception system 才是主成本。**因此性能对齐的正确做法是先在 `updateCall`/`perceptionSystem` 维度比较，而不是 `cpuFrame`/FPS（后者在 VM 上被渲染淹没）。**

## 待补（baseline 二期剩余）

- scheduler on/off 对照（开 `AIScheduler` 错峰后 perceptionSystem 应明显下降）。
- perception cache on/off 对照（`PerceptionResultCache` 落地后回测）。
- 非 VM / 真实 GPU 机器上的 `engineGap` 对照，隔离渲染环境因素。
- 必要时 Tracy capture。
- Sandbox17/18（6 agent）：AI 成本可忽略、帧时间 VM 渲染绑定，非性能基线重点，按需补。
