# Sandbox19 P3 分轮体验收口

日期：2026-09-13。状态：进行中。方向依据见 [项目方向](../../project-direction.md) 和 [当前周期](../../cycle-01.md)；任务状态仍以 [backlog](../../backlog.md) 为准。本页记录分轮交付、验证与尚需真人或 Windows 环境的证据。

## 目标与边界

围绕 Sandbox19 的完整短任务，缩小当前实机与[已批准目标图](../specs/2026-09-10-sandbox19-product-experience-design.md)的角色、武器、环境层次差距，并确认 MiniGame 式鼠标/角色控制及音频能实际使用。每轮只处理一个可观察问题，必要的代码、资源、文档和本地验证在同一检查点完成；通过后单独本地提交，不推送。保持中央直路、西绕行、任务规则和 Sandbox6/7/8 的章节回归面。概念稿是构图与层级参照，不承诺逐像素复制或以新 PBR 管线为前提。

真人持续键鼠和扬声器听感必须由实际操作记录证明；后台回放、屏幕抓帧和音频文件格式只能证明各自覆盖的环节。Windows D3D9 需要 Windows 桌面会话，当前 macOS 结果不代替它。P3 在这些必要边界关闭前保持未完成。

## 已完成检查点

- `ca6680c`：院区短掩体、外肩补给物、西绕行墙明度与 HUD 文案；真实 GL 多尺寸、导航/碰撞和自然对局证据见[构图续办](2026-09-13-sandbox19-visual-composition.md)。
- `5a6979f`：单独收束原先未提交的 A1 区域喷涂与 P3 续办记录；PNG 重建逐字节一致，当前 macOS GL 产品夹具 PASS，内部回放 77.451 秒自然胜利、重开/暂停/退出通过，1280×800 新抓帧确认 A1 可读。原始结果在本地 `tmp/goal-a1-20260913/` 和同日 `relay-product-fixture-*`、`relay-natural-*` 目录，不入库。

## 当前轮：人工验收入口

首个本地试玩包为 `tmp/goal-p3-playtest-20260913/HelloOgre3D.app`，压缩包为同名 `.zip`。包内说明增补实际键鼠/扬声器核对步骤：连续转镜与 W/A/D/开火、Alt 指针下令、失焦首击重捕、暂停/重开、声音辨识。包内二进制 SHA-256 为 `c4d32d5059889b4bc9cb24065ce43969b2ffacdd1180a590edb6c2687f7b8684`，与工作区一致；A1 PNG、短掩体网格和场景 Lua 哈希也一致。签名验证和 ZIP 完整性通过；从包内 launcher 启动的后台 GL 短回放成功加载上述资源并在 8 秒抓帧，记录在本地 `tmp/goal-p3-package-verify-20260913/`。这些验证没有物理输入，也没有非静音扬声器监听；人工结果仍为 NOT RUN。

## 后续切片与验收

1. 指挥官/枪械与动作：先以同条件入口、推进、院区图和侧移/开火回放核定问题，再做一名指挥官与其武器的完整小样；优先看轮廓、站立侧移、换弹和足底接地。必须检查真实枪口/刚体朝向与动作一致、移动不斜行，相关 C++ 改动做 Release 构建和受影响 sample 回归。缺可用骨骼或动画资产时明确写出具体限制，不把外观微调当成定制资产完成。
2. 场景模块：针对门厅、翼楼连接或近中景设施的重复感，选少量真正改善三阶段画面的资产或表面；新增实体复核中轴、西绕行、导航与 Bullet 碰撞，渲染装饰不伪造刚体。避免重做已经失败的贴片草和无收益的相机拉近。
3. 阴影与性能：在最新同一二进制/配置下重测开关阴影的可比帧耗时，先定位实际成本；只有画质与性能证据支持时才改现有渲染路径。保留 Tracy 埋点、原始日志和回退条件。
4. 平台与人工：收到实际试玩反馈后复现并修复可证实的问题；Windows 可用时验证新增材质/HLSL、鼠标捕获与普通窗口 resize 的 D3D9 路径。每轮报告静态、构建、运行、视觉和未验证边界，P3 状态按真实证据更新。

## 2026-09-13 站立侧移动作切片

可观察问题：旧版指挥官站立按 A/D 时，下身完整复用蹲姿侧步，1280×800 的 6100ms 与 7600ms 画面出现明显屈膝下沉；角色状态与姿态不一致。原资源包只有 `crouch_left_aim` / `crouch_right_aim`，没有可直接替换的站立左右侧步。此次只在 `SoldierLocomotionLayer` 的显示组合中给站立侧移分配 35% 站立待机下身权重，其余保留左右侧步及原步相；蹲姿侧移、Bullet 位移、枪口逻辑和动作通知不变。

同一 `strafe-replay.txt`、1280×800、仿真时钟 5800/6100/7300/7600ms 的旧版/新版 GL 图保留在本地 `tmp/goal-p3-strafe-20260913/{baseline,after}/`。左右两侧的站立下沉均减轻，腿部交替仍可见；未观察到这四帧中更明显的悬空，但没有足底滑移量化或真人连续观感结论。另以旧包二进制与新 Release、同一个 71 秒 `short-replay.txt`、1600×900、渲染时钟 8000/30000/65000ms 对照入口、推进、院区，画面主构图保持；两次交火的队友 HP 有小幅差异，不能据此宣称战斗逐帧确定性。旧/新程序 SHA-256 分别为 `c4d32d5059889b4bc9cb24065ce43969b2ffacdd1180a590edb6c2687f7b8684` / `59f440f5598616270c31d86d17fb0427bfaa39c4fee284cb4e62b470924370e2`。

验证：macOS arm64 Release 构建 PASS（初次沙箱 Xcode 模块缓存写入失败，允许系统缓存写入后重跑成功）；60Hz 动作探针 21/21，4 发/余弹 6，逻辑时钟未被显示采样推进；Sandbox6/7/8 各 15 秒 smoke PASS；Sandbox19 产品夹具的路线、真实碰撞与任务门禁 PASS；同一新二进制的内部输入自然对局在 77.484 秒胜利，并完成重开、暂停和正常退出。截图、构建、探针及运行原始日志均在本地 `tmp/goal-p3-strafe-20260913/`、`tmp/m1-smoke-20260913-111548/`、`tmp/relay-product-fixture-20260913-111525-am8wfndc/`、`tmp/relay-natural-20260913-111649-d6nczbfm/`，不入库。静态 `git diff --check` 通过；本轮没有修改 Lua、绑定或工程配置。

站立侧移轮的本地试玩包为 `tmp/goal-p3-playtest-strafe-20260913/HelloOgre3D.app`，ZIP 为同名 `.zip`。包内二进制与当前 Release 哈希一致；签名、ZIP 完整性、A1 资源及包内 launcher 后台 GL 启动抓帧通过。实际键鼠、扬声器与 Windows D3D9 仍为 NOT RUN。此次只是现有素材的姿态缓和，专用站立侧步、骨盆/步幅校正、足底 IK 或滑移量标定及中心角色/枪械资产品质仍列后续工作，P3 不关闭。

