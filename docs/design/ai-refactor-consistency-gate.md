# AI 重构一致性验证门禁

本文记录本轮 AI service/component 重构后的可观测验证面。目标不是证明每一帧完全相同，而是固定重构前后最容易漂移的行为边界：driver 初始化、BT action 运行态、感知缓存、TeamBlackboard typed fact、Hearing/Danger、战术影响力 debug 绘制。

## 核心原则

- 先跑静态门禁，再跑 sample：`git diff --check`、`tools/check_sandbox_architecture.ps1`。
- sample 使用固定入口、固定 preset、固定 trace 开关；普通运行默认不打开 trace。
- 对比时优先看结构化 trace 和 smoke 断言，不只靠截图或肉眼观察。
- C++ 类布局、虚函数、字段改动后先 clean rebuild，再做运行时对比。

## 快速门禁

```powershell
powershell -ExecutionPolicy Bypass -File tools\run_ai_refactor_consistency_gate.ps1 -StopExisting -NoTail
```

覆盖：

| 场景 | 入口 | 观察点 |
|---|---|---|
| Lua FSM / 动画 | `Sandbox3` | `UseCppFsm=false` 不应被默认 C++ FSM 抢跑；动画 trace 正常完成。 |
| C++ FSM | `Sandbox6` | 默认 FSM driver 仍能创建和 tick。 |
| DecisionTree | `Sandbox7` | 显式 `dt` driver 仍覆盖默认 driver。 |
| BehaviorTree | `Sandbox8` | BT action 不默认共享运行态；trace / runtime diag 正常。 |
| 感知压力 | `ai_perf_100` / `Sandbox16` | 感知缓存开关、scheduler/runtime diag 正常。 |
| C++ 战术 | `chapter9_tactics_cpp` / `Sandbox18` | TeamBlackboard typed fact、影响力地图、debug draw 投影统计正常。 |

## 完整门禁

```powershell
powershell -ExecutionPolicy Bypass -File tools\run_ai_refactor_consistency_gate.ps1 -Full -StopExisting -NoTail
```

额外覆盖：

| 场景 | 入口 | 观察点 |
|---|---|---|
| Hearing/Danger | `hearing_danger` / `Sandbox14` | C++ hearing/danger sense、RetreatPoint typed fact、cooldown 清理路径。 |
| 战术压力 | `chapter9_tactics_cpp_pressure` / `Sandbox18` | dirty region、candidate limit、debug draw 显式配置。 |
| Legacy parity | `chapter9_tactics_legacy_parity` / `Sandbox17` | legacy 对照 profile 的 trace 与 runtime diag。 |

## 判定口径

- 必须通过：无 Lua error、无 panic/assert、smoke `PASS`、runtime diag self-test 通过。
- 必须稳定：agent 数、driver 类型、关键 Blackboard key、TeamBlackboard typed fact 数量级、BT trace 完成状态。
- 允许容差：位置、速度、影响力采样、投影后 y 值。按 sample 语义使用 trace 容差，而不是要求逐浮点相等。
- 性能不能明显退化：`FramePerf` 和 runtime summary 的 p95/平均值不应超过基线约 10%，压力场景单独记录。

## 本轮新增关注项

- `AIController` 默认 driver 延迟到 services 注入后初始化，避免 `SetUseCppFsmFlag(false)` 的 sample 被默认 FSM 抢跑。
- `BehaviorTreeLoader` action 默认不复用；只有显式 `reuse=true` 或 `stateless=true` 才缓存 action node。
- `TacticalDebugDrawService` 使用 nav mesh 投影配置，并在 summary 中输出 `drawn/projected/projectionRejected`。
- `TeamBlackboardService` typed fact 合并优先较新时间，避免旧 fact 覆盖新位置。
- `HearingDangerSense` 每轮更新裁剪 cooldown map，避免长跑积累。
