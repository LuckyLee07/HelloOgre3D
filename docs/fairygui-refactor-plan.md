# FGUI 代码结构重构落地方案

> 状态：草案 / 待执行
> 适用范围：`src/HelloOgre3D/runtime/ui/fairygui/`、`bin/res/scripts/manager/fairygui/`、`bin/res/scripts/ui/` 及相关 autogen 工具
> 相关文档：[fairygui-final-roadmap.md](fairygui-final-roadmap.md)、[fairygui-production-convergence-todo.md](fairygui-production-convergence-todo.md)、[cpp-object-model-refactor-roadmap.md](cpp-object-model-refactor-roadmap.md)

---

## 背景与诊断

### 现状回顾

FGUI 子系统经过两轮拆分（C++ 侧 `FairyGuiLuaApi*` 拆入 `lua_bridge/`，Lua 侧 `FairyGuiManager` 的功能拆到 8 个子文件），整体架构是合理的。但仍存在以下集中性问题：

| # | 问题 | 位置 | 评级 |
|---|---|---|---|
| 1 | god-class：`FairyGuiManager.lua` 2,517 行 / 352 方法，集中持有全部共享状态 | [bin/res/scripts/manager/fairygui/FairyGuiManager.lua](../bin/res/scripts/manager/fairygui/FairyGuiManager.lua) | P0 |
| 2 | 命名歧义：两个同名 `FairyGuiManager.lua`（shim + 真身） | [bin/res/scripts/manager/FairyGuiManager.lua](../bin/res/scripts/manager/FairyGuiManager.lua) + [bin/res/scripts/manager/fairygui/FairyGuiManager.lua](../bin/res/scripts/manager/fairygui/FairyGuiManager.lua) | P0 |
| 3 | C++ 内部头臃肿：`FairyGuiSystemInternal.h` 1,245 行 / 集中 include 全部第三方头 | [src/HelloOgre3D/runtime/ui/fairygui/FairyGuiSystemInternal.h](../src/HelloOgre3D/runtime/ui/fairygui/FairyGuiSystemInternal.h) | P1 |
| 4 | Lua Services 多职责混杂：1,347 行包含 Toast/Tip/Loading/Mask/MessageBox/Dialog/PopupMenu/Service 框架 | [bin/res/scripts/manager/fairygui/FairyGuiServices.lua](../bin/res/scripts/manager/fairygui/FairyGuiServices.lua) | P1 |
| 5 | 命名风格不统一：`FairyGui` / `fairygui` / `fgui` 三套混用 | 全仓库 | P2 |

### 根因分析

**子模块虽然拆了，但状态没拆**：`FairyGuiManager` 持有所有共享状态（`uiRegistry / views / viewsByHandle / controllers / controllersByHandle / uiStack / currentSceneName`），子模块（Lifecycle / Layers / Lists 等）通过 `owner` 参数回过来直接 mutate 这些表，形成**星型耦合**。

具体证据：

```
FairyGuiManager.lua:126-155    local detachView() 直接 mutate manager.views/viewsByHandle/controllers/controllersByHandle
FairyGuiManager.lua:1416-1425  self.views[key] = autoGen / self.controllersByHandle[handle] = autoGen.ctrl
FairyGuiManager.lua:1498-1499  self.views[key] = view / self.viewsByHandle[handle] = view
FairyGuiManager.lua:2235-2236  self.uiStack 遍历
FairyGuiManager.lua:2284-2332  self.currentSceneName 切换
```

---

## 总体原则

1. **每一步独立可合入、可回滚**，不堆大 PR。
2. **不动外部调用方**：`FairyGuiManager:Method(...)` 这层公共 API 保持不变；改的是它背后的实现归属。
3. **每步用现成的 gate 验证**：`tools/check_fgui_static.ps1` + `tools/run_fgui_selftest.ps1`（参见 [fairygui-production-convergence-todo.md](fairygui-production-convergence-todo.md)）。

## 阶段总览

