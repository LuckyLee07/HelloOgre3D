# Cycle 01：战术场景收口与 AI 行为观察

> 2026-09-05 按个人项目定位调整。方向与详细里程碑见 [project-direction.md](project-direction.md)，任务勾选只在 [backlog.md](backlog.md)。
> 方向文档修订后已执行 M1，最新实现与验证见文末“M1 实施与缺口审计”；早期运行证据保留其原日期。外部试玩、五分钟时长与最终品类决定不再是完成前提。

## 当前追加任务：Sandbox19 稳定性（2026-09-06）

用户已认领：先定位 NaN 来源，补普通完整对局与重开验证，再考虑镜头/波间体验和 M2。本轮保持已有开阔场地与 M1 改动。

范围：普通/probe 模式分开复现、可复现的转向数值缺陷、诊断对象清理、完整对局和三波胜利生命周期回归。历史长局 NaN 只有确认同源或新复现证据后才关闭；阶段验收不使用 30 秒 smoke 代替完整对局。

当前状态：重合分离力 NaN 与 RuntimeDiag probe 清理已修复；完整稳定性尚未收口。普通三局与合成三波胜利/重开通过，但 probe 长局出现双方低血量持续回避，300 秒仍未终局。下一步围绕该僵持的场景规则收口；历史 NaN 同源性仍未证明。详细实现、失败与通过证据见 [稳定性记录](stability-2026-09-06.md)，M1 历史结论保持。

## M1 阶段定义（已完成）

| 项目 | 内容 |
|---|---|
| 问题 | 能否在现有战术场景里解释一个士兵从感知、记忆、指令到行动的完整过程？ |
| 最小产物 | Sandbox19 的单单位行为观察闭环，以及自主接敌、集火、目标丢失、指令过期四种过程的证据记录。 |
| 当前依据 | M1 已完成：缺口审计、观察面板、生命周期维护、跨帧自测和 macOS Release 相关回归，证据见文末。 |
| 完成条件 | 四种过程可观察且依据正确；死亡/重开清理正确；相关自测与 sample 回归有本轮证据。 |
| 复盘条件 | 本阶段验收已完成；下一次开发先认领 M2 的最小实验记录，不自动扩大本轮范围。 |
| 后续入口 | M1 完成后再认领 M2 的最小实验记录；团队对照实验 M3 留在候选池。 |

## 执行步骤与验收

1. 运行/读取现有诊断，核对 M1 六类信息的来源。列出“已有可用、表达不清、实际缺失”，避免重复建设。
2. 先补影响解释的最小缺口，优先复用 RuntimeDiag、BT trace 和现有 UI。新增 API 必须经过绑定链验证。
3. 逐个执行自主接敌、集火、目标丢失、指令过期；每例记配置、观察事实、判断依据与未知项。
4. 验证目标死亡和重开，不保留旧选中对象、指令或诊断历史引用。
5. 按实际改动选择构建、自测和 sample；记录平台与构建配置。手感和画面检查可由作者完成，自动化不代替主观体验判断。
6. 更新本页结论与 backlog，决定结束本阶段、缩小问题或暂停。

本阶段不做完整输入回放、通用调试编辑器、导航预算、多 agent 优化或全量架构清理。具体数据语义、开销边界和验收见方向文档 M1。

## 旧周期计划的处理

原周期计划为 2026-08-04 至 2026-08-31：指令输入、意图可视化、三波节奏与外部试玩。仓库保留了代码与自动化、macOS 窗口 QA 记录，未记录外部试玩及最终品类结论。

处理结论：已有战术切片作为个人综合实验成果保留；外部体验保持未知，不再阻塞后续工作；品类保持开放。下方历史记录中的 W4、真人试玩、五分钟验收等属于当时口径，不覆盖本页新的完成条件。这里没有将原计划追溯标记为全部完成。

## 历史实施与运行证据（原记录保留）

### W2/W3 实施合同（2026-08-13）

上轮方向讨论中的“小队命令 → AI 可解释 → 一局闭环”按当前 cycle 收敛为以下交付，不新增平行 AI system：

- **意图卡片**：每个存活友军头顶持续显示 `FOCUS / RETREAT / RALLY / ENGAGE / SEARCH / PATROL` 等玩家语义，以及“玩家指令 / 自主感知 / 最后已知位置”等原因。
- **目标表达**：集火目标有红色屏幕标记；撤退/编队目标点有对应颜色的落点标记；雷达 blip 同步采用当前意图颜色。
- **指令生命周期**：TTL 过期、集火目标死亡和重开时清理 `command.*` 与指令专用 `movePos`，HUD 显示活跃指令数量和剩余时间。
- **指挥官身份**：玩家仍可用第三人称移动和观察战场，但改用无武器 `commander_soldier` profile，不能直接开枪；战斗结果必须来自 AI 小队执行。
- **一局节奏**：部署准备 → 三波交火 → 波间整备 → 最终胜负；固定 seed、固定出生模板并记录阶段转换日志，保证同一 build 的基础场景可重复。
- **验证**：在既有 `[Sandbox19CommandSelfTest]` 外补意图和 match-flow 自测，再跑 Release x64 与 `Sandbox19 / Sandbox8 / Sandbox12 / Sandbox17` smoke；视觉可读性仍须真人桌面验收。

