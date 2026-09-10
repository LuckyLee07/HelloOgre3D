# 运行时切面（alias: runtime）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

引擎/中间件适配层：性能计数、资源诊断、相机控制、FairyGUI 系统集成与局部短音播放。应用层 GameManager 负责仿真暂停边界和服务装配。

## 2. 源码位置

- `src/HelloOgre3D/runtime/`（FairyGUI 详见 [[fgui]]）
- 应用验证入口：[InputReplay.cpp](../../src/HelloOgre3D/game/diagnostics/InputReplay.cpp)、[ClientManager.cpp](../../src/HelloOgre3D/game/ClientManager.cpp)；硬件输入装配位于 [InputManager.cpp](../../src/HelloOgre3D/sandbox/systems/input/InputManager.cpp)。

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `profiling/RuntimeProfileCounters.{h,cpp}` | 性能 | `Plot{AiScheduler,AgentPerception,HearingDangerSense,TeamBlackboard,FairyGuiService}Stats` + 帧时序 / Lua callback 计数；Tracy 关闭时是 no-op |
| `profiling/Profile.h` | 采样 | 时序宏 |
| `diagnostics/RuntimeResourceDiagnostics.{h,cpp}` | 诊断 | `BuildResourceDump` texture/mesh/buffer 清单 |
| `ogre/OgreCameraController.h` | 相机 | FREELOOK/ORBIT/MANUAL/FOLLOW 相机控制；FOLLOW 为第三人称弹簧跟随（后上方+看角色前方+弹簧阻尼），由 `PlayerController → CameraService → updateFollow` 每帧驱动；仍不做 FPS 模式 |
| `audio/RuntimeUiSound.{h,cpp}` / `RuntimeUiSoundMac.mm` | 声音 | PCM 16-bit WAV 短音单路播放；Windows PlaySound / macOS NSSound 适配，音频缓冲与 native handle 由 runtime 持有 |
| `ui/fairygui/FairyGuiSystem.*` | UI | cocoslite 内嵌，见 [[fgui]] |
| `RuntimeToLua.{cpp,pkg}` | 绑定 | runtime 层 tolua |

## 4. 公开能力要点

- 性能上报（AI/UI/帧分项/Lua callback count）、资源快照、既有相机模式、FairyGUI 栈；FGUI `AiDebugPanel` 可读取统一 `[AIRuntimeDiag]` 并按 `focusAgentId` / `filterText` 参数化筛选。
- [CameraService](../../src/HelloOgre3D/sandbox/systems/service/CameraService.h) 导出 `ConfigureFollowCamera` / `ResetFollowCamera` / `SetCameraRelativeMovement` / `SnapFollowTarget` / `GetFollowDistance`。sample 配置参数与范围，控制器统一驱动镜头；Sandbox19 使用距离 12、高 9、前视 6、眼高 1.5 和距离范围 8–18，WASD 沿相机平面移动、Q/E 调整偏航，未被 UI 消费的滚轮调整跟随距离。退出 FOLLOW 重置配置及相对移动开关，其他 sample 默认 tank 控制保持原语义。
- `SandboxAudio` 为 GameManager 注入的 `RuntimeUiSound`：`IsAvailable`、`Play(path)`、`StopAll`、`SetVolume(0..1)`、`GetVolume`。Lua 负责事件/限频/音量设置；runtime 读取、校验、缓存短 WAV 并持有播放缓冲。Sandbox19 的设置与事件入口见 [sandbox19_audio.lua](../../bin/res/scripts/samples/sandbox19_audio.lua)，自制素材来源记录见 [relay 音效说明](../../bin/res/audio/relay/README.md)。

## 5. 约束与红线

- 引擎/中间件耦合逻辑收口在 runtime（AGENTS.md 依赖流）。
- **判断 AI 成本看 `updateCall`/`perceptionSystem`，非 `cpuFrame`**（VM 上帧时间被渲染 engineGap 主导，见基线报告）。
- RuntimeProfileCounters 是 static，调用方控频；FairyGUI lua_bridge 是手工 glue（非 tolua），见 [[fgui]]。
- **输入到 Lua 的入口**由 `GameManager` 派发：`EventHandle_Keyboard(keycode, pressed)` 与
  `EventHandle_Mouse(ctype, x, y, button)`（2026-08-04 接通，此前 Lua 侧 stub 存在但 C++ 从未调用）。
  `ctype` 0=move / 1=down / 2=up，`button` 沿用 OIS 数值（左 0 / 右 1），move 传 -1。
  FGUI 消费掉的事件提前 return，**UI 优先于 sample** 的次序不可颠倒。
- `GameManager:getTimeInMillis()` 返回的是**启动至今的仿真时间**（非墙钟 epoch），
  sample 初始化期间可能为 **0**——用它做时间戳时不要拿 `>0` 当有效性判据。
