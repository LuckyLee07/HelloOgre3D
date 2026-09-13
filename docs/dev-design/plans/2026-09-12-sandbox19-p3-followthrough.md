# Sandbox19 P3 体验复核与试玩准备

日期：2026-09-12。状态：本轮可在 macOS 执行的复核、局部视觉补齐和试玩包已完成；P3 总体验仍未验收。

## 问题与取舍

当前 Release 二进制与上一轮动作验证的 SHA-256 同为 `08f91cd33982362294869227f37532efe1a33e08f78a6584f9e5504eea764227`。原 `ranged-and-regroup.txt` 在第二波尚有敌人时，于 66.6 秒用集合令替换集火，80 秒退出前没有自然结算。延后集合的隔离对照于 74.976 秒清敌、78.969 秒自然胜利，确认首轮失败只是旧固定时序不能证明通关，并未据此改战斗规则。正式配方改为第二波持续集火，清敌后由现有队友自主跟随完成集合；新增[自然对局门禁](../../../tools/run_sandbox19_natural.py)要求两波、自然胜利、指令实际执行、重开、暂停与正常退出按序出现，单纯按时退出不算通过。

试验期曾把 `HELLO_SANDBOX19_PRODUCT_TEST=1` 与会立即开始任务的输入回放并开，触发夹具的 `briefing-freezes-simulation` FAIL；两种模式已分开执行，未将那次混跑记为通过。

实机对照批准概念稿后，尝试把入口掩体和补给箱前移，结果大块掩体被画面边缘裁掉，已撤回。保留既有可验证路线，只将左翼一个重复窗位换成 A1 区域喷涂。透明字形由[项目自制生成器](../../../tools/generate_relay_zone_stencil.py)绘制；借用现有 GL/HLSL 叠加程序和无碰撞 VisualPlane，原实体墙仍在。首版 UV 在竖直平面上转成竖排，右侧试放又被棚架遮住；最终横向标识在开场实机图可读，没有黑边。它改善地点识别，不能代替角色、武器和高阶材质资产。

## 验证结果

| 检查 | 结果与证据 |
|---|---|
| 静态 | 项目 vendored Lua 5.1.4 `luac -p` 通过场景文件；Python 编译、A1 PNG 二次生成 SHA-256 相同、资源来源与 `git diff --check` 通过。 |
| 首轮当前平台运行 | macOS arm64 既有 Release 程序；首轮没有 C++ 或绑定改动，无需重新构建。沙盒内 OpenGL 初始化失败，在真实桌面图形会话重跑通过；失败不能记作游戏回归。后续鼠标朝向追加了 C++ 改动和干净重编，见下节。 |
| 产品夹具 | `python3 tools/run_sandbox19_stability.py --product-fixture --timeout 90` PASS，路径、碰撞和生命周期证据在本地 `tmp/relay-product-fixture-20260912-181346-pmd9_w9f/summary.json`。 |
| 自然对局 | 最终场景 `python3 tools/run_sandbox19_natural.py --timeout 150` PASS；第二波 74.151 秒清敌、78.045 秒自然胜利，随后重开、暂停和正常退出；`director=none`、focus/rally 实际执行、物理输入禁用、无捕获到的相关错误。汇总在本地 `tmp/relay-natural-20260912-181558-pmdc2kr9/summary.json`。这是合成输入，不是人工手感。 |
| 画面 | 1280×800 的 macOS GL 开场截图中 A1 横向可读；日志确认 `zone_a1_stencil.png` 以 512×1024 RGBA 实际加载。前后图在本地 `tmp/p3-audit-20260912/`，不入库。 |
| 音频静态 | 九个 relay WAV 均为非静音、22.05 kHz、16 位 PCM，符合 runtime 读取合同；后台验证按约定静音，未验证扬声器上的音色、响度或混音感受。现有 runtime 仍是单路短音。 |
| macOS 试玩包 | 更新包内操作说明；`tools/package_macos_playtest.sh` 生成本地 app/zip，签名校验通过。包内二进制与 A1 贴图哈希均和工作区一致；从包内 launcher 启动、加载贴图、抓帧并正常退出。输出为本地 `tmp/p3-audit-20260912/playtest-package/` 与同名 zip，不入库。 |
| Windows | NOT RUN：当前只有 macOS 图形会话。新增材质使用已有 HLSL 叠加程序，Python 门禁按 Windows 路径和后台模式设置；这些静态条件不替代 D3D9、普通窗口 resize 或整局实机结果。 |

## 后续验收入口

用包内 `HelloOgre3D.app` 普通启动并实际操作一局，重点感受持续移动/转镜/开火/下令的连贯性，以及枪声、受伤、命令与胜负提示在扬声器上的辨识度。当前合成回放不能回答这些主观问题。定制角色与枪械轮廓、成套环境资产和 PBR 材质仍低于目标稿；Windows 普通窗口动态 resize 及本轮新资源的 D3D9 呈现仍需在 Windows 桌面图形会话验证。保留 [P3 待办](../../backlog.md)，不因本轮局部升级关闭。

