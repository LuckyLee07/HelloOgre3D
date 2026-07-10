# Sandbox19 第三人称相机 + FairyGUI 雷达 — 设计

- **日期**：2026-07-10
- **作者**：lizhibao
- **状态**：已批准

## 1. 目的与背景

为可玩纵切片 `Sandbox19` 增加两项观感能力，对齐参考项目 `E:\Workspace\code-master`（Game Programming in C++）Chapter12 的第三人称 + 雷达效果：

1. **第三人称跟随相机**：相机置于玩家角色后上方、看向角色前方、弹簧阻尼平滑（照搬 code-master `FollowCamera` 数学）。
2. **圆形示意雷达**：左上角圆盘，按队伍色把其它 agent 画成 blip，player-up（玩家永远朝雷达上方），中心静止箭头。

参考实现分析（已通读源码，非仅截图）：

- 相机 `code-master/Chapter12/FollowCamera.cpp`：`idealPos = ownerPos − forward×horzDist(350) + up×vertDist(150)`；`lookAt = ownerPos + forward×targetDist(100)`；弹簧 `k=128`、`dampening=2√k`，阻尼平滑 `actualPos`。
- 输入 `Chapter12/FollowActor.cpp`：原作是 tank 式（A/D 转角色、W/S 沿朝向、无鼠标无横移）。**本设计不照搬控制**（见 §4）。
- 雷达 `Chapter12/HUD.cpp`：2D UI 叠加（非第二个 3D 相机）；把注册的 `TargetComponent` 相对玩家投影 → 按玩家朝向（atan2 + 2D 旋转矩阵）旋转使玩家朝上 → blip；`range=2000 / radius=92`。

### 非目标

- 不做 FPS 相机 / 第一人称武器视图 / 视角切换分支。
- **仅 `Sandbox19` 生效**，不改动其它 sample 的既有相机（吸取本轮已删除的全局俯视 minimap viewport 教训——那是「第二个 3D 相机渲染场景」的错误思路）。
- 不换角色 mesh、不引入新角色 / 骨骼。
- 雷达纯显示，无点击 / 交互。
- 不做鼠标捕获（mouse-capture / recenter）式全程转向；用 RMB 按住期间转向。
- 不引入 Def/CSV/触发器/编辑器（北极星暂缓项）。

### 方向前提（显式确认）

`docs/project-direction.md` §5 明确「FPS 相机不作为当前优先项」、非目标含「不新增相机模式」。第三人称 ≠ FPS，但确属**新增相机模式**，属方向调整。本设计将其**限定在 `Sandbox19`**，作为北极星「先在战斗 sample 加可玩切片、用实际手感定方向」的一部分；不影响其它 sample、不铺开新系统主线。**已经用户确认。**

## 2. 设计原则

- 忠实复用 code-master `FollowCamera` 的相机数学（后上方 + 看前方 + 弹簧阻尼）。
- **朝向由输入驱动、相机是朝向的因变量**（避免「朝向=相机 forward」与相机反向依赖形成循环）。
- 雷达数据逻辑放 Lua（投影 + 旋转，开销可忽略），渲染走 FairyGUI 程序化矢量图形（无需美术资源、无需 `.fui` 包）。
- `Sandbox19`-only 门控；sample reload 显式清理相机模式与 FGUI handle，不污染其它 sample。
- 分层遵循 `AGENTS.md`：相机模式落 runtime（`OgreCameraController`），服务门面落 sandbox（`CameraService`），玩家控制落 sandbox 组件（`PlayerController`），雷达落 Lua sample。
- tolua 强约束：新增导出给 Lua 的 C++ 入口手术式改绑定，禁 `tolua.bat` 全量重生成。

## 3. 架构与组件（分层）

### 3.1 第三人称相机（runtime + sandbox，C++）

**runtime 层 — `OgreCameraController`（`src/HelloOgre3D/runtime/ogre/`）**