## 2026-09-13 指挥官枪械轮廓切片

可观察问题：正常跟随视角中的原枪主要是手边细黑线，换弹举枪时也只有狭窄直杆，难以读出机匣和前段。原资源已包含 sniper 换弹动作和真实 `b_muzzle`；开火消耗弹药后按 R 的 1600×900 旧版回放在 3000/3600/4000ms 确认身体与枪械都随换弹运动，问题不在动作未触发。

新增项目自制 `commander_rifle_shell.mesh`（528 三角形，生成器可逐字节重建）和四种低饱和金属/标记材质。只有 `commander_soldier` 在工厂装配时启用外壳；`WeaponComponent` 持有独立 RenderComponent，与原枪在仿真和显示同步时共用右手骨骼解算后的位姿。原枪 mesh/skeleton、ASM、`b_muzzle` 与子弹生成链没有替换，也不新建刚体或 Lua 导出。外壳随武器可见性与重建/析构清理，保留原枪枪口段外露。

同一 `reload-replay.txt` 在旧版 `59f440f5598616270c31d86d17fb0427bfaa39c4fee284cb4e62b470924370e2` 与新版 `26839ab81b3cc9a78331f3dc45d072ef73a7eafc06604127974ef2d4153ff2e4` 上运行；1600×900 仿真时钟 2600/3000/3300/3600/4000/4500ms 的图在本地 `tmp/goal-p3-weapon-20260913/{reload-shot-baseline,shell-reload}/`。新版换弹时机匣和前段宽度更清楚，枪身跟手且没有遮住原枪口。前三发 `[WeaponShot]` 坐标/方向与旧版逐项相同到日志精度，仅第一发 Y 记录相差 0.00001m。普通背后瞄准时枪仍被右臂遮挡，不能把这一显示外壳说成高质量重制武器或角色完成。

用相同 71 秒短回放在 1600×900 渲染时钟 8000/30000/65000ms 对照入口、推进、院区；旧图来自上一检查点 `tmp/goal-p3-strafe-20260913/after-stages/`，新图在 `tmp/goal-p3-weapon-20260913/shell-stages/`，主构图与 HUD 未见退化。战斗 HP 在重复运行间有波动，画面对照只证明主要布局和枪身表现，不宣称逐帧确定性。

验证：仅清理主目标 Release 中间目录后 macOS arm64 完整重编 PASS；60Hz 动作合同 21/21（含武器重建、换弹、发弹与暂停），Sandbox6/7/8 各 15 秒 smoke PASS；Sandbox19 产品夹具含路线、实体碰撞和重开 PASS；未用夹具的自然对局在 78.045 秒胜利、重开/暂停/退出成功且无相关错误。最新包 `tmp/goal-p3-playtest-weapon-20260913/HelloOgre3D.app` 和同名 ZIP 的签名、压缩完整性、二进制/网格/材质哈希及包内 launcher 后台 GL 资源加载/抓帧通过。上述构建、截图、探针和原始运行数据在本地 `tmp/goal-p3-weapon-20260913/`、`tmp/m1-smoke-20260913-115055/`、`tmp/relay-product-fixture-20260913-115021-7c28k2gv/` 与 `tmp/relay-natural-20260913-115224-wgv1ejpn/`，不入库。没有修改 Lua、绑定或 Premake；真人操作/扬声器和 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 指挥官背部识别切片

可观察问题：1600×900 常用跟随镜头中，指挥官上背几乎是一整块浅色护甲，缺少目标稿中能识别主角的背甲与青色显示屏。旧版入口图在 `tmp/goal-p3-weapon-20260913/shell-stages/capture_08000ms.png`；本轮同条件入口、推进、院区新图在本地 `tmp/goal-p3-pack-20260913/stages/`，另有 1280×800 左右侧移 6100/7600ms 图。新背甲在三阶段与两侧侧移中保持在上背，显示屏可辨；主构图、HUD 与行进路线未见退化。不同回放交火的 HP 会波动，图像对照不代表逐帧战斗确定性。

新增项目自制 `commander_backpack.mesh`（484 三角形、四种低饱和材质；脚本逐字节重建通过），只给 `commander_soldier` 使用。直接把 Ogre Entity 挂在此模型的 `b_Spine` 后，首张 GL 图出现附件漂在场景中；诊断证实脊柱局部轴随瞄准姿态明显偏离人物上下/前后。因此最终由 `RenderComponent` 持有独立显示节点，在每帧动画显示后读取脊柱世界位置，并用身体当前物理朝向放置刚性背甲。主角身体网格/骨骼、枪械真枪口、AI、Bullet 和 Lua 导出未改。显隐同步，`initBody` 替换身体时复制附件配置并保留显隐，旧附件先于旧身体销毁。附件没有独立碰撞体。