| 阶段 | 目标 | 工作量 | 风险 |
|---|---|---|---|
| Step 1 | 消除命名歧义：删 shim | 0.5h | 极低 |
| Step 2 | 抽出 `FairyGuiStore`，把 Manager 上的 4 张表 + UI 栈 + scene 名搬过去 | 1d | 中 |
| Step 3 | 把 `detachView` / `_CloseUI` 等 mutating 逻辑搬进 Lifecycle，Manager 只剩门面 | 1d | 中 |
| Step 4 | 拆 `FairyGuiServices.lua` 为 5 个文件 | 1d | 低 |
| Step 5 | C++ 拆 `FairyGuiSystemInternal.h` 为 3 层 | 1d | 低（编译型） |
| Step 6 | 命名/路径规范写进 AGENTS.md | 0.5h | 极低 |

---

## Step 1：消除命名歧义（先做，零风险）

### 现状
- [bin/res/scripts/manager/FairyGuiManager.lua](../bin/res/scripts/manager/FairyGuiManager.lua) 只有 1 行：
  ```lua
  return require("res.scripts.manager.fairygui.FairyGuiManager")
  ```
- 全仓库只有一处用了短路径：[bin/res/scripts/script_init.lua:17](../bin/res/scripts/script_init.lua#L17)

### 动作
1. 把 [script_init.lua:17](../bin/res/scripts/script_init.lua#L17) 改成 `require("res.scripts.manager.fairygui.FairyGuiManager")`
2. 删除 shim 文件 `bin/res/scripts/manager/FairyGuiManager.lua`
3. `tools/check_fgui_static.ps1` 跑一遍

### 验收
- `grep -rn "manager\.FairyGuiManager"`（不带 `.fairygui.`）在全仓库无命中

---

## Step 2：抽出 `FairyGuiStore`（核心一步）

### 问题定位

Manager 看起来"god"，但读完发现大多数方法已经是 `return self:GetX():Y(...)` 的薄包装。真正的"god"性质来自**它持有所有共享状态**，所以子模块全得拿 `manager` 参数回来改它。

### 动作

**新建** [bin/res/scripts/manager/fairygui/FairyGuiStore.lua](../bin/res/scripts/manager/fairygui/FairyGuiStore.lua)：

```lua
local FairyGuiStore = Class("FairyGuiStore")

function FairyGuiStore:Init()
    self.uiRegistry          = {}   -- name -> config
    self.views               = {}   -- key  -> view
    self.viewsByHandle       = {}   -- handle -> view
    self.controllers         = {}   -- key  -> ctrl
    self.controllersByHandle = {}   -- handle -> ctrl
    self.uiStack             = {}   -- 栈：当前打开的 UI
    self.currentSceneName    = nil
    self.strictLifecycle     = nil  -- nil = 跟随 env var
end

-- View 索引
function FairyGuiStore:GetView(keyOrHandle) ... end
function FairyGuiStore:PutView(key, handle, view) ... end
function FairyGuiStore:RemoveView(key, handle) ... end

-- Controller 索引（同上）
function FairyGuiStore:GetController(keyOrHandle) ... end
function FairyGuiStore:PutController(key, handle, ctrl) ... end
function FairyGuiStore:RemoveController(key, handle) ... end

-- Registry / UI Stack / Scene
function FairyGuiStore:RegisterUI(name, config) ... end
function FairyGuiStore:GetUIConfig(name) ... end
function FairyGuiStore:PushUI(entry) ... end
function FairyGuiStore:PopUI() ... end
function FairyGuiStore:ListUIStack() ... end   -- 只读迭代器
function FairyGuiStore:GetCurrentScene() ... end
function FairyGuiStore:SetCurrentScene(name) ... end
```

**改 Manager**：
- `FairyGuiManager:Init()` 里加一行 `self.store = ClassList.FairyGuiStore.new()`，并把 `self.uiRegistry = {}` / `self.views = {}` 等初始化全部删掉
- 提供 `function FairyGuiManager:GetStore() return self.store end`
- Manager 上原有的 `RegisterUI / GetUIConfig / GetView / GetController` 改为 `return self.store:XXX(...)`（外部调用方零感知）

**改子模块**（Lifecycle / Layers / Events / Lists / Controls 等）：
- 构造时它们已经持有 `owner`（manager 引用），所以 `self.owner.views[x] = y` 这种全部改成 `self.owner:GetStore():PutView(...)`
- 可选：让子模块 Init 时缓存一份 `self.store = owner:GetStore()`，调用更短

### 风险点
- `detachView` 局部函数（第 126 行）直接 mutate 4 张表，需要改成 `store:RemoveView / store:RemoveController`
- 子模块共有约 60 处 `manager.views` / `manager.controllers` 形式的访问，需要全 grep 改

### 验收
1. `grep -rn "self\.\(views\|viewsByHandle\|controllers\|controllersByHandle\|uiRegistry\|uiStack\|currentSceneName\)" bin/res/scripts/manager/fairygui/` 只在 `FairyGuiStore.lua` 里命中
2. selftest All + LongLoop 全绿

---

## Step 3：把 mutating 逻辑收口到子模块（解开星型耦合）

完成 Step 2 后，Manager 上还残留几块**"既是状态又是动作"**的代码，需要搬到对应子模块：

| Manager 中的位置 | 内容 | 目标 |
|---|---|---|
| `FairyGuiManager.lua:126-155` | 局部 `detachView` 函数 | `Lifecycle:DetachView(objectInfo, reason)` |
| `FairyGuiManager.lua:1416-1499` | view / controller 注册逻辑 | `Lifecycle:AttachView(...)` |
| `FairyGuiManager.lua:2235` 附近 | uiStack 推 / 弹 / 查 | `Layers:PushTo(...) / PopFrom(...)` |
| `FairyGuiManager.lua:2284-2332` | scene 切换 | `Lifecycle:SwitchScene(...)` |

### 动作

每块单独提交一个小 commit：
1. `[refactor]detachView 迁入 Lifecycle:DetachView`
2. `[refactor]Attach 路径迁入 Lifecycle:AttachView`
3. `[refactor]uiStack 迁入 Layers`
4. `[refactor]scene 切换迁入 Lifecycle`

Manager 对应的方法变成 `return self:GetLifecycle():DetachView(...)`。

### 验收
- [FairyGuiManager.lua](../bin/res/scripts/manager/fairygui/FairyGuiManager.lua) 从 2,517 行降到**目标 < 800 行**
- 不再有任何 `local function` 直接读写 store

---

## Step 4：拆 `FairyGuiServices.lua`

### 现状

1,347 行 31 个公共方法，至少 5 类不同职责混在一起。按职责切分如下：

| 新文件 | 接管的方法 | 行数估计 |
|---|---|---|
| `FairyGuiServiceCore.lua` | `RegisterServiceSkin / GetServiceSkin / ResolveServiceSkin / OpenServiceContainer / Add*ServiceText/Image/Button / ApplyServiceLayout / GetServiceObject / CloseService(s) / HandleServiceClosed / GetServiceStats / DumpServices` | ~600 |
| `FairyGuiToast.lua` | `ClearToastQueue / GetToastQueueCount / CreateToastRequest / OpenToastRequest / ShowNextToast / ShowToast / CloseToast` | ~250 |
| `FairyGuiTip.lua` | `ShowTip / ShowHoverTip / HideTip` | ~150 |
| `FairyGuiLoading.lua` | `ShowLoading / GetLoadingRefCount / HideLoading` | ~120 |
| `FairyGuiOverlay.lua` | `ShowGuideMask / HideGuideMask / ShowMessageBox / ShowDialog / ShowPopupMenu` | ~220 |

### 动作

- 每个子文件都接收同一个 `ServiceCore` 引用（容器创建、皮肤解析、关闭分发都走它），**不直接读 store**
- `FairyGuiServices` 保留作为 **facade**：构造时实例化 5 个子组件，原有 `manager:GetServices():ShowToast(...)` 调用路径不变 → 内部转发到 `Toast:Show(...)`

### 风险点
- [bin/res/assets/fairygui_manifests/](../bin/res/assets/fairygui_manifests/) 里的 skin 注册数据要确认仍然能被 `ServiceCore` 找到（看 `RegisterServiceSkin` 调用方），不要漏迁

### 验收
- 每个新文件 ≤ 400 行
- selftest 里的 toast / loading / mask 用例全绿

---

## Step 5：拆 `FairyGuiSystemInternal.h`（C++）

### 现状

1,245 行：
- 第 5-23 行：fairygui 全家桶 + cocos2d.h
- 第 25-29 行：Ogre / ScriptLuaVM / profiling
- 第 40-47 行：windows.h + imm.h + imm32.lib
- 第 49 行起：整个 `FairyGuiSystemImpl` 类声明（300+ 成员）

### 动作

拆成 3 个文件：

1. **`FairyGuiPlatformIncludes.h`**：吃掉所有第三方头（cocos2d / Ogre / fairygui / windows.h + imm）。子领域 `.cpp` 不直接 include 这些，只 include 这个聚合头。
2. **`FairyGuiSystemImpl.h`**：纯类声明（成员函数 + 字段），include 只放需要前置声明的 + `FairyGuiPlatformIncludes.h`。
3. **`FairyGuiSystemInternal.h`**：保留为兼容门面（`#include "FairyGuiSystemImpl.h"`）或直接删除并改子领域 `.cpp` 的 include。

### 收益
- 编译时间下降（每个 `.cpp` 不再被迫吃全套 Ogre/cocos 头）
- **类定义 vs 平台依赖**的边界清晰

### 验收
1. **clean rebuild 必须做**（参见 `memory/feedback_abi_change_clean_rebuild.md`，ABI / 头文件变化必须清理重编）
2. 单测和 selftest 全绿

---

## Step 6：固化命名 / 路径规范

在 [AGENTS.md](../AGENTS.md) 里加一段 FGUI 章节：

```markdown
## FGUI 命名规约
- C++ 类型/文件：FairyGui*（PascalCase），例：FairyGuiSystem, FairyGuiLuaApi
- C++ 目录：runtime/ui/fairygui/（小写）
- Lua 模块：FairyGui*.lua，require 路径统一 res.scripts.manager.fairygui.*
- 工具/manifest/文档：可用 fgui 缩写（如 fgui_autogen, check_fgui_static.ps1）
- 禁止在新代码中引入 manager.FairyGuiManager 短路径（已删 shim）
```

---

## 时间线建议

| 阶段 | 内容 |
|---|---|
| Week 1 | Step 1 + Step 2（核心）。工程量约 1.5 天，剩余时间做回归 |
| Week 2 | Step 3（4 个小 commit）。解耦最关键的一环 |
| Week 3 | Step 4 + Step 5 + Step 6。收尾 |

**单独可中止点**：Step 1 / Step 2 / Step 5 任一做完就有可见收益，可以随时停下来观察一段时间再决定是否继续。最值得做的是 **Step 1（半小时）+ Step 2（一天）**，做完之后 god-class 问题就基本消解了。

---

## 验收 checklist 汇总

- [ ] Step 1：shim 删除，`script_init.lua` 改路径，static check 通过
- [ ] Step 2：`FairyGuiStore.lua` 落地，子模块共享状态访问全部走 store，selftest All + LongLoop 通过
- [ ] Step 3：`FairyGuiManager.lua` < 800 行，无 `local function` 直接读写 store
- [ ] Step 4：`FairyGuiServices.lua` 拆为 5 文件，每个 ≤ 400 行，service 相关 selftest 通过
- [ ] Step 5：`FairyGuiSystemInternal.h` 拆为 3 层，clean rebuild 通过，selftest 通过
- [ ] Step 6：AGENTS.md 增补 FGUI 命名规约章节
