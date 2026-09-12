# Relay Outpost 输入回归配方

这些文件由 `HELLO_INPUT_REPLAY` 读取，通过实际 InputManager/Lua/UI/PlayerController 链注入事件。它们不操作系统鼠标键盘，不是人工试玩，也不直接修改生命值、杀敌或传送单位。合成状态机 fixture 另用 `tools/run_sandbox19_stability.py --product-fixture`。

| 文件 | 实际像素尺寸 | 用途 |
|---|---|---|
| `ranged-and-regroup.txt` | 1280×800 | 第一段集火与中途集合，第二段持续集火；指挥官进入集合区，清敌后由队友自主跟随结算，再 Enter 重开、暂停、退出 |
| `deep-rally.txt` | 1280×800 | 第二段深入敌阵的策略对照；限时退出不等于自然结算 |
| `mouse-look.txt` | 1280×800 | 镜头相对转向、W 沿身体前进、Space 开火、Alt 指针模式右键下令和 Esc 释放的短回归 |
| `ui-720.txt` | 1280×720 | 点击开始、选择/框选、暂停中释放移动键、音量/静音、镜头转向/缩放、命令、观察器、重开和 UI 退出 |
| `settings-write-720.txt` | 1280×720 | 从默认 65% 写入 75%/静音，再通过 UI 退出 |
| `ui-1080.txt` | 1920×1080 | 重启后读取静音设置，再恢复 65%/非静音；点击、导航 debug、转向与 UI 退出 |

Windows 自动运行必须同时设 `HELLO_WINDOW_BACKGROUND=1`、`HELLO_AUDIO_SILENT=1`、`HELLO_WINDOW_WIDTH/HEIGHT`，并从游戏 `bin` 工作目录用普通 `Start-Process` 启动，不传 `-WindowStyle Hidden`。后台开关让 D3D9 在虚拟桌面范围外创建可渲染但不激活的工具窗口，并禁用 OIS 硬件输入；静音开关避免测试声音。尺寸变量是后台测试专用，普通 Play.cmd 使用默认窗口路径。每次串行运行前清理其它 `HELLO_*` 测试变量，固定 `HELLO_SANDBOX_SAMPLE=Sandbox19` / `HELLO_SAMPLE_PRESET=Sandbox19`。

`HELLO_INPUT_REPLAY` 使用文件绝对路径，时间单位是 Update delta 累加的毫秒，包含暂停等待。行格式为 `毫秒 事件 参数`；支持 `key_press/down/up`（含 `LMENU` / `RMENU`）、`mouse_down/up x y button`、`mouse_move x y`、`wheel x y delta`、`quit`。Sandbox19 活跃玩法默认鼠标转镜并左键开火；需要世界指令、HUD 或框选时，配方按住 `LMENU` 进入临时指针模式。日志带 `synthetic=true`，记录仿真时间、玩家和相机状态。真实战斗受调度与帧率影响，固定 seed 不保证跨机器逐帧确定性。

当前完整路线可运行 `python3 tools/run_sandbox19_natural.py`（Windows 用可用的 Python 3）；它要求第二波清敌、自然胜利、重开、暂停和正常退出全部出现在同一进程日志中，结果写本地 `tmp/relay-natural-*/summary.json`。超时、图形会话初始化失败或仅按时退出均为 FAIL；换平台或改动战斗节奏后应重看配方时点。

截图可另设 `HELLO_RENDER_CAPTURE=1`、`HELLO_RENDER_CAPTURE_DIR`、`HELLO_RENDER_CAPTURE_MS` 与 `HELLO_RENDER_CAPTURE_CLOCK=render`。RenderCapture 和 InputReplay 的计时不同，高频 PNG 写入会拖慢墙钟速度；视频仅证明连续呈现和事件顺序，不能据此测正常帧率。

本轮结果和限制见 [验收记录](../../../docs/playtest-relay-2026-09-10.md)。