验证：macOS arm64 Release 在清理主目标中间文件后完整重编成功，最终二进制 SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`。60Hz 动作合同 21/21、4 发/余弹 6；隔离 Lua 运行另外覆盖指挥官身体两次替换、显隐切换和延迟销毁，合同仍 PASS。Sandbox6/7/8 各 15 秒 smoke、Sandbox19 产品夹具路线/实体碰撞/门禁均 PASS。最终二进制自然对局在 76.626 秒胜利，完成集火、集合、重开及正常退出，无相关错误。原始结果在本地 `tmp/goal-p3-pack-20260913/`、`tmp/m1-smoke-20260913-124034/`、`tmp/relay-product-fixture-20260913-124008-k4114ivr/`、`tmp/relay-natural-20260913-123640-3sfcqd45/`，不入库。

最新 macOS 试玩包为 `tmp/goal-p3-playtest-pack-20260913/HelloOgre3D.app` 和同名 ZIP；包内二进制/背甲网格/材质哈希与工作区一致，签名、ZIP 完整性和从包内 launcher 后台 GL 启动的 8 秒抓帧通过。背甲仍是小型显示附件，不等于完整定制角色；它采用身体朝向稳定姿态，极端上身扭转时不保证逐骨骼贴合。真人持续键鼠、扬声器听感及 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 中继站侧体浅檐切片

可观察问题：同一 1600×900 跟随视角的推进（30 秒）与近门（65 秒）画面中，门厅两侧连接体仍是平整墙块，侧体顶部与正面缺乏清晰的厚度和投影。此轮为每侧放置一件项目自制 `relay_frontage_hood.mesh`（220 三角形，三种既有材质），由 [生成器](../../../tools/generate_relay_frontage_hood.py) 逐字节重建。混凝土顶板、暗色檐底、前缘金属唇和两端压边形成可见屋面边界；最低世界高度 3.385m，中央门口与檐口内沿之间各留约 0.2m 水平间隔。网格经既有 `_Asset` 创建真实 Bullet 凸包，只放在侧体上方，不增加地面通道。

使用前一检查点相同的 `short-replay.txt`、1600×900、GL 和渲染时钟 8000/30000/65000ms 对比本地 `tmp/goal-p3-pack-20260913/stages/` 与 `tmp/goal-p3-hood-20260913/stages/`。新浅檐在入口形成小幅顶线变化，在推进和近门画面明确分出顶面、深色前缘和墙面阴影；中央门、A1 标识、两翼与 HUD 仍可读。战斗 HP 因回放调度波动，截图只用于构图比较。

验证：仓库自带 Lua 5.1.4 的 `luac -p`、Python 生成器语法、二次生成网格 SHA-256 一致均 PASS；同一当前 Release 二进制哈希 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，本轮没有 C++ 或构建配置改动。Sandbox19 产品夹具的全部出生点、中央/西绕行路径和真实静态碰撞 PASS；F3 导航调试实机图见本地 `tmp/goal-p3-hood-20260913/nav-debug/`，青色可走区域仍连续且不爬上檐口。同二进制内部输入的自然对局在 77.055 秒胜利，第二波、集结、重开、暂停及正常退出均通过；原始记录在本地 `tmp/relay-natural-20260913-131007-s1czklg2/`。最新本地试玩包为 `tmp/goal-p3-playtest-hood-20260913/HelloOgre3D.app` 和同名 ZIP；签名、ZIP 完整性、包内二进制/新网格/场景 Lua 哈希及包内 launcher 的后台 GL 资源加载和抓帧通过。真人键鼠/扬声器和 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 最新版阴影成本复测

基线为 `808c107` 的同一 macOS arm64 Release 二进制 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c` 与当前场景资源。后台 GL、静止机位、同一个 Enter→18 秒退出回放、静音和物理输入禁用；按开/关/关/开依次运行，分别在 1280×720（1 秒快照）和实际 1600×900（250 毫秒快照）各做一组。启用 `HELLO_PERF_STALL_LOG`，阈值 1000ms 以免逐帧日志干扰；每轮剔除进入玩法后的前 4 个或 16 个快照及 `frameDelta>=100ms` 的样本。原始日志在本地 `tmp/goal-p3-shadow-20260913/`，不入库。

| 1600×900 轮次 | 保留快照 | cpuFrame 中位/均值 | engineGap 中位/均值 | 墙钟时间 |
|---|---:|---:|---:|---:|
| 开 1 | 56 | 7.76 / 8.56 ms | 5.17 / 7.82 ms | 19.14 s |
| 关 2 | 56 | 11.87 / 9.13 ms | 4.13 / 5.74 ms | 19.18 s |
| 关 3 | 56 | 8.00 / 8.64 ms | 4.33 / 6.26 ms | 19.29 s |
| 开 4 | 56 | 6.34 / 8.06 ms | 5.36 / 5.88 ms | 19.33 s |

720p 先导组同样交错：开两轮 `cpuFrame` 均值 9.60/6.93ms，关两轮 9.73/8.32ms（各保留 14–15 个快照）。四轮均正常退出、未见相关错误，日志分别确认 `[DirectionalShadows] texture modulative` 与 `disabled`。单独的 1600×900、5 秒实机截图在本地 `tmp/goal-p3-shadow-20260913/visual/{on,off}/`，确认开阴影时人物、掩体和建筑投影可见；截图运行不计入性能统计。

当前快照只记录每个采样点的一帧，`updateCall` 常为 0，仿真更新与等待帧交错，轮次中位数不适合当作稳定帧率。ABBA 的均值、墙钟及帧数也没有复现 2026-09-12 的明显开/关差距；不能据此断言阴影没有 GPU 成本或历史结果错误。此轮不调整 1536² 深度图、PCF、Tracy 或 modulative 接收链；后续若实际前台体验或 Windows D3D9 显示持续性能问题，再用可比 GPU/帧时间追踪定位。无代码改动；真人键鼠/扬声器和 Windows D3D9 仍为 NOT RUN，P3 保持开放。

## 2026-09-13 足底滑移诊断，未保留实现

可观察问题：现有 35% 站姿混合虽缓解蹲姿，但站立侧移时支撑脚仍随角色根部滑动。曾在 Lua `Sandbox_Update` 读取脚骨骼，得到约 1.6m/s 的整段平移；后来确认该采样位于显示层应用之前，只是原动作姿态，不能用来判断最终画面。正确采样在 `GameManager::RenderPresentation` 的 `AnimComponent::RenderPresentation` 之后、渲染前读取两脚世界位置。以隔离 Sandbox3 探针的 30Hz 仿真/60Hz 实机 GL、左右 1.6m/s 侧移为固定条件，每个 33ms 仿真步取最后显示帧；支撑脚选较低者，排除换脚且脚高超过 0.14m 的区间。当前已提交版本在 35 个有效步中，支撑脚水平位移中位 0.0261m、均值 0.0416m、P90 0.0974m；最低脚最高 0.110m。原始日志在本地 `tmp/goal-foot-render-20260913/rendered.log`。

先后做了五种未保留的显示层实验。单改侧移周期距离 1.375→0.8m，33 步中位位移 0.0224m，但 1280×800 左右侧移截图再次出现过深屈膝；调到 1.0m 时 34 步中位 0.0245m，P90 反而为 0.1042m。把站立跑步轨迹旋转 90°用于侧移，一段脚步接近锁定，但另一半周期最低脚抬至 0.249m，7/39 个显示步双脚都高于 0.2m，实机图明显悬空。保留原侧步素材仅转骨盆的正/反两个方向，动作合同均 21/21，但同条件 35 步支撑脚位移中位分别增至 0.0530m/0.0746m，且角色出现不自然的倾斜。该模型的侧步轨迹与骨盆局部轴不能靠单个步幅或骨盆偏角稳定对齐。

所有实验性 C++ 改动已撤回；重新构建后 `bin/HelloOgre3D` SHA-256 回到已验证的 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，工作区只留本次诊断文档。原始实验补丁、构建日志、动作合同与截图在本地 `tmp/goal-p3-foot-20260913/`，不入库。此项尚未修复；下一次实现应使用能匹配侧向根速度的专用站立侧步素材，或有落脚/离地切换约束的腿部 IK，并保持物理刚体与真实枪口为真源。没有人工连续动作、扬声器或 Windows D3D9 证据，P3 不关闭。

## 2026-09-13 对侧翼楼百叶舱切片

