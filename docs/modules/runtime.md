# 运行时切面（alias: runtime）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

引擎/中间件适配层：性能计数、资源诊断、窗口/相机控制、FairyGUI 系统集成与局部短音播放。应用层 GameManager 负责仿真暂停边界、窗口请求桥和服务装配。

## 2. 源码位置

- `src/HelloOgre3D/runtime/`（FairyGUI 详见 [[fgui]]）
- 应用验证入口：[InputReplay.cpp](../../src/HelloOgre3D/game/diagnostics/InputReplay.cpp)、[ClientManager.cpp](../../src/HelloOgre3D/game/ClientManager.cpp)；硬件输入装配位于 [InputManager.cpp](../../src/HelloOgre3D/sandbox/systems/input/InputManager.cpp)。

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `profiling/RuntimeProfileCounters.{h,cpp}` | 性能 | `Plot{AiScheduler,AgentPerception,HearingDangerSense,TeamBlackboard,FairyGuiService}Stats` + 帧时序 / Lua callback 计数；Tracy 关闭时是 no-op |
| `profiling/Profile.h` | 采样 | 时序宏 |
| `diagnostics/RuntimeResourceDiagnostics.{h,cpp}` | 诊断 | `BuildResourceDump` texture/mesh/buffer 清单 |
| `game/ClientManager.{h,cpp}` | 窗口 | 跨平台启动尺寸、延迟 resize、实际尺寸日志，以及输入/UI/FGUI/viewport 消费链同步 |
| `ogre/OgreCameraController.h` | 相机 | FREELOOK/ORBIT/MANUAL/FOLLOW 相机控制；FOLLOW 为第三人称跟随（主动转向、平移平滑、静态遮挡），Sandbox19 鼠标先转镜、身体跟随水平视线、W/S 沿身体朝向；RenderPresentation 每渲染帧驱动；仍不做 FPS 模式 |
| `audio/RuntimeUiSound.{h,cpp}` / `RuntimeUiSoundMac.mm` | 声音 | PCM 16-bit WAV 短音单路播放；Windows PlaySound / macOS NSSound 适配，音频缓冲与 native handle 由 runtime 持有 |
| `ui/fairygui/FairyGuiSystem.*` | UI | cocoslite 内嵌；渲染几何使用最终视图专用 visibility bit，避免被场景 compositor 采样，见 [[fgui]] |
| `RuntimeToLua.{cpp,pkg}` | 绑定 | runtime 层 tolua |

## 4. 公开能力要点

