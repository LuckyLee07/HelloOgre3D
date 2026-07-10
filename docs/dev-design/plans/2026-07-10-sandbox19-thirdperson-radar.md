# Sandbox19 第三人称相机 + FairyGUI 雷达 实施计划

**Goal:** 为 `Sandbox19` 加第三人称跟随相机（鼠标 X 驱动角色朝向、弹簧跟随后上方）与 FairyGUI 程序化矢量雷达（圆盘 + blip + 静止箭头，player-up）。
**Architecture:** 相机模式落 runtime（`OgreCameraController` 加 `CS_FOLLOW`），服务门面落 sandbox（`CameraService` 薄入口 + 注入 controller），玩家控制落 sandbox 组件（`PlayerController` 鼠标偏航 + 驱动 follow），雷达落 Lua（`Sandbox19.lua` 经全局 `FairyGuiRuntime` 建图）。相机全 C++-internal、雷达用现成绑定，**无 tolua 改动**。
**Spec:** [docs/dev-design/specs/2026-07-10-sandbox19-thirdperson-radar-design.md](../specs/2026-07-10-sandbox19-thirdperson-radar-design.md)
**状态:** 已完成

> ⚠️ **本次执行不做 git commit（用户要求）**：每 task 验证 = Release 编译 + 目标 sample smoke 不退化；改动留工作区，不 `git add`/`commit`。门禁状态用本文件「状态」字段追踪。

---

## 文件结构（改哪些 / 新建哪些）

| 文件 | 层 | 职责 | 动作 |
|---|---|---|---|
| `src/HelloOgre3D/runtime/ogre/OgreCameraController.h` | runtime | 加 `CS_FOLLOW` 模式 + 弹簧 follow 状态/参数/方法（header-only 内联） | Modify |
| `src/HelloOgre3D/sandbox/systems/service/CameraService.h/.cpp` | sandbox | 注入 `OgreCameraController*`；加 `EnterFollowMode/ExitFollowMode/UpdateFollow`（**非 tolua**） | Modify |
| `src/HelloOgre3D/game/GameManager.cpp` | game | 构造 `CameraService` 时注入 `m_pClientManager->getCameraController()` | Modify |
| `src/HelloOgre3D/sandbox/components/control/PlayerController.h/.cpp` | sandbox | 鼠标 X 偏航→角色朝向；WASD 相对角色朝向；驱动 follow；attach 进 / detach 退 FOLLOW | Modify |
| `bin/res/scripts/samples/Sandbox19.lua` | bin/res/scripts | FairyGUI 雷达：建图 + 每帧更新 + reload 清理 | Modify |

**tolua**：无。相机 follow 全走 C++（PlayerController → CameraService → OgreCameraController），不导出给 Lua；雷达用已导出的全局 `FairyGuiRuntime`（`FairyGuiLuaApi`）。故本计划**不动 `SandboxToLua.cpp`**。

---

### Task 1: OgreCameraController 加 CS_FOLLOW 弹簧跟随模式

**Files:**
- Modify: `src/HelloOgre3D/runtime/ogre/OgreCameraController.h`

**层归属:** runtime（引擎相机适配）。
**验证策略:** Release 编译 + `Sandbox19` smoke（此步仅新增未被调用的模式，行为不变，只验证编译与不退化）。

- [ ] **Step 1：枚举加 `CS_FOLLOW`**（`OgreCameraController.h:15-19`）
  ```cpp
  enum CameraStyle { CS_FREELOOK, CS_ORBIT, CS_MANUAL, CS_FOLLOW };
  ```