可观察问题：1600×900 推进画面中，A1 对侧翼楼仍有连续三个同样的深窗，远景左右重复且右翼没有能辨认用途的设施。此轮仅把世界坐标 x=-16m 的一处深窗替换为项目自制 `relay_service_bay.mesh`。网格为 660 三角形、2.72×2.295×0.385m，使用五种现有 Relay 材质构成浅色压顶、深色百叶与侧边识别条；[生成器](../../../tools/generate_relay_service_bay.py)二次运行所得 SHA-256 均为 `226b940a8b95d5f7190bb91d7bca89821126bc8dd98a3ac590139d8c375e7014`。网格靠在已有实墙前，由 `_Asset` 从同一顶点生成静态 Bullet 凸包；百叶不表示可穿透开口，中央门和西绕行未改。

同一 71 秒 `short-replay.txt`、1600×900、macOS GL 的 8000/30000/65000ms 截图对比，旧图在本地 `tmp/goal-p3-hood-20260913/stages/`，新图在 `tmp/goal-p3-service-bay-20260913/stages/`。推进图明确由重复暗窗变为可读的百叶舱，入口图仅小幅露出其上沿，近门图主要保持中轴/HUD 构图；战斗 HP 会随调度波动，不用于逐帧对齐。F3 实机导航图在 `tmp/goal-p3-service-bay-20260913/nav-debug/`，青色可走区域仍连续，未爬上百叶舱。

验证：仓库 Lua 5.1.4 `luac -p`、Python 生成器语法、二次生成哈希和 `git diff --check` 均通过；Sandbox19 产品夹具的出生点、中央/西绕行、真实静态碰撞 PASS。现有 macOS arm64 Release 二进制 SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c` 未变，本轮没有 C++ 或工程配置修改。同一二进制的内部输入完整自然对局于 78.045 秒胜利，第二波、集火、集合、重开、暂停及正常退出通过，错误列表为空；原始结果在 `tmp/relay-natural-20260913-135203-v2qquymf/`。本地新包 `tmp/goal-p3-playtest-service-bay-20260913/HelloOgre3D.app` 与同名 ZIP 的签名、压缩完整性、包内二进制/网格/Lua 哈希及从 launcher 后台 GL 加载新网格、抓帧与正常退出均通过，包内启动原始日志在 `tmp/goal-p3-service-bay-20260913/package-launch/`。真人持续键鼠、扬声器听感及 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 单关节支撑脚试验，未保留实现

在同一 30Hz 仿真/60Hz macOS GL 的隔离 Sandbox3 侧移探针中，试做显示帧支撑脚约束：站立横移稳定后选较低脚为支撑，用其世界位置作水平锚点，最大校正 0.12m；仅旋转该侧大腿使脚靠近锚点，再恢复脚的原朝向。动作/物理逻辑和真实枪口不参与这次求解。原始试验构建、日志、截图在本地 `tmp/goal-p3-foot-lock-20260913/`，对照为同目录已提交基线二进制的 4100/4600ms 截图及前一轮 `tmp/goal-foot-render-20260913/rendered.log`。

按相同“每个 33ms 步取末显示帧、较低脚不切换且脚高不超过 0.14m”口径，两次运行各 35 个有效步：基线水平位移中位/均值/P90 为 0.0261/0.0416/0.0912m，试验为 0.0195/0.0353/0.0963m；有效步的最高脚高由 0.1016m 升至 0.1184m。21 项动作合同、4 发/余弹 6 在试验程序上通过，但大滑移与脚高没有改善，侧移截图也没有足以抵消风险的轮廓收益。求解器从未修正的显示姿态选较低脚；测量在修正后读取，试验与基线的输出支撑脚切换时点不同，表明接触高度受到影响，不能把中位数下降称为脚滑修复。

试验性的 `SoldierLocomotionLayer`、`AnimComponent` 和测量用 `GameManager` 代码全部撤回，重新构建 macOS arm64 Release 成功；最终 `bin/HelloOgre3D` SHA-256 恢复为 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，工作区不留 C++ 改动。后续应从未修正的动画姿态确定接触与离地时段，并考虑膝/踝约束或匹配速度的专用站立侧步，而非只依赖单关节水平锚点。真人连续观感、扬声器与 Windows D3D9 未验，P3 仍开放。

## 2026-09-13 双关节接触试验，未保留实现

接着在显示帧尝试大腿/小腿两关节解析求解：从未修正姿态选择较低脚，在接触时保存世界水平锚点，以 0.10m 限幅目标、20° 关节旋转上限和原脚踝朝向求解；接触进入/离开用 75ms 淡入淡出，避免第一版释放时的足部跳回。只影响显示骨骼，不改 Bullet、动作时钟、通知、武器或任务规则。试验用 `HELLO_FOOT_RENDER_TRACE` 在 `GameManager::RenderPresentation` 后读最终双脚世界位置；同一隔离 Sandbox3、30Hz 仿真/60Hz GL、3465–4752ms 左右各一段 1.6m/s 侧移，按相同低脚不换且脚高不超过 0.14m 的 35 个有效仿真步计算位移。原始补丁、构建、逐帧日志、21 项动作合同及 4100/4600ms 对照截图在本地 `tmp/goal-p3-two-bone-20260913/`，不入库。

为排除运行批次波动，仅加测量日志的基线和求解版各重复三次。基线三次中位约 0.030m、均值约 0.042m、P90 约 0.0795m；接触阈值较晚时分别约 0.023m、0.035m、0.080m，脚高上限与基线同为约 0.10m。把阈值提前后，均值仍约 0.035m，P90 约 0.074m，但中位数回到约 0.030m。两种设置都通过隔离探针的 21 项动作合同，近侧截图未见足以支撑新增持续求解的明显姿态改善；晚启用改善常见小滑移，早启用改善起步尖峰，但都未解决全段脚滑。未以单一指标的局部收益宣称修复。

两关节、`AnimComponent` 帧参数和测量用 `GameManager` 代码均已撤回，macOS arm64 Release 重建 PASS，程序 SHA-256 回到 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，工作区仅留诊断文档。现有 crouch 侧步与站立根速度/接触时段不匹配；要继续这条路线需先有可信的落脚/离地时序或专用站立侧步资产，再校正脚底。真人连续观感、扬声器与 Windows D3D9 未验，P3 保持开放。

## 2026-09-13 操作提示与当前输入链一致性切片

可观察问题：任务准备页仍教玩家使用旧版 `MMB drag / Q/E orbit` 和不带 Alt 的右键下令；这与已落地的 MiniGame 式鼠标转镜、角色跟随朝向以及按住 Alt 临时进入指针模式相反。实际 1600×900 GL 旧图在本地 `tmp/goal-p3-briefing-20260913/baseline/`。本轮只改 `sandbox19_hud.lua` 的准备页三行操作说明与战斗中常驻上下文提示，明确 WASD、鼠标转镜、左键/Space 开火、Alt 指针/HUD/框选、Alt+右键对敌集火或对地移动，以及选择/指令/暂停键；不改输入、相机或玩法逻辑。

同一当前 macOS arm64 Release 的后台真实 GL 抓帧确认 1600×900 与 1280×720 准备页的三行说明完整显示，1600×900 战斗中 `Hold Alt + RMB` 提示完整可读；原始图和进程日志在本地 `tmp/goal-p3-briefing-20260913/{after-900,after-720,combat-900}/`。这些截图使用 `HELLO_INPUT_REPLAY`，后台物理输入被禁用，只证明渲染布局与合成事件链，不证明真人操作手感。

验证：仓库 Lua 5.1.4 `luac -p`、`git diff --check`、Sandbox19 产品夹具均 PASS。同一二进制 SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c` 未变；内部输入自然对局在 77.055 秒进入 VICTORY，第二波、集火/集合、重开、暂停和正常退出均通过，错误列表为空，摘要在本地 `tmp/relay-natural-20260913-144247-i5sszddc/summary.json`。本地新包 `tmp/goal-p3-playtest-briefing-20260913/HelloOgre3D.app` 与同名 ZIP 签名、ZIP 完整性、包内二进制及 Lua 哈希一致；从包内 launcher 启动的后台 GL 准备页抓帧和正常退出通过，日志在 `tmp/goal-p3-briefing-20260913/package-launch/`。本轮无 C++/绑定/构建配置变更，未重编程序。真人持续键鼠、扬声器听感及 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 地面旧化强度试验，未保留实现

