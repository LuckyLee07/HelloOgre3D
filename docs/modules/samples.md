# Sample 场景（alias: samples）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

Sandbox1-19 是 AI 学习章节 + 回归面：每个隔离场景演示一个 AI 概念/工程特性，是 smoke 验证入口；Sandbox19「Relay Outpost / 中继站行动」把已有 AI/对象/战斗能力收成伴随式小队指挥任务。

## 2. 源码位置

- `bin/res/scripts/samples/`（agent 脚本在 `bin/res/scripts/agent/`，见 [[ai-scripts]]）
- Sandbox19 编排入口：[Sandbox19.lua](../../bin/res/scripts/samples/Sandbox19.lua)。独立模块为 [场地与导航](../../bin/res/scripts/samples/sandbox19_scene.lua)、[命令生命周期](../../bin/res/scripts/samples/sandbox19_commands.lua)、[战术 HUD](../../bin/res/scripts/samples/sandbox19_hud.lua)、[音频与设置](../../bin/res/scripts/samples/sandbox19_audio.lua)、[产品集成 fixture](../../bin/res/scripts/samples/sandbox19_product_selftest.lua)。

## 3. Sandbox 清单

| Sandbox | 演示 | | Sandbox | 演示 |
|---|---|---|---|---|
| 1 | 物理/射击基础 | | 10 | 感知/记忆/lastKnown |
| 2 | 转向行为 | | 11 | 多单位感知/通信 |
| 3 | 骨骼动画状态机 | | 12 | TeamBlackboard(C++)；M3 共享开/关对照 |
| 4 | FSM 直接控制 | | 13 | 影响图(Lua 教学) |
| 5 | 路径/navmesh 可视化 | | 14 | **= Sandbox13 别名空壳** |
| 6 | 间接控制 FSM + 导航 | | 15 | **= Sandbox13 别名空壳** |
| 7 | 决策树(DT) | | 16 | 感知压力(ai_perf) |
| 8 | 行为树(BT) | | 17 | Chapter9 战术(Lua-first) |
| 9 | Chapter7 知识源 | | 18 | Chapter9 战术(C++) |
| 19 | 中继站行动（持枪 commander + 两名 BT 队友 + 两段接敌/清场集合 + 暂停结算） |
| 20 | Crossfire：双机器人、三场手工遭遇、暂停规划与方向盾侧击 |

入口：`game_init.lua`(选 HELLO_SANDBOX_SAMPLE，默认 Sandbox19)、`fgui_init.lua`、`parity_trace.lua`、`runtime_diagnostics.lua`。

## 4. 公开能力要点