上轮同时讨论的通用 `Pause / Resume / Reset`、driver 身份、寻路预算/排队和完整输入录制回放不塞进本应用 cycle；它们已回到 `backlog.md`，等技术 cycle 再按真实压力认领。

### W2/W3 代码完成记录（2026-08-13）

W2/W3 已提前完成代码落地：`Sandbox19` 现为无武器 commander 指挥两名 AI 友军，
以头顶 action/reason 卡片、目标标记、雷达意图色和 HUD 指令计数表达 AI 状态；
对局按 `PREPARE → WAVE × 3 → INTERMISSION → VICTORY/DEFEAT` 推进，固定 seed/出生点，波次参数收口在 `sample_presets.lua`。
指令会在 TTL、目标失效、波次结束和重开时清理，且只移除由指令拥有的 `movePos`。

自动化证据：Release x64 构建 0 warning / 0 error；
`[Sandbox19CommandSelfTest] PASS`、`[Sandbox19IntentSelfTest] PASS`、`[Sandbox19MatchSelfTest] PASS`；
`Sandbox19` / `Sandbox8` / `Sandbox12` / `Sandbox17` smoke 均为 `status=PASS`。

W2/W3 的“代码与自动化”已完成，但周完成标志仍不提前勾选：意图卡是否在真实战斗中可读、
三波节奏是否能支撑 5 分钟，以及玩家是否理解自己只能下令，必须由 W4 真人桌面试玩回答。

### macOS 独立工程 QA（2026-08-16）

已在 Apple Silicon / macOS 真实窗口完成 Release arm64 构建、启动和三局闭环验证，并补充
`tools/package_macos_playtest.sh`：脚本生成可双击启动的 `HelloOgre3D.app`、运行资源目录和 zip，
launcher 会先把工作目录切到随包 `bin/` 再启动游戏；产物是内部 ad-hoc 签名试玩包，不是公证发行包。

真实窗口检查确认：HUD 多行文本完整可读；友军意图卡会避开 HUD 并在互相遮挡时向上排布；
`F` 无目标、`T` 撤退、`G` 编队、右键清空选择和无选择下指令均有即时反馈；指令计数、TTL、
目标/落点标记、雷达意图色、胜负提示与 Enter 重开可见且生效。三局日志终局如下：

| 局 | 自动化行为 | 结果 | 日志终局时间 |
|---|---|---|---:|
| 1 | 撤退指令路径 | 第 2 波战败 | 145.695 秒 |
| 2 | 编队指令路径 | 第 1 波战败 | 156.486 秒 |
| 3 | 不下指令的基线 | 第 3 波胜利 | 204.369 秒 |

QA 同时复现了“单位脱离接触后波次永久停住”的节奏阻塞。当前 sample 先在 20 秒无击杀时把双方
`movePos` 指向最近对手；再次超时后把失联敌人用有限、固定偏移拉回最近对手周围。第一版曾直接使用
导航路径点并触发 Ogre `Node::setPosition` 的 NaN 断言，已删除该路径并改为有限值检查；改后完成上述
三局，未再出现 fatal Lua 错误或进程 abort。这个强制接触是 cycle 内的关卡导演兜底，不是通用 AI 能力；
W4 应根据真人观察决定保留它，还是改出生点/场地布局从根上减少失联。

最终包的关闭回归还抓到一处 macOS re-entrant teardown：Cocoa 会在 OIS 键盘 `capture()` 内同步派发
窗口关闭事件，旧逻辑在该回调里销毁 OIS，返回后继续访问已删除的键盘对象并触发 `EXC_BAD_ACCESS`。
现改为只在关闭回调中请求停止渲染，把输入析构延后到 `ClientManager` 正常析构，并在 capture 返回后
立即结束当前帧。Release 重建后再次从包内启动、点击关闭，进程正常退出且没有新增 crash report。

仍不能据此勾选完成定义：三局静止指挥官基线为约 2.4–3.4 分钟，尚未证明真人单局能支撑 5 分钟；
持续按住 `W/A/S/D` 的移动手感、右键拖框以及首次玩家是否理解意图仍须真人桌面验收。macOS OpenGL
运行日志还存在 Ogre 基础 shader 参数 warning，但本轮没有因此中断关卡。