对照已批准目标图和当前 1600×900 入口/推进/近门 GL 图，院区中央地面纹理层次仍偏弱。现有 `Relay/GroundDust` 已放置九块宽幅软贴层，材质 alpha 为 0.065。只试改该 tint 为 0.09：同一 71 秒 `short-replay.txt` 的 8000/30000/65000ms 画面和正常退出均取得，入口地面抽样区域比原版约差 2–4 RGB 级，正常观看几乎不能改善层次。再试 0.14 的 8 秒短回放：地面抽样区域约差 8–11 RGB 级，但两侧前景出现可辨的圆形暗斑边界，形成贴片感。原始日志及图在本地 `tmp/goal-p3-ground-dust-20260913/`，对照旧图为 `tmp/goal-p3-service-bay-20260913/stages/`；动作与 AI 帧间细节可能波动，像素统计仅取静态地面区域。

`ground_dust.png` 的 alpha 约 59.36% 非零、58.83% 为 255，较宽的不透明区解释了加深后先显露边界。两档试验均撤回，材质恢复 0.065，`git diff --check` 通过；没有代码或资源行为进入产品，本轮不重跑自然战斗/构建，也不更新试玩包。后续若要填补目标稿的地面层次，应先改善图案的局部细节和羽化，不能靠统一提高 tint alpha。真人键鼠/听感及 Windows D3D9 仍为 NOT RUN，P3 开放。

## 2026-09-13 稀疏地面磨痕切片

针对上一轮“仅调高旧 `GroundDust` tint 会显出圆形贴片边界”的问题，重新制作同一 `ground_dust.svg`/PNG，不增场景实体。旧 SVG 渐变经本机 ImageMagick 6 栅格化后，512² PNG 非透明像素约 59.36%，其中约 58.83% 全不透明；新版改为分散 `rgba(...)` 路径、细磨痕和小缺口，非透明约 4.85%、全不透明为 0，四周留透明空白。既有九块仅渲染贴层继续复用，材质 tint 从 0.065 调至 0.25；不创建刚体，不改导航、AI、光照或任务规则。`source/ground_dust.svg` 保持 CRLF；ImageMagick 6 用 `-strip` 重建两次 PNG SHA-256 均为 `901e6cfbc36b47095b68be88282b8dd0c04e5857952b265ca7db2d2e95ab8e78`，消除了时间戳导致的字节差异。来源与命令见 [纹理来源](../../../media/textures/sandbox19/SOURCE.md)。

同一 macOS GL、1600×900、回放与渲染时钟的旧版 8000/30000/65000ms 图在本地 `tmp/goal-p3-service-bay-20260913/stages/`，新版 0.25 的入口/推进图在 `tmp/goal-p3-ground-patina-20260913/v2-alpha025/`，院区 61000/65000ms 图在同目录 `v2-courtyard/`。另用上一轮独立试玩包的旧资源与当前资源各跑相同 1280×720、8000ms 回放，对照 `baseline-720/` 与 `v2-720/`：新图在指挥官两侧和院区前景增加细碎磨痕，未见旧版 0.14 alpha 试验的圆形暗板；角色、目标、补给物和 HUD 保持可读。曾试的新图 0.40 tint 在 900p 过于抢眼，未保留；该次 65 秒画面因正常战斗提前失败而出现结算面板，不作为近门构图证据。战斗调度会变动 HP，图像只评价静态场景层次。

验证：SVG XML、PNG RGBA/alpha 统计、Lua 5.1.4 `luac -p`、`git diff --check` PASS；程序仍为已验证的 macOS arm64 Release SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，本轮无 C++ 或工程配置改动。Sandbox19 产品夹具的出生点、双路线与真实静态碰撞 PASS；无夹具内部输入自然对局在 78.903 秒胜利，完成第二波、集火/集合、重开、暂停和正常退出，错误列表为空，原始摘要在 `tmp/relay-natural-20260913-150706-_a1fbee4/summary.json`。最新包 `tmp/goal-p3-playtest-ground-patina-20260913/HelloOgre3D.app` 与同名 ZIP 的签名、压缩完整性、程序/材质/PNG/SVG 哈希一致，包内 launcher 的 720p GL 抓帧与正常退出通过，原始结果在 `tmp/goal-p3-ground-patina-20260913/package-launch/`。真人持续键鼠、扬声器听感及 Windows D3D9 仍为 NOT RUN，P3 保持开放；磨痕改善属于局部地表层次，不等于整体材质达到目标稿。

## 2026-09-13 无敌人阶段集火提示切片

可观察问题：旧版 1600×900、65 秒院区画面已经显示 `REGROUP AT THE RELAY` 和 `Hostiles 0`，F 集火仍可按，实际返回 `No visible target. Move the squad into contact.`，与当前“带一名队友集合”的目标冲突。旧图在本地 `tmp/goal-p3-ground-patina-20260913/v2-courtyard/capture_65000ms.png`。本轮只修改 `Sandbox19.lua`：`ADVANCE`/`REGROUP` 阶段让 F 集火按钮禁用，按键给出对应阶段原因，底部常驻提示用更短的推进/集合指引；`WAVE` 阶段的集火与 AI 指令链不变。

同一 macOS arm64 Release 二进制、回放和真实 GL 窗口，新 1600×900 推进/集合画面在本地 `tmp/goal-p3-focus-phase-20260913/final-900/` 的 30000/65000ms 抓帧。推进时 F 置灰且指向院区；集合时 F 置灰，62 秒 F 按键横幅明确要求带队友集合，常驻短指引没有重复横幅。1280×720 同回放的 30000ms 推进提示无溢出；该次 65000ms 仍在第二波，F 保持可用，说明不同运行批次的战斗时点会波动，截图不作为逐帧战斗确定性证据。该尺寸原始图在 `tmp/goal-p3-focus-phase-20260913/final-720/`，全部原始产物仅本地保留。

