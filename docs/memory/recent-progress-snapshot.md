---
name: recent-progress-snapshot
description: 项目近期进度快照（截至 2026-06-12）：主线弧 + 当前阶段 + 进度真相来源指针
metadata:
  type: project
---

近期（2026-05-28 ~ 06-12，约 61 个提交）主线弧：
1. **2026-05-28~29**：FGUI internal helper / Store 收口 + 文档路线整理 + `SandboxServices` 依赖注入地基。
2. **2026-05-30 方向回正（关键拐点）**：提交「回正项目为 AI 学习沙盒并清理数据驱动触发器切片」——放弃数据驱动触发器/Lua 生物 Def 方向，定为「AI 学习与实验沙盒」。同期铺 AI 地基：Blackboard metadata/TTL/confidence、VisionSensor、MemoryStore lastKnown、BT Parallel/Random/reactive 重评估、Chapter7 KnowledgeSource。
3. **2026-05-31~06-01**：AI sample 连发——Sandbox10(记忆)/11(Chapter8 感知)/12(TeamBlackboard)/13(InfluenceMap)/16(感知压力)，听觉危险 + 阵型协作。
4. **2026-06-02~04 AI 热点 C++ 化**：`AgentPerceptionSystem`、`AgentSpatialIndexSystem`、`TeamBlackboardService`、`InfluenceMapSystem`、`TacticalQueryService`（Sandbox17 Lua-first / Sandbox18 C++）+ 感知性能基线 + 帧卡顿诊断 + navmesh 3D 影响图。
5. **2026-06-10~12 Chapter9 legacy parity**：对 `HelloOgre3DX/chapter_9_tactics` 做视觉/行为对齐——影响图贴地绘制、parity trace、chapter9 profile、action(movement/animation) intent trace、架构问题落档。

**当前阶段**：AI 学习沙盒推进到「可复现 / 可观测 / 可压测 / 逐步生产化」；主线是把感知/团队/战术/BT 热点从 Lua 下沉到 C++ system，并用 Sandbox + parity gate + perf 基线守回归。

**Why**：git log 只见单条提交、看不出方向拐点（5-30 回正）和系统化推进的整体弧；新会话需要先认知"现在在哪、往哪走"。

**How to apply**：这是**点时快照**，会过期；进度真相以活来源为准——`git log --since=<date>`、`docs/project-direction.md`(北极星)、`docs/planning/long-term-iteration-plan.md`(排期)、`docs/planning/high-priority-todo.md`(backlog)、`docs/modules/<alias>.md`+`docs/registry.json`(模块现状)。
