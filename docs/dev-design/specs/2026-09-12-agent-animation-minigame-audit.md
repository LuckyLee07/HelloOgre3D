# Agent 动作与 MiniGame 实现对照

状态：改造前的分析记录。分析阶段仅新增记录与隔离探针；用户随后授权的实现见[动作连续性改造](../plans/2026-09-12-agent-animation-smooth.md)。MiniGame 只读审阅，没有启动其客户端。

## 结论

当前项目已有 locomotion/action 意图、动画通知、下身步态层和物理姿态插值，基础方向可保留。仍存在动作取消后旧过渡继续执行、Agent 转向规则不一致及高低目标导致刚体倾斜的问题；骨骼采样时钟和横移资源也限制表现质量。上一轮玩家视线/转向修复没有覆盖整个 AI 动作链。

优先借鉴 MiniGame 的职责划分、动作替换规则和逐显示帧采样。无需移植其引擎、网络状态或重新设计 FSM/DT/BT。

## MiniGame 实际链路

参考根：`/Users/lizi/Desktop/Workspace/MiniGame`；客户端 `Miniw-Client`，`ActorBody.cpp` 的 `#ifndef IWORLD_SERVER_BUILD` 分支。MiniGame 版本为 `de20a4e97359de5823c7739daed8e5da08fed898`，本项目分析基线为 `114b6b10664a510e172f84e282876efad56ce85f`；源码哈希和原始测量位于本地 `docs/dev-design/specs/assets/minigame-agent-animation-20260912/results.json`（不入库）。

| 环节 | 实际实现 | 对本项目的价值 |
|---|---|---|
| 移动与操作 | `iworld/player/PlayerStateController.cpp:69` 同时更新 MovementFSM 和 ActionFSM，走路与用枪不必挤进一个组合状态枚举。 | 保留现有两类意图，让表现层明确处理能否同时发生及中断优先级。 |
| 全身与上身 | `iworld/actors/ActorBody.cpp:1454` 分别计算 animBody、animUpBody，最后调用 setCurAnim 的0/1层；死亡/骑乘等全身状态另行裁决。 | 将基础移动、上身操作、全身覆盖和观察修饰明确分工；不再靠新增大量 run+fire 组合状态。 |
| 幂等与混合 | `ActorBody.cpp:1425` 只在状态改变时播放；`OgreMain/OgreAnimationPlayer.cpp:114` 根据配置 priority、loopmode、replay 管理轨道，新同优先级轨道让旧轨道进入淡出。默认淡入/淡出为0.2秒，支持覆盖。 | 动作请求需区分保持、重启、替换、取消；已有幂等选状态可以保留，补上替换/中断合同。 |
| 骨骼权重 | `OgreAnimationPlayer.cpp:292` 按优先级排序；`OgreMain/OgreSkeleton.cpp:129` 逐骨骼叠加，高优先级未用完的权重可由低优先级补足。`bin/res/entity/animmap.csv:59` 起的第三人称持枪/射击/换弹优先级为2，普通站立/行走为0。 | 参考明确的层级与骨骼覆盖规则，不要求复制这套自定义 Ogre API。 |
| 逻辑与显示 | `iworld/actors/ClientActor.cpp:225` 的 tick 推进角色逻辑与动作选择；`:407` 的 update(dtime) 调用身体 update；`ActorBody.cpp:2377` 使用 framePosition 并更新 Entity，后者继续推进动画轨道。 | 逻辑事件保持仿真时钟，显示姿态可逐渲染帧求值；避免提高渲染帧率后重复发弹。 |
| 身体与观察 | `ActorBody.cpp:1267` 控制头部 yaw/pitch，`:1309` 维护 render yaw、头身夹角和静止后的身体跟随；`:2417` 给实体设置水平身体朝向。 | 身体保持直立并限速转向；头/脊柱承担有限瞄准偏移，超过范围再转身体。 |
| 用枪时序 | `iworld/player/GunUseState.cpp:168` 按 FireInterval 判断发射，调用 GunLogical::fireOnce 后播放枪动画/声音；`GunLogical.cpp:223` 扣弹并发起 useItem。 | 借鉴武器节奏、重播策略与表现的明确分工，不把它误说成“所有枪弹都由骨骼通知驱动”。本项目现有 shoot_fire 通知仍可作为发弹真源。 |