- [ ] **Step 2：加 follow 状态成员**（`protected` 段，紧接 `mSmoothedMouseDeltaY` 后）
  ```cpp
  Ogre::Vector3 mFollowTargetPos = Ogre::Vector3::ZERO;
  Ogre::Vector3 mFollowForward   = Ogre::Vector3::UNIT_Z;
  Ogre::Vector3 mFollowActualPos = Ogre::Vector3::ZERO;
  Ogre::Vector3 mFollowVelocity  = Ogre::Vector3::ZERO;
  Ogre::Real mFollowHorzDist   = 8.0f;   // 后方距离（Sandbox19 尺度，Task3 调）
  Ogre::Real mFollowVertDist   = 4.0f;   // 上方高度
  Ogre::Real mFollowTargetDist = 3.0f;   // 看向角色前方距离
  Ogre::Real mFollowEyeHeight  = 1.5f;   // 看向点抬高
  Ogre::Real mFollowSpring     = 64.0f;  // 弹簧刚度
  bool mHasFollowState = false;
  ```

- [ ] **Step 3：`setStyle` 加 CS_FOLLOW 分支**（`setStyle` 内 else-if 链尾部，`CS_MANUAL` 分支后）
  ```cpp
  } else if (mStyle != CS_FOLLOW && style == CS_FOLLOW) {
      mCamera->setAutoTracking(false);
      mCamera->setFixedYawAxis(true);
      mHasFollowState = false; // 下次 updateFollow 会 snap
  }
  ```
  说明：`frameRenderingQueued`/`injectMouseMove`/`injectKeyDown` 已对非 `CS_FREELOOK` 早退，CS_FOLLOW 下控制器自动忽略键鼠，无需改它们。

- [ ] **Step 4：加 public follow 方法**（`injectMouseUp` 后、`protected:` 前）
  ```cpp
  void setFollowParams(Ogre::Real horz, Ogre::Real vert, Ogre::Real target,
                       Ogre::Real eye, Ogre::Real spring) {
      mFollowHorzDist = horz; mFollowVertDist = vert;
      mFollowTargetDist = target; mFollowEyeHeight = eye;
      mFollowSpring = spring > 1.0f ? spring : 1.0f;
  }
  void enterFollow() { setStyle(CS_FOLLOW); }
  void exitFollow()  { setStyle(CS_FREELOOK); }

  // 由 PlayerController 每帧调用；dtSec = deltaMs/1000
  void updateFollow(const Ogre::Vector3& targetPos, const Ogre::Vector3& forwardXZ, Ogre::Real dtSec) {
      if (mStyle != CS_FOLLOW || !mCamera) return;
      mFollowTargetPos = targetPos;
      if (!forwardXZ.isZeroLength() && !forwardXZ.isNaN())
          mFollowForward = forwardXZ.normalisedCopy();
      const Ogre::Vector3 ideal = computeFollowIdeal();
      if (!mHasFollowState || dtSec <= 0.0f) {
          mFollowActualPos = ideal; mFollowVelocity = Ogre::Vector3::ZERO;
          mHasFollowState = true;
      } else {
          const Ogre::Real damp = 2.0f * Ogre::Math::Sqrt(mFollowSpring);
          const Ogre::Vector3 diff = mFollowActualPos - ideal;
          const Ogre::Vector3 accel = -mFollowSpring * diff - damp * mFollowVelocity;
          mFollowVelocity += accel * dtSec;
          mFollowActualPos += mFollowVelocity * dtSec;
      }
      const Ogre::Vector3 look = mFollowTargetPos + mFollowForward * mFollowTargetDist
                               + Ogre::Vector3::UNIT_Y * mFollowEyeHeight;
      mCamera->setPosition(mFollowActualPos);
      mCamera->lookAt(look);
  }
  ```
  并加 `protected` 辅助：
  ```cpp
  Ogre::Vector3 computeFollowIdeal() const {
      return mFollowTargetPos - mFollowForward * mFollowHorzDist
           + Ogre::Vector3::UNIT_Y * mFollowVertDist;
  }
  ```

- [ ] **Step 5：编 Release** `MSBuild build\HelloOgre3D.sln /t:HelloOgre3D /p:Configuration=Release /p:Platform=x64 /m`
- [ ] **Step 6：跑 smoke** `tools\run_sandbox_smoke.ps1 -Sample Sandbox19 -StopExisting -NoTail` → 预期 `status=PASS`