- 现有模式 FREELOOK/ORBIT/MANUAL，新增 `CAM_FOLLOW` 模式枚举。
- 新增状态：`m_followTargetPos`、`m_followForwardXZ`、`m_horzDist`、`m_vertDist`、`m_targetDist`、`m_eyeHeight`、`m_springConstant`、`m_followActualPos`、`m_followVelocity`、`m_hasFollowState`。
- 新增方法：`SetFollowTarget(pos, forwardXZ)`、`SetFollowParams(horz, vert, target, eye, spring)`、`SnapFollowToIdeal()`、`EnterFollow()/ExitFollow()`。
- `Update(dt)` 在 FOLLOW 模式：
  - `idealPos = targetPos − forwardXZ×horzDist + UNIT_Y×vertDist`
  - 弹簧阻尼：`accel = −k·(actual−ideal) − 2√k·vel`；`vel += accel·dt`；`actual += vel·dt`
  - `lookAt = targetPos + forwardXZ×targetDist + UNIT_Y×eyeHeight`
  - `camera->setPosition(actual)`；`camera->lookAt(lookAt)`
- FOLLOW 模式下**不消费** FREELOOK 的鼠标 look（模式互斥）。

**sandbox 层 — `CameraService`（`src/HelloOgre3D/sandbox/systems/service/`）**

- 加薄入口转发到 `OgreCameraController`：`EnterFollowMode(params)`、`ExitFollowMode()`、`SetFollowTarget(pos, forward)`、`SnapFollowToIdeal()`。
- **开放实现决策（plan 定夺）**：`CameraService` 目前只持 `Ogre::Camera*`/`SceneManager*`，不持 `OgreCameraController`。FOLLOW 入口的接线落点有三选：(a) 给 `CameraService` 注入 `OgreCameraController*`（GameManager 接线）；(b) 入口留在 `GameManager` 并导出；(c) `PlayerController` 经 `SandboxServices` 拿 controller。plan 首个任务定夺，倾向 (a)（保持 sandbox 只认服务门面）。

**sandbox 组件 — `PlayerController`（`src/HelloOgre3D/sandbox/components/control/`）**

- `onAttach`：`EnterFollowMode` + `SnapFollowToIdeal`；`onDetach`：`ExitFollowMode`（恢复 FREELOOK，保证 sample 切换/reload 干净）。
- 新增鼠标移动处理（`IInputHandler` 若无 `OnMouseMoved` 回调需补）：RMB 按住时 `m_yaw += mouseXRel × sensitivity`；由 `m_yaw` 算水平 forward → `owner->SetForward(forward)`。移除现有「朝向=相机 forward」。
- `UpdateMovement`：`forward` = 角色朝向水平化，`right = forward × UNIT_Y`；W/S = ±forward，A/D = ±right（相对**角色朝向**，不再相对相机）。
- `UpdateCameraFollow` 改为 `camera->SetFollowTarget(ownerPos, forwardXZ)`（替换现有 `TranslateCameraWorld` 平移跟随）。
- 射击方向 = `owner->GetForward()`。

### 3.2 雷达（Lua + FairyGUI）

已核实：`FairyGuiSystem` 在 `ClientManager` 主循环无条件 Initialize/Update/Render（仅受编译开关 `HELLO_ENABLE_FGUI` 约束）；Lua 经全局 `FairyGuiRuntime`（`FairyGuiLuaApi`）访问；程序化建图不需 `.fui` 包。

**`Sandbox19.lua`**

- init（`FairyGuiRuntime:IsAvailable()` 为真时）：`CreateContainer` 建雷达根容器 → `AddObjectToRoot` → `SetObjectPosition` 左上角；建：
  - 圆盘底 = `CreateGraphRegularPolygon(root, "disc", d, d, 40, ...)` 暗色半透。
  - （可选）距离环 = 再叠一个 polygon 描边。
  - 中心箭头 = `CreateGraphRegularPolygon(root, "arrow", w, h, 3, ...)` 居中、静止朝上。
  - 每 agent 一个 blip = `CreateGraphRegularPolygon(root, "blip_i", s, s, 12, teamColor)`，敌红 / 友绿，存 handle 表。
