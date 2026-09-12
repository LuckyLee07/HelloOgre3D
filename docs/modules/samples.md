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
| 19 | 中继站行动（持枪 commander + 两名 BT 队友 + 两段接敌/清场集合 + 暂停结算） | | | |

入口：`game_init.lua`(选 HELLO_SANDBOX_SAMPLE，默认 Sandbox19)、`fgui_init.lua`、`parity_trace.lua`、`runtime_diagnostics.lua`。

## 4. 公开能力要点

- ConfigManager 分层合并时，`spawnPoints`、`waveEnemyCounts`、`waveSpawnIndices` 整表替换（空表可清空）；其他配置表递归继承，包括按 agent id 索引的数值键 map。纯 Lua 回归入口 `lua5.1 tools/test_config_presets.lua`；详见 [配置修复与后台验证](../config-presets-2026-09-10.md)。

- Sandbox12 的 `team_sharing_experiment` preset 固定 A/B/敌人和一处实体遮挡，使用最小 BT 隔离发布、消费、移动与 TTL 清理。它只在 manifest 提供实验 run id 时启用，不改变默认 `team_blackboard` 教学场景。

- Sandbox19 场地为约 48×64 米单层中继站，中央直路与西侧遮挡路线汇入前方院区；复用 Nobiax 墙体/设备与士兵资源，材质入口为 `media/materials/sandbox19_relay.material`。relay 主体使用项目专用 `textures/sandbox19/relay_facade_diffuse.png`；庭院地面与中路共用项目专用 `courtyard_paving_diffuse_v2.png` 混凝土板缝贴图，以轻微 tint 区分路线。北边界后墙与浅色双翼连成院区立面，中门保留较深设备门面，翼楼使用六块 Nobiax 小窗实体网格和独立中性材质，压顶/屋顶设备不进入主路；另有结构框、信标和路线外设备。九块地表旧化及静态/角色接触阴影由无刚体 visual plane 承载，不进入 navmesh 或 AI 视线判定。此前 24 组 crossed-card 植被在实机形成黑边纸片，现已全部移除。六组长掩体位于约 `x=±8`，保留中央与西侧路线；其它装饰碰撞嵌入既有实体或位于非行走区。`sandbox19_scene.lua` 分别检查出生点/两条路线/终点的完整路径，以及真实 Bullet 墙体和地面射线；导航与路径 debug 默认关闭，F3 按需打开。
- 浅色混凝土墙与掩体的 `Relay/Concrete` / `Relay/Cover` 使用与现有 albedo 配对的 `courtyard_concrete_normal_v1.png`；地面板缝、路线和金属门面仍用平法线，避免不配对的裂纹方向。法线在 GL3+ 基础材质修复后才实际参与光照，细节幅度刻意较小；材质来源见[资源说明](../../media/textures/sandbox19/SOURCE.md)，共享 shader 与跨 sample 约束见 [[runtime]]。

- 对局为 `PREPARE → WAVE(门前两名守卫) → ADVANCE → WAVE(院区两名守卫) → REGROUP → VICTORY/DEFEAT`。准备阶段冻结，点击 START MISSION 或 Enter 主动开始；清除首段敌人后，指挥官或存活队友越过院区入口才产生第二段前方守卫。全部清敌后仍需指挥官与至少一名存活队友到达集合区；指挥官死亡或两名队友都阵亡即失败。普通流程没有 FORCE_CONTACT 传送或自动清敌，45 秒接敌无进展只报告 STALEMATE 与调整提示。

- 当前流程由 `Sandbox19.lua` 和 scene anchors 编排，preset 提供出生点、AI 配置及两段敌人数/出生槽位；旧 `prepareMs` / `intermissionMs` / `criticalRetreatMs` 已移除，不再存在自动开始、波间恢复或低血量回避计时。
- 当前 `commanderMatch` 为指挥官 160 HP、每名队友 240 HP、每名守卫 100 HP。AI 在 12 米内且可见时射击，追击到达距离为 9 米；队友/守卫视野分别为 24/20 米、默认接敌半径为 26/16 米。无命令且非 hold 的队友每 250ms 更新玩家前方左右编队锚点；显式命令仍优先。sample 启用 `sandbox19.aimedFire`，从真实枪口向目标位置发射 `ShootBulletAt`，伤害仍依赖 Bullet 子弹碰撞。物理弹明确以 48 m/s 发射，短时世界空间曳光由 BillboardChain 显示；`weapon.actionOwnsFire` 避免动画事件重复发射，其他章节未启用开关时保持原路径。

- 各 sample `Sandbox_Initialize` + 键盘/鼠标事件；是 AI 行为回归基线。

## 5. 约束与红线