---

### Task 2: CameraService 注入 controller + follow 门面入口

**Files:**
- Modify: `src/HelloOgre3D/sandbox/systems/service/CameraService.h`
- Modify: `src/HelloOgre3D/sandbox/systems/service/CameraService.cpp`
- Modify: `src/HelloOgre3D/game/GameManager.cpp:117`

**层归属:** sandbox/systems/service + game 接线。
**验证策略:** Release 编译 + `Sandbox19` smoke（仍未被 PlayerController 调用，验证编译/注入/不退化）。

- [ ] **Step 1：`CameraService.h` 前向声明 + 注入成员 + 入口**（`class CameraService` 内）
  - 顶部前向声明加 `class OgreCameraController;`
  - 构造签名加第 4 参：`CameraService(Ogre::Camera* camera, Ogre::SceneManager* sceneManager, OgreCameraController* cameraController, const ProfileTimeGetter& profileTimeGetter);`
  - **非 tolua** public 方法（放在 `//tolua_end` 之后、`TranslateCameraWorld` 附近）：
    ```cpp
    void EnterFollowMode(float horz, float vert, float target, float eye, float spring);
    void ExitFollowMode();
    void UpdateFollow(const Ogre::Vector3& targetPos, const Ogre::Vector3& forwardXZ, float dtSec);
    ```
  - private 加成员：`OgreCameraController* m_cameraController; // non-owning; injected by GameManager`

- [ ] **Step 2：`CameraService.cpp` 实现**
  - `#include "ogre/OgreCameraController.h"`
  - 构造函数存 `m_cameraController = cameraController;`
  - 实现：
    ```cpp
    void CameraService::EnterFollowMode(float horz, float vert, float target, float eye, float spring) {
        if (m_cameraController) { m_cameraController->setFollowParams(horz, vert, target, eye, spring);
                                  m_cameraController->enterFollow(); }
    }
    void CameraService::ExitFollowMode() { if (m_cameraController) m_cameraController->exitFollow(); }
    void CameraService::UpdateFollow(const Ogre::Vector3& p, const Ogre::Vector3& f, float dt) {
        if (m_cameraController) m_cameraController->updateFollow(p, f, dt);
    }
    ```

- [ ] **Step 3：`GameManager.cpp:117` 注入 controller**（`new CameraService(` 调用，在 camera/sceneManager 后加第 3 参）
  ```cpp
  m_pCameraService = new CameraService(
      pCamera, pSceneManager,
      m_pClientManager->getCameraController(),   // 新增
      [this](CameraService::ProfileTimeKind kind) -> long long { /* 原 lambda 不变 */ });
  ```
  确认 `GameManager.cpp` 已 include `game/ClientManager.h`（`getCameraController` 声明处）；若无则加。

- [ ] **Step 4：编 Release**（同 Task1 Step5）
- [ ] **Step 5：跑 smoke** `Sandbox19` → `status=PASS`

---

### Task 3: PlayerController 第三人称控制（鼠标偏航 + 相对角色移动 + 驱动 follow）

**Files:**
- Modify: `src/HelloOgre3D/sandbox/components/control/PlayerController.h`
- Modify: `src/HelloOgre3D/sandbox/components/control/PlayerController.cpp`

**层归属:** sandbox/components/control。
**验证策略:** Release 编译 + `Sandbox19` smoke（`status=PASS`）+ `Sandbox17` smoke（确认相机未被 FOLLOW 污染）+ 手动手感验收（转向/移动/射击/相机跟随，本次执行标注待人工）。

- [ ] **Step 1：`PlayerController.h` 加成员 + override**
  - override 加：`virtual bool OnMouseMoved(const OIS::MouseEvent& evt) override;`
  - 私有成员加：
    ```cpp
    bool m_rmbHeld;
    Ogre::Real m_yaw;        // 弧度，角色偏航
    bool m_hasYaw;
    ```
  - 私有方法加：`void UpdateFacingForward();`（由 yaw 算 forward 并 SetForward）

