# chapter-8 感知 vs Sandbox11 逐帧对拍：速度已修 + 感知分岔天花板（双铁证）

类型：project

**目标**：让 Sandbox11（modern，`chapter8_perception_legacy_parity` preset + `Chapter9LegacySoldierAgent.lua`）复刻 HelloOgre3DX `chapter_8_perception` 的 agent 逐帧路径，**同时保持 modern 独立架构**。对拍工具 `tools/run_chapter8_parity_gate.ps1 -ModernPreset chapter8_perception_legacy_parity`，指标 maxPosError（当前 14.286，agent#3 主导）。

**结论（2026-07-12 收尾）**：根因链已 100% 查清，落到两个具体机制（都有实测铁证），**不是当初错判的"混沌天花板"**。但**在保持独立感知架构的前提下，逐帧 100% 复刻不可达**——因为根因②（感知可见性判定）在两套独立实现间不可逐位对齐，这一点有 agent#2/#3 双点交叉铁证。速度根因已修，前 7 帧逐帧对齐。

## 根因① 速度：capsule 禁转导致滑动摩擦 3× 衰减【已修】
- modern 三处 factory（AgentFactory/SoldierFactory/PhysicsComponent::RebuildCapsule）对 agent capsule 设 `setAngularFactor(0,0,0)`（禁刚体旋转让士兵直立不倒——**必要，去掉会翻滚**）；legacy `PhysicsUtilities::CreateCapsule` 无此设置。
- 物理后果：modern 禁转 → capsule 只能"滑" → 主导**默认线性摩擦 0.5**，每帧多吃 ~0.24 水平速度；legacy 允许转 → **rolling friction 0.2 主导** → 每帧只吃 ~0.082。**modern 衰减是 legacy 的 3 倍**，即那 5% 速度差。
- **修复**：`PhysicsFactory::CreateRigidBodyCapsule` 给 capsule 加 `m_friction = 0.15f` 补偿 + 去掉 `movementProfile.forceScale` fudge（parity preset 改回 1.0）。修复后 **agent#3 前 7 帧逐帧对齐（posDist<0.2）**。
- 诊断法（A3DIAG）：埋点比两边每帧 `setVel`（Lua 算的，物理前）vs `speed`（GetSpeed，物理后）——modern 每帧掉 0.24、legacy 掉 0.082，直接暴露物理衰减差；逐个排除 dt(33ms)/mass(90.7)/maxForce(1000)/opensteer 公式后落到 setAngularFactor。

## 根因② 感知可见性分岔：多 agent 全局发散，不可逐位对齐【查清·天花板】
**误差不是 agent#3 一个人的事，是全局多体发散**（compare 实测）：agent#2 从 s3 最早发散（1.17→8.37 单调增长）、agent#4 s4、agent#5 s7、agent#1/#3 在 s8 集体转折，s8 后多体交互加速放大到 14.286。agent#3 只是放大最猛的，**不是源头**。

机制：每个 agent"首次锁定哪个敌人"取决于**感知可见性判定**（spawn/首帧朝向、b_Head1 骨骼视野方向、45°视锥 dot 临界、遮挡、射线）。任一 agent 与 legacy 看见的敌人集不同 → 目标选择分岔 → 寻路分道 → 位置发散。**双铁证**：

- **agent#3 → id118（modern "多"看见）**：legacy 侧 legvis.log 实测——agent#3 射线打在一个 y=-0.52 的**顶起关卡 box** 上被挡，`see=0`。legacy 关卡 box 是动态刚体（mass=1）被基础几何顶起成挡视线的墙；modern 静态 box（mass=0）沉在 y=-1.555 不挡 → modern 一直"看得见"id118 死追，legacy 记忆过期（age>1000）切 id120。
- **agent#2 → id119（modern "少"看见）**：modern 侧 visdbg116 实测——id116 可见敌人集 `{115(d45), 117(d47)}`，**不含最近的 119(d27)**（119 在巡逻朝向 +x+z 的正后方 -x，落在 45°视锥外）；legacy id116 首帧视野朝 -x 侧看见 119 并锁定。`_ChooseBestEnemy` 纯 nearest，在**能看见**的敌人里正确选了最近的 115 → 逻辑无误、随机点已对齐，唯一差异是"首帧视野朝向决定看见谁"。

两个 agent 方向对称（一个 modern 多看见、一个少看见），指向同一天花板。

**已排除的非根因**（这次坐实）：
- 随机数序列：`legacyRandomMovePoints` 已把每个 agent 首帧 randomMove 点注入对齐（preset 配置 = legacy trace 逐字匹配，如 id116=`[27.334,4.011]`）；agent#2 巡逻点相同仍分岔 → 分岔纯在感知层，与随机数无关。
- 选择逻辑：`_ChooseBestEnemy` 纯 nearest（`distanceSq < bestDistanceSq`），正确执行（选可见集中最近者）。

## 天花板结构与唯一完整路径
- 感知可见性判定依赖 spawn/骨骼朝向、视锥临界、遮挡、射线——两套独立实现**不可逐位一致**，且是临界点问题（敌人恰在视锥/遮挡边界），对微小差异极敏感。
- 唯一能完整压住感知分岔的是**逐帧注入每个 agent 的目标敌人**（从 legacy trace 提取），但那从"独立架构复刻"滑向"回放 legacy 决策录像"，违背保架构目标。
- 因此：**保架构 → 逐帧 100% 复刻不可达；根因链已全清**。若未来仍要推进 agent#2，唯一修复路径是对齐 id116 spawn/首帧朝向让 modern 也看见 id119——但临界脆弱、只治一个、后续切换仍分岔。

## 已固化的真实修复（勿删）
- `PhysicsFactory::CreateRigidBodyCapsule` `m_friction = 0.15f`（根因①）。
- `sample_presets.lua` chapter8 movementProfile `forceScale = 1.0`（去 fudge；L629 的 1.08 是别的 preset，勿动）。
- `sample_presets.lua` `legacyRandomMovePoints` 首帧巡逻注入（排除随机数分岔）。
- `global.lua` `CreateLevelBox` `SetMass(0)`（关卡 box 静态，稳定几何）。
- parity 诊断埋点（modern visdbg116/118、legacy legvis/boxdump/_LevelBoxDump）已全部清理，回归确认 maxPosError 不变。

## 证伪的假设（别再走）
- ❌「混沌敏感天花板/浮点不可消除」——根因①是**错判**（禁转滑动摩擦，系统性可修）；根因②是**真·结构性天花板**（感知不可逐位对齐），有双铁证，性质不同于浮点噪声。系统性稳定偏差绝不是浮点末位噪声。
- ❌「只是 agent#3 的孤立 id118 遮挡」——错，是全局多体发散，agent#2 从 s3 更早、机制同源。
- ❌「随机数序列不一致导致分岔」——已注入对齐，agent#2 巡逻点相同仍分岔。
- ❌「两套不同 Bullet 库/版本差」——两边都 `BT_BULLET_VERSION 281`。
- ❌「物理 timestep/substep 差」——主因是 friction；回退单步后 friction 0.15 仍让前 7 帧对齐。
- ❌「头骨 forward 算法不同」——modern `GetBoneForward` 已用 `legacyRotation*(0,0,-1)` + 欧拉往返对齐。
- ❌「C++ VisionSensor 粘滞/pursue 每帧重选」——legacy_parity 是纯 Lua 驱动（C++ 感知旁跑）；pursue 每帧重选会卡死 agent。

相关：[[recent-progress-snapshot]]、[[build-release-for-runtime]]、[[parity-gate-python]]。
