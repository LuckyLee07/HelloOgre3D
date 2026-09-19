# Crossfire 三关运行基线（2026-09-19）

本记录用于定位新版场景/界面的运行成本，不以帧数宣称美术品质或全部平台达标。硬件为 MacBookPro18,1 / Apple M1 Pro / 16 GiB，macOS 15.7.3 (24G419)。使用同一 arm64 Release，SHA-256 `b103c4248319c3d84adf758e40e11cec4eb5970d56700f4b0dfa137bdb48fd09`。没有更改 C++ 或 Tracy 埋点。

## 方法

冻结包 v8（`f0bbb9a`）对比本轮 v9。窗口实际 1280×720、GL3+、FSAA4、VSync Yes、默认仿真 30Hz，无额外渲染限速；后台/静音、真实输入禁用、独立 profile、无 fixture、无截图。使用同一普通输入三关路线，输入 SHA-256 `0f2a6671c765d3285796474dcdc9280d05522c6110688bc85438ee56d41ede24`，三关从选关到通关再回标题，末尾正常 quit。输入是应用内 synthetic 回放，原生鼠标另外验证。

可先运行 `python3 tools/run_crossfire_gate.py --mode controls --width 1280 --height 720` 得到 `controls/projected-input.txt`，再对同一个文件、各自包的 Launcher 运行以下环境：清除继承的 `HELLO_*`，设置 `HELLO_WINDOW_BACKGROUND=1`、`HELLO_AUDIO_SILENT=1`、`HELLO_WINDOW_WIDTH=1280`、`HELLO_WINDOW_HEIGHT=720`、`HELLO_RENDER_FSAA=4`、`HELLO_INPUT_REPLAY=<该回放绝对路径>`、`HELLO_INPUT_REPLAY_WAIT_FOR_PLAYER=0`、`HELLO_RENDER_CAPTURE=0`、`HELLO_CROSSFIRE_PROFILE=<每次独立路径>`。cwd 为包的 bin。记录 stdout、stderr、Sandbox.log、二进制和输入散列。

全帧采样增加 `HELLO_PERF_STALL_LOG=1`、`HELLO_PERF_STALL_THRESHOLD_MS=0.001`、`HELLO_PERF_SUMMARY_INTERVAL_MS=0`；threshold=0 会回退到默认阈值，不能用于全采样。低频控制使用 threshold=100ms、summary=1000ms：只记录极慢帧与每秒单帧快照，不从这些快照计算全程分位数。

`frameDelta` 是帧起点间隔，精度约 1ms；`cpuFrame` 在 frameRenderingQueued 结束，包含 CPU/驱动调用、在交换/VSync 前截止，并非 GPU 时间。每条完整报告同步写 stdout 和 Ogre 日志，会影响下一帧，所以这是一份可比较的诊断基线，不是零开销 FPS 基准。按真实 Screen/Pause/Outcome/Transition 日志分执行、规划、结果、标题和切关；Lua 成本只在 `simulate.delta>0` 的更新帧统计。桌面其他活动没有完全隔离。

首对 full 样本观察到 v8 关2/3导航缓存 miss、v9 hit；且 v8 运行尾部有打包 I/O，因此只作为预热和诊断，不用这对切关耗时差宣称优化。另补双方热缓存、无打包/截图/构建的 warm 对照及新版 sparse 控制。

## 结果

基线 8,525 帧、候选 8,534 帧，索引各自从 1 连续至末帧、计时块完整，stdout 与去时间戳后的 Sandbox.log 一致。两版均正常退出、依次取得三关胜利，无 Lua/OGRE/断言/gate 错误；stderr 仅有相同的一次 gldCopyBufferSubData 驱动提示。

各关执行期如下；单位 ms，三值为 p50 / p95 / 最大值。Lua 样本仅来自 `simulate.delta>0` 的更新帧。

| 关卡 | 版本 | 执行帧数 | cpuFrame | Lua更新样本 | sandboxLua |
|---|---|---:|---|---:|---|
| 1 | 基线 | 1235 | 9.41 / 12.57 / 46.99 | 312 | 1.20 / 1.84 / 7.66 |
| 1 | 候选 | 1225 | 9.44 / 12.52 / 17.44 | 310 | 1.19 / 1.69 / 7.39 |
| 2 | 基线 | 1184 | 9.24 / 13.20 / 17.88 | 299 | 1.23 / 2.10 / 7.68 |
| 2 | 候选 | 1215 | 9.21 / 12.67 / 17.68 | 307 | 1.24 / 2.10 / 6.49 |
| 3 | 基线 | 1731 | 9.33 / 13.82 / 30.84 | 437 | 1.66 / 2.77 / 25.57 |
| 3 | 候选 | 1826 | 9.46 / 13.96 / 18.93 | 460 | 1.67 / 2.99 / 11.03 |