MiniGame 也不是完整的现代动画方案：上述路径没有可直接搬用的足底锁定/步长标定；`ActorBody::updatePlayAnim` 虽计算 animspeed，当前函数没有把这个局部值提交给播放轨道；`lerpRotateTo` 使用固定 tick 及直接角度插值，不能照抄来替代本项目已有最短角、按时间转向。

## 当前项目的已确认问题

### 1. 取消动作没有取消正在进入的旧状态

`AnimComponent::EnterIdleIntent()` 清除 action intent，但 `SoldierAnimController::ApplyBodyState` 发现 idle 仍是当前状态便直接返回。底层 ASM 的 next 仍可能是 reload。`AgentAnimStateMachine::RequestState` 也只在 next 为空时更新它；新的 desired 会排在旧过渡之后。

隔离运行实际记录：

| 仿真时间 | 调用/结果 |
|---|---|
| 330ms | 请求 Reload |
| 363ms | 当前 idle、next=reload；调用 EnterIdleIntent 取消 |
| 660ms | 已经进入 reload，next=idle；旧动作仍在播放 |
| 990ms | 请求 Shoot |
| 1617ms | 射击请求已超过600ms，仍是 reload |
| 2541ms | 才产生 shoot_fire，距请求1551ms |

这是受控中断序列，不代表每次正常射击都有1.55秒延迟。它说明“逻辑动作已取消”和“动画过渡已取消”没有同一语义。Lua ShootAction 又有从初始化起算的600ms预算，存在表现尚未就绪便被预算终止的组合风险；本次没有把该 Lua 完整序列作为单独复现项。

参考源码：[意图清理](../../../src/HelloOgre3D/sandbox/components/anim/AnimComponent.cpp)、[控制器](../../../src/HelloOgre3D/sandbox/objects/animation/SoldierAnimController.cpp)、[过渡队列](../../../src/HelloOgre3D/sandbox/objects/animation/AgentAnimStateMachine.cpp)、[Lua 射击](../../../bin/res/scripts/ai/decision/actions/ShootAction.lua)。

建议先定义动作优先级和取消策略：死亡强制覆盖；允许取消的动作应撤销旧 next 并从当前混合姿态退出；不能取消的动作应显式拒绝/排队。不要让 current/next/desired 的偶然组合决定操作效果。动作实例与通知消费可沿现有事件体系标识，避免旧动作的通知误归新请求。

### 2. C++ FSM 的 Agent 会瞬时转身，还会朝高低目标倾斜整个刚体

`AgentActionContext::FaceEnemy()` 把 enemy.position-agent.position 原样传给 AgentObject::SetForward，无转速上限，也没有清除Y。后者最终写入 PhysicsComponent 的 Bullet 方向。导航 ApplySteering 也直接把身体朝向设成新水平速度。

探针直接调用相同下游 SetForward 方法：方向(0,3,10)得到身体 forward=(0,0.287348,0.957826)，俯仰16.699°。这验证了刚体倾斜；没有模拟敌人感知和寻路全过程。地形高差、跳起或碰撞顶起的目标均可能让该调用路径出现非水平输入，日常发生频率未统计。

与此同时，Lua DT/BT 的 Soldier_FaceDirection 已压平Y并限速，但为12rad/s，约688°/s；玩家则为18/s指数响应、540°/s上限。因此当前“玩家转向平滑”不能推出所有 Agent 也相同。

参考源码：[FSM 瞄准](../../../src/HelloOgre3D/sandbox/ai/fsm/AgentActionContext.cpp)、[导航朝向](../../../src/HelloOgre3D/sandbox/ai/fsm/AgentStateController.cpp)、[物理朝向](../../../src/HelloOgre3D/sandbox/components/physics/PhysicsComponent.cpp)、[Lua 转向](../../../bin/res/scripts/agent/AgentUtils.lua)。

建议用现有 AgentLocomotion/控制设施统一水平身体转向策略，按秒推进并取最短角；需要高低瞄准时由受限的上身/头部偏移承担，不把观察俯仰写进整个胶囊刚体。

## 表现质量缺口

### 3. 位置插值已经有了，骨骼还按仿真步采样

默认仿真30Hz。AnimComponent::update 推进 ASM 和 SoldierLocomotionLayer；GameManager::RenderPresentation 只插值实体位置/朝向、重算武器挂点和更新相机，没有插值或重采样骨骼。重复求值 skeleton 的同一个 AnimationState 时间，不等于产生新的动作帧。

