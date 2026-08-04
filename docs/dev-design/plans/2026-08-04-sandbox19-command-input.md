# Sandbox19 指令输入层 实施计划

**Goal:** 玩家（小队长）用 RMB 选中友军、F/R/G 下达集火/撤退/编队指令，AI 有可观察反应。

**Architecture:** C++ 只补两个能力——`game` 层 `GameManager` 把鼠标事件派发到已存在的 Lua stub `EventHandle_Mouse`；`sandbox/systems/service` 层 `CameraService` 加 `WorldToScreen` / `ScreenToGroundPoint` 两个 tolua 方法。玩法语义全部在 `bin/res/scripts` 层：新建 Sandbox19 专用 BT（`Subtree` 引用现有 combat 分支，共享 `SoldierBT` 零改动），指令走独立 `command.*` blackboard 命名空间。

**Spec:** [docs/dev-design/specs/2026-08-04-sandbox19-command-input-design.md](../specs/2026-08-04-sandbox19-command-input-design.md)

**状态:** 已批准，执行中

---

## 文件结构

| 文件 | 层 | 职责 | 动作 |
|---|---|---|---|
| `src/HelloOgre3D/sandbox/systems/service/CameraService.h` | sandbox | 声明两个屏幕坐标互转方法（tolua 导出） | Modify |
| `src/HelloOgre3D/sandbox/systems/service/CameraService.cpp` | sandbox | 实现两个方法 | Modify |
| `src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp` | sandbox | 手术式补两个绑定函数 + 两处注册 | Modify |
| `src/HelloOgre3D/game/GameManager.cpp` | game | 鼠标 move/down/up 派发到 `EventHandle_Mouse` | Modify |
| `bin/res/scripts/agent/BehaviorSoldierAgent.lua` | Lua agent | BT 模块/全局名/条件表改为 preset 可覆盖，缺省不变 | Modify |
| `bin/res/scripts/ai/behavior/Sandbox19CommandConditions.lua` | Lua BT | 三个指令条件，读 `command.*` | Create |
| `bin/res/scripts/ai/behavior/config/Sandbox19CommandBT.lua` | Lua BT | 顶层 Selector = commandBranch + Subtree 引用 combat | Create |
| `bin/res/scripts/config/sample_presets.lua` | Lua 配置 | Sandbox19 preset 指定 BT 模块与条件表 | Modify |
| `bin/res/scripts/samples/Sandbox19.lua` | Lua sample | 选择集、点选/框选、指令语义、写 fact + bb、UI、reload 清理 | Modify |

---

## Task 1: CameraService 屏幕坐标互转

**Files:**
- Modify: `src/HelloOgre3D/sandbox/systems/service/CameraService.h`
- Modify: `src/HelloOgre3D/sandbox/systems/service/CameraService.cpp`

**层归属:** `sandbox/systems/service`
**验证策略:** Release 编译 + `Sandbox19` / `Sandbox17` smoke

- [ ] **Step 1：`CameraService.h` 在 `//tolua_begin` 块内、`GetCameraOrientation();` 之后加两行声明**

```cpp
	Ogre::Vector2 WorldToScreen(const Ogre::Vector3& world);
	Ogre::Vector3 ScreenToGroundPoint(Ogre::Real screenX, Ogre::Real screenY, Ogre::Real groundY);
```

同时在文件顶部 `#include "OgreQuaternion.h"` 之后加 `#include "OgreVector2.h"`。

- [ ] **Step 2：`CameraService.cpp` 顶部补 include**

```cpp
#include "OgreViewport.h"
#include "OgrePlane.h"
#include "OgreRay.h"
```

- [ ] **Step 3：`CameraService.cpp` 在 `GetCameraOrientation()` 实现之后追加两个实现**

