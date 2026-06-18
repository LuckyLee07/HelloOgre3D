# Chapter9 → Sandbox17 完全复刻：穷尽差异分析与复刻计划

> 日期：2026-06-15
> 目标：让新版 Sandbox17 完全复刻旧版 HelloOgre3DX `chapter_9_tactics` 的运行效果——动作轨迹、绘制线、影响图。
> 方法：4 维度（影响图算法 / 绘制 / agent 行为 / 事件感知）逐项对照旧版事实基准与新版现状。
> 配套：本文是 `docs/design/chapter9-parity-architecture-notes.md`（架构归档）的"可执行差异清单"续篇。

## 0. 核心结论

1. **新版内核已是旧版的忠实端口**：C++ `InfluenceMapSystem` 的 3D 体素化建图、27 邻居扩散、falloff/inertia、双缓冲；`AgentUtils` 的转向/steering 权重/朝向插值；移动速度（stand 3 / crouch 1）、到达半径（move/flee 1.5、pursue/shoot 3）、危急阈值（0.2）、idle 时长（2s）、slow 阻尼（0.91）——**全部逐行/逐值一致**。
2. **差异集中在可配置层**：影响图源类型、绘制层与样式、几个 agent 时序参数、感知视野、事件源路径。**绝大多数是 Lua 配置/脚本改动，不需要编译 C++。**
3. **旧版实际决策驱动是行为树（BT）**：chapter_9 用 `demo_framework/IndirectSoldierAgent.lua`（chapter_9 目录自己没有该文件），选 `SoldierLogic_BehaviorTree`，叶子动作经 `SoldierController` 命令队列驱动动画/转向。新版默认 `DecisionSoldierAgent`（决策树 DT），与旧版 BT **条件结构同构**（isAlive→critical→enemy→ammo→shoot/pursue→move/random/idle）。
4. **设计岔路**：项目已有两个 preset——`chapter9_tactics_lua`（Sandbox17 默认，luaTeaching 语义）与 `chapter9_tactics_legacy_parity`（复刻对照面，已对齐 FOV=90/visionInterval=1/teamUseLegacyPointSpread 等）。`docs/design/chapter9-parity-architecture-notes.md` 明确**不建议**把 Sandbox17 默认整体改回 legacy 风格。**推荐路线：把剩余 gap 补进 `legacy_parity` 路径，让 Sandbox17 跑 `-Preset chapter9_tactics_legacy_parity` 时完全复刻**；若要默认即复刻，只需切换 Sandbox17 默认 preset。

## 1. 事实基准：旧版 chapter_9_tactics 怎么跑

- `Sandbox.lua`（148 行）：建关卡/navmesh，6 个 `IndirectSoldierAgent`（demo_framework），随机放置（互距²≥725），`SetTarget(自身)`，委托 `SoldierTactics`。
- `SoldierTactics.lua`：引擎内置影响图 `CreateInfluenceMap{CellWidth=2,CellHeight=1,BoundaryMinOffset=(0.18,0,0.35)}`；2 层 danger(0)/team(1)，500ms 更新，falloff 0.2 / inertia 0.5；**单点 `SetInfluence(±1)` + `SpreadInfluenceMap`**；事件由 agent 真实战斗/感知涌现；**实际只画 danger 层（team 层 draw 被注释）**。
- agent：BT 决策 + `SoldierController` 命令队列驱动动画；真实射击产生子弹（impulse 750、-5 血）；`AgentSenses` 视野 FOV±45°、raycast 遮挡、~1ms/帧扫描、敌人记忆 1000ms。

## 2. 差异总表

