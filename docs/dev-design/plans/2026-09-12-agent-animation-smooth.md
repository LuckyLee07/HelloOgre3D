# Agent 动作连续性改造

状态：已完成当前 macOS 范围的实现与验证。用户已允许必要架构调整并要求动作顺滑。

## 目标与依据

依据[MiniGame 对照与改造前复现](../specs/2026-09-12-agent-animation-minigame-audit.md)，解决动作取消无效、不同 driver 转向规则不一致、骨骼显示停留在仿真采样帧以及横移步频过快的问题。借鉴 MiniGame 的动作替换、职责分层与显示采样设计，保留既有 FSM/DT/BT、动画通知和 Bullet 真源。

## 已落地行为

- [x] 动作替换：Soldier 控制器使用可中断混合。新请求从当前轨道权重开始，140ms平滑切换；重复保持不重启，死亡80ms覆盖且不可被普通动作取消。旧轨道继续贡献姿态，不触发通知。共用同一片段的逻辑状态按实际轨道去重。
- [x] 水平转向：AgentLocomotion 提供最短角、540°/s上限与18/s响应入口，玩家FOLLOW、FSM和Lua DT/BT共用；8°内视为对准。C++ FSM对准后才请求射击，路径切换不直接跳转身体朝向。
- [x] 逻辑时钟：AgentAnim 自有逻辑时间、权重和启用状态；Ogre临时显示状态不影响 GetTime/GetWeight、通知或扣弹。通知带 PlaybackId，防止替换后继续消费旧动作；FSM只根据真实射击令牌扣弹。Lua射击的600ms预算从射击启动计时，转向等待单独限1000ms。
- [x] 骨骼插值：runtime 的 EntityPoseInterpolation 缓存前后局部骨骼姿态，显示帧只插值位置、缩放和最短弧旋转。仿真前恢复 Ogre 状态，实际仿真步完成后采样；所有相机模式生效。暂停冻结历史，恢复不回跳旧帧。
- [x] 步态连续性：物理后读取实际水平位移，按100ms响应平滑移动、四方向和站蹲权重，保留连续步相。玩家后退最多2.25m/s、横移最多1.6m/s，均尊重更低的 maxSpeed；前向保留原冲刺，零速配置安全。
- [x] 生命周期：身体/武器替换和detach先恢复、释放显示缓存。更换身体保留视觉偏移和变换，消除重建后的悬空；站蹲目标进入动画，物理尺寸仍在过渡完成后提交。视觉偏移与物理姿态共同采样，避免胶囊改变高度时出现显示跳动。
- [x] 验证与文档：Release主目标ABI干净构建、Lua 5.1、动作探针、原始动画图、FSM/DT/BT sample、控制回放及真实窗口抓图，相关模块/索引同步。

## 职责与接口

| 层 | 负责内容 |
|---|---|
| AgentAnim / AgentAnimStateMachine | 仿真轨道、可中断混合、通知；通用RequestState继续保留配置过渡语义 |
| SoldierAnimController | locomotion/action意图、替换策略、死亡优先级与消费状态 |
| AnimComponent / SoldierLocomotionLayer | 物理后步态采样、上下身组合、显示缓存生命周期 |
| runtime/ogre/EntityPoseInterpolation | Ogre借用指针、局部骨骼快照、显示插值及状态恢复 |
| AgentLocomotion | 公共水平转向；保留刚体/渲染真源约束 |
| GameManager / Application | 仿真与显示编排、暂停边界、可选诊断限帧 |

新增 Lua 接口为 `agent:GetLocomotionComponent():FaceDirection(direction, deltaTimeInMillis)`，返回是否基本对准。已同步[头文件](../../../src/HelloOgre3D/sandbox/components/agent/AgentLocomotion.h)、[pkg 的 cfile 引用](../../../src/HelloOgre3D/sandbox/scripting/SandboxToLua.pkg)、[局部绑定](../../../src/HelloOgre3D/sandbox/scripting/SandboxToLua.cpp)与[调用点](../../../bin/res/scripts/agent/AgentUtils.lua)。没有新增Lua所有权、userdata、GC或callback/ref，未运行全量tolua生成器。

## 验证结果

本地证据 `docs/dev-design/specs/assets/agent-animation-smooth-20260912/results.json` 包含代码/程序哈希、三档结果、控制回放与smoke日志位置；原始数据和媒体不入库，新 checkout 可用下方脚本重新生成。最终探针使用的 Release 程序 SHA-256 为 `08f91cd33982362294869227f37532efe1a33e08f78a6584f9e5504eea764227`。自动窗口均后台启动，禁硬件输入与声音；下面的自动输入不等同于人工手感验收。