```cpp
// 世界坐标 → 屏幕像素。相机后方或无 viewport 时返回 (-1,-1)，调用方据此丢弃。
Ogre::Vector2 CameraService::WorldToScreen(const Ogre::Vector3& world)
{
	Ogre::Camera* pCamera = GetCamera();
	if (pCamera == nullptr)
		return Ogre::Vector2(-1.0f, -1.0f);

	Ogre::Viewport* pViewport = pCamera->getViewport();
	if (pViewport == nullptr)
		return Ogre::Vector2(-1.0f, -1.0f);

	// 视图空间 z >= 0 表示点在相机后方（Ogre 右手系，相机朝 -z）。
	const Ogre::Vector3 eyeSpace = pCamera->getViewMatrix(true) * world;
	if (eyeSpace.z >= 0.0f)
		return Ogre::Vector2(-1.0f, -1.0f);

	// Ogre 的 Matrix4 * Vector3 已含透视除法，结果是 [-1,1] 的 NDC。
	const Ogre::Vector3 ndc = pCamera->getProjectionMatrix() * eyeSpace;
	const Ogre::Real width = static_cast<Ogre::Real>(pViewport->getActualWidth());
	const Ogre::Real height = static_cast<Ogre::Real>(pViewport->getActualHeight());
	return Ogre::Vector2((ndc.x * 0.5f + 0.5f) * width, (0.5f - ndc.y * 0.5f) * height);
}

// 屏幕像素 → 与水平面 y=groundY 的交点。射线与平面平行/背离时返回相机位置，调用方据此兜底。
Ogre::Vector3 CameraService::ScreenToGroundPoint(Ogre::Real screenX, Ogre::Real screenY, Ogre::Real groundY)
{
	Ogre::Camera* pCamera = GetCamera();
	if (pCamera == nullptr)
		return Ogre::Vector3::ZERO;

	Ogre::Viewport* pViewport = pCamera->getViewport();
	if (pViewport == nullptr)
		return pCamera->getDerivedPosition();

	const Ogre::Real width = static_cast<Ogre::Real>(pViewport->getActualWidth());
	const Ogre::Real height = static_cast<Ogre::Real>(pViewport->getActualHeight());
	if (width <= 0.0f || height <= 0.0f)
		return pCamera->getDerivedPosition();

	const Ogre::Ray ray = pCamera->getCameraToViewportRay(screenX / width, screenY / height);
	const Ogre::Plane ground(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0.0f, groundY, 0.0f));
	const std::pair<bool, Ogre::Real> hit = ray.intersects(ground);
	if (!hit.first)
		return pCamera->getDerivedPosition();

	return ray.getPoint(hit.second);
}
```

- [ ] **Step 4：编 Release** `"C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe" build\HelloOgre3D.sln /t:HelloOgre3D /p:Configuration=Release /p:Platform=x64 /m` → 预期 0 错
- [ ] **Step 5：commit**（`git add` 精确两个路径；message `[dev]CameraService 增加屏幕坐标互转`）

> 本 task 只加 C++ 方法未接绑定，Lua 侧还调不到，smoke 与 Task 2 合并跑。

---

## Task 2: 手术式补 tolua 绑定

**Files:**
- Modify: `src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp`

**层归属:** `sandbox/scripting`
**验证策略:** Release 编译 + `Sandbox19` / `Sandbox17` smoke
**tolua 步骤:** 手术式改绑定，**禁止 `tolua.bat` 全量重生成**（历史教训见 `AGENTS.md`）

- [ ] **Step 1：在 `tolua_SandboxToLua_CameraService_GetCameraOrientation00` 函数块之后，插入两个绑定函数**

照抄现有 `GetCameraPosition00`（返回 `Ogre::Vector3`）与 `UIFrame::GetDimension`（返回 `Ogre::Vector2`）的模板：

```cpp
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_WorldToScreen00
static int tolua_SandboxToLua_CameraService_WorldToScreen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* world = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'WorldToScreen'", NULL);
#endif
  {
   Ogre::Vector2 tolua_ret = (Ogre::Vector2)  self->WorldToScreen(*world);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector2)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector2));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'WorldToScreen'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_ScreenToGroundPoint00
static int tolua_SandboxToLua_CameraService_ScreenToGroundPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
  Ogre::Real screenX = ((Ogre::Real)  tolua_tonumber(tolua_S,2,0));
  Ogre::Real screenY = ((Ogre::Real)  tolua_tonumber(tolua_S,3,0));
  Ogre::Real groundY = ((Ogre::Real)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ScreenToGroundPoint'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ScreenToGroundPoint(screenX,screenY,groundY);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ScreenToGroundPoint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE
```

