---
name: no-greenfield-event-infra
description: 别假设是 greenfield；已有 VS2017 风格 typed Blackboard + SandboxEventPayload/Dispatcher
metadata:
  type: project
---

做 AI / 事件 / 数据通道设计时，**别假设是空地（greenfield）可以随便上现代设施**。本项目已有：
- **typed `Blackboard`**（VS2017 时代风格，7 类标量 + 4 类数组，entry 带 confidence/ttl/source/decay；**不是 `std::variant`**）——见 `docs/modules/ai-common.md`。
- **事件总线脚手架** `SandboxEventPayload` / `SandboxEventDispatcher` / `SandboxEventDispatcherManager`（`sandbox/core/event/`，name→dispatcher + Subscribe/Emit/Token，目前同步直发、无缓冲队列）——见 `docs/modules/core-object.md`。
- 命名占用预警：已有 `SandboxContext`（事件 payload），依赖注入聚合体已命名 `SandboxServices`（勿再叫 SandboxContext）。

**How to apply:** 提 AI/事件方案前先看现有 `Blackboard`/`SandboxEventDispatcher` 实现，在其上扩展而非另起一套；行为树/黑板能力缺口见 `docs/design/behavior-tree-gap-analysis.md`（G6 黑板类型、G2 事件节点等）。
