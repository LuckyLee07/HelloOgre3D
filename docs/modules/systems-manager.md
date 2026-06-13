# 系统管理枢纽（alias: systems-manager）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

ObjectManager（对象注册/更新/AI 调度/导航/战术 facade）与 SandboxMgr（场景/相机/UI 门面）。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/systems/manager/`

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `ObjectManager.{h,cpp}` | 枢纽 | registry + Update 循环 + AIScheduler/Perception/Team/Tactical facade + navmesh + 战术调试绘制；持 SandboxServices owner |
| `SandboxMgr.{h,cpp}` | 门面 | 相机/光照/场景/CreateAgent/CallFile；部分纯转发 ObjectFactory/service |

## 4. 公开能力要点

- 对象枚举/过滤/计数、AI 调度配置、战术/团队/影响图 Lua facade、各 `build*DebugSummary`。

## 5. 约束与红线

- **C3**：ObjectManager 职责过载（registry+update+调度+输入+navmesh+战术绘制揉一起），待拆薄服务。
- 战术调试绘制（`rebuildTacticalInfluenceLayerDebugVisual`/`configureTacticalInfluenceFromNavMesh`）逻辑待下沉 [[ai-tactics]] service。
- **C2**：SandboxMgr 部分"门面转发门面"。
- 改导出给 Lua 的方法走**手术式** tolua（[[scripting-tolua]]），勿全量重生成。

## 6. 数据流 / 与其他模块关系

`GameManager 填 SandboxServices → ObjectManager`；每帧驱动 AIScheduler/Perception/Team/Influence；Lua 经 tolua 调其 facade。关联几乎所有 AI/对象模块。

## 7. 验证策略

- 回归 sample：`Sandbox6`(调度/导航)、`Sandbox12`(team)、`Sandbox18`(影响图)。

## 8. 已知 gap / 相关文档

- 待：C3 拆服务（ObjectRegistry/UpdateSystem/TacticalDebugDraw）、C2 SandboxMgr 减肥、P1 g_* 清零。`docs/design/architecture-improvement-plan.md` C2/C3/P1、`docs/planning/long-term-iteration-plan.md`。