### W1 完成记录（2026-08-04）

经 `/hello-develop-design` 六阶段流程落地，spec / plan 见
`dev-design/specs/2026-08-04-sandbox19-command-input-design.md`、
`dev-design/plans/2026-08-04-sandbox19-command-input.md`。

W1 当时的操作是 `LMB` 射击 · `RMB` 点选/框选友军 · `F` 集火 · `T` 撤退 · `G` 编队；
W2/W3 已按本 cycle 的原始问题改成无武器 commander，`LMB` 射击不再存在。
（撤退用 `T` 而非原定 `R`——`R` 是 `PlayerController` 的换弹键。）

自动化证据：`[Sandbox19CommandSelfTest] PASS`（指令命中 / 三指令互斥 /
集火对 `blackboard.enemy` 的覆写生效 / TTL 过期后落回自主战斗）+
`Sandbox19` `Sandbox8` `Sandbox12` `Sandbox17` 四个 smoke 全 `status=PASS`。

实施中修掉两个真实缺陷（都是 selftest 抓出来的，不是设计阶段预见的）：
`ObjectManager:getObjectById` 未导出给 Lua；指令 TTL 判定在仿真时间为 0 时失效。

**仍欠**：真人手感验收——headless smoke 驱动不了鼠标键盘，"能下指令、AI 有可观察反应"
这一条 W1 完成标志只有自动化侧证据，手感必须真跑一局才算数。

**W2 是这个 cycle 真正的技术价值**，也是最能搬回生产工作的部分：把 AI 内部状态做成玩家可读的表达。这是所有战术类游戏最难的一环，而这个项目恰好已经有全套可观测数据（`[AIRuntimeDiag]`、BT trace、InfluenceMap debug draw、TeamBlackboard fact）——现在只是把它们从"给我看"改成"给玩家看"。做成了，无论最后做不做游戏，都是能写出来、能讲的东西。

## 开工前复盘记录（2026-09-05，M1 进展见文末）

- 已有成果：按历史记录，战术指令、意图表达、三波对局与 macOS 打包/窗口运行已经形成可用基础。
- 尚未知：作者当前主观体验、完整行为因果链的诊断覆盖、不同指挥策略的稳定收益、陌生人理解程度。
- 风险：关卡导演强制接触会干预自然 AI 行为，后续 M2/M3 实验须隔离或记录；历史三局结果不足以证明某种指令更优或更差。
- 下一步：只认领 M1 的观察缺口审计与最小补齐。未执行的内容保持待办，不把本次方案落盘计为实现完成。
- 暂停恢复入口：阅读本页和方向文档 M1，检查当前代码与工作区，先验证 Sandbox19 和已有诊断是否可用，再继续缺口清单。

## M1 实施与缺口审计（2026-09-05）

### 数据来源与实际改动

| 信息 | 原有能力 / 缺口 | M1 处理 |
|---|---|---|
| 身份 | 对象 id、组件、BT driver 可读；没有通用 profile getter | 场景按实际 spawn 参数记录 profile，并明确标为 spawn；选中多个单位时稳定观察最小 id |
| 感知 | `perception.hasTarget/targetId` 已有 | 显示最近感知结果，明确不是重新扫描；观察代码不调用会改状态的 HasEnemy/CanShootEnemy |
| 记忆 | MemoryStore 已写本地视觉 snapshot、时间、位置；团队支持有独立键 | 分别显示本地视觉记忆与团队支持 fact；不把团队信息当作直接看见，不据 fact 存在宣称实际消费 |
| 指令 | `command.*` 已有；清理混在意图显示函数 | 提取只读有效性读取，生命周期由 `_MaintainCommands` 维护；补最后清理原因、死亡与失效选择清理 |
| 决策 | `__bt.currentAction/currentActionStatus` 已有，trace 默认关闭 | 仅对当前观察单位启用 trace，离开/关闭后恢复原设置；显示最后动作、真实采样分支和 BB 决策目标 |
| 执行 | 速度、弹药、动画、路径点数、目标均可读；没有最近寻路请求结果 | MoveHelpers 在 Sandbox19 记录最后请求 success/failure 与仿真时间；明确“最后请求”，不推断当前是否被挡住 |

新增 `bin/res/scripts/samples/ai_observer.lua`：只读 snapshot、有限频率采样与格式化。默认 200ms 采样一个选中单位，不保存 agent/Blackboard userdata；决策目标的裸指针只与 live agent 列表比较，匹配后读取 id。按 `I` 关闭时恢复原 trace 配置并清空 snapshot，`O` 按需导出全文和 BT trace。

