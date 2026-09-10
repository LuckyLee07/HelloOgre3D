# 行为树执行引擎（alias: ai-behavior）

> `/hello-develop-design` 阶段 0 注入；改源码后经 doc-sync 更新。

## 1. 职责

响应式行为树运行时：组合/装饰/动作节点 + reactive 重评估，由 BehaviorTreeDriver 驱动 Lua 叶与 C++ 条件。

## 2. 源码位置

- `src/HelloOgre3D/sandbox/ai/behavior/`（Lua 树配置在 `bin/res/scripts/ai/behavior/`，见 [[ai-scripts]]）
- Sandbox19 专用树：[Sandbox19CommandBT.lua](../../bin/res/scripts/ai/behavior/config/Sandbox19CommandBT.lua)、[条件表](../../bin/res/scripts/ai/behavior/Sandbox19CommandConditions.lua)；命令生命周期由 [sandbox19_commands.lua](../../bin/res/scripts/samples/sandbox19_commands.lua) 编排。

## 3. 关键类 / 文件

| 文件 | 角色 | 说明 |
|---|---|---|
| `BehaviorNode.h` | 基类 | 3 态 RUNNING/SUCCESS/FAILURE + Tick/Reset |
| `BehaviorComposite.{h,cpp}` | 组合 | Sequence/Selector/**Parallel**/**RandomSelector**；`ConfigureReevaluation(reevaluateMs)` 反应式重评估 |
| `BehaviorDecorator.h` | 装饰 | Inverter/ForceSuccess/ForceFailure |
| `BehaviorAction.h` / `LuaBehaviorAction.h` | 动作 | OnInitialize/OnUpdate/OnCleanUp；Lua 回调叶 |
| `LuaCondition.h` | 条件 | Lua evaluator；可选 result cache，按 Blackboard 全局 revision 或依赖 key revision 失效 |
| `BehaviorTreeDriver.{h,cpp}` | 驱动 | IDecisionDriver；持所有节点所有权 + `NewXxx` 工厂（tolua 不暴露构造器）；当前 driver 内 rebuild storage pool 复用可 reset 节点/树 |
| `BehaviorTrace.h` | 调试 | label/status 逐帧 |

## 4. 公开能力要点

- 节点：Sequence/Selector/Parallel/RandomSelector/Inverter/ForceSuccess/ForceFailure/Wait/LuaAction/Condition。
- **G1（重评估）、G4（Parallel/Random）已完成**（2026-06）。
- `BehaviorTreeLoader` 支持 `subtrees` + `Subtree` 引用节点、`cacheMs` / `resultCacheMs` 和 `cacheKeys` / `dependsOn` / `dirtyKeys`；`SoldierBT` 已把通用 combat branch 抽成 subtree，并给 Hearing/Danger、Team、Memory、Formation 等纯 Blackboard 条件接入第一批缓存。
- `BehaviorTreeLoader.BuildFromModule(...)` / `ReloadModule(...)` 已提供显式 Lua config 模块热重载第一段：重建整棵树、保留 driver/Blackboard 运行配置，旧 LuaAction/LuaCondition 继续走 driver 延迟释放策略。

## 5. 约束与红线

- 节点所有权全归 driver；Lua 只经工厂拿指针，勿自管生命周期。
- 性能项：`BehaviorTreeDriver` 已支持 trace sampling（`HELLO_BT_TRACE_SAMPLE_INTERVAL` / `SetDebugTraceSampleInterval`）、runtime tick LOD（`HELLO_BT_TICK_INTERVAL_MS` / `HELLO_BT_TICK_STAGGER` 或 preset `behaviorTree.tickIntervalMs` / `tickStagger`）、基于 perception / sense / knowledge / memory / move target 距离的 distance LOD（`HELLO_BT_DISTANCE_LOD_NEAR` / `HELLO_BT_DISTANCE_LOD_FAR` / `HELLO_BT_DISTANCE_LOD_MAX_MULTIPLIER` 或 preset `behaviorTree.distanceLodNear` / `distanceLodFar` / `distanceLodMaxMultiplier`）、每帧树 tick budget（`HELLO_BT_MAX_TREE_TICKS_PER_FRAME` 或 preset `behaviorTree.maxTreeTicksPerFrame`）、LuaCondition result cache、显式模块热重载，以及 driver-local rebuild storage pool（复用 Sequence/Selector/Parallel/Random/Wait/Decorator/BehaviorTree；LuaAction/LuaCondition 因 Lua env/ref 生命周期延迟到下一次 C++ tick 释放并按 build 重建）；RuntimeDiag 输出 `[BTStats] ticks/treeTicks/tickSkipped/budgetSkipped/tickIntervalMs/effectiveTickIntervalMs/distanceLodMultiplier/distanceLodSkipped/budgetMax/cacheHits/invalidated/storageResets/nodeReuses/treeReuses/retiredLuaActions/retiredLuaConditions`。仍缺跨 agent/template 级 node cache。
- 开放：G2 事件节点（依赖统一事件总线）、G3 参数运行时求值；G5 子树复用和显式热重载第一段已落地，文件 watcher、运行中节点状态迁移和跨 agent/template 级缓存仍待后续。
- **每 sample 换树**：`BehaviorSoldierAgent` 的 BT 绑定从 `preset.behaviorTree` 读
  `module` / `global` / `conditionsGlobal`，缺省仍是 `SoldierBT` + `SoldierConditions`。
  中继站版本的 `Sandbox19CommandBT` 是独立限定范围守卫树：存活检查后依次尝试集火、撤回、集结、自主局部交战、返回守卫点与 idle，死亡走 die；响应式重评估间隔 150ms。
  它复用 `SoldierBT` 的 actions/actionDir 与既有 shoot/pursue/reload/move，不再挂载整棵 `SoldierBTConfig.tree`。
  旧三波版本继承的低血量回避、声音调查、编队等整树分支不能再视为当前 Sandbox19 行为；共享章节树未据此裁剪。
  `Sandbox19CommandConditions` 仍用 `__index` 继承共享条件；自主敌人筛选限制在守卫锚点 14 米范围，撤回到达后收紧为 6 米；离锚点超过 2 米时申请返岗移动。
  ⚠️ `conditionsGlobal` 查 `_G` 失败会**静默回落**到 `SoldierConditions`，条件表模块必须在 agent 建树前 require。
  ⚠️ `HasCommandFocus` 是**带副作用的条件**（命中时覆写 `blackboard.enemy`，用来压过 `AIController::WritePerceptionResult`
  每 tick 的改写，从而复用既有 shoot/pursue 而不新造动作），因此**绝不能包进 `CachedCondition`**——缓存跳过求值会丢副作用。