- [ ] **Step 2：在 `tolua_function(tolua_S,"GetCameraOrientation",...)` 注册行之后加两行注册**

```cpp
   tolua_function(tolua_S,"WorldToScreen",tolua_SandboxToLua_CameraService_WorldToScreen00);
   tolua_function(tolua_S,"ScreenToGroundPoint",tolua_SandboxToLua_CameraService_ScreenToGroundPoint00);
```

- [ ] **Step 3：编 Release** → 预期 0 错
- [ ] **Step 4：跑 smoke** `tools\run_sandbox_smoke.ps1 -Sample Sandbox19 -StopExisting -NoTail` 与 `-Sample Sandbox17` → 预期均 `status=PASS`
- [ ] **Step 5：commit**（message `[dev]手术式补 CameraService 屏幕坐标互转的 tolua 绑定`）

---

## Task 3: GameManager 鼠标事件派发到 Lua

**Files:**
- Modify: `src/HelloOgre3D/game/GameManager.cpp:364-397`（`OnMouseMoved` / `OnMousePressed` / `OnMouseReleased`）

**层归属:** `game`
**验证策略:** Release 编译 + `Sandbox19` smoke

约定 `ctype`：`0` = move，`1` = down，`2` = up。按钮沿用 OIS 数值（`MB_Left=0` / `MB_Right=1`）。FGUI 消费掉的事件仍然提前 return，不派发到 sample，保持既有优先级。

- [ ] **Step 1：三个处理函数在各自 `#endif` 之后、`return false;` 之前各加一行派发**

`OnMouseMoved`：
```cpp
	m_pScriptVM->callFunction("EventHandle_Mouse", "iiii", 0, evt.state.X.abs, evt.state.Y.abs, -1);
	return false;
```

`OnMousePressed`：
```cpp
	m_pScriptVM->callFunction("EventHandle_Mouse", "iiii", 1, evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn));
	return false;
```

`OnMouseReleased`：
```cpp
	m_pScriptVM->callFunction("EventHandle_Mouse", "iiii", 2, evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn));
	return false;
```

- [ ] **Step 2：更新所有 sample 的 `EventHandle_Mouse` stub 签名**——全仓 `grep -rn "function EventHandle_Mouse" bin/res/scripts`，把每处签名改为 `function EventHandle_Mouse(ctype, x, y, button)`，函数体保持原样（多数为空）。Lua 允许多传参，但显式改签名避免后续误读。
- [ ] **Step 3：编 Release** → 预期 0 错
- [ ] **Step 4：跑 smoke** `-Sample Sandbox19 -StopExisting -NoTail` → 预期 `status=PASS`（验证每帧鼠标派发不刷错误日志）
- [ ] **Step 5：commit**（message `[dev]GameManager 派发鼠标事件到 Lua EventHandle_Mouse`）

---

## Task 4: BehaviorSoldierAgent 的 BT 模块 preset 化

**Files:**
- Modify: `bin/res/scripts/agent/BehaviorSoldierAgent.lua:12`（`_SOLDIER_BT_GLOBAL` / `_SOLDIER_BT_MODULE` 附近）与其 `BuildFromModule` 调用处（约 :74-80）

**层归属:** `bin/res/scripts/agent`
**验证策略:** `Sandbox8` smoke（默认路径未变的证据）+ `Sandbox19` smoke

照搬 2026-07-12 已落库的 `chapter8Config.agentScript or 默认值` 写法。

- [ ] **Step 1：把硬编码的模块名/全局名/条件表改为读 preset，缺省值保持现状**

```lua
local _DEFAULT_BT_MODULE = "res.scripts.ai.behavior.config.SoldierBT.lua"
local _DEFAULT_BT_GLOBAL = "SoldierBTConfig"

local function _GetBtBinding()
    local cfg = ConfigManager:GetSampleConfig and ConfigManager:GetSampleConfig() or nil
    local bt = cfg ~= nil and cfg.commandBt or nil
    local moduleName = (bt ~= nil and bt.module) or _DEFAULT_BT_MODULE
    local globalName = (bt ~= nil and bt.global) or _DEFAULT_BT_GLOBAL
    local conditions = SoldierConditions
    if bt ~= nil and bt.conditionsGlobal ~= nil and _G[bt.conditionsGlobal] ~= nil then
        conditions = _G[bt.conditionsGlobal]
    end
    return moduleName, globalName, conditions
end
```