| 检查 | 结果与证据 |
|---|---|
| 静态 | Lua 5.1.4检查业务2文件和探针；绑定声明/包装/注册/调用点一致；Python编译、文档链接与git diff --check通过 |
| 构建 | macOS arm64 Release；AgentAnim/AnimComponent/ASM等布局变更后主目标120个编译单元干净重编，最后小改动增量构建通过 |
| 动作合同 | 每档21项：取消换弹、替换、取消发弹、死亡覆盖、单次消费、身体/武器重建、站蹲、转向高低目标/±180°、暂停及弹药 |
| 原始动画图 | 原始Sandbox3隔离副本保留Lua驱动和显式过渡，触发跑步、射击、换弹，正常退出且无脚本/Ogre错误 |
| 现有sample | Sandbox19及Sandbox6/7/8各15秒smoke通过；只表示相关启动/运行与自测marker，不表示完整对局或平台全套gate |
| 玩家回归 | 既有9项Sandbox19控制回放通过；180°首发约175.98°，最大仿真转角17.82°，4发子弹，Q键400ms约29.88° |
| 视觉/导航 | 核对真实窗口的横移、站蹲、重建后高度、Sandbox19持枪画面；Sandbox6隔离副本开启既有红色路径和蓝色navmesh绘制并正常退出 |

### 显示频率对比

仿真固定30Hz，每档使用相同输入/动作序列。初始化后的idle相位显式归零，避免启动时首个不完整tick成为对比变量。

| 设置 | 实测中位显示频率 | 动作检查 | 实际发弹/剩余弹药 | 同仿真步内逻辑时钟变化 |
|---|---:|---:|---:|---:|
| 30Hz | 29.97Hz | 21/21 | 4 / 6 | 0 |
| 60Hz | 60.03Hz | 21/21 | 4 / 6 | 0 |
| 120Hz | 120.61Hz | 21/21 | 4 / 6 | 0 |

三档的四发子弹生成位置和方向逐项一致。60/120Hz各有34个移动仿真步观测到多次不同足部姿态采样，逻辑clip时间保持不变，证明骨骼在仿真步之间继续显示变化。取消换弹后再请求射击的通知延迟从改造前受控序列的1551ms降到66ms；这不是硬件输入延迟或所有实战射击的统计值。高低目标转向的身体Y分量为0，33ms最大转角17.82°。

横移、蹲姿、Sandbox19持枪及导航路径/debug画面位于本地 `docs/dev-design/specs/assets/agent-animation-smooth-20260912/`（不入库）。画面不代表新增动画素材，也不替代主观运动体验。

## 复跑

在仓库根目录，先构建匹配的Release程序，再运行：

```bash
python3 tools/tests/run_agent_animation_probe.py --rates 30 60 120
python3 tools/tests/check_agent_animation.py <上一步输出目录>
python3 tools/run_m1_smoke.py --samples Sandbox19 Sandbox6 Sandbox7 Sandbox8 --seconds 15
```

[复跑器](../../../tools/tests/run_agent_animation_probe.py)复制脚本和程序到独立输出目录，仅在那里替换Sandbox3；[探针](../../../tools/tests/agent_animation_probe.lua)通过空DT隔离动作意图，[检查器](../../../tools/tests/check_agent_animation.py)校验逻辑事件、轨迹与显示时钟。可选 `HELLO_RENDER_MAX_FPS=15..240` 供诊断限帧，默认关闭；`HELLO_ANIMATION_POSE_TRACE=1` 输出逐显示帧证据。未改变普通启动的VSync策略。

基线/迭代原始输出仅本地保留，不入库，位于 `tmp/agent-animation-smooth-20260912/`，最终三档在 `final-validation/`。构建为 `build-clock.log` 和 `build-verified.log`，原始动画图为 `legacy-animation/stdout.log`，导航为 `navigation/stdout.log`。导航初次尝试因InputReplay等待玩家对象而超时；改用隔离Lua的仿真计时退出后通过，初次不记PASS。

## 保留边界

- Windows运行、人工持续输入手感及硬件延迟本轮未验；不据此关闭P3完整验收。
- 显示姿态落后逻辑至多约一个仿真步，与实体插值保持一致；没有让渲染帧率驱动游戏逻辑。
- 站立横移仍复用蹲姿侧步素材，外观上保留战术屈膝风格。没有新增站立侧步、足底IK、骨盆步幅校正或脚滑量标定。
- 本轮没有加入受限上身/头部观察偏移；高低目标的身体保持水平。FPS对比验证了发射数量与生成变换一致，未单独统计跨高差目标的命中率。
- 不迁移MiniGame引擎、网络或资源管线；通用动画图与现有sample语义继续保留。
