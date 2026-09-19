# Crossfire 切关优化对照（2026-09-19）

## 结论

仅改变 PhysicsFactory 两阶段凸包创建，热缓存三次过关切换 CPU 长帧从约 0.38–0.55 秒降至 0.054–0.068 秒，下降 85.6%–87.6%。三关结果逐字段一致，独立形状等价比较 42/42 通过。仍有约 3–4 个 60Hz 帧的同步停顿，不宣称零卡顿或 GPU 性能改善。

## 条件与口径

- Apple M1 Pro / MacBookPro18,1 / 16 GiB / macOS 15.7.3（24G419），arm64 Release，GL3+，实际 1280×720、FSAA 4、VSync Yes、默认仿真 30 Hz。沿用[上一轮基线](crossfire-three-court-baseline-20260919.md)。
- 基线代码 `1b4598d`（v9），候选仅 PhysicsFactory 增量构建（v10），资源、Lua 与战斗配置一致。包内 launcher 默认选 Sandbox20。
- 自动进程串行后台、静音、硬件输入禁用；测量关闭截图。相同 45 事件、71 秒内部输入回放，正常 quit，不使用物理夹具修改 HP/位置。
- 全帧日志阈值 .001 ms、周期 summary 关闭；每帧序号连续、完整 8 行。CPU frame 不含随后 swap/VSync 等待，不等于 GPU 时间；Lua 分段在隔离诊断包用 os.clock，仅作 CPU 热点归因。
- baseline/candidate 分别 8448/8418 帧，正常退出、三次胜利、无 Lua/断言/运行错误。stderr 均只有既有 GL 驱动 `gldCopyBufferSubData: NEEDS IMPLEMENTATION` 一次提示。
- 两次正式对照所有导航均命中缓存，约 3.7–4.6 ms。全帧日志有同步 I/O 开销，只使用同口径结果；相邻 frameDelta 归属于上一帧。单对照不用于宣称全局 fps 提升。

## 正式三关对照（ms）

| 切换 | 原 CPU frame | 新 CPU frame | 减少 | SandboxLua 原→新 | 下一帧 delta 原→新 |
|---|---:|---:|---:|---:|---:|
| 首关→冷却区 | 547.31 | 68.03 | 87.6% | 544.24 → 64.80 | 547 → 68 |
| 冷却区→联锁区 | 433.84 | 56.90 | 86.9% | 430.74 → 53.24 | 434 → 57 |
| 联锁区→标题 | 376.35 | 54.32 | 85.6% | 372.99 → 51.12 | 376 → 55 |

正常执行帧 CPU p50/p95 原 8.84/13.24 ms，新 8.61/12.99 ms；本轮收益主要是切关。相同输入下三关 elapsedMs 为 10329 / 10197 / 15444，全部双机存活，damage 72 / 24 / 108，medal 2 / 3 / 2；flankHits 与 blocked 也逐字段相同。

## 选关与冷导航补充

同一段 2→3→1→2→3→1 标题选关，在后一组三次热切换中，原 CPU 541.59 / 423.70 / 362.39 ms，新 44.13 / 41.86 / 38.64 ms。第一次切到 2/3 都是导航缓存 miss，原 610.34 / 488.88 ms，新 110.15 / 104.67 ms；这包含导航构建，单列而不混入热缓存收益。两者场景对象数 167 / 162 / 153 保持一致。

## 根因与等价边界

本仓 Bullet 的 `btConvexHullShape::addPoint` 无“跳过 AABB”重载，每次立即进行六方向全点扫描，原逐点创建的扫描工作随顶点数平方增长。新实现用对齐数组批量传入点集，源壳 margin .001 与最终 margin .01、设置/AABB 先后顺序保持；无共享 shape 或改动第三方。

42 组独立比较包括 33 个 float32 实际资产点集、盒体、4096 重复点、8192 点椭球及空/单点/重合/共线/共面/近退化。比较所有点和简化索引、199 个方向支持点、13 个变换下的 AABB、compound AABB 和三档惯量，严格最大差值 0。只批量而不校正源壳 AABB 的对照组有 41 组不同，因此保留那次线性重算。

## 本地证据（不入库）

- `tmp/crossfire-transition-20260919/`：build.log、diagnostic-fixed、baseline/candidate-preview、baseline/candidate-campaign、perf-analysis.json、perf-frames.csv、完整 stdout/stderr/Sandbox.log。
- `run.py` 记录实际包、输入和 binary SHA256；`summarize.py` 调用上一轮只读解析器。临时 Lua 诊断不会进入成品包。原始失败诊断输出保留但不作 PASS。
- `hull_equivalence.cpp`、`hull_equivalence.stdout.log`、`hull_provenance.json`：形状对照与库/点集指纹。
- Windows 与作者主观品质、扬声器听感不由这份 macOS 性能证据覆盖。

二进制 SHA256：

- v9：`b103c4248319c3d84adf758e40e11cec4eb5970d56700f4b0dfa137bdb48fd09`
- v10：`55bbde206d07af7ea7c08ff4fe5ad18ccdc4bc474e5cd191691b5076b8ab6b36`
- 输入：`0f2a6671c765d3285796474dcdc9280d05522c6110688bc85438ee56d41ede24`