验证：仓库 Lua 5.1.4 `luac -p`、`git diff --check`、产品夹具 PASS（路线/碰撞、两波阶段与真实 BT 指令生命周期，错误列表为空）；无夹具内部输入自然对局 78.903 秒胜利，完成第二波、集合、重开、暂停和退出，错误列表为空，摘要在本地 `tmp/relay-natural-20260913-152709-wegx6sdg/summary.json`。本轮没有 C++、绑定或工程配置改动；二进制 SHA-256 仍为 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，无需重编。新版试玩包 `tmp/goal-p3-playtest-focus-phase-20260913/HelloOgre3D.app` 与同名 ZIP 的签名、ZIP 完整性、程序/Lua 哈希一致；包内 launcher 的 720p 后台 GL 8 秒抓帧与正常退出通过，记录在 `tmp/goal-p3-focus-phase-20260913/package-launch/`。后台回放禁用物理输入；真人持续键鼠、扬声器听感和 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 指令失败提示可操作性切片

可观察问题：集火目标丢失时 HUD 直接显示 `Order failed: target-lost`，把内部原因码交给玩家且未说明下一步。此轮仅在 `sandbox19_commands.lua` 把 `owner-down`、`target-lost`、`path-failed`、`no-progress` 映射为简短操作建议，并给未知失败原因提供通用文案。日志和 Blackboard 仍记录原始原因码；指令接受、BT 执行、终态和任务规则不变。

同一 1600×900、GL、产品夹具和渲染时钟 8000ms 的旧包/新版截图位于本地 `tmp/goal-p3-order-hints-20260913/{baseline-fixture,final-fixture}/`：旧版为 `Order failed: target-lost`，新版为 `Target lost. Reposition and focus again.`，提示完整可读。另用同一回放在 8000/30000/65000ms 对照 `tmp/goal-p3-focus-phase-20260913/after-900/` 与 `tmp/goal-p3-order-hints-20260913/after-900/`；入口、推进、院区的设施、中央通道、HUD 与角色构图未见退化。交火帧的队友 HP 和最终存活者有调度差异，画面不作为逐帧战斗等价证据。

验证：从仓库 Lua 5.1.4 源码在本地 `tmp/` 编译的 `luac -p`、`git diff --check` PASS；Sandbox19 产品夹具 PASS，含真实导航、实体碰撞、BT 指令生命周期、目标丢失与队友阵亡终态，摘要在 `tmp/relay-product-fixture-20260913-154251-99sxziqc/summary.json`。无夹具内部输入自然对局在 78.012 秒胜利，完成第二波、集合、重开、暂停、正常退出且错误列表为空，摘要在 `tmp/relay-natural-20260913-162724-h3n16f3d/summary.json`。真实 GL 三阶段与新包 launcher 的 8 秒启动抓帧、正常退出均通过，原始日志在 `tmp/goal-p3-order-hints-20260913/`。最新本地包 `tmp/goal-p3-playtest-order-hints-20260913/HelloOgre3D.app` 与同名 ZIP 签名和压缩完整性通过，包内 Lua SHA-256 `25c379f7d9ac96dceb0e1cc7fdcb8d3b03b9d3b39fec31ab3e5ddf402a3d2c06` 与工作区相同。仅修改 Sandbox19 Lua 文案，复用已验证的 arm64 Release 二进制 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，本轮不重编或重跑未受影响的 Sandbox6/7/8。后台输入为 synthetic，真人持续键鼠、扬声器听感和 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 中央门厅浅门框切片

可观察问题：最新 1600×900 近门画面中，中央青色双门仍像直接贴在平直墙面上，缺少能投出阴影的入口进深。此轮只在既有实体门前新增浅门楣、深色檐底和两侧门框，沿用场景已有材质；`_Box` 让显示几何与静态 Bullet 尺寸一致。框体位于中继站门前的既有不可通行墙面，不延伸到集合点或中央/西绕行路线；任务规则和角色控制未改。

使用上轮同一 arm64 Release 二进制、1600×900 GL、相同回放和渲染时钟 8000/30000/65000ms，对照本地 `tmp/goal-p3-order-hints-20260913/after-900/` 与 `tmp/goal-p3-portal-20260913/after-900/`。入口和推进画面中门框形成细小中心轮廓，近门画面可辨浅色门楣、暗檐底、两侧深框与青色门板的前后关系；A1、翼楼、设施、HUD 和集合目标仍可读。队友 HP 和最终存活者受交火调度影响，不把这些截图当作战斗逐帧等价证据。F3 实际 GL 导航图在 `tmp/goal-p3-portal-20260913/nav-debug/`，青色可走区域在中央与两翼之间连续，未爬上门框。

验证：仓库 Lua 5.1.4 `luac -p`、`git diff --check` PASS；Sandbox19 产品夹具的全部出生点、中央与西绕行导航、真实静态碰撞和 BT 指令生命周期 PASS，摘要在 `tmp/relay-product-fixture-20260913-232436-lwlkls5e/summary.json`。无夹具内部输入自然对局于 78.606 秒胜利，完成第二波、集合、重开、暂停与正常退出，错误列表为空，摘要在 `tmp/relay-natural-20260913-232757-q3a4x48l/summary.json`。本轮没有 C++、绑定或工程配置变更，复用已验证的 arm64 Release 二进制 SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，不重编或重跑未受影响的 Sandbox6/7/8。最新本地试玩包为 `tmp/goal-p3-playtest-portal-20260913/HelloOgre3D.app` 与同名 ZIP；签名、ZIP 完整性、包内程序/场景 Lua 哈希一致，从包内 launcher 的后台 GL 导航抓帧与正常退出通过，日志在 `tmp/goal-p3-portal-20260913/package-launch/`。后台输入为 synthetic；真人连续键鼠、扬声器听感及 Windows D3D9 仍为 NOT RUN，P3 不关闭。

## 2026-09-13 门框版本阴影开销再测

