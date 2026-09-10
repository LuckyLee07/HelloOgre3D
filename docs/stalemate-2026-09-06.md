# Sandbox19 低血量僵持修复（2026-09-06）

## 问题与规则

前一轮 probe 完整对局分别在 120 秒、300 秒超时。长局快照中双方四名 AI 均为 15/100 生命，仍有弹药，感知、tick 与移动持续推进。共享 SoldierBT 在低于 20% 生命时优先执行 move/flee；这个分支没有退出时限，Sandbox19 又只在波间恢复生命，因此双方可能一直回避，导演反复拉近距离也不能解决。原始证据见 [稳定性记录](stability-2026-09-06.md)。

本轮在 Sandbox19 内规定自主低血量回避窗口，双方相同：

- 生命低于现有 20% 阈值时开始计时，默认 `commanderMatch.criticalRetreatMs=6000`。
- 到期后，低血量条件不再占据自主行为最高优先级，恢复正常危险回避、求援、接敌等既有分支。仍低血量时不会反复开启窗口。
- 生命恢复到阈值以上后重置，下一次受伤可再次回避；死亡/重开随 agent/Blackboard 生命周期清理。
- 玩家指令分支仍优先，主动撤退的 TTL、movePos 和清理规则不受影响。策略本身不改生命、弹药或移动目标，不强制击杀，也不伪造终局。

6 秒是当前场景的可调规则，不是普遍适用的战术最优参数，也不代表低血量单位必定存活。它为自主回避提供有限机会，然后允许战斗继续；更复杂的撤退治疗/后援机制留给具体玩法实验。

## 实现与观测

`Sandbox19.lua` 对自己的 ai_soldier 更新 `sandbox19_retreat.lua`；状态写入每个对象的 Blackboard（`sandbox19.retreatStartedMs` / `sandbox19.retreatExhausted`），不保存 Lua userdata 缓存。状态转换输出 `[Sandbox19Retreat]` BEGIN / EXHAUSTED / RESET。条件表 `Sandbox19CommandConditions` 覆盖 `IsCriticalHealth`，仍调用共享条件判断原有生命阈值；共享 SoldierConditions、SoldierBT 及章节 FSM/DT/BT 的规则未修改。

更新独立于当前 BT 分支，因此玩家指令暂时占优也不会停止计时。到期会经既有 BT 重新评估生效，不直接中断 C++ action 或覆盖其路径。

## 验证口径

本轮仅改 Lua 与 Python，使用已有最新 macOS arm64 Release 二进制，无 C++/绑定/ABI 改动，不重复构建。

```bash
./tmp/stability/luac51 -p bin/res/scripts/samples/sandbox19_retreat.lua bin/res/scripts/samples/sandbox19_retreat_selftest.lua bin/res/scripts/samples/Sandbox19.lua bin/res/scripts/ai/behavior/Sandbox19CommandConditions.lua bin/res/scripts/config/sample_presets.lua bin/res/scripts/samples/sandbox19_stability_selftest.lua
python3 tools/run_m1_smoke.py --samples Sandbox19
python3 tools/run_sandbox19_stability.py --low-health --probe --rounds 3
python3 tools/run_sandbox19_stability.py --rounds 3
python3 tools/run_sandbox19_stability.py --probe --rounds 3
```

真实 agent/Blackboard/条件绑定的边界自测覆盖：零时刻开始、到期前、精确到期、共享原条件不变、玩家撤退与目标保持、耗尽后不永久重启、恢复阈值、再次受伤。测试在单帧内临时修改已有友军生命并恢复，只在原 smoke 自测运行。

`--low-health` 是独立压力模式：每名 AI 在进入波次时一次性设为 15% 生命，后续靠真实 AI 与战斗结算。它与 `--scripted-victory` 互斥，不计入普通基线；runner 还要求实际出现回避耗尽日志，缺少证据不会判 PASS。普通/probe 对局不使用低血量注入。

## 本轮证据

- Lua 5.1.4 语法、Python 编译与 `git diff --check` 通过。
- Sandbox19 30 秒 smoke 通过，包含八项新策略边界用例及原 Command / Intent / Match / Arena / Observation 自测；证据 `tmp/m1-smoke-20260906-165551/Sandbox19.log`。

- 低血量 + probe 压力场景三局 PASS：分别在第 1/3/2 波自然战败，15.576 / 52.668 / 40.128 秒，两次 restart-clean=true；所有首波四名 AI 均有 EXHAUSTED 记录，后续波间恢复和再次低血也产生 RESET/BEGIN。证据 `tmp/stability-20260906-165802-low-health-probe-rub3igi_/summary.json`。
- 普通模式三局 PASS：均首波自然战败，20.724 / 28.776 / 15.840 秒，两次 restart-clean=true；证据 `tmp/stability-20260906-170024-ordinary-lzr33njh/summary.json`。
- runner 的低血量证据门禁用独立假执行器验证：只有汇总 PASS 但未进入该场景时返回 `scenario-not-exercised` / FAIL，出现初始化与 EXHAUSTED 才接受。这项只验证脚本判断，不计为游戏对局。

- 普通生命 + probe 三局 PASS：均首波自然战败，14.388 / 14.124 / 30.624 秒，两次 restart-clean=true，probe 清理仍通过；实际出现四次自主回避 EXHAUSTED。证据 `tmp/stability-20260906-170148-ordinary-probe-dcohxhyb/summary.json`。

上述九局使用真实战斗结束，没有设置胜负或清敌；低血量压力三局的初始生命注入单独标明。全程有限值检查与 fatal 错误扫描通过，旧的 120/300 秒失败日志保留。全部结果为战败，不把它当作胜率或战术平衡验收。

共享 BT 章节 Sandbox8 的 30 秒回归通过，证据 `tmp/m1-smoke-20260906-170343/Sandbox8.log`。本轮必要运行检查均在 macOS arm64 Release 完成；Windows 未运行。运行脚本按约定终止自己的测试进程，不把它记为窗口关闭验证。文档链接、registry JSON 与新文件 CRLF 核对通过。

本问题按以上边界完成，任务状态已同步 cycle/backlog。历史 Ogre NaN 与之前分离力缺陷的同源性仍未证明，本轮没有据此关闭历史追踪项。