- 性能上报（AI/UI/帧分项/Lua callback count）、资源快照、既有相机模式、FairyGUI 栈；FGUI `AiDebugPanel` 可读取统一 `[AIRuntimeDiag]` 并按 `focusAgentId` / `filterText` 参数化筛选。
- [CameraService](../../src/HelloOgre3D/sandbox/systems/service/CameraService.h) 导出 `ConfigureFollowCamera` / `ResetFollowCamera` / `SetCameraRelativeMovement` / `SnapFollowTarget` / `GetFollowDistance`。sample 配置参数与范围，控制器统一驱动镜头；Sandbox19 使用距离 6.5、高 3.2、前视 0、眼高 1.5 和距离范围 5.5–11，鼠标相对位移或 Q/E 调整镜头方向，W/S 沿角色朝向、A/D 相对角色侧移，滚轮调整跟随距离。退出 FOLLOW 重置配置及控制开关，其他 sample 默认 tank 控制保持原语义。
- `GameManager:RequestWindowSize(width, height)` 接受 640–3840 × 360–2160 的逻辑内容尺寸，由 `ClientManager` 排到下一帧安全点执行；Lua 鼠标回调不直接进入 Cocoa/Win32 resize。启动环境覆盖、后台 Windows 窗口或非法尺寸会拒绝请求。窗口事件继续同步 OIS 鼠标范围、Lua/UIManager、FairyGUI root、viewport 和相机宽高比；FairyGUI 原生 screen/root 必须先于 `FairyGuiManager_HandleWindowResized` 更新，否则 Lua 层会查询到旧尺寸。
- `SandboxAudio` 为 GameManager 注入的 `RuntimeUiSound`：`IsAvailable`、`Play(path)`、`StopAll`、`SetVolume(0..1)`、`GetVolume`。Lua 负责事件/限频/音量设置；runtime 读取、校验、缓存短 WAV 并持有播放缓冲。Sandbox19 的设置与事件入口见 [sandbox19_audio.lua](../../bin/res/scripts/samples/sandbox19_audio.lua)，自制素材来源记录见 [relay 音效说明](../../bin/res/audio/relay/README.md)。
- Scene compositor 通过 [[systems-service]] SceneService 按相机 viewport 启停。Sandbox19 的 Relay/SceneGrade 仅处理三维 scene texture；Gorilla active-viewport 守卫和 FairyGUI visibility bit 让两套 UI 留在最终 viewport，不被滤色或重复绘制。macOS 默认选择受支持且不超过 4× 的 FSAA，`HELLO_RENDER_FSAA` 可显式覆盖；Relay/SceneGrade 的 scene RTT 不再使用 `no_fsaa`，以免主窗口抗锯齿仅作用在最终全屏四边形。Windows 保留原有 FSAA=0 条件分支，D3D9 新设置未实机复核。完整设计与实机证据见[场景色调与 UI 合成隔离](../dev-design/plans/2026-09-12-sandbox19-scene-grade.md)和[核心战斗体感复核](../dev-design/plans/2026-09-12-sandbox19-core-combat-feel.md)。
- `base_material` 的 GL3+ `diffuse_vs_glsl` / `diffuse_ps_glsl` 现与 HLSL 路径一样读取模型 tangent 和纹理单元 2 的 `normalMap`；片元阶段正交化切线基，切线退化时回退几何法线。GLSL 的三路 sampler 明确绑定 0/1/2，未使用的自动参数已移除。改变共享基础材质后需检查 Sandbox6/7/8，不能仅凭 Sandbox19 的配对混凝土判断其它网格正确；Windows HLSL 分支未改但 D3D9 仍需单独实机复核。实现与证据见[GL3+ 基础材质法线计划](../dev-design/plans/2026-09-12-sandbox19-normal-lighting.md)。

- `RuntimeOgre::ConfigureDirectionalShadows` 由 SceneService 调用，Sandbox19 显式使用单张方向光 R32F 深度图与 receiver-plane bias、PCF 接收。Light 为借用指针，阴影纹理由 SceneManager 管理；关闭清空 caster/receiver 和纹理，其他 sample 不主动启用。GL/HLSL 分别处理 -1..1 / 0..1 clip Z；modulative 会同时压低 ambient，尚非 PBR 光照。基础 ambient samplers 显式绑定 diffuse/AO/emissive 0/1/2，既有头盔/武器发光图只加一次，默认黑图无额外颜色。详见[本轮实机记录](../dev-design/plans/2026-09-12-sandbox19-visual-goal.md)。

## 5. 约束与红线

