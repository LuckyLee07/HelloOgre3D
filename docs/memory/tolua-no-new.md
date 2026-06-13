---
name: tolua-no-new
description: 项目 tolua 不暴露 ClassName.new()；Lua 端造对象走 functor 或引擎 Create*/New* 工厂
metadata:
  type: feedback
---

本仓 tolua 模式（`//tolua_exports`、`//tolua_begin/end`）**不生成 `ClassName.new(...)` 构造函数**。Lua 端任何 `XXX.new(...)` 都报 `attempt to call field 'new' (a nil value)`。

**Why:** SandboxToLua.cpp 里所有 `tolua_cclass` 都没注册 `new`。值类型用 functor 调用 `rcConfig()` / `Vector3(x,y,z)`；引用类型一律由 C++ 工厂返回 raw 指针、C++ 持所有权。曾把 chapter_6 的 `LuaDecisionAction.new()` / `DecisionBranch.new()` 模式照搬，7 个 agent 全部 Agent_Initialize 报错。

**How to apply:** 给 Lua 新增可创建的 C++ 类型时，参照 `bin/res/scripts/samples/Sandbox6.lua` 的两类 idiom：
1. **值类型/配置体**：让 tolua 自动 functor `Foo()` / `Foo(args)`，不写 `.new`。
2. **生命周期由引擎管理的对象**：在已暴露的单例/agent/driver 上挂 `Create*`/`New*` 工厂，C++ 持所有权（容器 + 析构 delete），返回 raw 指针。参考 `DecisionTreeDriver` 的 `NewTree/NewBranch/NewLuaAction`（driver 持 `m_ownedNodes`，析构统一 delete）。

排查信号：stderr `attempt to call field 'new' (a nil value)` → 看现有 `Sandbox*.lua` 怎么造对象。改导出 API 的 tolua 注意见 [[validation-loop]]。