`Sandbox19.lua` 接入右侧观察面板、spawn profile、指令维护和重开清理。头顶卡保留玩家语义，右侧面板显示实际运行证据；两者不同不能据此判为 AI 错误。例如有敌人记忆并不意味着当前动作一定是搜索。卡片避让观察面板。

实际运行发现客户端可能在场景初始化前调用 `Sandbox_Update`，已补初始化保护，避免新增面板在首帧为空时报 Lua 错误。此项由新 Release smoke 捕获并修复。

### 可重复验证入口

在 macOS 可创建图形窗口的桌面会话中，从仓库根目录执行：

```bash
python3 tools/run_m1_smoke.py
```

脚本顺序启动 Sandbox19/8/12/17，每个默认观察 30 秒，隔离继承的 `HELLO_*` 配置，检查必要 marker、Lua/断言错误和提前退出，日志写 `tmp/m1-smoke-<时间>/`。只终止自己创建的子进程；限时结束不算正常关闭验证。可用 `--samples Sandbox19` 单独重跑。

M1 自测入口是 `HELLO_SANDBOX19_OBSERVATION_SELF_TEST=1`，由 `ai_observer_selftest.lua` 跨真实仿真帧驱动。普通游玩不启用该测试，不会自动下令、移动测试敌人或杀死测试单位。自测修改的场景在末尾重开。

| 用例 | 检查依据 |
|---|---|
| 自主接敌 | 真实感知有目标、实际 pursue/shoot、无玩家指令、有效 BT trace |
| 玩家集火 | trace 进入 commandFocus，指令目标有效；没有靠再次调用条件来伪造执行 |
| 丢失视野 | 测试将敌人移出视距，下一批真实 tick 后无直接目标、有本地记忆且执行 moveToLastKnownEnemy |
| 指令过期 | 另发撤退并等待完整 8000ms TTL 后确认指令被清理；丢失视野阶段另有提前过期的布置操作 |
| 目标死亡 | 集火后测试目标死亡，原因记录为 focus-target-invalid |
| 单位死亡 | 受令单位死亡，清命令与选择，观察快照不再指向它 |
| 重开 | 旧 id 不再存在，观察对象为新 id；自测装配后指令为空 |
| 只读采集 | 每次 Capture 前后 Blackboard revision 一致；不重跑感知或行为条件 |

### 验证结果与边界

Release macOS 构建已成功；vendored 依赖存在编译 warning，未宣称零 warning。真实窗口已经确认面板文字可读、I 开关、O 导出、Enter 后新身份；窗口关闭后对应测试应用进程消失。

最终限时回归：

| 验证 | 结果 | 本地证据 |
|---|---|---|
| macOS Release 构建 | BUILD SUCCEEDED | `tmp/m1-build.log` |
| Sandbox19，30 秒 | PASS，M1 全部跨帧用例及原 Command/Intent/Match 自测通过，无捕获到的 Lua/断言错误 | `tmp/m1-smoke-20260905-104609/Sandbox19.log` |
| Sandbox8，30 秒 | PASS，场景初始化和错误扫描通过 | `tmp/m1-smoke-20260905-104310/Sandbox8.log` |
| Sandbox12，30 秒 | PASS，TeamBlackboardSmoke / LifecycleSelfTest | `tmp/m1-smoke-20260905-104310/Sandbox12.log` |
| Sandbox17，30 秒 | PASS，Chapter9TacticsSmoke | `tmp/m1-smoke-20260905-104310/Sandbox17.log` |
| Lua 语法、Python 编译、git diff --check | PASS | 本轮命令输出 |

首轮新 Release 的 Sandbox19 记录包含初始化错误，已修复并仅复跑该场景；上表引用的是修复后的结果，不覆盖失败记录。

**阶段结论：M1 完成，M2 保留为下一候选。** 已能关联直接感知/本地记忆、有效指令、真实 BT 动作/分支与执行状态，并保留无法证明的原因。本次没有 Windows 执行环境验证，不宣称跨平台运行全通过。

首次未修改代码的基线运行启用了 `HELLO_RUNTIME_DIAGNOSTIC_SELF_TEST=1`（会生成额外 probe），约 194535ms 的第二波出现 Ogre `Node::setPosition` NaN 断言。证据为本地 `tmp/m1-baseline.log`；尚未定位根因，不能推断普通场景必现，也不能以本轮短 smoke 排除长期稳定性问题。M1 后续自测未启用该 probe 模式。

剩余边界：BT 与感知是采样结果，面板标明时间/trace；共享 fact 的来源可见不等于共享信息已被行为消费（M3 验证）；最近寻路结果不能证明当前路径仍有效；无法从既有信息确定的动作原因显示 unknown。这些边界保留在文档中，不编造因果关系。