- Sandbox20 为独立 Crossfire 三关：维修庭院、偏置冷却区与双塔联锁区。标题 `1/2/3` 预览、Enter 进入；战斗 `1/2` 或点击机器人/标签/卡片选择，左键点地面移动、点可见敌人指定目标（兼容右键），顶部执行 / Space 暂停与共同提交；E / “推进 2 秒”共同提交并在 2000 ms 仿真后自动暂停，菜单/重试/切关取消旧计时；X 警戒、R 重试、M 静音、Esc 设置、H 帮助。菜单关闭保持规划状态；胜负立即冻结战斗，约 0.9 s 展示战场收尾后展开报告，期间 Enter 只展开报告；胜利报告再按 Enter 下一关，第三关后回选关。
- 关卡值定义在 `crossfire_encounters.lua`；HUD 只消费值并复用分层图元。scheduler 33 ms / 每帧最多 4 个单位，普通 Agent 复用 BT、真实物理弹/方向盾；射程 18 m（视野 22 m），指定目标死亡正确完成。第三关要交叉指定目标；实体哨卫与残骸继续挡弹；从真实枪口显示当前第一阻挡者，遇近塔/残骸遮挡需换位。预览不是抵达后的射击保证。`crossfire_review.lua` 只消费纯值快照，连续受阻至少 1200 ms 且新增至少两发才提醒；“查看受阻”暂停、选中对应机器人并标出当前阻挡处。复盘分开记录实弹侧击/盾挡次数与两机分别累计的射界受阻时长，后者不是实弹命中数；最长合格单机受阻片段保留纯值历史，移动/死亡不抹掉证据，结算点明机器人、目标与建议，重开清空。
- `crossfire_effects.lua` 固定复用 28 个无碰撞视觉面：枪口 8 / 护盾 8 / 命中 8 / 摧毁 4，寿命 90/220/150/620 ms，只消费真实发弹与 Blackboard 命中/死亡事件。规划冻结，结果阶段继续消退；重试 Reset 复用，切关先 Release 断引用再清场。`Scene.UpdateVisuals` 同样区分仿真与 UI 时钟，风扇仅执行时旋转，胜利供电 900 ms 从前向后恢复；失败熄灭、重试复位。详见[动态场景与战斗演出](../dev-design/plans/2026-09-19-crossfire-motion-finish.md)。
- 首关表现基准：HUD 底栏 76 px、镜头安全区 top96/bottom=height-104，单位标签避让机体；执行中仅保留当前受阻射界或主动查看，真实锁定点用间断预警线。Guard 的纯值阶段进度/剩余时间驱动蓄力、连发与冷却显示；`crossfire_signals.lua` 每塔一个无碰撞炮口面，四档充能与冷却短线随仿真冻结，重试复用、切关先断引用。音量经 master 即时更新，同帧命中声/特效均取最后物理事件。详见[首关联合精修](../dev-design/plans/2026-09-19-crossfire-court-polish.md)。
- `crossfire_tactics.lua` 只读真实锁定对象/阶段，顶部提示、机体标签和卡片同步指明受威胁机器人；冷却提示明确护盾仍有效。没有 AI 目标时，Feedback 仅为显示查询最近存活哨卫的当前枪口射界，注明“附近射界”，不写命令、不纳入实射受阻复盘。详见[战术窗口与重试反馈](../dev-design/plans/2026-09-19-crossfire-tactical-decisions.md)。
- `crossfire_profile.lua` 的 `Precision` 与 Record 共用最高奖章条件（双机存活、损伤不超过 60、关卡 parMs 内），选关和结算直接显示；原计分、排序及存档格式不变。独立保存声音、引导和三关最佳记录到 `crossfire_settings.cfg`；仅解析有界数字数据。记录按奖章、用时、损伤排序。`HELLO_CROSSFIRE_PROFILE` 用于隔离测试，`HELLO_CROSSFIRE_QUICKSTART=1` / `HELLO_CROSSFIRE_LEVEL=1..3` 供针对性回放；普通启动进入标题，不携带开发者成绩。`HELLO_AUDIO_SILENT=1` 不可被 M 解除。
- 重开先释放夹具/计划/观察引用，再清弹、Agent、团队状态；切关额外 `clearAllObjects(MGR_OBJ_BLOCK,true)` 清视觉平面与实体，重建 shadows 并替换同名导航。Scene 只复用一盏 Light，避免累计照明。图元/按键沿锁存继续复用，失焦由 InputManager 发 key-up。
- `python3 tools/run_crossfire_gate.py` 分自然胜负、物理夹具、独立查询夹具（`--mode queries`）、三关交叉目标与标题/设置/记录重启；自然/三关回放另核对结果收尾时钟、特效过期/复用和供电序列；`--capture-ms` 可指定连续画面时刻。界面另用 `--mode interface --width 960 --height 720`；`--mode pacing` 覆盖两秒推进、菜单/重试取消、恢复持续执行及收复状态复位。镜头适配有效战场，VEGA 青绿尖头、ROOK 钢蓝宽肩；中文字库、双色路径、落点表面反馈和持续战术提示见[可读性升级](../dev-design/plans/2026-09-19-crossfire-readable-combat.md)。原创地砖表面、三种区域装饰和 `Scene.SetOutcome` 供电指示全部保持原碰撞/导航；新增透明面无刚体，重试复位、切关清引用，见[场景与节奏升级](../dev-design/plans/2026-09-19-crossfire-scene-rhythm.md)。技术与作者主观验收分开，详见[三关实施记录](../dev-design/plans/2026-09-19-crossfire-three-encounters.md)。