读 preset 的确切入口在 Step 2 确认后再定死：若 `ConfigManager` 无 `GetSampleConfig`，改用该文件内已有的 preset 读取方式（与 `_GetChapter9Config` 在 `Chapter9LegacySoldierAgent.lua` 中的写法一致）。

- [ ] **Step 2：先 `grep -n "ConfigManager" bin/res/scripts/agent/BehaviorSoldierAgent.lua` 与 `grep -n "_GetChapter9Config" -A 8 bin/res/scripts/agent/Chapter9LegacySoldierAgent.lua`**，确认本仓读 preset 的既有写法，按其改写 Step 1 的 `_GetBtBinding`，不引入新读取方式。
- [ ] **Step 3：把 `BuildFromModule(_SOLDIER_BT_MODULE, _SOLDIER_BT_GLOBAL, agent, driver, bb, SoldierConditions)` 与 warmup 处的 `BehaviorTreeLoader.Build(SoldierBTConfig, ...)` 改为使用 `_GetBtBinding()` 的三个返回值。**
- [ ] **Step 4：Lua 语法检查** `"D:\Program Files (x86)\Lua\5.1\luac.exe" -p bin\res\scripts\agent\BehaviorSoldierAgent.lua` → 预期无输出
- [ ] **Step 5：跑 smoke** `-Sample Sandbox8 -StopExisting -NoTail` 与 `-Sample Sandbox19` → 预期均 `status=PASS`（证明缺省路径行为不变）
- [ ] **Step 6：commit**（message `[dev]BehaviorSoldierAgent 的行为树模块改为 preset 可覆盖`）

---

## Task 5: Sandbox19 专用指令 BT 与条件

**Files:**
- Create: `bin/res/scripts/ai/behavior/Sandbox19CommandConditions.lua`
- Create: `bin/res/scripts/ai/behavior/config/Sandbox19CommandBT.lua`
- Modify: `bin/res/scripts/config/sample_presets.lua`（`SamplePresets.Sandbox19` 段）

**层归属:** `bin/res/scripts/ai/behavior`
**验证策略:** `Sandbox19` smoke + `Sandbox8` smoke（共享 BT 未被污染的证据）

blackboard 键约定（本 task 定义，Task 6 写入）：`command.kind`（`"focus"`/`"retreat"`/`"rally"`）、`command.focusTargetId`、`command.retreatPos`、`command.rallyPos`、`command.issuedMs`。TTL 8000ms。

- [ ] **Step 1：新建 `Sandbox19CommandConditions.lua`**

```lua
-- Sandbox19CommandConditions.lua
-- 玩家指令条件：只读 command.* 命名空间，不碰 sense.* / formation.*。
-- 与 SoldierConditions 平行；Sandbox19CommandBT 通过 preset 指定使用本表。

Sandbox19CommandConditions = {}

local _COMMAND_TTL_MS = 8000

-- 继承 SoldierConditions 的全部条件（combat subtree 仍要用 HasEnemy / HasAmmo 等）。
setmetatable(Sandbox19CommandConditions, { __index = SoldierConditions })

local function _IsFresh(bb)
    if bb == nil then return false end
    local issuedMs = bb:GetInt("command.issuedMs", 0)
    if issuedMs <= 0 then return false end
    local nowMs = SandboxCamera:GetSimulateTime()
    return (nowMs - issuedMs) <= _COMMAND_TTL_MS
end

local function _IsKind(bb, kind)
    return _IsFresh(bb) and bb:GetString("command.kind", "") == kind
end

function Sandbox19CommandConditions.HasCommandFocus(agent, bb)
    if not _IsKind(bb, "focus") then return false end
    local targetId = bb:GetObjectId("command.focusTargetId", 0)
    if targetId == 0 then return false end
    local target = ObjectManager:getObjectById(targetId)
    return target ~= nil and target:GetHealth() > 0
end

function Sandbox19CommandConditions.HasCommandRetreat(agent, bb)
    return _IsKind(bb, "retreat")
end

function Sandbox19CommandConditions.HasCommandRally(agent, bb)
    return _IsKind(bb, "rally")
end
```

