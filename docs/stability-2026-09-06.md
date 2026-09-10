# Sandbox19 稳定性：2026-09-06

> 后续：本页发现的低血量僵持已通过 Sandbox19 有限自主回避规则修复，新增九局验证见 [僵持修复](stalemate-2026-09-06.md)。下文保留当时的通过、失败与未完成结论；历史 NaN 同源性仍未证明。

## 修复与因果边界

确认并修复了可独立复现的分离力 NaN：OpenSteer `steerForSeparation` 对重合邻居执行 `offset / -distanceSquared`，得到 0/0。`AgentLocomotion::ForceToSeparate` 现在在本项目适配层保留邻域判定与普通距离下的 1/d 权重；小于 1mm 时限制权重，完全重合时按对象 id 给出相反且可重复的水平力。没有修改 vendored OpenSteer，也没有用最后有效坐标掩盖错误。

真实 Lua → 绑定 → C++ 路径的重合用例在旧二进制失败；新 Release 的 separated/coincident/repeatable/near-coincident/outside-neighborhood 五项均通过。普通运行不启用测试；`HELLO_LOCOMOTION_SELF_TEST=1` 只在初始化时暂移已有两名友军，完成后恢复位置与速度，不推进物理模拟。

另一个确定问题是 RuntimeDiag 的 component/animation probe 原先永久作为 team=0 存活对象留在 ObjectManager，污染 Sandbox19 敌人数与导演重定向。现在测试装配完成后立即置零生命并移离战场，保留动画 tick；两秒后经 `SandboxObjects:RequestDestroyAgent(id)` 标记生命周期删除；该入口先断开存活子弹指向待删 agent 的 non-owning owner 引用。动画 probe 的 move/idle 请求仍有日志。延迟回调只保存 id；C++ 检查有效 agent 后标记清理，拒绝非法、非 agent 和失效 id，不在 Lua 回调内立即 delete。

**尚未证明历史崩溃同源。** 2026-09-05 的 crash report 表明 NaN 经 `SoldierObject::Update → RenderComponent::SyncFromOwnerTransform → Ogre::Node::setPosition` 到达渲染节点；它没有给出最初制造 NaN 的位置。历史脚本隔离重跑约 286 秒没有复现断言，但仍被旧 probe 敌人数卡在首波。不能把本次数值修复直接等同于历史第二波根因关闭。

## 验证方式

```bash
xcodebuild -project build/HelloOgre3D/HelloOgre3D.xcodeproj -target HelloOgre3D -configuration Release ARCHS=arm64 ONLY_ACTIVE_ARCH=YES build
python3 tools/run_sandbox19_stability.py --rounds 3
python3 tools/run_sandbox19_stability.py --probe --rounds 3 --timeout-per-round 300
python3 tools/run_sandbox19_stability.py --scripted-victory --rounds 3
python3 tools/run_m1_smoke.py
```

完整对局脚本每 100ms 检查活跃 agent 的位置/速度有限值，要求进入真实终局后重开并验证旧 agent id 不再存在。普通/probe 模式不发指令、不杀敌，但保留 sample 原有关卡导演；scripted-victory 模式显式清敌，只能证明三波胜利与重开生命周期，不能证明自然战斗质量。两者都与 M1 合成自测隔离。

runner 检查独立日志、Lua/Ogre/断言错误、提前退出及模拟/墙钟超时；即使退出码为 0，提前退出也失败。假执行器已验证提前退出、PASS 后混入错误均被拒绝。输出目录使用唯一后缀。收集证据后 runner 终止自己的进程，`graceful_shutdown_verified=false`，不能以此声称正常关闭窗口。

## 构建及边界用例证据

| 检查 | 结果 | 本地日志 |
|---|---|---|
| macOS Release arm64 | BUILD SUCCEEDED，存在第三方 warning | `tmp/stability/build-arm64.log`；最后增量 `tmp/stability/build-arm64-final.log` |
| 修复前真实分离力测试 | coincident=false，FAIL | `tmp/stability/separation-before.log` |
| 修复后五项分离力测试 | PASS | `tmp/stability/separation-after.log` |
| RuntimeDiag 与 probe 生命周期 | self test=true；requested=2 / remaining=0；invalid/staleRejected=true；动画 move/idle 请求完成 | 同上 |

默认通用 Release 构建的 x86_64 Bullet intrinsic 参数越界失败，见 `tmp/stability/build-desktop.log`；本轮只构建并运行 arm64。受限沙箱的 module cache / OpenGL 初始化失败分别保留在 `tmp/stability/build.log`、`probe-before.log`，随后在允许图形访问的会话重跑。没有 Windows 运行证据，也没有重新生成全量 tolua；`.pkg` 已引用 ObjectFactory 头文件，本次局部同步绑定。