## 追加：场景光标控制指挥官朝向

作者实际试玩指出鼠标无法设置 Agent 朝向。源码根因是 PlayerController 在 Sandbox19 的相机相对模式把身体/开火方向固定取自 GetFollowForward，鼠标移动只送给 HUD/框选，中键单独转镜头。现在 PlayerController 在场景有效移动事件中记录光标位置，CameraService 的 C++ 专用 ScreenToHorizontalAim 将屏幕射线投到玩家高度的水平面；光标高于地平线时用射线远处水平角，鼠标贴近角色导致方向不稳定时保持上一有效朝向。身体沿既有 FaceDirection 平滑转动，真实枪口随骨骼发弹；首版仍让 WASD 沿 FOLLOW 镜头平面移动，二次反馈确认这会让 W 在侧向瞄准时斜走，见下节修正。无光标事件、暂停/重开后退回镜头方向，默认 tank sample 不变。Lua UI/包内操作说明已同步；没有新增 Lua 绑定。

本机对新增 PlayerController 状态做 arm64 Release 主 target 中间产物隔离后干净重编（旧目标文件保留在本地 `tmp/mouse-facing-20260912/HelloOgre3D-Release-obj-before-clean/`），`build-desktop.log` 为 BUILD SUCCEEDED，新 `bin/HelloOgre3D` SHA-256 是 `d54132591c965814b5d6f7571e720834e089d13f00ad4e6163dd1d24f1223486`。先前受限沙盒构建因 Xcode 模块缓存写入受拒失败，改在允许本机 Xcode 缓存的会话重跑通过；不把环境错误算实现失败。

1280×800 真实 macOS GL 后台窗口、物理输入禁用的定向回放：鼠标移至屏幕两侧后，玩家水平 `forward.x` 先为 -0.885、后为 +0.889；首版同时按 W 时速度仍约 `(0,0,2.976)`，身体却朝侧面；这正是二次反馈指出的缺陷证据。上方无地面交点位置稳定朝前。随后持 Space 的两侧开火，`ShotTrace owner=227` 的真实枪口 `direction.x` 从 -0.889 变为 +0.884；`capture_02100ms.png` / `capture_04700ms.png` 显示角色模型随之左右转向。证据在本地 `tmp/mouse-facing-20260912/`，仅供验收，不入库。这是内部合成输入的真实窗口观察，不替代人工手感。

鼠标朝向首版二进制的产品夹具 PASS (`tmp/relay-product-fixture-20260912-183906-zyr7exj4/summary.json`)；自然对局门禁 PASS (`tmp/relay-natural-20260912-183930-1cvde40v/summary.json`)，第二波 74.151 秒清敌、78.111 秒胜利，重开、暂停、退出完整；Sandbox6/7/8 各 30 秒 smoke PASS (`tmp/m1-smoke-20260912-184230/`)。Lua 5.1 语法、registry JSON 和 diff 检查通过。macOS 试玩包在原本地路径重打，签名与 zip 完整性通过，包内二进制与新 SHA-256 一致；从包内 launcher 再跑同一鼠标回放，左右朝向与 W 速度复现、正常退出。Windows D3D9 与真人持续键鼠手感仍未验证；P3 保持未完成。

## 二次反馈：W/S 必须沿角色朝向

作者试用首版后指出：光标让角色面向侧方时，按 W 仍按镜头前方移动，视觉上斜着走。原因是 PlayerController::UpdateMovement 把 FOLLOW 的 m_yaw 生成的镜头水平向量用于 W/S 和 A/D，而身体已经由 FaceDirection 独立朝光标转。现在在每次 UpdateTurning 后，从当前刚体 `owner->GetForward()` 取移动基准；W/S 沿当前身体朝向前后移动，A/D 以此求左右侧移。这样转身中的速度也与可见朝向对齐，镜头继续独立跟随，默认 tank sample 不变。没有新增字段、Lua 绑定或资源。

本机 arm64 Release 增量构建 PASS，当前二进制 SHA-256 为 `a2db5250cb23aa8ed3d875dcb3dbb77206813c212070725ecfd11883ce04c65b`。1280×800 真实 macOS GL 后台窗口、物理输入禁用的 `body-move-probe.txt` 将光标置于侧方后分别按 W/A/D：W 时 `playerForward=(0.889,0,0.458)`、`velocity=(2.646,0,1.362)`，归一化点积大于 0.995；A/D 速度与身体方向近似正交且互为反向。`body-move-captures/capture_02300ms.png` 与 `capture_02900ms.png` 显示模型保持朝光标侧方并沿该方向前进；具体日志、截图和本地回放在 `tmp/mouse-facing-20260912/`，不入库。

