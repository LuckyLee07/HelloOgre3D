---
name: lua-missing-exports-and-clock
description: Lua 侧两个反直觉事实：ObjectManager:getObjectById 没导出（按 id 找对象要扫 getAllAgents）；getTimeInMillis 是启动至今的仿真时间且初始化期可能为 0
metadata:
  type: project
---

写 Lua sample / BT 条件时会踩的两个坑，**光看 C++ 头文件判断不出来**，两个都是 2026-08-04 做 Sandbox19 指令层时实测踩到的。

## 1. `ObjectManager:getObjectById` 未导出给 Lua

`ObjectManager.h` 里**有** `BaseObject* getObjectById(int objid);` 的声明，但它**不在 `//tolua_begin` / `//tolua_end` 块内**，所以 Lua 侧调用会在运行时报：

```
attempt to call method 'getObjectById' (a nil value)
```

**判据**：光 grep 到声明不算导出，必须确认它落在 tolua 块内（`grep -n "tolua_begin" -A N <header>`），或在 `SandboxToLua.cpp` 里搜到对应的 `tolua_function(tolua_S,"<名字>",...)` 注册行。

**替代写法**：按 id 找对象只能扫 `ObjectManager:getAllAgents()` 比对 `GetObjId()`。sample 规模（个位数 agent）代价可忽略。

**附带好处**：这样只在选择集里存 objId、每次现扫，避免跨帧持有 agent userdata——对象被销毁后那个 userdata 就是悬垂的，而 Lua 侧不会有任何提示。

## 2. `GameManager:getTimeInMillis()` 是仿真时间，不是墙钟

实现就是 `return m_SimulationTime;`——**启动至今的仿真毫秒**，不是 epoch。因此：

- **sample 初始化期间它可能是 `0`**（`Sandbox_Initialize` → `_SpawnEncounter` 里读到的就是 0）。
- 拿它做时间戳时，**不要用 `value > 0` 当"有没有值"的判据**——0 是合法时刻。有没有值应该看 `blackboard:Has(key)`。
- TTL 判定写成 `0 <= (now - issuedMs) <= ttl`；只写 `<= ttl` 会让"未来时间戳"（now 比 issuedMs 小，如 reload 后时钟回退）被误判成新鲜。

**踩坑现场**：指令 TTL 条件在 smoke selftest 里同时出现"刚下达的指令判为过期"和"故意设成过期的指令判为新鲜"两个相反症状，根因就是这一条。

相关：[[validation-loop]]、[[run-and-diagnose]]。