- [ ] **Step 2：构造/attach 初始化**（`PlayerController.cpp`）
  - 构造函数初始化 `m_rmbHeld(false), m_yaw(0.0f), m_hasYaw(false)`。
  - `onAttach`：在 `ResetCameraFollow()` 后，取 CameraService 进 FOLLOW：
    ```cpp
    m_rmbHeld = false; m_hasYaw = false;
    const SandboxServices* services = GetSandboxServices();
    CameraService* camera = services ? services->camera : nullptr;
    if (camera) camera->EnterFollowMode(8.0f, 4.0f, 3.0f, 1.5f, 64.0f); // 尺度待手感调
    ```
  - `onDetach`：加 `CameraService* camera = ...; if (camera) camera->ExitFollowMode();`（恢复 FREELOOK，防污染其它 sample）。

- [ ] **Step 3：RMB 跟踪 + 鼠标偏航**（`PlayerController.cpp`）
  - `OnMousePressed`：现只处理 `MB_Left`；加 `if (btn == OIS::MB_Right) { m_rmbHeld = true; return true; }`。
  - `OnMouseReleased`：加 `if (btn == OIS::MB_Right) { m_rmbHeld = false; return IsAlive(); }`。
  - 新增 `OnMouseMoved`：
    ```cpp
    bool PlayerController::OnMouseMoved(const OIS::MouseEvent& evt) {
        if (!IsAlive() || !m_rmbHeld) return false; // 不消费→让相机控制器（FOLLOW 下忽略）
        const Ogre::Real sensitivity = 0.005f; // 弧度/像素，待调
        m_yaw -= static_cast<Ogre::Real>(evt.state.X.rel) * sensitivity;
        UpdateFacingForward();
        return true; // 消费，避免 FREELOOK 抢
    }
    ```

- [ ] **Step 4：朝向计算 + 移动改相对角色**
  - `UpdateFacingForward`：
    ```cpp
    void PlayerController::UpdateFacingForward() {
        SoldierObject* owner = GetSoldierOwner();
        if (owner == nullptr) return;
        const Ogre::Vector3 forward(Ogre::Math::Sin(m_yaw), 0.0f, Ogre::Math::Cos(m_yaw));
        m_aimDirection = forward;
        owner->SetForward(forward);
    }
    ```
  - `UpdateAimDirection`：改为——首次（`!m_hasYaw`）从 owner 现有朝向初始化 `m_yaw = atan2(fwd.x, fwd.z); m_hasYaw = true;` 然后 `UpdateFacingForward()`；不再从相机 basis 取朝向。
  - `UpdateMovement`：把 `ResolveCameraGroundBasis(...)` 换成基于角色朝向的 basis：
    ```cpp
    Ogre::Vector3 forward = m_aimDirection; forward.y = 0.0f;
    if (forward.isZeroLength()) forward = Ogre::Vector3::UNIT_Z; forward.normalise();
    Ogre::Vector3 right = Ogre::Vector3::UNIT_Y.crossProduct(forward); right.normalise();
    // W/S ±forward, D/A ±right（其余保留：sprint、停步、EnterMove/IdleIntent）
    ```

- [ ] **Step 5：驱动 follow**（`update` / `UpdateCameraFollow`）
  - `UpdateCameraFollow` 改为喂 follow 目标（替换现有 `TranslateCameraWorld` 平移逻辑）：
    ```cpp
    void PlayerController::UpdateCameraFollow(int deltaMs) {
        SoldierObject* owner = GetSoldierOwner();
        const SandboxServices* services = GetSandboxServices();
        CameraService* camera = services ? services->camera : nullptr;
        if (owner == nullptr || camera == nullptr) return;
        const Ogre::Vector3 pos = owner->GetPosition();
        if (pos.isNaN()) return;
        camera->UpdateFollow(pos, m_aimDirection, static_cast<float>(deltaMs) / 1000.0f);
    }
    ```
  - `update(int deltaMs)`：去掉 `(void)deltaMs`；调用顺序改为 `UpdateAimDirection(); UpdateCombat(); UpdateMovement(); UpdateCameraFollow(deltaMs);`（先定朝向再算移动，最后跟随）。
  - 射击方向已是 `m_aimDirection`（=角色朝向），`ExecuteAnimShoot` 不变。

