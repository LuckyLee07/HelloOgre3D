# 对象核心基础（alias: core-object）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

对象/组件运行时基础：对象根、组件容器、事件总线、服务注入聚合体。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/core/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `object/BaseObject.{h,cpp}` | 对象根 | 组件容器 `std::map<string,IComponent*>` + objid/teamid + `SetSandboxServices` |
| `object/SandboxObject.{h,cpp}` | 基类 | 虚 Update |
| `component/IComponent.{h,cpp}` | 组件接口 | onAttach/onDetach/update + `GetSandboxServices()` |
| `SandboxServices.h` | 注入聚合 | `{objects, physics, sandbox, script}`（P1 核心） |
| `event/SandboxEventDispatcher*.h` | 事件 | name→dispatcher / Subscribe/Emit/Token |
| `event/SandboxContext.h` | payload | 事件上下文（勿与 SandboxServices 撞名） |
| `SandboxMacros.h` | 宏 | SAFE_DELETE |

## 4. 公开能力要点

- 对象生命周期 + 组件容器（AddComponent/FindComponent<T>/GetComponent(key)）+ 服务注入 + 事件总线。

## 5. 约束与红线

- **P4**：组件容器裸指针 + SAFE_DELETE（待 unique_ptr）；BlockObject 双重持有。
- **P1（部分）**：SandboxServices 已落地下发，但仍 15 文件 `#include GameManager`、`g_*` 未清零、静态门禁未加；新代码禁直接 `g_*`。
- 事件总线目前同步直发，无缓冲队列/节流。

## 6. 数据流 / 与其他模块关系

`ObjectManager 填 SandboxServices → BaseObject.SetSandboxServices → IComponent.GetSandboxServices`；对象创建链（[[systems-service]]）透传。被所有对象/组件依赖。

## 7. 验证策略

- 回归 sample：`Sandbox6`；对象创建销毁无泄漏/野指针（`buildObjectDebugSummary`）。

## 8. 已知 gap / 相关文档

- 待：P1 收尾（g_* 清零+门禁）、P4 unique_ptr。`docs/design/architecture-improvement-plan.md` P1/P4、`docs/design/cpp-object-model-refactor-roadmap.md`。