- Sandbox19 的 `HasCommandFocus` 先通过执行者 `AIController:CanSeeEnemy(id)` 再解析存活对象，不能用 `blackboard.enemy` 或小队别人的感知代替该执行者可见性；`CanShootEnemy` 解析仍存活的对象指针，并要求 10 米内且当前可见。射击/追击的共享动作继续负责实际执行。
- `HasCommand*` 条件在真实 BT 命中时才写 `command.status=executing`；命令模块接收时只写 accepted。条件与相关交战组合保持 150ms 重评估，测试不得直接调用这些条件来制造执行证据。Sandbox19 设置 `pursue.reach=9`、`sandbox19.aimedFire=true` 与 `weapon.actionOwnsFire=true`：ShootAction 在原耗弹时机调用 `WeaponComponent:ShootBulletAt(enemy:GetPosition())`，从真实 muzzle 产生需要 Bullet 碰撞的子弹；SoldierObject 动画发射回调跳过第二次发射。未启用开关的共享章节保持原射击路径。
- 语义命令由 `sandbox19_commands.lua` 写入 `command.semantic/kind/issuedMs/status/targetPos/focusTargetId` 并维护 accepted/executing 与唯一终态；BT 条件不再按统一 8 秒 TTL 结束命令。集火目标死亡完成、视野丢失失败，移动到达完成、路径失败/10 秒无进展失败；替换、取消与结算/重开时的命令清理由命令模块负责。撤回到达留下 `sandbox19.holdPos` 与锚点，结束的是移动命令，局部守卫持续至新命令或取消。

## 6. 数据流 / 与其他模块关系

`AIController.SetDriverByType("bt") → BehaviorTreeDriver`；树由 Lua `BehaviorTreeLoader` 装配；每帧 `driver.Tick → root.Tick`，叶读写 [[ai-common]] Blackboard；条件缓存通过 Blackboard revision / key revision 判定失效，避免跳过 RUNNING action。

## 7. 验证策略

- 回归 sample：`Sandbox8`（BT 战斗兵，与 DT `Sandbox7` 行为应等价）；BT tick/distance LOD / condition cache / subtree smoke 用 `bt_runtime_lod` preset（`Sandbox10` + RuntimeDiag）确认 `tickSkipped`、`distanceLodMultiplier`、`distanceLodSkipped` 和 `cacheHits`；BT frame budget smoke 用 `bt_runtime_budget` preset 确认 `budgetSkipped`；BT rebuild storage pool smoke 用 `bt_runtime_rebuild` preset 确认 `storageResets/nodeReuses/treeReuses`；BT 显式模块热重载 smoke 用 `bt_hot_reload` preset 确认 `[BTHotReloadSelfTest] PASS` 和 `treeBuilds/storageResets`。
- Sandbox19 的命令/守卫分支使用 `sandbox19_product_selftest.lua`，关注 `[Sandbox19ProductSelfTest]` 的真实导航到达、撤回后守卫、可见目标执行、无统一 8 秒过期及生命周期 marker；fixture 的挪动/清敌是显式合成步骤，不能当作普通对局或玩家输入证据。旧 Command/Intent/M1 自测 marker 不再是当前 sample 的验收合同。
- 2026-09-10 Windows Release 全量重编、Lua 语法及强化产品 fixture 完整通过，见[本机 gate 摘要](../../tmp/relay-product-fixture-20260910-140218-noxicg2_/summary.json)。该轮等待真实 BT 执行，覆盖替换/取消幂等、目标丢失/死亡、owner 死亡、活动命令重开与统计守恒。内部输入回放完成 69.267 秒真实战斗通关及部署策略比较；人工外部输入与 macOS 尚未验证，共享章节回归和最终记录由 [[samples]] 与[本轮验收](../playtest-relay-2026-09-10.md)汇总。

## 8. 已知 gap / 相关文档

- `docs/design/behavior-tree-gap-analysis.md`（G1/G4 已完成、G5/G7 第一切片已落地，G2/G3/G6/G8–G11 开放）、`docs/archive/ai-technical-iteration-plan.md`。