- [ ] **Step 6：编 Release**（同上）
- [ ] **Step 7：跑 smoke** `Sandbox19` → `status=PASS`；再跑 `Sandbox17 -StopExisting -NoTail` → `status=PASS`（相机未污染）。
- [ ] **Step 8（人工待办，本次执行标注 skip-manual）**：手动跑 Sandbox19 验收——RMB 转向、WASD 相对角色移动、LMB 沿朝向射击、相机后上方弹簧跟随、重开无残留。

---

### Task 4: Sandbox19 FairyGUI 矢量雷达（Lua）

**Files:**
- Modify: `bin/res/scripts/samples/Sandbox19.lua`

**层归属:** bin/res/scripts（Lua sample）。
**验证策略:** Lua 语法 `luac -p` + Release 无需重编（纯 Lua）+ `Sandbox19` smoke（`status=PASS`）+ `tools\run_fgui_production_gate.ps1 -Mode Full -StopExisting`（FGUI 未退化）+ 手动看雷达（标注待人工）。

- [ ] **Step 1：雷达状态 + 常量**（文件顶部 local 区）
  ```lua
  local _radar = { root = nil, blips = {}, ok = false }
  local RADAR = { cx = 90, cy = 90, radius = 78, range = 60, discSides = 40, blipSize = 8 }
  ```
  （`range` 是世界单位、`radius` 是屏幕像素，按 Sandbox19 尺度调。）

- [ ] **Step 2：建雷达（在 `_CreateHud` 后新增 `_CreateRadar`，`Sandbox_Initialize` 调用）**
  ```lua
  local function _CreateRadar()
      if FairyGuiRuntime == nil or not FairyGuiRuntime:IsAvailable() then return end
      local root = FairyGuiRuntime:CreateContainer("radar")
      if root == 0 then return end
      FairyGuiRuntime:AddObjectToRoot(root)
      FairyGuiRuntime:SetObjectPosition(root, 20, 20)
      local d = RADAR.radius * 2
      local disc = FairyGuiRuntime:CreateGraphRegularPolygon(root, "disc", d, d, RADAR.discSides, 0.05, 0.08, 0.10, 0.72)
      FairyGuiRuntime:SetObjectPosition(disc, 0, 0)
      -- 中心箭头（三角形，静止朝上）
      local arrow = FairyGuiRuntime:CreateGraphRegularPolygon(root, "arrow", 14, 16, 3, 0.9, 0.9, 0.95, 1.0)
      FairyGuiRuntime:SetObjectPosition(arrow, RADAR.cx - 7, RADAR.cy - 8)
      _radar.root = root
      _radar.ok = true
  end
  ```

- [ ] **Step 3：每 agent 建 blip（`_SpawnEncounter` 末尾，agents 就绪后）**
  ```lua
  local function _CreateBlips()
      if not _radar.ok then return end
      _radar.blips = {}
      local agents = ObjectManager:getAllAgents()
      for i = 0, agents:size() - 1 do
          local a = agents[i]
          if a ~= nil and a ~= _player then
              local enemy = a:GetTeamId() ~= _player:GetTeamId()
              local r,g,b = enemy and 0.95 or 0.2, enemy and 0.2 or 0.9, enemy and 0.15 or 0.35
              local h = FairyGuiRuntime:CreateGraphRegularPolygon(_radar.root, "blip", RADAR.blipSize, RADAR.blipSize, 12, r, g, b, 1.0)
              if h ~= 0 then table.insert(_radar.blips, { handle = h, agent = a }) end
          end
      end
  end
  ```

