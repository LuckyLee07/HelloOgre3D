# FairyGUI UI 框架（alias: fgui）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

FairyGUI UI 完整栈：C++ 系统（init/render/input/对象管理）+ Lua bridge + MVC/service 应用框架。

## 2. 源码位置

- `src/HelloOgre3D/runtime/ui/fairygui/`、`bin/res/scripts/ui/`、`bin/res/scripts/manager/fairygui/`

## 3. 关键文件

| 路径 | 角色 |
|---|---|
| `runtime/ui/fairygui/FairyGuiSystem.*` | C++ 核心门面（cocoslite 内嵌）：LoadPackage/CreateObjectHandle/Inject*/Update/Render |
| `runtime/ui/fairygui/lua_bridge/FairyGuiLuaApi*.cpp` | 手工 C++/Lua glue（对象/包/输入/事件/诊断 API） |
| `bin/res/scripts/manager/fairygui/FairyGuiManager.lua` | 服务管理器单例 |
| `bin/res/scripts/manager/fairygui/FairyGuiService*.lua` | service 模式：Toast/MessageBox/Loading/Tip/PopupMenu/GuideMask |
| `bin/res/scripts/ui/FairyGuiBase*.lua` | MVC view/ctrl/model 基类 + AutoGen |

## 4. 公开能力要点

- 包加载、对象 handle 创建与属性、输入注入、事件订阅、列表/控制器；Lua 侧 service 栈 + MVC + AutoGen。

## 5. 约束与红线

- lua_bridge 是**手工 glue（非 tolua）**，新增功能 C++/Lua 两端同步。
- handle 生命周期需显式销毁（销毁父容器不自动清子）；二次销毁崩溃。
- 输入 `Inject*` 返回 bool 表是否被 FGUI 拦截，主循环据此决定是否透传游戏逻辑。
- cocoslite 内嵌渲染，改版本需完整回归 FGUI gate。

## 6. 数据流 / 与其他模块关系

`FairyGuiSystem.Initialize → LoadPackage`；`Lua FairyGuiManager → lua_bridge → FairyGuiSystem`；输入经 Inject*；属 [[runtime]] 适配层。

## 7. 验证策略

- gate：`tools/run_fgui_production_gate.ps1 -Mode Full -StopExisting`（含 Fast/All/LongLoop/Pressure + 静态检查）。
- 自测：`HELLO_FGUI_SELF_TEST_ALL=1`。

## 8. 已知 gap / 相关文档

- 待：lua_bridge 自动生成、handle 池化、输入路由文档、element inspector。`docs/fgui/fairygui-production-gate.md`、`docs/fgui/fairygui-final-roadmap.md`、`docs/planning/long-term-iteration-plan.md` §5。