Step 1 前先 `grep -n "GetInt\|GetString\|GetObjectId\|GetVector3" bin/res/scripts/ai/behavior/SoldierConditions.lua` 确认 blackboard getter 的确切签名与默认值参数形式，按实际签名改写上面三处读取；`ObjectManager:getObjectById` 若不存在则改用 `SoldierConditions.HasObjectId` 同款查询方式。

- [ ] **Step 2：新建 `Sandbox19CommandBT.lua`**

```lua
-- Sandbox19CommandBT.lua
-- Sandbox19 指挥切片专用拓扑：玩家指令分支优先于 AI 自主 combat 分支。
-- combat 分支通过 Subtree 引用 SoldierBT，共享拓扑零改动。

require("res.scripts.ai.behavior.config.SoldierBT.lua")

Sandbox19CommandBTConfig = {
    actionDir = SoldierBTConfig.actionDir,
    actions = SoldierBTConfig.actions,
    subtrees = SoldierBTConfig.subtrees,

    root = {
        node = "Selector",
        name = "sandbox19Root",
        children = {
            {
                node = "Selector",
                name = "commandBranch",
                children = {
                    {
                        node = "Sequence",
                        name = "commandFocus",
                        children = {
                            { node = "Condition", condition = "HasCommandFocus" },
                            { node = "Action", action = "pursue" },
                        },
                    },
                    {
                        node = "Sequence",
                        name = "commandRetreat",
                        children = {
                            { node = "Condition", condition = "HasCommandRetreat" },
                            { node = "Action", action = "move" },
                        },
                    },
                    {
                        node = "Sequence",
                        name = "commandRally",
                        children = {
                            { node = "Condition", condition = "HasCommandRally" },
                            { node = "Action", action = "move" },
                        },
                    },
                },
            },
            { node = "Subtree", subtree = "combat" },
        },
    },
}
```

Step 2 前先 `grep -n "root\|Subtree\|subtree" bin/res/scripts/ai/behavior/config/SoldierBT.lua bin/res/scripts/ai/behavior/BehaviorTreeLoader.lua` 确认根节点字段名（`root` 还是别的）与 `Subtree` 节点的确切字段名，按实际写法改写。

`pursue` / `move` action 读的目标键在 Task 6 由指令层写入：`pursue` 读现有的敌人目标键，`move` 读现有的移动目标键——Step 3 据实际 action 实现确定键名。

- [ ] **Step 3：`grep -n "" bin/res/scripts/ai/decision/actions/PursueAction.lua bin/res/scripts/ai/decision/actions/MoveAction.lua`** 读全两个 action，确认它们各自从 blackboard 读哪个键；把键名记录到本 plan 的 Task 6 Step 1，确保指令层写的键与 action 读的键一致。
- [ ] **Step 4：`sample_presets.lua` 的 `SamplePresets.Sandbox19` 段加 BT 绑定**

```lua
SamplePresets.Sandbox19.commandBt = {
    module = "res.scripts.ai.behavior.config.Sandbox19CommandBT.lua",
    global = "Sandbox19CommandBTConfig",
    conditionsGlobal = "Sandbox19CommandConditions",
}
```

- [ ] **Step 5：Lua 语法检查** 三个文件各跑 `luac.exe -p` → 预期无输出
- [ ] **Step 6：跑 smoke** `-Sample Sandbox19 -StopExisting -NoTail`（预期 `status=PASS`，且日志里 BT 能正常构建）与 `-Sample Sandbox8`（预期 `status=PASS`，证明共享 BT 未受影响）
- [ ] **Step 7：commit**（message `[dev]新增 Sandbox19 指令行为树与指令条件`）

---

## Task 6: Sandbox19 指令层（选择集 + 指令语义 + 写入）

**Files:**
- Modify: `bin/res/scripts/samples/Sandbox19.lua`

**层归属:** `bin/res/scripts/samples`
**验证策略:** `Sandbox19` smoke + `Sandbox12` smoke（TeamBlackboard typed fact 链路未退化）

阈值按 spec：点选命中 48 像素、拖拽 8 像素以上算框选、指令 TTL 8000ms、集火兜底扇形 ±45°。

- [ ] **Step 1：加选择集与鼠标状态（文件顶部 local 区）**