### 2.1 影响图算法（详见维度A）
| 项 | 旧版 | 新版 Sandbox17 现状 | 复刻改动 |
|---|---|---|---|
| 网格 cell / 扩散内核 / falloff·inertia | 2.0×1.0；27 邻居；0.2/0.5 | **一致**（C++ 端） | 无需改 |
| **team 层源类型** | 单点 ±1 + Spread | 半径源 `AddRadialSource(r=11)` 不扩散 | preset 设 `teamUseLegacyPointSpread=true, teamSpreadPasses=20`（路径已存在） |
| **danger 层源类型** | 单点 -1 + Spread | 半径源（r=8/10/12/14）**且从不扩散，无开关** | **改 Sandbox17.lua**：`_UpdateDangerousAreas` 加 `dangerUseLegacyPointSpread` 分支（`AddPointSource`+`SpreadLayer("danger",20,0.2,0.5)`），仿 team 层 |
| 单点强度 ±1 vs 半径衰减累加 | ±1 覆盖 | 半径衰减 `+=` 累加 | 切 point source 后自动恢复 |
| 视角 team / 正负极性 | team2 视角；team1=-1/team2=+1 | `dangerPerspectiveTeamId=0`/`teamPositiveTeamId=0` | 对齐到旧版 team2 对应 id（legacy_parity 用 1） |
| 网格原点相位 | navmesh.min + (0.18,0.35) | C++ 把 minX/minZ **吸附到 cellSize 栅格**（有意，对齐地板砖缝） | 仅"数值严格复刻"才需改 C++ `BuildFromNavMesh`；观感复刻可保留 |

### 2.2 绘制 / 可视化（详见维度B）
| 项 | 旧版 | 新版 Sandbox17 现状 | 复刻改动 |
|---|---|---|---|
| **画哪一层** | 仅 **danger(0)** | 仅 **team** | preset `drawDangerLayer=true, drawTeamLayer=false`（画的是不同数据层，最大可见差异） |
| **target 红圈** | **无** | `drawTargetRadius=true`（每 agent target 处红圈） | preset `drawTargetRadius=false`（旧版完全没有，最显眼的多余绘制） |
| **grid 轮廓线 α** | 黑 α=**0.5**（明显） | `grid=(0,0,0,0.14)`（很淡） | 改 `Sandbox17.lua:37` 为 α=0.5 |
| cell 几何相位 | 格**角**对齐 | 格**中心**对齐（错开半 cell=1.0） | 像素级对齐需改 C++ `BuildSquareVertices`；多数可不改 |
| value→颜色 / palette | 蓝正/黑零/红负，线性插值 | **数值与公式一致** | 无需改 |
| y 抬高 | 恒 0 | `teamDrawYOffset=-0.03`（仅 team 层；danger 已 y=0） | 画 danger 层后该参数不作用，无需改 |
| 填充+线框叠加 / 材质 | 一致结构（新版 overlay 材质更稳） | 一致 | 无需改 |
| agent / event marker | 无（默认） | 默认关 | 一致 |

### 2.3 agent 行为 / 动作轨迹（详见维度C）
| 项 | 旧版 | 新版（DT 默认） | 复刻改动 |
|---|---|---|---|
| **Move 段时长** | **500ms** 切片重评 | **2000ms** | preset `move.segmentMs=500`（轨迹"频繁重评 vs 一根筋"最大差异） |
| **Pursue repath** | **每帧** FindPath 到敌当前位 | **500ms** 且位移²>1 才 repath | `PursueAction.lua` repath 间隔→0、去位移阈值 |
| **Pursue 终止半径** | 3.0 | 2.0 | `PursueAction.lua` `_PURSUE_REACH=3.0` |
| 决策结构 / 转向 / 速度 / 到达半径 / steering 权重 / 阻尼 | — | **逐行/逐值一致** | 无需改 |
| 射击节奏 | 动画长度驱动；子弹由动画状态回调发 | 固定 600ms 上限；200ms 计时发弹 | 用 fire 动画播完驱动、动画回调发弹（精确复刻） |
| 换弹节奏 | 动画长度 | 固定 1500ms | 用 reload 动画长度驱动 |
| Flee 选点 | 32 点危险评分选最优 | 仅"离敌≥min 的可达点"（无危险评分） | `FleeAction` 接入影响图/事件危险评分 |
| RandomMove 选点 | while 重试至可达 | 取一次 RandomPoint | 加 while 重试或 Move 内重建 |
| mass/maxForce / maxAmmo | 90.7 / 1000 / 10 | 需核对 SoldierObject 默认 | 确认或显式设 |

### 2.4 事件来源 / 感知（详见维度D）
> 重要更正：Sandbox17 默认 **不是脚本导演**。默认 `scriptedEvents=false, useCppEventSource=true`，事件由 C++ 真实涌现（VisionSensor→EnemySighted、WeaponComponent→BulletShot、碰撞→BulletImpact、SetHealth→DeadFriendlySighted）。这已是旧版"真实涌现"的等价路径，只是感知参数未对齐。

