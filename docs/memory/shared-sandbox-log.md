---
name: shared-sandbox-log
description: 多实例运行时 Sandbox.log 可能混入别的回放记录
type: project
---

2026-09-12 转向验证发现：本轮9秒回放的共享 `bin/Sandbox.log` 混入另一实例的 `40000 key_up SPACE` 与 WeaponShot 行；同一子进程独立重定向的 stdout.log 无这些行。即使启动/完成标记只有一组，共享日志仍不能证明全部行来自同一实例。自动化保留每个子进程独立 stdout/stderr、环境和二进制哈希；统计优先使用隔离输出，不根据共享日志终止他人进程。案例见 `docs/dev-design/plans/2026-09-12-sandbox19-control-feel.md`。
