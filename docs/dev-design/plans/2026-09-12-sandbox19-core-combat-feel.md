# Sandbox19 核心战斗体感修正

日期：2026-09-12
状态：核心切片已实现并经 macOS GL 运行验证；目标稿级角色/武器资产及人工手感仍待完成

## 反馈与基线

当前实机与目标概念图的主要差距不在 relay 灯效，而在玩家、Agent 和交火本身不可相信：24 组交叉植被卡片形成黑边纸片；macOS 实际以 FSAA=0 启动；commander_soldier 没有武器却沿用持枪身体姿势；0.1 kg 子弹承受 750 N.s 冲量后约以 7500 m/s 跨帧飞行；小队默认锚点与 18 m 视野又容易让第一波在 20 m 初始间距上长期僵持。

目标概念图只作为构图、战场层次、交火可读性和 HUD 克制度基准，不把其中不存在于仓库的高质量角色、武器、环境资产误认成当前可直接达到的运行画质。

## 本切片

- 移除全部交叉植被卡片，保留路线、掩体和接地层次，不用低质量装饰填画面。
- macOS 默认选择渲染器支持的最高不超过 4× 的 FSAA，继续允许 HELLO_RENDER_FSAA 显式覆盖；核对 Windows 条件编译路径不受影响。
- 让指挥官装配真实 WeaponComponent，保留 LMB 框选语义，新增 Space 射击与 R 换弹；同步产品 fixture 与操作提示。
- 把物理弹改为明确的 48 m/s 游戏速度并扩大短时曳光，保留真实碰撞、枪口火光和命中特效。
- 让无指令、非 hold 的两名队友以玩家前方左右编队锚点自主跟随；扩大本 sample 的感知/接敌半径，使首波能自然接战，指令仍可抢占默认行为。
- relay 状态反馈计划暂停；已有未提交实现不继续扩张，待核心体验通过后再决定保留或缩减。

## 验证

- Lua 5.1 兼容语法、macOS arm64 Release 构建、git diff --check。
- 默认启动日志必须是非零且受支持的 FSAA；用同构图抓帧对比锯齿和去草结果。
- 输入回放覆盖 Enter、WASD/QE、Space/R；截图和日志确认指挥官武器、枪口火光、曳光与角色位移。
- 不启用产品 fixture 的自然运行必须出现真实射击/伤害进展，且第一波不再触发 45 秒 STALEMATE。
- 产品 fixture 与 Sandbox19/6/7/8 smoke 回归；Windows 构建/实机若无环境则分别记为 NOT RUN。

## 完成条件

- 默认画面无交叉草卡，macOS 日志确认有效 FSAA。
- 玩家模型确实持有武器，射击能生成肉眼可见的弹道并保留真实碰撞伤害。
- 队友在无显式命令时会跟随并主动接敌；focus/rally/retreat/hold 仍保持更高优先级。
- 上述结论同时有源码断言、运行日志和真实 GL 抓帧，不以 synthetic fixture 代替自然行为证据。

## 结果与边界（2026-09-12）