以 `9dfc39f` 场景和同一 macOS arm64 Release 二进制 SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c` 为最新基线，重做后台 GL、静止机位、Enter→18 秒退出、静音、物理输入禁用的阴影开/关/关/开测量。1600×900 每 250ms 记录一帧，1280×720 每 1000ms 一帧；开启 `HELLO_PERF_STALL_LOG`，阈值 1000ms，分别剔除开始后的 16/4 个快照和 `frameDelta>=100ms` 样本。每轮窗口实际像素符合请求，阴影模式日志分别为 `texture modulative` / `disabled`，均正常退出且无相关错误。脚本、原始日志和解析结果在本地 `tmp/goal-p3-shadow-latest-20260913/`，不入库。

| 1600×900 轮次 | 保留快照 | cpuFrame 中位/均值 | engineGap 中位/均值 | 最后采样帧号 |
|---|---:|---:|---:|---:|
| 开 1 | 56 | 8.655 / 9.120 ms | 5.500 / 6.364 ms | 2129 |
| 关 2 | 56 | 8.240 / 8.768 ms | 5.485 / 7.708 ms | 2157 |
| 关 3 | 56 | 8.830 / 8.748 ms | 7.790 / 7.662 ms | 2156 |
| 开 4 | 56 | 8.385 / 8.971 ms | 7.400 / 8.285 ms | 2138 |

720p 四轮每轮保留 14 帧，开启组 `cpuFrame` 均值为 8.574/9.352ms，关闭组为 8.829/9.401ms，最后采样帧号分别为开启 2051/2041、关闭 2058/2061。900p 开启组采样均值约 9.05ms、关闭组约 8.76ms，差约 0.29ms；720p 采样均值方向相反。最后采样帧号受 250/1000ms 采样相位影响，不能当作精确总帧数或稳定 FPS。`updateCall` 多个快照为 0，`engineGap` 含未细分的渲染/调度时间；这些数据不能给出可靠 GPU 阴影成本。

另外在最新场景用相同 1600×900、5 秒真实 GL 画面开/关对照 `visual/{on,off}/`：开启时角色脚下、掩体和棚架投影可见，关闭后明显变平。相对可见画质收益，现有采样只支持至多较小且未精确归因的成本；本轮不改 1536² 深度图、PCF、Tracy 或 modulative 接收链。若真人前台或 Windows D3D9 后续出现稳定卡顿，再用该环境下连续帧/GPU 计时定位。无产品代码或资源改动，此轮只记录测量；真人持续键鼠、扬声器和 Windows D3D9 仍为 NOT RUN，P3 保持开放。

## 2026-09-16 前台 macOS 原生输入链复核

可观察问题：截至上一轮，最新试玩包只有后台 InputReplay、包内启动抓帧和真人检查清单，没有前台 macOS 原生窗口事件链证据。本轮从 `tmp/goal-p3-playtest-portal-20260913/HelloOgre3D.app` 启动 1280×720 前台窗口，不设置 `HELLO_WINDOW_BACKGROUND` 或 `HELLO_INPUT_REPLAY`，通过桌面 UI 自动化向实际应用窗口发送按键与点击。该方式经过 macOS 窗口和输入桥，但不是人手持续操作，证据边界与后台合成回放不同。

启动日志记录 `[WindowMode] platform=mac background=false requestedLogical=1280x720 pixels=1280x720`，没有 InputReplay marker；包内 `bin/HelloOgre3D` 与工作区 Release 的 SHA-256 均为 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`。前台 Return 进入第一波，任务随后从 `WAVE` 到 `ADVANCE`。切到 Finder 使应用失焦后，再点击游戏窗口中心，日志出现 `[MouseLook] capture=on reason=resume-click`；这覆盖了 `InputManager` 的待重捕首击路径。Alt+数字键时编队选择环发生变化，日志在同一轮记录 `capture=off reason=mode-change` 后重新 `capture=on`，说明前台 Alt 模式切换到达游戏输入链。

Escape 打开暂停菜单后点击 Retry，日志先记录 `[Sandbox19Pause] paused=true`，随后由玩家 `241` 重建为 `326`，并重新进入 `[Sandbox19Match] phase=WAVE wave=1 enemies=2 director=none elapsedMs=0`；前台画面同步显示计时 00:00、敌人 2、指挥官 160 HP、两名队友各 140 HP。最后再次 Escape 并点击 Quit，日志完整到达 `OGRE Shutdown`。原始日志及提取事件保存在本地 `tmp/goal-p3-native-ui-20260916/{Sandbox.log,events.log}`，不入库。

本轮没有产品代码、Lua、资源或工程配置改动，因此不重复构建、Lua 语法、Sandbox6/7/8、产品夹具和自然通关；继续沿用同一二进制此前已通过的对应证据。桌面 UI 自动化不能持续按住 W/A/D、鼠标键或 Alt 并同时点击右键，亦不能评价音质，因此连续转镜、持续移动、移动射击、Alt+右键地面/敌人下令、真人主观手感与扬声器听感仍为 NOT RUN。Windows D3D9 和普通窗口 resize 同样未运行。此轮只缩小前台原生事件链缺口，不关闭 P3。

## 2026-09-16 入口场地发电机切片

可观察问题：最新 1600×900 入口画面在中央路线两侧仍以同系列绿色补给箱近似镜像，首屏前景缺少能说明设施用途的横向轮廓。此轮只把世界坐标 `(7.6, 0.61, -11.6)` 的一只低补给箱一对一替换为项目自制 `relay_field_generator.mesh`，不增加实体数量。发电机实际边界约 2.10×1.12×1.103m、616 三角形，使用暖灰机身、深色端架/进气格栅和青色状态屏五种专用材质；[生成器](../../../tools/generate_relay_field_generator.py)连续两次重建 SHA-256 均为 `e21730ab4d2849400c5eba7279787b0d74c6f252e657240106bc02589e04a827`。网格由同一顶点建立真实 Bullet 凸包；旋转后最内侧仍在 `x>6.48m`，不占 `x=-5..5m` 中央路线，西侧 `x=-18m` 绕行未改。

同一 macOS arm64 Release、1600×900 GL、`short-replay.txt` 和渲染时钟 8000/30000/65000ms 的基线与最终图分别在本地 `tmp/goal-p3-generator-20260916/{baseline,final}/`。入口图中原左右同款绿色短箱改为一侧横向格栅设施，状态屏与指挥官背甲同属克制青色识别点；角色、敌我轮廓、中轴门厅和 HUD 仍保持主次。推进与院区图中该设施已离开视野，原构图未见退化；战斗 HP 随调度变化，不作为逐帧等价证据。1280×720 F3 图在同目录 `nav-debug/`，青色可走区域绕开发电机实体并保持中央、西侧路线连续。

验证：仓库 Lua 5.1.4 `luac -p`、生成器执行、网格逐字节重建和 `git diff --check` PASS；程序仍为已验证的 Release SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，本轮没有 C++、绑定或工程配置改动，不重复构建及未受影响的 Sandbox6/7/8。Sandbox19 产品夹具 PASS，包含全部出生点、中央/西绕行、原补给箱 hull、新 `field-generator-hull` 和真实任务/BT 生命周期，摘要在本地 `tmp/relay-product-fixture-20260916-232408-dia61zvn/summary.json`。无夹具内部输入自然对局在 78.705 秒胜利，随后重开、暂停并正常退出，错误列表为空；摘要在 `tmp/relay-natural-20260916-232453-249o34wl/summary.json`。