执行期合并 cpuFrame p50/p95 为 9.32/13.39→9.36/13.42ms；>33ms 帧数为 1→0，>50ms 均为 0。此对未显示常态持续大幅回退；单次尖峰差异不足以认定性能优化，不报告百分比。

全程包含启动、规划、战斗、收尾、报告及切关；未删除尖峰：

| 版本 | frameDelta p50/p95/max | >33 / >50 次数 | cpuFrame p50/p95/max | >33 / >50 次数 |
|---|---|---|---|---|
| 基线 | 10.00 / 13.00 / 537.00 | 6 / 4 | 9.62 / 13.09 / 538.65 | 4 / 3 |
| 候选 | 9.00 / 13.00 / 548.00 | 5 / 4 | 9.46 / 13.17 / 548.56 | 3 / 3 |

切关尖峰单列；每格为 cpuFrame / sandboxLua / 后一帧 frameDelta：

| 切关 | 基线 | 候选 |
|---|---|---|
| 关1→关2 | 538.65 / 535.69 / 537.00 | 548.56 / 545.51 / 548.00 |
| 关2→关3 | 431.47 / 428.30 / 432.00 | 436.63 / 433.52 / 438.00 |
| 关3→标题 | 360.55 / 357.73 / 361.00 | 366.21 / 363.06 / 367.00 |

切关仍有约 0.36–0.55s 同步尖峰，主要位于 Sandbox_Update；不能写作已消除切关卡顿。第一对基线关2/3导航 cache miss、尾部并发打包，不作为切关收益证据。第一对与本对各关执行时长/命中数也略有变化，回放相同不等于逐帧负载完全相同。

口径：状态按实际 Screen/Pause/Menu/Outcome/Transition 日志分类，执行期排除规划、收尾、报告和切关。frameDelta[n] 属前一帧开始后的间隔，归到 n−1 的阶段；CPU/Lua归当前帧。首条 frameDelta=0 不统计，因此全程间隔样本为 8,524 / 8,533。p50为中位数，p95为最近秩 ceil(.95*n)。所有其他样本保留。

计时限制：frameDelta约1ms量化；cpuFrame于swap/VSync前结束，不能解释为GPU时间。逐帧日志同步刷新stdout和文件，其开销主要落在后一条frameDelta；无需截图也不等于零扰动。静音跳过实际音频处理。低频记录只用于核验慢帧，不计算全帧p95。
低频交叉检查：`candidate-sparse` 设置 stall 阈值 100ms、summary 间隔 1000ms，记录 78 个样本（71 个定期快照、7 个慢帧记录）。正常 quit、三关胜利、四次 nav cache hit，无 gate 错误；stdout 与 Sandbox.log 去时间戳后仍一致。

| 切关 | 候选逐帧 cpuFrame / sandboxLua | 候选低频 cpuFrame / sandboxLua |
|---|---|---|
| 关1→关2 | 548.56 / 545.51 | 545.83 / 542.69 |
| 关2→关3 | 436.63 / 433.52 | 441.59 / 438.03 |
| 关3→标题 | 366.21 / 363.06 | 372.56 / 369.46 |

低频下切关尖峰依然约 0.37–0.55s，说明它们并非逐帧性能日志单独造成；不可据此推算 GPU 开销。低频样本经过周期/阈值筛选，不用于全帧 p50/p95、长帧总数或总渲染帧数。无需继续扩大本轮基准。

## 后续

切关处理是本轮观测到的突出长帧来源，应单独分解清场、资源创建和导航成本，再选择缓存或分帧策略；不能只看切关 Lua 总耗时就认定全部来自导航。声音本轮未发现必须修改的生命周期缺陷，未进行扬声器主观听感验收。Windows、GPU 时间及严格前台帧率预算仍未验证。

原始数据只留本地 `tmp/crossfire-quality-20260919/`，不入库。实施与视觉/输入证据见[三关品质记录](../dev-design/plans/2026-09-19-crossfire-three-court-quality.md)。