- 引擎/中间件耦合逻辑收口在 runtime（AGENTS.md 依赖流）。
- compositor scene RTT 与最终 viewport 会复用同一 SceneManager/render queue；render-queue listener 必须核对 active viewport，不能只看 queue id。最终 UI 使用 0x80000000，可做 scene-only target 的 0x7fffffff 排除位；修改默认 viewport visibility mask 时须保留该位。
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
- Windows 音频动态装载 `winmm.dll` 并调用异步 PlaySound；Apple 分支使用 NSSound，其他平台 `IsAvailable=false`。API 可用与 Play 返回成功都不能证明扬声器听感、事件时序或平台验收；macOS arm64 Release 已实际构建并运行 Sandbox19，但本轮没有扬声器听感证据。
- Windows `HELLO_WINDOW_BACKGROUND=1` 使用 `initialise(false)` 后创建 `hidden=false` 的无边框 D3D9 工具窗口；窗口坐标计算到整个虚拟桌面范围之外，Ogre 的 `noActivate` 参数在创建时设置 `WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW`，显示使用 `SW_SHOWNOACTIVATE`，因此 D3D9 可渲染且不会覆盖或激活用户桌面。InputManager 按环境开关跳过 OIS 硬件输入初始化，日志应包含 `hidden=false noActivate=true offscreen=true foregroundUnchanged=true` 与 `background=true physical-input=disabled`。后台窗口不能用来验收真实键鼠输入；2026-09-11 修正后的 10 秒与 40 秒 Windows Release smoke 均通过，1280×800 实际位置为 `-3264,-864`。
- `HELLO_WINDOW_WIDTH`（640–3840）与 `HELLO_WINDOW_HEIGHT`（360–2160）现在跨 Windows/macOS 控制启动内容尺寸；非法值回退平台默认（macOS 1280×720，Windows 1280×800）。普通 Windows 窗口按 DPI 换算为物理像素，Windows 后台专用路径继续把值当明确物理像素并拒绝运行时 resize。任一启动尺寸变量存在时，Sandbox19 DISPLAY 控件锁定，确保启动覆盖优先。`[WindowMode]` / `[WindowResize]` 必须同时记录请求与实际尺寸。
- Sandbox19 普通启动可从 `relay_settings.cfg` 读取 `window=宽x高` 并通过延迟请求应用；后台自动化不消费保存值，避免污染固定尺寸 gate。macOS 实机已验证 1280×720→1600×900 动态切换和重启读取；请求 1920×1080 时，本机标题栏窗口受可见工作区限制为实际 1920×945，日志与 HUD 必须显示实际值，不能把请求值当作 1080p PASS。证据见[动态窗口与多分辨率](../dev-design/plans/2026-09-12-sandbox19-dynamic-window.md)。`HELLO_AUDIO_SILENT=1` 只把本轮播放音量置零，不改写用户 muted 偏好。
- `HELLO_INPUT_REPLAY=<文件>` 启用 GameManager 更新入口的内部事件回放，默认等待 PlayerController 存在后计时；无玩家的受控 sample 可同时设置 `HELLO_INPUT_REPLAY_WAIT_FOR_PLAYER=0`，从首个更新循环开始计时。回放时钟包含暂停期间的 update delta，支持恢复菜单操作。事件经过 InputManager/应用输入路由，日志统一标 `[InputReplay] synthetic=true`；文件结束释放持有按键，QUIT 请求应用退出。它既不是硬件输入，也不是人工手感或外部窗口交互证据。

## 6. 数据流 / 与其他模块关系

`ObjectManager/各 AI 系统 → RuntimeProfileCounters.Plot*`；`FairyGuiSystem ← ScriptLuaVM(lua_bridge)`；相机服务 [[systems-service]] CameraService 包装；`Lua sample → SandboxAudio → RuntimeUiSound → 平台短音后端`。GameManager 装配服务并把仿真推进与仍响应的 UI 分开。

## 7. 验证策略

- 回归 sample：`Sandbox16`(性能采样)；gate：`run_fgui_production_gate.ps1`、`ai_perf_1000`。改 compositor/UI viewport 隔离时还要跑使用该 compositor 的真实抓帧，并至少执行 FairyGUI All 自测，分别证明场景链与常规 UI 渲染。
- 相机/输入/暂停改动还需 Sandbox19 真实窗口：WASD/QE/滚轮、UI 命中优先级、调整尺寸、暂停前后按键与战斗冻结。macOS 的内部回放已覆盖 DISPLAY 点击与 720p→900p resize，产品 fixture 可证明合成场景下时钟/位置/HP 冻结和恢复流程；二者均不能代替外部输入、窗口拖动或镜头手感验收。Windows 普通窗口动态 resize 仍须对应平台运行。
- 音频需分别核查短音资产、失败退化、静音/音量与设置重载，以及设备实际播放；Windows 和 macOS 分开记录。Windows Release 全量重编、Lua 语法及强化产品 fixture 完整通过，内部输入回放已取得独立包自然通关、重开/退出、两尺寸和声音设置跨进程保存证据，见[本轮验收](../playtest-relay-2026-09-10.md)。2026-09-12 macOS arm64 Release、Sandbox19 产品 fixture、Sandbox19/6/7/8 smoke、720p→900p 动态 GL 画面和设置重启读取也已通过；这些仍不证明人工外部输入或扬声器听感。

## 8. 已知 gap / 相关文档

- 待：profiler UI、FairyGUI element inspector。`docs/archive/long-term-iteration-plan.md` §5、`docs/perf/ai-perf-release-baseline-20260612.md`。

