# 项目记忆（工具无关）

> 这是 HelloOgre3D 的**唯一权威项目记忆**，随仓库走、纯 markdown，任何 LLM agent（Claude Code / Codex / 其它）都应读取与写入这里，**不区分工具**。
>
> 约定：
> - **开工前**先读本 `MEMORY.md` 索引，再按需读对应条目文件。
> - **学到新经验**（非显然、跨会话有用、且代码本身不记录的事实）就在此目录新建 `<slug>.md` 并在下方加一行索引。
> - 一文件一事实。类型：`user`（用户是谁/偏好）、`feedback`（协作方式纠正）、`project`（进度/约束/坑）、`reference`（外部资源指针）。
> - 别记代码本身已记录的（结构、修复、git 历史）；那些查代码/`git log` 即可。
> - 入口由 `AGENTS.md` 的「项目记忆」节指向本目录——这是让各工具都生效的公约数。

## 索引

- [用户画像与协作偏好](user.md) — code-anchored 真值核对、verify-first、要 observable 验证、方向先看 project-direction
- [Release 构建跑运行时](build-release-for-runtime.md) — bin 卡顿先查是不是 Debug 构建覆盖了
- [parity gate 的 Python](parity-gate-python.md) — 默认 python 是坏 shim，用 E:\SoftWare\Python\Python310\python.exe
- [CRLF / 编码坑](crlf-encoding-gotcha.md) — 新建带中文注释的 .h/.cpp 必须 UTF-8 with BOM + CRLF，否则 MSVC 按 GBK 误读爆 C2447；另：git bash `sed -i` 会把 CRLF 剥成 LF，safecrlf 让 `git add` 直接 fatal，批量改文档改用 PowerShell 或事后补 `s/$/\r/`
- [ABI 改动要 clean rebuild](abi-change-clean-rebuild.md) — 改虚函数/继承/字段布局后必须全量重编，否则 dynamic_cast 在 RTTI 内崩
- [tolua 不暴露 .new()](tolua-no-new.md) — Lua 端造对象走 functor 或引擎 Create*/New* 工厂
- [Lua 侧缺失导出与时钟坑](lua-missing-exports-and-clock.md) — `ObjectManager:getObjectById` 有声明但没导出（按 id 找对象要扫 `getAllAgents`）；`getTimeInMillis` 是启动至今的仿真时间、初始化期可能为 0，别拿 `>0` 当有效性判据
- [验证环](validation-loop.md) — 混合 Lua/C++ 改动最小验证：luac -p + git diff --check + Release x64 + run_sandbox_smoke
- [自己跑自己读日志](run-and-diagnose.md) — 改完自己跑 exe 读 stderr/Sandbox_d.log 定位，别甩给用户
- [别假设 greenfield 事件设施](no-greenfield-event-infra.md) — 已有 typed Blackboard + SandboxEventPayload/Dispatcher，勿提 std::variant/新建事件系统
- [近期进度快照](recent-progress-snapshot.md) — 截至 2026-06-12 的主线弧：方向回正→AI 地基→sample→热点 C++ 化→Chapter9 parity
- [Tracy profiler 采样方式](tracy-profiler-no-cli-capture.md) — 仓库自带 tracy-profiler.exe 只有打开/连接 GUI，没有无界面 capture CLI
- [中间产物放本地](local-output-not-cloud-artifact.md) — 诊断图/可视化默认写项目内 tmp、自包含 HTML，别默认发 Artifact/claude.ai
- [chapter-8 对拍：速度已修+感知天花板](chapter8-parity-rootcause.md) — 根因全清但**保架构下逐帧复刻不可达**：①速度=capsule setAngularFactor(0)禁转→滑动摩擦3×衰减(已修:friction 0.15+去forceScale，前7帧对齐)；②感知可见性分岔=全局多体发散(agent#2 s3最早/agent#3 s8集体转折)，**双铁证**:agent#3多看见id118(legvis:顶起box挡视线)、agent#2少看见id119(visdbg116:视锥后方)，两套独立实现不可逐位对齐。已排除随机数(巡逻点已注入对齐)/选择逻辑(纯nearest)。诊断法:A3DIAG对比setVel(物理前)vs speed(物理后)