- `python3 tools/run_crossfire_tactics.py --cases l1_split,l1_evade --repeat 2` 是普通输入的路线诊断，支持独立包 `--executable/--cwd/--launcher-default`。逐局隔离成绩/日志、禁硬件输入、静音，按实际相机投影并以额外地面点校准，保留输入/二进制/Lua 指纹。技术执行与 VICTORY/DEFEAT/INCOMPLETE 分列；没有终局的观察窗不算胜利，技术失败/超时返回非零。不传送或改血、不改战斗规则，也不替代原生鼠标试玩。

- ConfigManager 分层合并时，`spawnPoints`、`waveEnemyCounts`、`waveSpawnIndices` 整表替换（空表可清空）；其他配置表递归继承，包括按 agent id 索引的数值键 map。纯 Lua 回归入口 `lua5.1 tools/test_config_presets.lua`；详见 [配置修复与后台验证](../config-presets-2026-09-10.md)。

- Sandbox12 的 `team_sharing_experiment` preset 固定 A/B/敌人和一处实体遮挡，使用最小 BT 隔离发布、消费、移动与 TTL 清理。它只在 manifest 提供实验 run id 时启用，不改变默认 `team_blackboard` 教学场景。

- Sandbox19 场地为约 48×64 米单层中继站，中央直路与西侧遮挡路线汇入前方院区；复用 Nobiax 墙体/设备与士兵资源，材质入口为 `media/materials/sandbox19_relay.material`。relay 主体使用项目专用 `textures/sandbox19/relay_facade_diffuse.png`；庭院地面与中路共用项目专用 `courtyard_paving_diffuse_v3.png`；`Relay/Ground` 按世界坐标生成 2.4×3.2 米错缝板，接缝按屏幕像素覆盖衰减，轻微 tint 区分路线。北边界后墙与浅色双翼连成院区立面，中门保留较深设备门面，翼楼使用五块 Nobiax 小窗实体网格、一处项目自制 A1 喷涂和独立中性材质，压顶/屋顶设备不进入主路；另有结构框、信标和路线外设备。九块地表旧化及静态/角色接触阴影由无刚体 visual plane 承载，不进入 navmesh 或 AI 视线判定。此前 24 组 crossed-card 植被在实机形成黑边纸片，现已全部移除。六组长掩体位于约 `x=±8`，保留中央与西侧路线；其它装饰碰撞嵌入既有实体或位于非行走区。`sandbox19_scene.lua` 分别检查出生点/两条路线/终点的完整路径，以及真实 Bullet 墙体和地面射线；导航与路径 debug 默认关闭，F3 按需打开。
- 浅色混凝土墙与掩体的 `Relay/Concrete` / `Relay/Cover` 使用与现有 albedo 配对的 `courtyard_concrete_normal_v1.png`；地面板缝、路线和金属门面仍用平法线，避免不配对的裂纹方向。法线在 GL3+ 基础材质修复后才实际参与光照，细节幅度刻意较小；材质来源见[资源说明](../../media/textures/sandbox19/SOURCE.md)，共享 shader 与跨 sample 约束见 [[runtime]]。

- 对局为 `PREPARE → WAVE(门前两名守卫) → ADVANCE → WAVE(院区两名守卫) → REGROUP → VICTORY/DEFEAT`。准备阶段冻结，点击 START MISSION 或 Enter 主动开始；清除首段敌人后，指挥官或存活队友越过院区入口才产生第二段前方守卫。全部清敌后仍需指挥官与至少一名存活队友到达集合区；指挥官死亡或两名队友都阵亡即失败。普通流程没有 FORCE_CONTACT 传送或自动清敌，45 秒接敌无进展只报告 STALEMATE 与调整提示。