- 移除 24 组植被；玩家装枪且 Space 开火不再取消移动。复核时又发现旧控制器把镜头转向后的射击沿上一移动方向发出、横移开火会把枪口转向横移方向，且 A/D 的相机相对 right 向量取反。现把相机水平方向作为持枪瞄准，移动方向独立，并更正 A/D 手性。战斗物理弹速度为 48 m/s，使用共享场景生命周期清理的 BillboardChain 显示细短曳光；先前固定管线材质的几何曳光在 GL3+ 不可见，因此改用已有 shader 的 debug_draw 材质。正面交火见 `tmp/sandbox19-core-final-capture-20260912/core_final_05000ms.png`。
- macOS 实际窗口日志为 `Mac render FSAA: 4`；Relay/SceneGrade 的 scene RTT 不再声明 `no_fsaa`，继承主窗口采样。实机抓帧在 `tmp/sandbox19-fsaa-fix-capture-20260912/`；这是抗锯齿路径修复，不声称旧低面数资产或材质变成目标概念图质感。
- 参考图中的铺装层次仍缺失，因此额外以 ImageGen 生成并等比重采样项目专用 1024×1024 混凝土板缝 albedo；`Relay/Ground` 与 `Relay/MainRoute` 共用材质贴图、用轻微 tint 区分中路，旧混凝土资产不覆盖。GL 日志确认新纹理加载，最终截图在 `tmp/sandbox19-paving-gl-preview-20260912/paving_1024_03500ms.png`，来源与完整提示词见 `media/textures/sandbox19/SOURCE.md`。这是静态地面层改进；角色、枪械、立面与环境道具离概念稿仍远，Windows/D3D9 未验。
- 无测试生命/强制清敌的 60 秒内部输入回放：W 持续到 32 秒，Space 持续到 13 秒；玩家从 z=-15.615 移至 z=36.909，约 13 秒清首波，36.696 秒进入 REGROUP，40.260 秒 VICTORY，两名队友存活，无 STALEMATE。回放为 `HELLO_INPUT_REPLAY` 合成输入，不能当作人工手感证据；逐时实机抓帧见 `tmp/sandbox19-core-final-capture-20260912/`。
- 额外 6 秒 Q 转镜头 + Space 按住开火 + D 横移回放见 `tmp/sandbox19-aim-strafe-replay.txt` 与 `tmp/sandbox19-aim-strafe-capture-20260912/`：D 初次输入将 x 从 0 增至 0.993；Q 后摄像机向 x≈0.95，玩家枪弹 forward≈(0.986,0,0.168)；D 横移时玩家 z 从 -18.035 到 -22.538，枪弹仍向镜头方向而非横移方向。该回放之后的 arm64 Release 重建通过，未借此声称真实手动瞄准已验。
- 瞄准修正后再次运行同一无作弊 60 秒自然配方，抓帧于 `tmp/sandbox19-final-aim-longrun-20260912/`：玩家 32 秒位于 z=36.909，36.762 秒 REGROUP，40.392 秒 VICTORY，无 STALEMATE、正常退出。产品 fixture 重跑为 `tmp/relay-product-fixture-20260912-101123-z1544b5d/summary.json` 中 `PASS reason=evidence-complete synthetic=true`；其它章节 smoke 在此前的独立回归已通过，瞄准修正未改这些 sample 的相机相对模式。
- 最终铺地材质追加后再次执行同一 60 秒自然配方，独立保留日志和三张 GL 图在 `tmp/sandbox19-final-paving-longrun-20260912/`：FSAA=4、1024×1024 铺地纹理加载、植被=0、首波 12.903 秒完成、第二波 17.919 秒触发、36.564 秒 REGROUP、40.161 秒 VICTORY，正常退出无 STALEMATE。产品 fixture 同样重跑通过，摘要为 `tmp/relay-product-fixture-20260912-102902-bu0eb0h_/summary.json`（`synthetic=true`）。
- macOS arm64 Release 构建、产品 fixture、Sandbox19/6/7/8 smoke 均通过；产品 fixture 会调整对象与生命，只验证合同，不代替自然局。Windows/D3D9、真实键鼠连续操作和扬声器听感未在本机完成。relay 灯效的部分实现留在本轮工作区，但不作为核心体感完成条件；需在视觉验收后决定去留。
- 2026-09-12 再尝试通过 Computer Use 连接正常启动的当前 `bin/HelloOgre3D` 做外部键鼠验收：应用名被 LaunchServices 关联到 `tmp/HelloOgre3D-macos-playtest/bin/HelloOgre3D` 的旧试玩副本（旧 HUD、无武器），绝对可执行文件路径又不被应用连接器接受。已退出两个探针进程，不把旧副本的画面或按键算进新版本验收；该项仍为 NOT RUN。