- `GameManager:SetSimulationPaused(bool)` / `IsSimulationPaused()` 在应用时钟边界暂停：不推进 `m_SimulationTime`、Lua `__tick__`、ObjectManager/AI 或 Bullet；`Sandbox_Update(0)` 与 UI 输入继续响应。暂停入口清理 PlayerController 瞬时按键，恢复不累计暂停期间的补帧。sample 的阶段/命令更新也必须受自身暂停状态约束，不能因 UI 继续 tick 而偷偷推进战斗。Sandbox19 的准备、Esc 菜单和结算共同使用此契约；退出请求走 `GameManager:RequestQuit()`。
- 声音当前仅支持 PCM 16-bit、单/双声道、8–96kHz、单文件不超过 2MiB；缓存最多 32 个路径。播放新短音会停止上一段，音量缩放 PCM，静音调用 StopAll；没有混音、空间声或音乐播放合同。Lua 不持有原生音频对象，runtime 析构停止声音；sample 重开另行清掉事件采样并 StopAll。
- Windows 音频动态装载 `winmm.dll` 并调用异步 PlaySound；Apple 分支使用 NSSound，其他平台 `IsAvailable=false`。API 可用与 Play 返回成功都不能证明扬声器听感、事件时序或平台验收；macOS 分支本轮尚未实际构建/播放验证。
- Windows `HELLO_WINDOW_BACKGROUND=1` 使用 `initialise(false)` 后创建 `hidden=true` 的无边框 D3D9 窗口，从创建阶段避免普通显示路径激活窗口；保持渲染 active。InputManager 检测到隐藏窗口便跳过 OIS 硬件输入初始化，日志应包含 `hidden=true physical-input=disabled`，另记录 `foregroundUnchanged` 与实际像素尺寸。后台窗口不能用来验收真实键鼠输入。
- 后台像素尺寸由 `HELLO_WINDOW_WIDTH`（640–3840，默认 1280）与 `HELLO_WINDOW_HEIGHT`（360–2160，默认 800）指定；非法值回退默认。两目标尺寸分别设为 `1280/720`、`1920/1080`，该专用路径直接创建指定像素窗口，不使用普通窗口的 DPI 放大模式。`HELLO_AUDIO_SILENT=1` 让 Sandbox19 本轮播放音量为 0，不改写 `relay_settings.cfg` 中用户保存的 muted 偏好。
- `HELLO_INPUT_REPLAY=<文件>` 启用 GameManager 更新入口的内部事件回放，默认等待 PlayerController 存在后计时；无玩家的受控 sample 可同时设置 `HELLO_INPUT_REPLAY_WAIT_FOR_PLAYER=0`，从首个更新循环开始计时。回放时钟包含暂停期间的 update delta，支持恢复菜单操作。事件经过 InputManager/应用输入路由，日志统一标 `[InputReplay] synthetic=true`；文件结束释放持有按键，QUIT 请求应用退出。它既不是硬件输入，也不是人工手感或外部窗口交互证据。

## 6. 数据流 / 与其他模块关系

`ObjectManager/各 AI 系统 → RuntimeProfileCounters.Plot*`；`FairyGuiSystem ← ScriptLuaVM(lua_bridge)`；相机服务 [[systems-service]] CameraService 包装；`Lua sample → SandboxAudio → RuntimeUiSound → 平台短音后端`。GameManager 装配服务并把仿真推进与仍响应的 UI 分开。

## 7. 验证策略

- 回归 sample：`Sandbox16`(性能采样)；gate：`run_fgui_production_gate.ps1`、`ai_perf_1000`。
- 相机/输入/暂停改动还需 Sandbox19 真实窗口：WASD/QE/滚轮、UI 命中优先级、调整尺寸、暂停前后按键与战斗冻结。新产品 fixture 可证明合成场景下时钟/位置/HP 冻结和恢复流程，不能代替外部输入或镜头手感验收。
- 音频需分别核查短音资产、失败退化、静音/音量与设置重载，以及设备实际播放；Windows 和 macOS 分开记录。当前（2026-09-10）Windows Release 全量重编、Lua 语法及强化产品 fixture 完整通过，见[本机 gate 摘要](../../tmp/relay-product-fixture-20260910-140218-noxicg2_/summary.json)；该 fixture 不证明硬件输入、听感或自然通关。内部输入回放已取得独立包自然通关、重开/退出、两尺寸和声音设置跨进程保存证据；人工外部输入、听感及 macOS 未验，见[本轮验收](../playtest-relay-2026-09-10.md)。

## 8. 已知 gap / 相关文档

- 待：profiler UI、FairyGUI element inspector。`docs/archive/long-term-iteration-plan.md` §5、`docs/perf/ai-perf-release-baseline-20260612.md`。