- **Sandbox14/15 是 Sandbox13 的别名空壳**（4 行 require，无独立实现）；要独立需新建 preset+入口（行为变更，基线后做）。
- sample 是回归面：改 AI/对象/组件/绑定必须回归对应 sample（smoke `status=PASS`）。
- Sandbox17/18 是 Chapter9 对照面，受 `run_chapter9_parity_gate`/`visual_capture` 守。
- Sandbox19 的 `commander_soldier` / `ai_soldier` profile 必须保持 controller 互斥；commander 装 `PlayerController` 与 `WeaponComponent`，不装 `AIController`。产品 fixture 检查玩家有武器；LMB 保留选择，Space 按住开火、松开停火，R 换弹，开火期间仍允许 WASD 移动。相机相对移动时，A/D 沿正确左右方向横移；开火瞄准沿镜头水平朝向，不随横移改射线，Q/E 转向在静止时同样生效。
- Sandbox19 显式配置 FOLLOW 相机（距离 6.5、高 3.2、前视 0、眼高 1.5，滚轮距离范围 5.5–11），并启用相机平面 WASD 移动、Q/E 镜头偏航；控制器退出 FOLLOW 时重置 profile，其他 sample 保留原默认控制。macOS 默认选取受支持且不超过 4× 的 FSAA，显式 `HELLO_RENDER_FSAA` 可覆盖；scene-only 的 Relay/SceneGrade 场景纹理继承主窗口 FSAA，资源不可用则保持原场景；Gorilla/FairyGUI 不进入调色纹理。详见 [[runtime]]。
- 左键点选/拖框、1/2 或两张队友卡选择友军，Tab 全选存活队友；右键可见敌人 / F 集火，右键地面指定移动点，G 集结到下令时的指挥官位置并给队友分开落点，T 撤至入口安全点并守住局部区域，X 取消并恢复当前位置的自主守卫。HUD 命中先消费鼠标事件；模态面板拦截场景点击。
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
- 当前验收边界：Windows Release 全量重编、Lua 5.1 语法、强化产品 fixture，以及独立包 69.267 秒自然通关、重开/正常退出、720p/1080p 与设置保存已有证据，见[本轮验收](../playtest-relay-2026-09-10.md)。2026-09-11 至 12 又完成 macOS arm64 Release、产品 fixture、Sandbox19/6/7/8 smoke、场景色调/UI 隔离、720p→900p 同进程 resize、保存值重启读取和 1600×900 FairyGUI 适配，见[视觉复盘](../sandbox19-visual-convergence-2026-09-11.md)与[动态窗口计划](../dev-design/plans/2026-09-12-sandbox19-dynamic-window.md)。当前机器缺 Lua 5.1 可执行文件，相关脚本仅由 luac 5.3 解析并由游戏内嵌 Lua 5.1 实际运行；macOS 的 1920×1080 请求受可见工作区约束为实际 1920×945，因此不能称为 macOS 1080p PASS。HLSL/D3D9 后处理、Windows 普通窗口动态 resize、人工外部输入与扬声器听感仍未验证，不据此宣称 P3 全部验收完成。
- `HELLO_INPUT_REPLAY=<事件文件>` 是应用内部合成输入回放，日志标 `synthetic=true`，可检查输入路由但不能冒充人工操作。Windows 后台验证用 `HELLO_WINDOW_BACKGROUND=1` 在虚拟桌面范围外创建不激活的 D3D9 渲染窗口并禁用硬件输入；跨平台可用 `HELLO_WINDOW_WIDTH/HEIGHT` 指定启动尺寸并锁定暂停层 DISPLAY，`HELLO_AUDIO_SILENT=1` 临时静音而不改变保存偏好。配置与日志边界见 [[runtime]]；多尺寸截图/回放仍不能替代外部点击验收。
- M2 入口为 `python tools/run_ai_experiment.py tools/experiments/sandbox19-focus-order.json`。runner 清除继承的 `HELLO_*` 后按 manifest 生成成对内部回放，保存 HEAD/dirty patch、配置原文与 hash、可执行文件 hash、每局原始日志和结果；只统计配置证据完整且达到终局/观察窗的局。`HELLO_EXPERIMENT_RUN_ID` 与 `HELLO_EXPERIMENT_HORIZON_MS` 仅为该入口启用 Sandbox19 结构化事件，普通游玩不输出；实际批次必须在可创建 D3D9 device 的桌面会话运行。精确变量与边界见 [M2 成对实验计划](../dev-design/plans/2026-09-10-m2-repeatable-comparison.md)。
- 通用 runner 的 `event-comparison` profile 由 manifest 声明必需/禁止事件、字段断言和数值指标；观察窗未达到会以 `UNRESOLVED` 失败，已出现的指标源事件必须提供数值字段，确实不适用的缺失字段须在 manifest 显式标为 `optional`。`replay.wait_for_player` 默认 `true`，无 PlayerController 的受控 sample 必须显式设为 `false`。M3 入口为 `python tools/run_ai_experiment.py tools/experiments/sandbox12-team-sharing.json`；修复后二次正式批次仍为 18/18，并额外验证事实失效后的累计行程、最大偏移、残余路径与末速度，结果见 [M3 实验记录](../sandbox12-team-sharing-experiment-2026-09-10.md)。

## 8. 已知 gap / 相关文档

- 待：Sandbox14/15 独立化、parity trace 正式化、smoke 入 CI。`docs/archive/long-term-iteration-plan.md` §1/§5。

- [2026-09-06 Computer Use 试玩与改进](../playtest-2026-09-06.md)、[第二轮视野与接敌布局](../playtest-visibility-2026-09-06.md)、[稳定性记录](../stability-2026-09-06.md)与[僵持修复](../stalemate-2026-09-06.md)保留为旧三波版本的历史证据；其中 RMB 选择、8 秒 TTL、tank 控制、雷达、自动波间恢复和 FORCE_CONTACT 不再是现行合同。
- `tools/run_sandbox19_stability.py` 已迁为显式 `--product-fixture` 入口；旧 `--rounds/--probe/--scripted-victory/--low-health` 会立即报迁移说明，不启动游戏。`sandbox19_stability_selftest.lua` / `sandbox19_retreat*.lua` 仅保留历史实现，当前 sample 不挂载；runner 终止自身子进程不构成正常窗口关闭证据。