新二进制的产品夹具 PASS (`tmp/relay-product-fixture-20260912-191727-0pr6qe8v/summary.json`)；自然对局门禁 PASS (`tmp/relay-natural-20260912-191755-ld6seesb/summary.json`)，第二波 74.877 秒清敌、78.837 秒胜利，之后重开、暂停并正常退出。包内操作说明已写明 W/S 与 A/D 语义；试玩包在原本地路径重打，签名和 zip 完整性通过，包内二进制哈希与工作区一致，从包内 launcher 回放也复现 W 朝向/速度并正常退出。真人持续键鼠手感与 Windows D3D9 本次控制变更仍未实测，P3 保持未完成。

## 三次反馈：按 MiniGame 的相对鼠标链路重做控制

作者确认首两版“屏幕光标射线 → Agent 朝向”的模型不符合 MiniGame。参考实现由原生鼠标的 deltaX/deltaY 驱动 GameCamera，角色身体追随镜头的水平视线；前进方向再取当前角色朝向。Sandbox19 现沿同一链路：活跃 FOLLOW 战斗默认先把指针移到窗口中心，再捕获鼠标并隐藏光标，相对位移转镜头，角色通过既有 FaceDirection 平滑追随镜头水平前方，W/S 沿刚体当前朝向、A/D 相对身体侧移。鼠标左键继续开火，滚轮缩放。之前的 ScreenToHorizontalAim 及光标状态已删除，不再有“镜头前进、身体侧向”的两套方向。

Sandbox19 仍需要 HUD、框选和世界指令，故按住任一 Alt 暂时释放鼠标，保留旧指针链路及中键/QE 转镜；松开 Alt 恢复捕获。暂停、失焦、关闭窗口均释放捕获；失焦恢复后的首个点击仅用于重新捕获，避免意外开火/下令。macOS 使用 CGAssociateMouseAndMouseCursorPosition 加 NSEvent 原始 delta，Windows 用客户区 ClipCursor 加 OIS 相对位移；后台窗口和合成输入不抓物理鼠标。C++ 接口没有新增 Lua 导出，Sandbox19 帮助文字、十字准星和试玩包说明同步调整，回放中的 Alt 切换也已写入配方。

最终 macOS arm64 Release 使用隔离 OBJROOT 完整构建通过，二进制 SHA-256 为 `c4d32d5059889b4bc9cb24065ce43969b2ffacdd1180a590edb6c2687f7b8684`。真实 1280×800 GL 后台窗口定向回放 `tmp/mouse-look-20260912/mouse-look-centered.log`：转镜后 `cameraForward.x=-0.201`、`playerForward.x=-0.202`；W 放开时 `playerForward=(-0.208,0,0.978)`、水平速度 `(-0.619,0,2.910)`，归一化同向；Space 发出真实枪口射击；按 Alt 移动光标时镜头朝向不变，右键 rally 被两名队友接受并在 66 ms 内执行；松开 Alt 后再移动，镜头与角色继续转向。回放正常 `completed reason=quit`。最终二进制的完整自然对局门禁 PASS (`tmp/relay-natural-20260912-232621-m7avf26r/summary.json`)，77.022 秒胜利，后续重开、暂停与退出按序通过；产品夹具、Sandbox6/7/8 smoke 和 `ui-720` HUD/下令回放也均通过。Lua 5.1 语法、diff 检查通过。包内二进制哈希与工作区一致、签名与 zip 完整性通过；最终包前台产品自测记录 `PASS all=true`；居中前版本在前台产品自测曾记录原生 `[MouseLook] capture=on/off`。最终包的自测开始时窗口尚未激活，因此未记录捕获切换，不能以这次自测证明居中后的物理鼠标链路。

这些窗口和前台证据仍使用合成事件或程序状态，未实测最终版真人持续拖鼠/手感、居中后的实际鼠标位移及失焦后首击重捕的物理交互，也未在 Windows D3D9 桌面运行此次新路径。P3 保持未验收，试玩包供人工复核。

## 2026-09-13 A1 独立提交前复核

以已提交的 `ca6680c` 构图改动为基线，当前 `zone_a1_stencil.png` 与项目生成器二次输出逐字节一致（SHA-256 `3ba08e9f8c97d0b2ed05c3243b994934d456c4db848a7cea9e93818ef1321dbe`）；Lua 5.1.4 场景语法、Python 编译和 diff 检查通过。新一轮 macOS 桌面图形会话的产品夹具为 PASS，完整内部输入回放于 77.451 秒自然胜利，随后重开、暂停并正常退出；当前短回放的 1280×800 实机图确认左翼 A1 横向可读、没有黑边，日志确认 512×1024 RGBA 贴图加载。原始证据分别在本地 `tmp/relay-product-fixture-20260913-105255-io3_zq2u/`、`tmp/relay-natural-20260913-105405-h4fckbnw/` 和 `tmp/goal-a1-20260913/`，不入库。受限会话先前在 Lua 前创建 OpenGL 上下文失败，不能计入游戏回归；上述桌面重跑才是有效证据。真人连续键鼠、扬声器听感和 Windows D3D9 仍未运行，P3 保持开放。