- 当前流程由 `Sandbox19.lua` 和 scene anchors 编排，preset 提供出生点、AI 配置及两段敌人数/出生槽位；旧 `prepareMs` / `intermissionMs` / `criticalRetreatMs` 已移除，不再存在自动开始、波间恢复或低血量回避计时。
- 当前 `commanderMatch` 为指挥官 160 HP、每名队友 240 HP、每名守卫 100 HP。AI 在 12 米内且可见时射击，追击到达距离为 9 米；队友/守卫视野分别为 24/20 米、默认接敌半径为 26/16 米。无命令且非 hold 的队友每 250ms 更新玩家前方左右编队锚点；显式命令仍优先。sample 启用 `sandbox19.aimedFire`，从真实枪口向目标位置发射 `ShootBulletAt`，伤害仍依赖 Bullet 子弹碰撞。物理弹明确以 48 m/s 发射，弹道、枪口焰与命中由已修复的粒子材质呈现；`weapon.actionOwnsFire` 避免动画事件重复发射，其他章节未启用开关时保持原路径。

- 各 sample `Sandbox_Initialize` + 键盘/鼠标事件；是 AI 行为回归基线。

- 2026-09-12 实机视觉迭代修复非等边盒体的巨柱/长条，Sandbox19 现使用中性日光、方向光深度投影、低对比铺地 v3 与连接棚架。`HELLO_RENDER_SHADOWS=0/false/off/no` 可关闭作对照。材质与资源不代表概念稿品质，性能、自然战斗和 fixture 分开记录，见[实机收敛](../dev-design/plans/2026-09-12-sandbox19-visual-goal.md)。

## 5. 约束与红线