源码可确认该限制，本轮未用逐帧骨骼抓取量化观感误差。它会使显示帧率升高后，移动位置更连续，但腿、手和枪口动作仍受约33ms的采样间隔限制。Sandbox6/7/8 的非 FOLLOW 视图还不进入这条姿态插值路径。

建议保留一次仿真更新/一次通知，增加只读的显示姿态采样或骨骼姿态插值。显示求值不能改写用于通知判断的动作时间；暂停、body替换和退出须清理该显示状态。

参考源码：[默认仿真频率](../../../src/HelloOgre3D/game/ClientManager.cpp)、[动画更新](../../../src/HelloOgre3D/sandbox/components/anim/AnimComponent.cpp)、[显示更新](../../../src/HelloOgre3D/game/GameManager.cpp)。

### 4. 站立横移是资源复用，步态混合仍较生硬

SoldierLocomotionLayer 已解决“移动射击只动上身、腿停住”和侧步髋部带偏枪向等问题；不应退回全身状态互斥。但当前资源没有站立左右侧步，站立横移复用 crouch_left/right_aim。横移权重、方向正负和步长立即随速度变化，没有专门的起停或方向过渡策略。

现有参考步长：站立前后2.25m/周期，侧步1.375m/周期。以3m/s横移举例，1.375秒的侧步片段会按约3倍原速度播放；若相同基速再乘玩家1.75倍冲刺，约为5.25倍。这里是公式推导，不是本轮量测的每个 Agent 实际速度。单纯降低播放速度又会放大脚滑，需同时调整资源、位移速度或步幅。

此外，动画层在 ObjectManager 更新期间读取速度，Bullet 碰撞解算在后；这并不等同于已经测得本步实际位移。贴墙、急停和碰撞时的足底一致性仍需专门验证。

建议先校准前后/侧向片段的参考速度与相位，补站立侧步或限制其速度；对移动权重和方向切换加短过渡。再评估是否需要足底锁定。不要把足底IK作为修复动作取消、转向和采样时钟的前提。

参考源码：[步态层](../../../src/HelloOgre3D/sandbox/objects/animation/SoldierLocomotionLayer.h)、[玩家速度](../../../src/HelloOgre3D/sandbox/components/control/PlayerController.cpp)、[更新次序](../../../src/HelloOgre3D/game/GameManager.cpp)。

## 建议实施顺序与验证边界

1. 修动作取消/替换合同及 Agent 水平转向，统一 FSM/DT/BT/玩家的公共方向规则。验证：取消换弹、移动转射击、连续切目标、背后目标、跨±180°、高低目标、死亡抢占。
2. 分离仿真动作时间与显示骨骼姿态。验证：30/60/120Hz显示下对比手脚与枪口连续性；同一仿真序列的发弹次数、弹药、命中与暂停恢复保持一致。
3. 校准移动资源和步频，补站立侧步或限速，处理方向混合和起停。验证：A/D互换、斜向转正向、冲刺急停、贴墙、移动换弹与射击；量化足底滑移后再决定IK。
4. 在以上基础上增加受限上身瞄准和头部观察，让角色有明确的注视对象。保留刚体真源，视觉偏移不写回碰撞体。

分析阶段未修改 C++、Lua 业务或绑定，未运行 tolua，未重建游戏。使用上一轮已验证的 Release 程序副本，正常退出且无 Lua/Ogre 错误；隔离目录禁硬件输入。MiniGame 的分析为源码证据，不声称进行了其手感或性能实测。

[改造前基线探针](../../../tools/tests/agent_animation_baseline_probe.lua)可在隔离 bin/res 中替换 Sandbox3.lua，使用对应分析基线的 Release 副本、`HELLO_SANDBOX_SAMPLE=Sandbox3`、`HELLO_WINDOW_BACKGROUND=1`、`HELLO_AUDIO_SILENT=1`、`HELLO_ANIMATION_TRACE=1`启动。它用空 DT 避免默认AI干扰，调用既有动画意图与物理接口，3300ms主动退出。不要替换正式 sample。完整本地日志（不入库）：`tmp/agent-animation-audit-20260912/runtime.log`；初版探针受默认AI/退出接口错误影响，已废弃，不作为上述证据。