最新本地试玩包为 `tmp/goal-p3-playtest-generator-20260916/HelloOgre3D.app` 与同名 ZIP；临时签名、ZIP 完整性、程序/场景 Lua/材质/新网格哈希一致，从包内 launcher 的 1280×720 GL 抓帧和正常退出通过。ZIP SHA-256 为 `40e31f2c7151b3ed3f9f055bc83b9385257eeca7e3cffa183bee6fad4965099a`。真人持续键鼠、Alt+右键、移动射击手感、扬声器听感和 Windows D3D9 仍为 NOT RUN，P3 保持开放；本轮是局部入口设施去重复，不等于整体环境或角色资产达到概念稿。

## 2026-09-16 中段配电柜切片

可观察问题：最新入口战斗画面右肩仍并列两只同系列绿色补给箱，中段掩体两侧也重复高箱轮廓，设施用途和层次不足。此轮只把世界坐标 `(-10.5, 0.87, 6.1)` 的一只高补给箱一对一替换为项目自制 `relay_switchgear.mesh`，不增加实体数量。配电柜实际边界约 1.56×1.745×0.926m、704 三角形，复用现有发电机/箱体材质形成双门、散热槽、深色框架和青色状态面板；[生成器](../../../tools/generate_relay_switchgear.py)连续两次重建 SHA-256 均为 `5e0cf4b28f9c5e678243bad2042e8a9c17d21279dd124956b62c53f37ad292e9`。网格顶点同时建立真实 Bullet 凸包；占位沿用原高箱且最内侧仍约在 `x=-9.7m`，不占 `x=-5..5m` 中央路线，`x=-18m` 西绕行未改。

同一 macOS arm64 Release、1600×900 GL、`short-replay.txt` 和渲染时钟 8000/30000/65000ms 的基线与最终图分别在本地 `tmp/goal-p3-generator-20260916/final/` 与 `tmp/goal-p3-switchgear-20260916/final/`。入口图中原高箱的平顶储物轮廓改为带百叶和状态灯的竖向设备，画面右肩与前景发电机形成横/竖两种设施语言；中轴、敌我轮廓、HUD 与门厅仍保持主次。推进和院区阶段该占位退出近景，构图未见退化；交火 HP 随调度变化，不作为逐帧等价证据。1280×720 F3 图在同目录 `nav-debug/`，青色可走区域保持中央与两侧连续。

验证：仓库 Lua 5.1.4 `luac -p`、生成器执行、两次确定性网格重建和 `git diff --check` PASS；程序仍为已验证的 Release SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，本轮没有 C++、绑定或工程配置改动，不重复构建或未受影响的 Sandbox6/7/8。Sandbox19 产品夹具 PASS，包含全部出生点、中央/西绕行、原补给箱、`field-generator-hull`、新 `switchgear-hull` 和真实任务/BT 生命周期，摘要在本地 `tmp/relay-product-fixture-20260916-234535-3ohqtsxn/summary.json`。无夹具内部输入自然对局在 78.210 秒胜利，随后重开、暂停并正常退出，错误列表为空；摘要在 `tmp/relay-natural-20260916-234615-kxyaldi8/summary.json`。

最新本地试玩包为 `tmp/goal-p3-playtest-switchgear-20260916/HelloOgre3D.app` 与同名 ZIP；临时签名、ZIP 完整性、程序与新网格哈希一致，从包内 launcher 的 1280×720 GL/F3 抓帧确认资源实际加载并正常退出，记录在 `tmp/goal-p3-playtest-switchgear-20260916-launch/`。ZIP SHA-256 为 `845757682c891908df0f3450750f08759209fff74959a64e7ec5b24ce4d7fc15`。真人持续键鼠、Alt+右键、移动射击手感、扬声器听感和 Windows D3D9 仍为 NOT RUN，P3 保持开放；专用站立侧步/足底约束也仍受现有动画资源限制。

## 2026-09-17 院区前肩配电柜复用切片

可观察问题：30 秒推进画面中，院区前肩两道掩体外侧仍各立一只同系列绿色高补给箱，形成明显镜像。此轮只把世界坐标 `(10.3, 0.87, 24.1)` 的一只高箱一对一替换为上一轮已验证的 `relay_switchgear.mesh`；资源、实体总数、任务规则和材质均未增加。入口右肩与院区前肩两台柜体分别在 8 秒和 30 秒视角成为近中景，避免同一画面再次形成设施重复。新占位沿用原高箱位置，最内侧仍约在 `x=9.5m`，不占 `x=-5..5m` 中央路线或 `x=-18m` 西绕行。

同一 macOS arm64 Release、1600×900 GL、`short-replay.txt` 和渲染时钟 8000/30000/65000ms 的基线与最终图分别在本地 `tmp/goal-p3-switchgear-20260916/final/` 与 `tmp/goal-p3-courtyard-switchgear-20260916/final/`。推进图左肩原绿色高箱改为深色百叶柜和青色状态点，与右肩保留高箱形成清楚差异；入口图只把新柜体作为远景预示，院区近门图则不再显示该占位。中轴、门厅、敌我轮廓与 HUD 未见退化。1280×720 F3 图在同目录 `nav-debug/`，中央与两侧青色可走区域连续。

验证：仓库 Lua 5.1.4 `luac -p` 和 `git diff --check` PASS；本轮只改场景 Lua 与文档，继续使用 Release 二进制 SHA-256 `b13b94745614b5c101472fcbaf17f625fb58b281492d1479a601e86b52b7ca9c`，不重复构建或未受影响的 Sandbox6/7/8。Sandbox19 产品夹具 PASS，新增 `courtyard-switchgear-hull` 与原 `switchgear-hull`、`field-generator-hull`、全部出生点、中央/西绕行和任务/BT 生命周期均通过，摘要在本地 `tmp/relay-product-fixture-20260917-000113-0g9p3_f3/summary.json`。无夹具内部输入自然对局在 76.989 秒胜利，随后重开、暂停并正常退出，错误列表为空；摘要在 `tmp/relay-natural-20260917-000204-d173z1jj/summary.json`。

最新本地试玩包为 `tmp/goal-p3-playtest-courtyard-switchgear-20260917/HelloOgre3D.app` 与同名 ZIP；临时签名、ZIP 完整性和包内程序/网格/场景 Lua 哈希一致，从包内 launcher 的 1280×720 GL/F3 抓帧确认最新布局并正常退出，记录在 `tmp/goal-p3-playtest-courtyard-switchgear-20260917-launch/`。ZIP SHA-256 为 `d59cd49bd598cb768a1897495c1cffbe3b4642c405332d24565a8d4e7bffcb9f`。真人持续键鼠、Alt+右键、移动射击手感、扬声器听感、专用站立侧步/足底约束和 Windows D3D9 仍为 NOT RUN，P3 保持开放。