- **Sandbox14/15 是 Sandbox13 的别名空壳**（4 行 require，无独立实现）；要独立需新建 preset+入口（行为变更，基线后做）。
- sample 是回归面：改 AI/对象/组件/绑定必须回归对应 sample（smoke `status=PASS`）。
- Sandbox17/18 是 Chapter9 对照面，受 `run_chapter9_parity_gate`/`visual_capture` 守。
- Sandbox19 的 `commander_soldier` / `ai_soldier` profile 必须保持 controller 互斥；commander 装 `PlayerController` 与 `WeaponComponent`，不装 `AIController`。产品 fixture 检查玩家有武器；活跃玩法捕获鼠标，相对位移先转 FOLLOW 镜头，身体和真实枪口持续追随镜头水平视线；LMB 或 Space 按住开火、松开停火，R 换弹，开火期间仍允许 WASD 移动。W/S 沿角色当前物理朝向前后移动，A/D 相对角色侧移；Q/E 辅助转镜，中键仅在临时指针模式拖动镜头。
- Sandbox19 显式配置 FOLLOW 相机（距离 6.5、高 3.2、前视 0、眼高 1.5，滚轮距离范围 5.5–11），并启用角色朝向的 WASD 移动、Q/E 镜头偏航；控制器退出 FOLLOW 时重置 profile，其他 sample 保留原默认控制。macOS 默认选取受支持且不超过 4× 的 FSAA，显式 `HELLO_RENDER_FSAA` 可覆盖；scene-only 的 Relay/SceneGrade 场景纹理继承主窗口 FSAA，资源不可用则保持原场景；Gorilla/FairyGUI 不进入调色纹理。详见 [[runtime]]。
- 按住 Alt 临时释放鼠标、显示指针后，可左键点选/拖框或点击队友卡选择友军；1/2 选人、Tab 全选存活队友。该模式的右键可见敌人 / F 集火，右键地面指定移动点；G 集结到下令时的指挥官位置并给队友分开落点，T 撤至入口安全点并守住局部区域，X 取消并恢复当前位置的自主守卫。HUD 命中先消费鼠标事件；模态面板拦截场景点击。暂停时指针也恢复。
- `sandbox19_commands.lua` 单独拥有 `command.*`、活动命令表和按队友统计的终态，BT 只执行。语义命令没有统一 8 秒 TTL：集火目标死亡完成，执行者失去目标可见性失败；移动到达完成，路径失败或 10 秒无进展失败；新有效命令替换旧命令，取消/结算/重开清理命令拥有的移动请求。两名队友同时接受一次操作会记录两条 ally orders，前置检查拒绝不计 issued；每个已接受命令只记录一次 completed/failed/replaced/cancelled。
- `Sandbox19CommandBT` 使用专用限定范围守卫树，复用共享动作而不再以整棵 `SoldierBT` 兜底；共享章节树保持独立。集火必须通过各执行者的 `CanSeeEnemy(id)`，敌人拾取与目标标记也检查小队当前可见性。见 [[ai-behavior]]。
- 正常 HUD 为 Gorilla `UIFrame` / `UIPolygon`：上方目标与敌人数、左下两张 HP/选择/指令卡、下方情境命令、右下指挥官状态，以及短反馈。指挥官/队友 HP 下降由 `Sandbox19.lua` 的真实数值差分产生 220ms 提示：指挥官闪四边和状态卡，队友只闪对应卡；暂停/结算清掉反馈计时，重开同时清空采样。默认无雷达和长期导航线；准备、暂停、结算使用独立模态布局。HUD 只接受纯值模型，图元由 UIManager 持有并复用，Hide/Destroy 只隐藏；不能跨已销毁 UI 场景复用旧实例。
- Esc 暂停、准备和结算通过 `GameManager:SetSimulationPaused` 冻结仿真时钟、对象/AI/物理与 Lua tick，HUD 继续更新；重试开启新任务，BRIEFING 返回准备入口，退出调用 `RequestQuit`。重开清理旧 agent、子弹、选择/拖拽、命令统计、观察 trace 与音频采样，HUD/静态场地复用。
- `sandbox19_audio.lua` 统一管理声音和显示设置：通过 `SandboxAudio` 播放自制 PCM 短音，覆盖选择、命令接受/失败、开始/暂停/结果，以及弹药消耗/HP 下降反馈；后两类按实际状态差分采样并限频。暂停层可在 1280×720、1600×900、1920×1080 三档间切换；音量/静音和所选 `window=宽x高` 保存在工作目录的 `relay_settings.cfg`。显式 `HELLO_WINDOW_WIDTH/HEIGHT` 启动覆盖优先并锁定 DISPLAY；后台自动化跳过保存尺寸。声音后端仍是单路替换播放，无空间音效或混音；设备不可用或播放失败时 UI 显示不可用，不能把静态接线当作听觉验收。
- **`ObjectManager:getObjectById` 未导出给 Lua**（`.h` 有声明但不在 tolua 块内）：Lua 侧按 id 找对象只能扫
  `getAllAgents()`；顺带避免跨帧持有 agent userdata 的悬垂风险。
- sample reload 须清理上轮 agent/UI/debug draw；Sandbox19 重开还会 `TeamBlackboard:Reset()`，当前玩家命令不额外写 TeamBlackboard typed fact。

- M1 单单位观察（2026-09-05）：Sandbox19 右侧观察面板默认隐藏（2026-09-06 试玩调整），按 I 开启，以最小选中 id 为观察对象；`I` 开关、`O` 导出最后 snapshot 与 BT trace。`ai_observer.lua` 只读采样（200ms），只在选中对象上临时开启 trace，切换/关闭恢复原设置；只保留 id 与标量/字符串，不跨帧保留对象 userdata。
- `Observer.Command` 对 `command.semantic=true` 显示执行状态，不把旧 TTL 当作当前指令寿命；维护与清理归 `sandbox19_commands.lua`，独立于面板开关。
- `HELLO_SANDBOX19_PRODUCT_TEST=1` 或 smoke 模式启用新产品 fixture；旧 `HELLO_SANDBOX19_OBSERVATION_SELF_TEST=1` 仅作为触发同一 fixture 的兼容入口，不能继续声称覆盖旧 M1 观察全套用例。