```lua
local _selection = {}          -- objId -> true
local _dragging = false
local _dragStart = { x = 0, y = 0 }
local _dragNow = { x = 0, y = 0 }
local _lastPickedEnemyId = 0

local COMMAND = {
    ttlMs = 8000,
    pickRadiusPx = 48,
    dragThresholdPx = 8,
    fallbackFovDeg = 45,
    rallySpacing = 2.5,
    groundY = 0.0,
}
```

- [ ] **Step 2：实现 `EventHandle_Mouse(ctype, x, y, button)`**，替换现有空 stub：`button == 1`（RMB）按下记录 `_dragStart` 并置 `_dragging`；move 更新 `_dragNow`；抬起时按位移是否超过 `dragThresholdPx` 分派 `_PickAt(x, y)` 或 `_BoxSelect(...)`。LMB（`button == 0`）不处理，射击仍由 `PlayerController` 消费。
- [ ] **Step 3：实现 `_PickAt` / `_BoxSelect`**，两者都用 `SandboxCamera:WorldToScreen(agent:GetPosition())`：`_PickAt` 取屏幕距离最小且 `<= pickRadiusPx` 的己方存活 agent（无命中则清空 `_selection`）；若最近的是敌方 agent 则记入 `_lastPickedEnemyId` 供集火使用。`_BoxSelect` 收所有投影落在拖拽矩形内的己方存活 agent。投影返回 `(-1,-1)` 的（相机后方）直接跳过。
- [ ] **Step 4：实现三个指令函数**，均先判 `next(_selection) == nil` → HUD 提示后返回：

```lua
local function _IssueCommand(kind, focusTargetId, targetPos)
    local nowMs = SandboxCamera:GetSimulateTime()
    local teamId = _player:GetTeamId()
    local slotIndex = 0
    for objId in pairs(_selection) do
        local agent = ObjectManager:getObjectById(objId)
        if agent ~= nil and agent:GetHealth() > 0 then
            local bb = agent:GetAIComponent():GetBlackboard()
            -- 三个指令互斥：写入前清另两个键
            bb:SetString("command.kind", kind)
            bb:SetInt("command.issuedMs", nowMs)
            ...
            slotIndex = slotIndex + 1
        end
    end
end
```

Step 4 前先 `grep -n "GetBlackboard\|SetString\|SetInt\|SetVector3\|SetObjectId" bin/res/scripts/ai/AgentComponentAccess.lua bin/res/scripts/samples/Sandbox13.lua` 确认 blackboard 写入 API 的确切名称与取 Blackboard 的既有写法（模块文档要求走 `AgentComponentAccess.lua` / typed component getter，不走 SoldierObject facade），按实际 API 补全上面的 `...` 部分。

同时按 spec 写 typed fact：`TeamBlackboard:RememberFocusTarget` / `RememberRetreatPoint` / `RememberFormationSlot`，参数结构照抄 `Sandbox13.lua:1198-1250` 的现有调用。

- [ ] **Step 5：在 `EventHandle_Keyboard` 里接三个键**：`OIS.KC_F` → 集火（目标取 `_lastPickedEnemyId`，为 0 时取玩家朝向 ±45° 扇形内最近存活敌人，扇形内无敌人则 HUD 提示且不下达）；`OIS.KC_R` → 撤退到 `_player:GetPosition()`；`OIS.KC_G` → 编队，按 `slotIndex` 在玩家周围以 `rallySpacing` 排列。
  > 注意 `R` 当前是换弹键（`infoText` 第 31 行 `LMB: fire    R: reload`），由 `PlayerController` 消费。撤退改用 `OIS.KC_T`，并同步更新 `infoText`，避免与换弹冲突。
- [ ] **Step 6：reload 清理**：在 `_RestartEncounter`（:186-194）里清 `_selection`、`_dragging`、`_lastPickedEnemyId`，并调 `TeamBlackboard:Reset()`。
- [ ] **Step 7：更新 `infoText`** 加三行指令说明与 RMB 选择说明。
- [ ] **Step 8：Lua 语法检查** `luac.exe -p bin\res\scripts\samples\Sandbox19.lua` → 预期无输出
- [ ] **Step 9：跑 smoke** `-Sample Sandbox19 -StopExisting -NoTail` 与 `-Sample Sandbox12` → 预期均 `status=PASS`
- [ ] **Step 10：commit**（message `[dev]Sandbox19 增加选择集与集火撤退编队指令`）