- [ ] **Step 4：每帧更新（`Sandbox_Update` 内，`_UpdateHud()` 后）**
  ```lua
  local function _UpdateRadar()
      if not _radar.ok or _player == nil then return end
      local pp = _player:GetPosition()
      local pf = _player:GetForward(); pf.y = 0
      local flen = math.sqrt(pf.x*pf.x + pf.z*pf.z)
      if flen < 1e-4 then return end
      local fx, fz = pf.x/flen, pf.z/flen      -- 角色前向(XZ)
      local rx, rz = -fz, fx                    -- 右向 = UNIT_Y × forward
      local scale = RADAR.radius / RADAR.range
      local halfBlip = RADAR.blipSize * 0.5
      for _, b in ipairs(_radar.blips) do
          local a = b.agent
          if a ~= nil and a:GetHealth() > 0 then
              local ap = a:GetPosition()
              local dx, dz = ap.x - pp.x, ap.z - pp.z
              local fwdC = dx*fx + dz*fz         -- 前向分量 → 屏幕上(-y)
              local rgtC = dx*rx + dz*rz         -- 右向分量 → 屏幕右(+x)
              if fwdC*fwdC + rgtC*rgtC <= RADAR.range*RADAR.range then
                  local sx = RADAR.cx + rgtC*scale - halfBlip
                  local sy = RADAR.cy - fwdC*scale - halfBlip
                  FairyGuiRuntime:SetObjectPosition(b.handle, sx, sy)
                  FairyGuiRuntime:SetObjectVisible(b.handle, true)
              else
                  FairyGuiRuntime:SetObjectVisible(b.handle, false)
              end
          else
              FairyGuiRuntime:SetObjectVisible(b.handle, false)
          end
      end
  end
  ```

- [ ] **Step 5：reload/清理**（`_RestartEncounter` 开头销毁旧 blip 再重建；避免 FGUI 二次销毁崩溃）
  ```lua
  local function _DestroyBlips()
      for _, b in ipairs(_radar.blips) do
          if b.handle ~= nil and b.handle ~= 0 then FairyGuiRuntime:RemoveObject(b.handle) end
      end
      _radar.blips = {}
  end
  ```
  `_RestartEncounter` 里 `ObjectManager:clearAllObjects(...)` 前调 `_DestroyBlips()`；`_SpawnEncounter` 后调 `_CreateBlips()`。root/disc/arrow 只建一次不销毁（sample 生命周期内常驻）。

- [ ] **Step 6：Lua 语法检查** `luac -p bin/res/scripts/samples/Sandbox19.lua`（用仓库 luac；或运行时加载即验证）。
- [ ] **Step 7：跑 smoke** `Sandbox19` → `status=PASS`；`run_fgui_production_gate.ps1 -Mode Full -StopExisting` → 通过。
- [ ] **Step 8（人工待办，标注 skip-manual）**：手动看左上角圆形雷达、敌红友绿 blip 随移动/转向更新、player-up、重开无残留。

---

## 自审对照

- spec §3.1 相机 → Task1/2/3；§3.2 雷达 → Task4；§4 控制 → Task3；§6 失败语义 → 各 Task 空指针/IsAvailable/RemoveObject 兜底；§7 回归 → 各 Task 验证策略（Sandbox19 + Sandbox17 + fgui gate）；§9 开放项①接线 → Task2 定为 CameraService 注入、②③参数 → Task1/3/4 初值+待手感调。
- 占位扫描：无 TBD/TODO；参数初值均给具体数（标「待调」但有值）。
- 类型/命名：`CS_FOLLOW`、`EnterFollowMode`、`updateFollow`、`FairyGuiRuntime:CreateGraphRegularPolygon` 全程一致。
- 每 task 均标层归属 + 验证策略；tolua 步骤：全计划无（已在文件结构说明）。