## 6. 数据流 / 与其他模块关系

`game_init → require SandboxX → Sandbox_Initialize`（创建 agent、影响图、team）；驱动 [[ai-scripts]] [[ai-controller]] 等。

## 7. 验证策略

- 各 sample 使用 `tools/run_sandbox_smoke.ps1 -Sample SandboxX -NoTail`；Chapter9 跑 parity_gate。只有确认旧进程属于本轮验证或已获授权时才加 `-StopExisting`。
- 默认 Sandbox18（无 preset 或 `Sandbox18` 别名）同样要求 Chapter9 C++ 全部 smoke marker；`[...Smoke] FAIL` 会令 runner 失败。2026-09-10 修复了 Lua 对旧诊断摘要结尾的硬匹配及 runner 漏报，原失败与复测见[本轮验收](../playtest-relay-2026-09-10.md)。
- 产品 gate：用 Python 3 执行 `tools/run_sandbox19_stability.py --product-fixture --timeout 90`，必须同时取得产品 `PASS all=true synthetic=true`、导航与真实静态碰撞 marker。强化 fixture 等待真实 BT 写入 executing 与实际动作，覆盖替换、取消幂等、目标失去视野/死亡、执行者死亡、暂停、结算及活动命令重开清理；它显式提高测试生命、挪动/杀死对象，因此不证明自然战斗平衡或外部输入。`tools/run_m1_smoke.py` 采用同一组 marker，macOS 仍需实际运行。
- 自然对局 gate：`python3 tools/run_sandbox19_natural.py --timeout 150` 使用 `ranged-and-regroup.txt` 的合成输入，但不启用强化 fixture；只在两波、清敌、`director=none` 自然胜利、重开、暂停和正常退出按序成立时 PASS。需要可渲染的桌面图形会话；它仍不能代替人工操作和扬声器听感。2026-09-12 的运行与剩余边界见 [鼠标与镜头控制](../dev-design/plans/2026-09-12-sandbox19-mouse-camera-control.md)。
- 当前验收边界：Windows Release 全量重编、Lua 5.1 语法、强化产品 fixture，以及独立包 69.267 秒自然通关、重开/正常退出、720p/1080p 与设置保存已有证据，见[本轮验收](../playtest-relay-2026-09-10.md)。2026-09-11 至 12 又完成 macOS arm64 Release、产品 fixture、Sandbox19/6/7/8 smoke、场景色调/UI 隔离、720p→900p 同进程 resize、保存值重启读取和 1600×900 FairyGUI 适配，见[视觉复盘](../sandbox19-visual-convergence-2026-09-11.md)与[动态窗口计划](../dev-design/plans/2026-09-12-sandbox19-dynamic-window.md)。当前机器缺 Lua 5.1 可执行文件，相关脚本仅由 luac 5.3 解析并由游戏内嵌 Lua 5.1 实际运行；macOS 的 1920×1080 请求受可见工作区约束为实际 1920×945，因此不能称为 macOS 1080p PASS。HLSL/D3D9 后处理、Windows 普通窗口动态 resize、人工外部输入与扬声器听感仍未验证，不据此宣称 P3 全部验收完成。
- `HELLO_INPUT_REPLAY=<事件文件>` 是应用内部合成输入回放，日志标 `synthetic=true`，可检查输入路由但不能冒充人工操作。Windows 后台验证用 `HELLO_WINDOW_BACKGROUND=1` 在虚拟桌面范围外创建不激活的 D3D9 渲染窗口并禁用硬件输入；跨平台可用 `HELLO_WINDOW_WIDTH/HEIGHT` 指定启动尺寸并锁定暂停层 DISPLAY，`HELLO_AUDIO_SILENT=1` 临时静音而不改变保存偏好。配置与日志边界见 [[runtime]]；多尺寸截图/回放仍不能替代外部点击验收。
- M2 入口为 `python tools/run_ai_experiment.py tools/experiments/sandbox19-focus-order.json`。runner 清除继承的 `HELLO_*` 后按 manifest 生成成对内部回放，保存 HEAD/dirty patch、配置原文与 hash、可执行文件 hash、每局原始日志和结果；只统计配置证据完整且达到终局/观察窗的局。`HELLO_EXPERIMENT_RUN_ID` 与 `HELLO_EXPERIMENT_HORIZON_MS` 仅为该入口启用 Sandbox19 结构化事件，普通游玩不输出；实际批次必须在可创建 D3D9 device 的桌面会话运行。精确变量与边界见 [M2 成对实验计划](../dev-design/plans/2026-09-10-m2-repeatable-comparison.md)。
- 通用 runner 的 `event-comparison` profile 由 manifest 声明必需/禁止事件、字段断言和数值指标；观察窗未达到会以 `UNRESOLVED` 失败，已出现的指标源事件必须提供数值字段，确实不适用的缺失字段须在 manifest 显式标为 `optional`。`replay.wait_for_player` 默认 `true`，无 PlayerController 的受控 sample 必须显式设为 `false`。M3 入口为 `python tools/run_ai_experiment.py tools/experiments/sandbox12-team-sharing.json`；修复后二次正式批次仍为 18/18，并额外验证事实失效后的累计行程、最大偏移、残余路径与末速度，结果见 [M3 实验记录](../sandbox12-team-sharing-experiment-2026-09-10.md)。

