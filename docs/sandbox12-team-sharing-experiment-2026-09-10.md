# Sandbox12 团队信息共享开启/关闭实验

日期：2026-09-10

## 问题与结论

本实验回答：A 能直接看到敌人而 B 被实体遮挡时，团队共享是否会让 B 读取 A 发布的敌情并移动；敌人离开视野、团队事实过期后，B 是否会停止依赖旧信息。

在本轮固定场景与调度下，答案是肯定的。共享关闭时 9/9 局均没有发布、消费或移动，B 的总位移为 0。共享开启时 9/9 局均在 33ms 发布并由 B 在同次 Lua 更新中消费，B 在 396ms 后出现可测移动，观察窗内总位移为 6291mm。事实失效并清理移动状态后，两组的额外位移都为 0。九组成对结果完全一致。

这证明当前 Sandbox12 的“直接目击 → 团队事实 → 队友消费 → 行为响应 → TTL 清理”链路在受控条件下成立。它不证明团队共享在自然战斗中一定改善胜率。

## 实验设计

- `Sandbox12` 使用独立 `team_sharing_experiment` preset：A 在 `(8, -6)`，B 在 `(0, -6)`，敌人在 `(0, 6)`；`(0, 2)` 的实体遮挡让 B 的射线受阻，同时保留 A 的直接视线。
- 三个 agent 使用只响应 `movePos` 的最小行为树，排除随机巡逻、追击和射击。AI 调度固定为 50ms tick、每帧最多三个 agent。
- 唯一变量 `team_sharing_enabled` 同时控制 C++ agent 感知快照自动同步、Sandbox12 的直接目击发布和队友消费。关闭组不允许这些共享路径产生响应。
- 2000ms 后场景脚本把敌人移到双方身后，制造固定的视野丢失；团队事实 TTL 为 1500ms。该位置变化记录为 `direct_state_mutation=true`。
- 三个 seed、每个条件各重复三次，共 18 局；同一 seed/repetition 组成一对，执行顺序交替反转。所有运行使用 Windows Release x64、1280×800 隐藏窗口、静音和内部合成 InputReplay。

版本化实验定义是 [sandbox12-team-sharing.json](../tools/experiments/sandbox12-team-sharing.json)，执行入口为：

```powershell
python tools/run_ai_experiment.py tools/experiments/sandbox12-team-sharing.json
```

## 结果

正式批次 18/18 局均为 `evidence-complete`，两组各 9/9 局通过事件契约；每局到达 7000ms 观察窗后按 `TIME_LIMIT` 正常结束。这里的 `PASS` 表示运行和证据完整。

| 指标 | 共享关闭 | 共享开启 | 成对差值（开启－关闭） |
|---|---:|---:|---:|
| 视野隔离建立 | 33ms | 33ms | 0ms |
| A 发布敌情 | 无 | 33ms | — |
| B 消费延迟 | 无 | 0ms | — |
| B 首次可测位移延迟 | 无 | 396ms | — |
| 已发布事实失效年龄 | 不适用 | 3498ms | — |
| B 总位移 | 0mm | 6291mm | +6291mm |
| 失效后的额外位移 | 0mm | 0mm | 0mm |

每项的九次取值均相同。关闭组仍在 3531ms 记录一次“已无事实、支援意图和移动状态”的清理检查点；由于该组从未发布事实，不把这个检查点解释为事实年龄。开启组在 33ms 发布，3531ms 确认事实和移动状态均已清理，因此已发布事实年龄为 3498ms。0ms 消费延迟表示发布与消费发生在同次 Lua 更新，不代表跨线程或网络传输没有成本。

正式批次的本地临时证据位于 `tmp/m3-team-sharing-20260910-1932/`，含逐局日志、`result.json`、配置/二进制指纹、`summary.json` 和 `summary.md`；该目录不随仓库分发。可复现的长期真源是 manifest、runner 和场景代码。

## 实现与验证

- `TeamBlackboardService` 增加可恢复的 agent 自动同步开关；关闭时 `SyncFromAgents` 仍推进时间并清理 TTL，但不再扫描 agent 或发布新事实。`Reset` 恢复默认开启。
- `TeamBlackboard.lua` 通过局部 tolua 绑定配置该开关；未运行已知会破坏 Sandbox18 的全量 `tolua.bat`。
- Sandbox12 的实验路径输出 `ready / visibility / published / consumed / moved / target-hidden / expired / horizon` 有界事件。关闭组禁止 `published / consumed / moved`，开启组要求全部事件和终态字段成立。
- 清理共享支援时同时删除 Blackboard 移动意图、locomotion path/target/velocity。共享 `MoveAction` 在 `movePos` 被外部删除时也终止并清理运动状态，避免旧路径继续漂移。
- 通用 runner 新增声明式事件契约、字段断言与 manifest 指标，同时保持 M2 入口可用；无 PlayerController 的 sample 由 manifest 设置 `replay.wait_for_player=false`，从首个更新循环开始回放。

本轮 Windows Release x64 构建通过；正式实验 18/18 局通过；Sandbox12 默认 `team_blackboard` smoke、Sandbox6/7/8 smoke 通过；Python runner 10 项单元测试、Python 编译、M2/M3 dry-run、Lua 5.1 语法和 JSON 解析通过。人工输入、手感、听感和 macOS 未由本实验验证。

## 适用边界

- 敌人由受控脚本移动，目的是固定视野丢失时刻；这不是自然敌方移动证据。
- 专用行为树隔离了通信与移动链，因此不衡量战斗结果、团队策略质量或 Sandbox19 玩家指令交接。
- 三个 seed、九组成对运行用于验证当前固定场景和实现，不能外推一般胜率；固定 seed 也不保证不同机器上的 Bullet 仿真逐帧一致。
