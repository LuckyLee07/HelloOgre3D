# Sandbox19 入口集火成对实验

日期：2026-09-10。对应方向为 [project-direction](project-direction.md) M2，实验设计与恢复方式见 [M2 计划](dev-design/plans/2026-09-10-m2-repeatable-comparison.md)。

## 问题与条件

本实验回答：两组都先通过同一右键地面指令把两名队友部署到入口后，在 5500ms 增加一次 `F` 集火，是否改变首次伤害、第一段清场和 30 秒时双方状态？

运行条件为 Windows Release x64、1280×800、隐藏窗口、静音、物理输入禁用、Sandbox19 preset、固定出生、AI scheduler 开启（50ms tick、每帧最多 6 个）、director=`none`。使用 seed `20260710/11/12`，每个条件在每个 seed 下重复三次，成对顺序逐次反转，共 18 局。输入由内部 InputReplay 注入，`synthetic_input=true`，不直接修改位置、生命或 AI 状态。

对照 `autonomous` 只保留共同入口部署；变体 `focus-5500ms` 追加一次 `F`。runner 要求对照局零条 focus executing、变体局恰有两条，并核对 sample、preset、seed、出生模式、调度、单位生命、窗口/输入状态、run id、观察窗或终局以及回放正常退出。

## 运行证据

完整批次位于 `tmp/ai-experiment-sandbox19-focus-order-20260910-175034/`：18/18 局为 `evidence-complete`，9/9 对配对完整，无错误或 director 干预；所有局都在 30 秒观察窗以有效的 `TIME_LIMIT` 收口。此时双方均已清完第一段，仍处于 `ADVANCE wave=1`，所以限时结果不表示战败。

| 指标 | autonomous，9 局 | focus-5500ms，9 局 | 变体－对照（每一对） |
|---|---:|---:|---:|
| 首次伤害 | 4686ms | 4686ms | 0ms |
| 第一名敌人倒下 | 14256ms | 12210ms | -2046ms |
| 第一段全部清场 | 14256ms | 24222ms | +9966ms |
| 集火 BT 接管 | 无 | 两名队友，最大 198ms | — |
| 30 秒队友存活 | 2 | 2 | 0 |
| 30 秒队友总 HP | 280 | 260 | -20 |
| 30 秒敌人存活 / 总 HP | 0 / 0 | 0 / 0 | 0 / 0 |
| 指令完成 / 失败 | 2 / 0 | 4 / 0 | +2 / 0 |

九个配对的上述值完全一致，没有观察到 seed 或先后顺序带来的分布。批次记录 HEAD `b7614f1a7f3a04d44ce21fb44530c5daf2f2f50e`、tracked patch SHA-256 `1a78d2741dd36ca120a31ac8b9c6c96288c61a8381713701bd685bd333ea9ed3`、可执行文件 SHA-256 `7c110de2985619cafc6a291856328e6aa91338d60c745a051d4390b9cb25afe4`，并保存 manifest、runner、相关配置原文与各自 hash。逐局事件、结果和配对差值见该目录的 `runs/`、`summary.json`、`summary.md` 与 `metadata.json`。

## 结论

在这个对称双敌入口条件中，5500ms 的集火确实改变了战斗过程，但结果偏负面。两名队友在 198ms 内共同锁定同一目标，使第一名敌人提前 2046ms 倒下；与此同时，另一名敌人更久保持较高生命并持续输出，导致整段清场晚 9966ms，队友总 HP 额外损失 20。首次伤害、最终存活数和 30 秒敌方清零结果不变。

这说明当前场景的自动行为会自然分摊两个对称目标，而单次全员集火产生过度集中。该结论不能外推到高威胁目标、更多敌人、不同掩体或完整任务，也不能评价操作手感和听感。固定 seed 在本机得到完全一致结果，只证明这个窄条件的当前运行具有重复性，不保证 Bullet 跨机器或跨版本逐帧一致。