| 项 | 旧版 | 新版 Sandbox17 现状 | 复刻改动 |
|---|---|---|---|
| **FOV** | 半角 45°（±45=90°锥） | **360° 全向** | preset `perception.fieldOfViewDegrees=90`（必改；新版"全知"导致锁敌过早） |
| **感知频率** | ~1ms/帧 | `visionIntervalMs=100, staggerScans=true` | preset `visionIntervalMs=1, staggerScans=false` |
| 事件 TTL（子弹） | 1500ms | `eventTtlMs=1800` | preset `eventTtlMs=1500` |
| 视距 | 隐式无限 | 60 | 保持 60 或调大（一般够） |
| EnemySighted 记忆窗 | 1000ms | sensor TTL 控制 | 对齐 1000ms |
| 事件总线 / 4 类事件触发 / seenAt 语义 | — | 机制等价 | 无需改（参数对齐即可） |
| `useLegacyAgentEvents` 路径 | — | **只发 EnemySighted**，不发 bullet/dead（legacy agent 不真开火） | 不要单靠它；用 C++ 真实事件源 + 参数对齐 |

## 3. 完全复刻分阶段计划

> 落点：除标注 [C++] 外全部是 `bin/res/scripts/config/sample_presets.lua`（Sandbox17 或 legacy_parity 的 chapter9Tactics/aiBlackboard）与 `bin/res/scripts/...lua`（action / Sandbox17.lua），**纯 Lua、不需编译**。

**P0 — 影响图与绘制（最大可见差异，纯 Lua）**
1. 画对层：`drawDangerLayer=true, drawTeamLayer=false`。
2. 关 target 红圈：`drawTargetRadius=false`。
3. grid α→0.5（`Sandbox17.lua:37`）。
4. team 层 legacy 点源+扩散：`teamUseLegacyPointSpread=true, teamSpreadPasses=20, teamFalloff=0.2, teamInertia=0.5`。
5. danger 层 legacy 点源+扩散：**改 `Sandbox17.lua` `_UpdateDangerousAreas`** 加 `dangerUseLegacyPointSpread` 分支（point source + `SpreadLayer("danger",20,0.2,0.5)` + `_SpreadCppInfluenceLayer`），preset 同步开。
6. 视角/极性：`dangerPerspectiveTeamId` / `teamPositiveTeamId` 对齐旧版 team2 对应 id。

**P1 — 感知与轨迹时序（纯 Lua）**
7. FOV：`perception.fieldOfViewDegrees=90`。
8. 感知频率：`perception.visionIntervalMs=1, staggerScans=false`。
9. 事件 TTL：`eventTtlMs=1500`。
10. Move 段时长：`move.segmentMs=500`。
11. Pursue：repath 每帧、终止半径 3.0（`PursueAction.lua`）。
12. 敌人记忆窗 1000ms。

**P2 — 战斗/选点细节（Lua，部分需运行核对）**
13. 射击/换弹用动画长度驱动（`ShootAction.lua`/`ReloadAction.lua`）。
14. Flee 危险评分 32 点选最优（`FleeAction` / `AgentActionContext::SelectFleeDestination`）。
15. RandomMove while 重试可达点。
16. 核对 mass=90.7 / maxForce=1000 / maxAmmo=10、子弹 impulse 750 / -5 血（[C++] `WeaponComponent::ShootBullet` 如有偏差）。

**P3 — 数值级网格相位（[C++]，可选）**
17. 若要网格相对地板的相位与旧版逐格一致：改 C++ `InfluenceMapSystem::BuildFromNavMesh` 去掉 minX/minZ 栅格吸附、改用 `aMin + boundaryMinOffset`，preset 补 `influenceMap.boundaryMinOffset={0.18,0,0.35}`。与"对齐地板砖缝"设计冲突，仅在确认错位是问题时做。

## 4. 验证链路（前置：当前 parity gate 失败）

- **现状**：`run_chapter9_parity_gate.ps1` 失败——旧版 `chapter_9_tactics` **从未接入 ParityTrace/HELLO_ 钩子**（`Sandbox.lua` 是原版），35s 跑完不产 trace。这是"可量化复刻"的第一道缺口（架构归档 P0 也是"先补能比较的能力"）。
- **修复**：照 chapter4/chapter3 先例，给旧版 `chapter_9_tactics/script/` 复制 `ParityTrace.lua` + 改 `Sandbox.lua` 接入 trace（采集 agent 位置/朝向/速度/目标 + 影响图 active cell + 事件计数）。旧引擎 `RenderCaptureGate`/`HELLO_PARITY_TRACE` 设施已存在。
- **对比手段**：① `run_chapter9_parity_gate.ps1`（trace 位置/目标/cell 容差）；② `run_chapter9_visual_capture.ps1`（old/new 同时刻截图肉眼对比绘制/影响图）。复刻每改一批，跑这两个看收敛。

