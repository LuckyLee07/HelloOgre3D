# Lua 绑定（alias: scripting-tolua）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

C++↔Lua 绑定：tolua++ 生成导出、手工钩子捕获 Lua 回调、对象级脚本环境隔离。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/scripting/`、生成脚本 `src/HelloOgre3D/tolua.bat`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `SandboxToLua.pkg` | 输入 | `$cfile` 列举导出头（ObjectManager/SandboxMgr/AgentObject/AIController/DecisionTree/BehaviorTree…） |
| `SandboxToLua.cpp` | 生成物 | tolua++ 产物（**勿手编、除非手术式改单个绑定函数**） |
| `ManualToLua.cpp` | 手工钩子 | `setPluginEnv`/`SetEvaluator`/`SetCondition` 捕获 Lua function ref |
| `LuaPluginMgr.h` | 插件 | `BindLuaPluginByFile` 对象级隔离 env |

## 4. 公开能力要点

- tolua 自动绑定 + 手工钩子（Lua closure 捕获）+ 动态脚本加载与环境隔离。

## 5. 约束与红线

- **改导出 API：优先手术式改 `SandboxToLua.cpp` 对应绑定函数 + 头文件签名；禁 `tolua.bat` 全量重生成**——曾致 Sandbox18 崩溃（0xC0000409 栈溢出）。
- 改绑定必须跑用到它的 sample（影响图绑定改动跑 `Sandbox17` **和** `Sandbox18`；DT/BT 回调跑 `Sandbox7/8`）。
- **P2/P5 导出收窄（2026-06-19）**：`SoldierObject` 不再向 Lua 导出 `getWeapon`/AI/maxHealth/ammo/敌人查询/移动目标/射击/Enter*Anim 纯组件转发；`AIController`、`DecisionTreeDriver`、`BehaviorTreeDriver`、`LuaDecisionAction`、`LuaBehaviorAction` 向 Lua 暴露泛化 `GetAgentOwner()` 而非 Soldier `GetOwner()`；DT/BT Lua action 生命周期回调已统一传 `u[AgentObject]`；脚本改走 `GetAIComponent()` / `GetAttribComponent()` / `GetWeaponComponent()` / `GetAnimComponent()` 或 `AgentComponentAccess.lua`，避免新脚本继续依赖对象层样板。
- Lua function ref（evaluator/condition）须在对象销毁/reload 前清理，防悬空。

## 6. 数据流 / 与其他模块关系

编译期 `.pkg →(tolua++)→ .cpp`；运行期 `tolua_*_open` 注册 + `ManualToLua` 钩子。导出几乎所有 [[systems-manager]] [[objects]] [[ai-*]] 类给 [[ai-scripts]] [[samples]]。

## 7. 验证策略

- 改 .pkg/绑定后：编译 + 跑 `Sandbox7/8/17/18`（覆盖 evaluator/condition/影响图绑定）。

## 8. 已知 gap / 相关文档

- 待：导出清单治理、回调生命周期清理流程。`docs/design/architecture-improvement-plan.md` P1、`AGENTS.md`（tolua.bat 说明）。
