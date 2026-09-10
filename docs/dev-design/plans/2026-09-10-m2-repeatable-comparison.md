# M2：可重复成对实验首版

日期：2026-09-10。方向与验收依据见 [project-direction](../../project-direction.md) M2；当前任务状态统一写入 [cycle](../../cycle-01.md) 与 [backlog](../../backlog.md)。P3 的人工手感、听感、动态窗口和 macOS 验收暂候作者有时间，不作为本实验前置。

## 问题与最小产物

本轮回答：固定代码、配置、场景和调度后，入口交战时增加一次集火命令，是否会改变首次接敌、第一段清场、伤亡或剩余生命？

最小产物包括：

- 一个版本化 JSON 实验定义，保存问题、单变量、三个 seed、重复次数、观察窗、输入事件和配置源文件。
- 一个只启动自身子进程的 Python runner，串行执行成对用例，保存代码版本、工作区状态、可执行文件和配置指纹、原始日志、逐局结果以及 JSON/Markdown 比较摘要。
- Sandbox19 的实验专用结构化事件：准备、阶段变化、首次伤害、伤亡、固定间隔快照、观察窗终点和终局；命令执行日志附带指令到 BT 接管的仿真延迟。普通游玩不启用结构化快照。
- 解析与分类回归，区分游戏内 `VICTORY` / `DEFEAT` / `TIME_LIMIT` 和基础设施的启动错误、运行错误、墙钟超时或中断。
- 每局核对 manifest 声明的 director、出生模式、AI tick、每帧调度上限及三类单位生命配置；对照局必须没有集火 `executing`，集火局必须恰有两名队友进入集火执行，失败局不进入指标和成对差值。

## 实验设计

两组都使用 `Sandbox19` preset、1280×800、Release、隐藏窗口、静音、固定出生点和相同调度。每个 seed 下重复三次；成对执行顺序逐次反转，减少固定先后顺序造成的环境偏差。

唯一变量为 `focus_order_enabled`：

- `autonomous`：完成共同的入口部署后，不下集火命令。
- `focus-5500ms`：完成同一入口部署后，仿真开始 5500ms 时按一次 `F`，默认已选中两名队友。

两组共同在 0ms 开始任务，600/650ms 以相同右键按下/抬起把小队部署到入口，30000ms 留下观察窗快照，30500ms 请求退出。该部署沿用已验证的 1280×800 场景坐标，使双方进入真实导航与射击链；两组之间唯一差异仍是 5500ms 的 `F`。内部 InputReplay 只注入输入事件，不改生命、位置或 AI 状态；结果必须标记 `synthetic_input=true`，不能替代人工操作。

“首次接敌”在本实验中明确取任一战斗单位第一次损失生命值的仿真时间；它是可核对的首次伤害代理，不推断更早但未命中的感知或射击。

## 实现位置与依赖

| 层 | 文件 | 预期行为 |
|---|---|---|
| 场景事件 | `bin/res/scripts/samples/Sandbox19.lua` | 仅在 `HELLO_EXPERIMENT_RUN_ID` 有效时输出有界结构化记录；不改变任务规则。 |
| runner | `tools/run_ai_experiment.py` | 校验 manifest，生成每局回放，隔离 `HELLO_*`，串行运行并生成逐局/成对摘要。 |
| 实验定义 | `tools/experiments/sandbox19-focus-order.json` | 保存全部变量、固定条件、输入与配置文件列表。 |
| 解析回归 | `tools/test_ai_experiment.py` | 覆盖胜利、观察窗结束和图形设备启动失败分类。 |
| 调用隔离 | `tools/run_sandbox_smoke.ps1`、`tools/package_windows_playtest.ps1` | 清除实验环境变量，避免普通 smoke 或试玩继承实验模式。 |

## 验证

1. Python AST、runner 单元测试、manifest dry-run、Lua 5.1 语法和 PowerShell 解析。
2. 当前 Windows Release 无 C++ 改动，沿用匹配二进制；实际成对运行必须读取每局日志并确认配置、seed、run id、InputReplay 完成和结构化快照。
3. 所有运行基础设施成功且产生可解析观察窗或终局，runner 才可报告实验 `PASS`。游戏内失败或观察窗未结算是有效结果，不等同于 runner 失败。
4. 输出必须保留逐局值和范围；固定 seed 不视为逐帧确定性，也不把九次重复解释为广泛统计结论。

若当前桌面图形设备不可用，完成静态与解析验证并把实际实验保留为 `NOT RUN`，不得据历史日志勾选 M2。

## 2026-09-10 执行证据

Python 解析契约、AST/字节码编译、manifest dry-run、Lua 5.1 语法、PowerShell 解析、JSON/路径与 `git diff --check` 均通过。受限会话在第 1/18 局、Lua 初始化之前报 D3D9 `Cannot create device`；runner 正确分类 `graphics-device` 并停止剩余 17 局，失败证据位于 `tmp/ai-experiment-sandbox19-focus-order-20260910-174508/`。

桌面图形会话的修正批次位于 `tmp/ai-experiment-sandbox19-focus-order-20260910-175034/`，18/18 局、9/9 对全部证据完整，summary 状态为 `PASS`。集火不改变 4686ms 的首次伤害，但使首名敌人提前 2046ms 倒下、第一段清场晚 9966ms、30 秒队友总 HP 少 20；精确结果与适用边界见 [实验记录](../../sandbox19-focus-order-experiment-2026-09-10.md)。