## 8. 已知 gap / 相关文档

- 待：Sandbox14/15 独立化、parity trace 正式化、smoke 入 CI。`docs/archive/long-term-iteration-plan.md` §1/§5。

- [2026-09-06 Computer Use 试玩与改进](../playtest-2026-09-06.md)、[第二轮视野与接敌布局](../playtest-visibility-2026-09-06.md)、[稳定性记录](../stability-2026-09-06.md)与[僵持修复](../stalemate-2026-09-06.md)保留为旧三波版本的历史证据；其中 RMB 选择、8 秒 TTL、tank 控制、雷达、自动波间恢复和 FORCE_CONTACT 不再是现行合同。
- `tools/run_sandbox19_stability.py` 已迁为显式 `--product-fixture` 入口；旧 `--rounds/--probe/--scripted-victory/--low-health` 会立即报迁移说明，不启动游戏。`sandbox19_stability_selftest.lua` / `sandbox19_retreat*.lua` 仅保留历史实现，当前 sample 不挂载；runner 终止自身子进程不构成正常窗口关闭证据。

2026-09-12 核心修复接入共享粒子程序、上下身分层与真实枪口发弹；MMB/QE 跟随旋转、静态遮挡和显示插值已接入。后台/回放禁硬件输入，Space 与点选释放互不干扰。自然对局68.508秒胜利、集合与重开证据及剩余资产差距见[体验修复](../dev-design/plans/2026-09-12-sandbox19-experience-fixes.md)。

- 2026-09-12 场景资产追加：六组掩体改用自制倒角实体，外侧配置十二个高/低补给箱；三张原生 Ogre 网格由 `tools/generate_relay_meshes.py` 重建，保留 submesh 材质，碰撞仍由同一网格构造。产品自测新增低箱实体/上方净空和高箱实体射线，原主路/绕行不变。来源与实机见[场景资产与地表升级](../dev-design/plans/2026-09-12-sandbox19-scene-assets.md)。
- 2026-09-13 构图续办：FOLLOW 镜头维持 6.5/3.2/眼高 1.5；院区敌人后侧新增两组 2.4m 短掩体及两件外肩补给物，西侧绕行墙使用单独的较浅混凝土材质。中央门路与西绕行不变；自检额外核对短掩体命中和门前净空。HUD 任务副标题缩为 `Hostiles`，避免 900p 院区阶段丢失敌人数。实机与回归证据见[构图续办](../dev-design/plans/2026-09-13-sandbox19-visual-composition.md)。
