# 游戏对象（alias: objects）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

游戏对象层：Agent/Soldier/Block 对象 + OpenSteer 转向/路径适配，承载运动、朝向、转向力、路径跟踪。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/objects/`（动画子系统单列 [[objects-anim]]）

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `AgentObject.{h,cpp}` | 基础对象 | ~788 行，含 150+ 转发样板 |
| `SoldierObject.{h,cpp}` | 战斗特化 | ~651 行，绑武器/动画/AI；`Update` 手写组件次序 |
| `BlockObject.{h,cpp}` | 静态块 | 刚体+渲染双组件，双重持有 |
| `steer/OpenSteerAdapter.{h,cpp}` | 转向 | OpenSteer AbstractVehicle 适配；部分基向量方法是空桩 |
| `steer/AgentPath.{h,cpp}` | 路径 | PolylinePathway 包装 |

## 4. 公开能力要点

- 位置/朝向/速度、转向力（ForceToPosition/AvoidAgents/Wander…）、路径设置与跟踪。

## 5. 约束与红线

- **位置真源**（AGENTS.md）：有有效刚体 → PhysicsComponent 权威、RenderComponent 同步；无刚体 → RenderComponent 权威。对象层只触发同步，不手写 Bullet→SceneNode；视觉偏移用 `SetVisualOffset`。
- **P2**：150+ forwarder 样板原样保留；新增对象类型勿复制，新代码勿加单组件转发。
- **P4**：BlockObject 既 AddComponent 又存裸成员（双重持有）；容器裸指针 + SAFE_DELETE。
- OpenSteerAdapter 多个转向辅助方法是空桩（疑未用）。

## 6. 数据流 / 与其他模块关系

对象持 [[components]]；`SoldierObject.Update`：AI→anim→render→weapon 次序（[[objects-anim]] [[systems-physics]]）；服务经 `GetSandboxServices`（[[core-object]]）。

## 7. 验证策略

- 回归 sample：`Sandbox6/7/8`（战斗）；对象创建销毁看 `buildObjectDebugSummary`。

## 8. 已知 gap / 相关文档

- 待：P2 forwarder 减负、P4 unique_ptr。`docs/design/architecture-improvement-plan.md` P2/P4、`docs/design/cpp-object-model-refactor-roadmap.md`、`docs/archive/SoldierObjectRefactor.md`。