---

## Task 7: 选中高亮与框选矩形

**Files:**
- Modify: `bin/res/scripts/samples/Sandbox19.lua`

**层归属:** `bin/res/scripts/samples`
**验证策略:** `Sandbox19` smoke + 手动确认

全部复用 `UIFrame`（已有 `setPosition` / `setDimension` / `setBackgroundColor` / `setVisible`），不新增 Gorilla 图元。

- [ ] **Step 1：建框选矩形**：`_CreateHud` 里加 `_dragRect = SandboxUI:CreateUIFrame()`，半透明填充 `ColourValue(0.35, 0.75, 1.0, 0.18)`，初始 `setVisible(false)`。
- [ ] **Step 2：建选中高亮池**：按 `RADAR` blip 池的现有写法建 4 个 `UIFrame`（16×16 小方框，`ColourValue(0.3, 1.0, 0.4, 0.55)`），初始全部 `setVisible(false)`。
- [ ] **Step 3：在 `Sandbox_Update` 末尾加 `_UpdateCommandUi()`**：`_dragging` 时按 `_dragStart`/`_dragNow` 摆放并显示 `_dragRect`，否则隐藏；遍历 `_selection` 用 `SandboxCamera:WorldToScreen` 摆放高亮框，投影为 `(-1,-1)` 或超出选中数的池对象 `setVisible(false)`。
- [ ] **Step 4：Lua 语法检查** `luac.exe -p` → 预期无输出
- [ ] **Step 5：跑 smoke** `-Sample Sandbox19 -StopExisting -NoTail` → 预期 `status=PASS`
- [ ] **Step 6：commit**（message `[dev]Sandbox19 增加选中高亮与框选矩形`）

---

## Task 8: 收尾回归与手动验收

**Files:** 无（仅验证）

**层归属:** 全链路
**验证策略:** Release 编译 + 四个 sample smoke + 手动

- [ ] **Step 1：Release x64 全量重编** → 预期 0 错
- [ ] **Step 2：跑四个 smoke**：`Sandbox19`（主）、`Sandbox8`（共享 BT）、`Sandbox12`（TeamBlackboard）、`Sandbox17`（相机未污染）→ 预期全部 `status=PASS`
- [ ] **Step 3：手动跑 `Sandbox19`**，确认 cycle-01 W1 完成标志「能下指令，AI 有可观察反应」：RMB 点选/框选有高亮、F 集火后友军扑向指定敌人、T 撤退后友军回到玩家身边、G 编队后友军在玩家周围排开、8 秒后指令过期自动回到自主战斗、重开无残留。
- [ ] **Step 4：把手动结果记入 `docs/cycle-01.md` 的 W1 行**（阶段 5 doc-sync 一并提交）

---

## 自审

1. **spec → task 映射**：§3 架构表 9 个文件全部出现在 Task 1–7；§5 三个指令语义在 Task 6 Step 5；§6 失败语义在 Task 5 Step 1（TTL/目标失效）与 Task 6 Step 6（reload 清理）；§9 验证策略逐 task 落到 Step。
2. **占位扫描**：无 "TBD/TODO/后续补充/类似 Task N"。Task 4/5/6 中三处 "先 grep 确认既有写法再改写" 是**显式的探查步骤**（给出了确切 grep 命令与判定依据），不是占位——因为这些 API 的确切签名必须以仓库现状为准，写死反而会引入错误。
3. **类型/命名一致**：`command.kind` / `command.focusTargetId` / `command.retreatPos` / `command.rallyPos` / `command.issuedMs` 在 Task 5 定义、Task 6 写入，命名一致；`Sandbox19CommandBTConfig` / `Sandbox19CommandConditions` 在 Task 5 定义、preset 引用一致。
4. **每 task 标注**：8 个 task 均标了层归属 + 验证策略；Task 2 标了 tolua 手术式步骤与禁止全量重生成。
5. **发现并修正**：`R` 键与现有换弹冲突 → 撤退键改为 `T`，并要求同步更新 `infoText`（Task 6 Step 5）。