## 完整对局与回归记录

普通模式连续三局 PASS：第 2 波战败 31.680s、第 1 波战败 29.568s、第 2 波战败 81.048s；两次 restart-clean=true。这里是仿真时间，不是墙钟时间。证据：`tmp/stability-20260906-152219-ordinary-cvlfoxhl/summary.json`。

首个 probe 模式首局在 120 秒超时，未出现 NaN，但反复 FORCE_CONTACT 仍未终局；正确保留为 FAIL，不能被后续通过覆盖。probe 已删除且 enemies=2，因此该超时不能归因于旧 probe 残留计数。证据：`tmp/stability-20260906-152522-ordinary-probe-mfn3mtu0/summary.json`。

将 probe 首局观察延长到 300 秒后仍首波超时，见 `tmp/stability-20260906-152829-ordinary-probe-1_7wmppw/summary.json`。随后一局在 13.992 秒首波战败并 PASS，见 `tmp/stability-20260906-153359-ordinary-probe-xhtiekvp/summary.json`。这说明停滞并非每次启动必现，不能用偶然通过覆盖超时。

为定位停滞，最终 Release 的 probe 回归增加每 60 秒的 agent/AI 快照。60 秒时，四名 AI 均为 15/100 生命，弹药分别 5/4/5/3，BT 持续执行 move/RUNNING；位置与速度有限，sense/memory 与 tick 仍更新。结合 `SoldierConditions.IsCriticalHealth` 的 20% 阈值和现有回避分支，证据指向双方低血量持续回避，导演拉近后又跑开。它是独立的对局规则/AI 行为问题，不应靠杀敌自测掩盖。诊断采样后主动结束该进程，runner 正确记录 early-exit / FAIL；证据 `tmp/stability-20260906-153536-ordinary-probe-ggl8o2qw/summary.json` 及同目录日志。

静态检查使用仓库 vendored Lua 编译的 `tmp/stability/luac51`（Lua 5.1.4），四份改动 Lua 的 `-p` 通过；系统 `luac` 实际为 5.3，未以它代替 5.1 验收。Python 编译与 `git diff --check` 通过。

最终 Release 的 scripted-victory-probe 连续三局 PASS，均进入第 3 波胜利，耗时 20.460 / 20.328 / 20.328 秒，两次 restart-clean=true；probe 清理仍全部通过。证据：`tmp/stability-20260906-153802-scripted-victory-probe-wpygz0sy/summary.json`。这只覆盖胜利状态机与对象清理。

最终 Release 的 M1 相关四个 sample 各运行 30 秒：Sandbox19（Command/Intent/Match/Arena/Observation）、Sandbox8（初始化与限时错误扫描）、Sandbox12（TeamBlackboard/Lifecycle）、Sandbox17（Chapter9Tactics）均 PASS。日志目录 `tmp/m1-smoke-20260906-153935/`。仍存在 Ogre 基础 shader 参数的既有非致命 compiler error 输出；此处 PASS 指目标运行 marker 与无捕获到的 Lua/断言/Ogre exception，不表示日志完全无 warning/error 字样，也不代表行为逐帧对拍。

Sandbox6 FSM、Sandbox7 DT 各运行 20 秒，真实驱动身份/运行状态、RuntimeDiag 与 probe 清理通过，未捕获上述 fatal 错误。日志 `tmp/stability/sandbox6-final.log`、`sandbox7-final.log`，检查摘要 `tmp/stability/sample-regressions.json`。限时结束由测试程序终止子进程，不作为正常退出证据。

另行通过现有 `tmp/m1-window/HelloOgre3D-M1.app` 包装器（bin 软链指向当前工作区的最终 Release）启动 Sandbox19，点击真实窗口关闭按钮。关闭后窗口查询返回 procNotFound，`open -W` 完成，进程清单无游戏进程；stdout 与 `bin/Sandbox.log` 出现完整 OGRE / OpenGL shutdown，stderr 无新增断言/崩溃。证据 `tmp/stability/window-close.log`、`window-close.stderr`。该局自然在第二波 27.951 秒战败后等待关闭；不能把停留在结算画面的时间算作长期战斗验证。

## 本轮结论

重合分离力 NaN 和诊断 probe 生命周期修复有构建、边界用例与相关运行证据；普通三局、合成三波胜利重开、FSM/DT/BT 等相关 sample 及真实关闭均通过。完整稳定性不标为全部通过：probe 长局仍有低血量僵持，历史第二波 NaN 同源性未证明；Windows/x86_64 未通过本轮运行验收。后续优先收口僵持的场景规则，再推进镜头体验或 M2；具体任务状态只维护在 backlog/cycle。