- `Sandbox_Update`：取 `_player` pos + forward（XZ）→ `playerYaw = atan2`：
  - `rel = agentPosXZ − playerPosXZ`；`|rel| > range` 或 agent 死亡 → `SetObjectVisible(blip, false)`；
  - 否则 `scaled = rel × (radarRadius/radarRange)`；按 `−playerYaw` 旋转（Lua 2D：`rx = cos·x − sin·z`，`ry = sin·x + cos·z`，映射雷达屏坐标，注意屏 y 向下）；
  - `SetObjectPosition(blip, centerX+rx, centerY+ry)` + `SetObjectVisible(true)`。
- reload / 退出（`_RestartEncounter` 与 sample 卸载）：对所有 handle `RemoveObject` 显式销毁并置 nil（FGUI 红线：销毁父不自动清子、二次销毁崩溃）。

## 4. 控制方案（第三人称射击，已确认「鼠标 X 驱动」）

- **朝向**：RMB 按住时鼠标左右移动 → 角色偏航 yaw；松开 RMB 不转向。
- **移动**：W/S 沿角色朝向前 / 后；A/D 沿角色右向左 / 右横移。
- **射击**：LMB 沿角色朝向；换弹 R；冲刺 Shift。
- **相机**：始终 FOLLOW 跟在角色朝向后上方（弹簧平滑）。
- 不照搬 code-master 的 A/D 转角色（tank，不适合射击）；不做全程鼠标转向（避免窗口捕获）。

## 5. 数据流

- 相机：`PlayerController.update → CameraService.SetFollowTarget(pos, forward) → OgreCameraController(FOLLOW).Update → Ogre::Camera(pos/lookAt)`。
- 雷达：`Sandbox_Update(Lua) → 取 agents pos → 投影 + 旋转 → FairyGuiRuntime:SetObjectPosition/SetObjectVisible`。

## 6. 错误处理与失败语义

- 相机：owner/camera 空 → 跳过跟随、保持上帧，不崩；forward NaN/零 → 兜底上帧或 `UNIT_Z`；EnterFollow 失败不阻断游戏。
- 雷达：`IsAvailable()=false` → 整个雷达跳过，玩法不受影响；handle 创建返回 0 → 跳过该元素；reload 必须先销毁旧 handle 再重建，防泄漏与二次销毁崩溃。
- sample 切换：`PlayerController.onDetach` 必须 `ExitFollowMode` 恢复 FREELOOK，避免其它 sample 相机被 FOLLOW 卡住。

## 7. 与现有系统 / sample / 规则的关系（回归面）

- 影响面：`OgreCameraController`(runtime)、`CameraService`(systems-service)、`PlayerController`(components/control)、`Sandbox19.lua`(samples)、tolua 绑定(scripting-tolua)、可能 `GameManager`(接线 controller)。
- 回归：`Sandbox19` smoke（`status=PASS` 不退化）+ 手动手感验收（移动 / 转向 / 射击 / 雷达 / 重开）；其它 sample（如 `Sandbox17`）smoke 确认相机未被污染；FGUI 改动跑 `run_fgui_production_gate.ps1 -Mode Full`；Release x64 编译。
- 约束保持：`player_soldier`/`ai_soldier` controller 互斥断言不变；FGUI handle 生命周期红线；`CameraService` 不 include `ClientManager.h`。

## 8. 已知局限

- 无鼠标捕获，转向靠 RMB 按住（窗口模式取舍）。
- blip 颜色建时定死（FGUI 无 per-frame 颜色 setter）→ 按队伍预建 handle。
- 雷达 player-up、箭头静止（同 code-master）。
- 相机 `horz/vert/target/spring` 参数需按 `Sandbox19` 世界尺度（`SoldierObject` ~人高，远小于 code-master 350/150/100）实测调。
- `CameraService` 当前不持 `OgreCameraController`，FOLLOW 入口接线落点见 §3.1 开放决策，plan 首个任务定夺。
- 雷达依赖编译开关 `HELLO_ENABLE_FGUI`（Release 已开、fgui gate 在跑）；若某构建未开，雷达自动降级跳过。

## 9. 开放实现决策（交 plan 定夺，非 spec 缺口）

1. FOLLOW 入口接线落点（§3.1，倾向 CameraService 注入 controller）。
2. 相机参数初值（按 Sandbox19 尺度实测调优，impl 阶段定）。
3. 雷达尺寸 / 位置 / range / 颜色具体数值（impl 阶段调）。