## 2026-09-12 战斗反馈与跟随呈现

- Particles/Bullet、Impact 显式使用 GLSL/HLSL 顶点色乘纹理程序，Dust 使用程序化柔边透明片元。新粒子出生颜色必须可见，不能依赖下一次 affector 更新后才从黑色变亮。共享 GL 材质恢复后缩减原有粒子尺寸/发射量；Gun stabilization/thrusters 复用 base_material。相关运行证据见[体验修复](../dev-design/plans/2026-09-12-sandbox19-experience-fixes.md)。
- FOLLOW 中键拖动控制偏航/俯仰（0.12°/相对输入单位，俯角限制8–65°）；Q/E 为75°/s、逐渲染帧积分，释放立即停。主动方向直接响应，只平滑目标平移；静态球扫掠缩距不改变视线方向，避免平滑注视点越过镜头时翻转。左键框选期间不抢中键，暂停和中键释放清除拖动。
- `Application::frameStarted` 在 Ogre 绘制前依次采集输入、更新仿真、呈现与 UI；queued 回调收集完整帧时序和截图。RenderPresentation 在全部相机模式下插值角色节点和骨骼，同步武器挂点；FOLLOW 另更新镜头，不写回物理。暂停固定当前显示快照，恢复不回跳旧帧。
- Xcode Release 依赖已在 Premake 真源改成配置对应的归档完整路径；不要用 proxy 的显示名判断实际链接。构建和性能核验见[Release 依赖经验](../memory/xcode-release-dependency-proxy.md)。

后台或 HELLO_INPUT_REPLAY 存在时，InputManager 在各平台都不创建 OIS 设备或 macOS 鼠标监听；macOS 由独立 Cocoa 事件泵维持窗口响应，内部回放继续走同一 listener 入口。正常交互启动不设这两个变量，物理输入行为保持。PlayerController 分别保存 Space/左键的开火位，释放其中一种不能取消仍按住的另一种。

- Sandbox19 活跃玩法在 macOS 先把指针移到窗口中心，再用 `CGAssociateMouseAndMouseCursorPosition(false)` 隐藏并捕获光标，NSEvent 相对位移累计小数余量后转 FOLLOW 镜头；按住 Alt、暂停、失焦或退出时释放。Windows 先移到客户区中心，再裁剪并隐藏光标；背景/回放仍禁用物理输入。指针模式保持原 UI 绝对坐标。滚轮事件不附带虚假转向增量，FREELOOK 保持原路径。证据见[鼠标与镜头控制](../dev-design/plans/2026-09-12-sandbox19-mouse-camera-control.md)。

- Sandbox19 地表使用独立 `Relay/Ground` GLSL/HLSL 程序与现有铺地 albedo：按米制世界坐标生成错缝板，窄缝通过 `fwidth` 像素覆盖衰减避免远处深格。单 pass 读取当前 ambient/方向光；沿用已有方向光 shadow receiver，不修改共享 `base_material`。补给箱多 submesh 仍复用 `base_material`，新增灰度涂层 albedo。具体资源和性能见[场景资产与地表升级](../dev-design/plans/2026-09-12-sandbox19-scene-assets.md)。

## 2026-09-12 骨骼显示与仿真隔离

`ogre/EntityPoseInterpolation.h` 借用 Entity，采集前后局部骨骼姿态；显示时暂停 Ogre 动画轨道对骨骼的直接作用，按位置/缩放线性插值及最短弧四元数插值设置手动骨骼。仿真前恢复手动标记与启用状态，替换资源前释放；析构不解引用借用对象。

`GameManager` 在输入回放/Lua/AI 前恢复姿态，仅在实际推进仿真后采样；在全部相机模式呈现。显示采样不推进 ASM 通知。`HELLO_ANIMATION_POSE_TRACE=1` 输出逐帧姿态/逻辑时间，`HELLO_RENDER_MAX_FPS=15..240` 仅作为可选诊断限帧，默认关闭；VSync 仍由原配置控制。帧率对比与暂停证据见[动作连续性改造](../dev-design/plans/2026-09-12-agent-animation-smooth.md)。