## 5. 执行建议

- 推荐在 `chapter9_tactics_legacy_parity` preset 上补齐 P0/P1（多数项 legacy_parity 已对齐一半），保持 Sandbox17 默认 teaching 语义不变；用 `-Preset chapter9_tactics_legacy_parity` 跑出复刻效果。
- 先修验证链路（§4），再按 P0→P1→P2 逐批改、逐批用 visual_capture/parity_gate 验证，避免无数据调魔法数（架构归档反复强调）。
- C++ 改动只在 P0#5（其实是 Lua）、P2#16、P3#17 出现，且 P3 可选；主体复刻无需重编。

## 6. 本轮已落地修复（2026-06-16，方向 B 视觉复刻）

> 背景：旧版 chapter9 的 trace + fixed-spawn 接入被误删（旧仓库非 git，无法 git 恢复），导致 parity gate 跑完不产 trace。本轮先重建验证链路，再做视觉复刻。

| 项 | 改动 | 文件 |
|---|---|---|
| 重建旧版 trace 接入 | `ParityTrace.lua`（含 AgentSnapshot/Start/Tick，按 chapter4 模板重写） | `HelloOgre3DX/src/chapter_9_tactics/script/ParityTrace.lua`（新建） |
| 重建旧版 fixed-spawn + 钩子 | trace + 6 固定出生点 + camera preset + hide UI/influence + agent count/spawn override；消费 `HELLO_PARITY_*`/`HELLO_CH9_*` | `HelloOgre3DX/.../Sandbox.lua`（重写） |
| 修 parity gate 新版 trace 读取 | 调 smoke 前设 `HELLO_PARITY_TRACE_FILE=$ModernTrace`，新版直接写文件（outputPath 非空自动不写 console），Sandbox.log 改 fallback | `tools/run_chapter9_parity_gate.ps1` |
| team 分配对齐 | 旧版 fixed-teams 从 id%2 交替 → 前 lightCount 分块（teamId1=team2=蓝），消除影响图颜色左右镜像 | `HelloOgre3DX/.../Sandbox.lua` |
| grid 线 α 可配 | `_colors.grid` → `_GetGridColor()` 读 `gridLineAlpha`（默认 0.14），legacy_parity 设 0.5 复刻旧版明显黑网格 | `Sandbox17.lua` + `sample_presets.lua` |

**视觉复刻验证结果**（visual_capture，出生点对齐）：2500ms 与 5000ms 两版影响图渲染（蓝红 cell / 网格线 / 贴面 / 调色板 / 颜色空间分布）**高度一致**。早期担心的 `target ring 颜色`差异系误判——高亮实为 agent 队伍色（蓝/红），两版一致。

**parity gate 量化现状**：team 分配已对齐；agent 逐帧 `pos/target` 仍发散（旧版纯随机 BT vs 新版脚本编排），属方向 A 范畴，非视觉渲染问题。

## 7. 逐帧 parity（方向 A）可行性结论

- **物理不是障碍**：chapter4 逐帧 parity 已 PASS（maxPosError 0.064），证明新旧物理/移动积分在确定输入下差异远小于容差（1.05）。
- **关 RandomPoint 是必要前提但不充分**：还需对齐决策时序（BT 段时长/状态切换）、消除敌人选择浮点临界、确定化/禁用战斗分支。
- **本质是对称编排**：新版已有 `legacyRandomMovePoints`/`legacyForceEnemyAfterMs`（编排新版确定序列），需在旧版注入完全相同的序列（关 RandomPoint + 喂相同固定点）。
- **第一步验证**：补回旧版 `HELLO_CH9_MOTION_PROBE` 接入（也被误删），跑单 agent 逐帧探针确认物理/时序在容差内，再决定是否全量对称编排。
- **代价**：即使做到也极脆弱，任一边微调即破——这是它需要维护大套 `legacyForce*` 的原因。
