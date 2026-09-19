# Crossfire 连续切关资源回收（2026-09-19）

状态：已完成实现与 macOS 验证，Windows 未运行。基线 `9983f8c`。沿用已授权的升级方向和每轮本地提交约定。

## 目标与设计

修复反复选关时残留的视觉面网格与模型子凸包，保持场景、碰撞、导航和操作。先用相同输入测到资源增长，再验证同一关资源数量回到固定值。

- ObjectFactory 仅为 CreateVisualPlane 的唯一临时 Mesh 建立局部拥有者。销毁 Block 时从 MeshManager 移除该网格，现有 Entity 的 MeshPtr 保持其有效直到 RenderComponent 销毁；不改文件资源、通用 RenderComponent 或 Lua 所有权。
- PhysicsFactory 用局部 compound 子类记录独占的 hull。只删除工厂明确接管的 hull，不递归删除 Bullet 通用的 borrowed/shared child。
- 生命周期 gate 复用 GameManager 的现有资源诊断绑定，通过普通选关/重试输入观察资源数；诊断显式 opt-in，不改对局对象或伤害。

## 验证计划

1. 原二进制运行新诊断，证明重复选关 Mesh 数增长；新版相同回放要求预热后各关 Mesh 数/字节、对象数/Agent 数稳定，同关重试一致。
2. 局部 owning hull 真实虚析构计数，覆盖独占、共享借用、重复引用。
3. arm64 Release 构建，三关、查询/物理、Sandbox6/7/8/19 及真实窗口的场景/输入检查。
4. 包内验收、必要文档与本地提交。原始测量、程序和截图只放 `tmp/crossfire-lifecycle-20260919/`；Windows 未运行项明确保留。

## 实施与证据

- 原版负对照：同一 Release v10 二进制运行新增只读诊断；5 圈 2→3→1、2 次 R 重试，正常退出而资源检查 FAIL。初始 Mesh 96，最后 1194；首圈资源预热后每圈仍增加 218。不是环境失败，也没有把进程退出当作回收通过。
- 新版：同回放和 18 次快照，预热后首关/冷却区/联锁区 Mesh 固定为 104/115/110，对应 2817/2818/2817 KB（Ogre getSize 汇总、取整）；对象数 153/167/162，Agent 数 3/3/4。13 次同关比较全 PASS，R 重试也保持资源数量。这里证明该路径资源不增长，不等于进程 RSS/GPU 显存或整个引擎无泄漏。
- 物理测试直接提取本次实际局部 helper，链接现有 Release Bullet；拥有/外部共享/栈借用/重复引用/刚体替换的真实虚析构计数均通过。10,000 次 created=destroyed、live=0，ASan 测试程序 stderr 为空；20 姿态 AABB、margin、三档质量惯量严格一致。此为独立程序检查，未把整个游戏或 vendored 库称作 ASan 构建。
- arm64 Release 增量构建 PASS。两类新增拥有者均为 cpp 局部类，既有头文件与跨模块 ABI 未改；Lua 使用现有 GameManager 诊断导出，不新增绑定，不改第三方。
- Lua 5.1 loadfile、Python 语法与 diff 检查通过；独立 review 未发现释放顺序、借用语义或 gate 判定问题。
- 独立 v11 包 controls、queries（23 项）、physics（7 项与重试）全部 PASS。普通三关 elapsedMs 10329/10197/15444、双机存活、damage 72/24/108、medal 2/3/2，与上轮同路线一致；默认启动不出现生命周期测试日志。Sandbox6/7/8/19 的 12 秒入口 smoke 通过，无 Lua/断言错误；不以入口 smoke 代替旧 sample 完整通关。
- Computer Use 普通窗口（无 InputReplay）鼠标执行 2→3→1→2、进入冷却区、分路、E 两秒与继续执行。场景标牌/供电/接触阴影在重建后仍可见；冷却区 9.933 s、双机存活、36 损伤、三星；重试复位与菜单正常退出，无运行错误。此为本机外部 UI 操作，不替代作者主观品质或扬声器听感。
- ZIP 与实测二进制一致，包含新诊断脚本且没有测试成绩或日志。原生测试成绩/日志已移出包并保留到本地证据目录。

## 试玩包

`tmp/Crossfire-lifecycle-20260919-v11/Crossfire.app` 与同名 ZIP；二进制 SHA256 `7d8685edd96ba00a45a08b092e34b7bf5e16b78d51b1e73c0df3b4205f6f481a`。本轮未修改资产、碰撞几何、战斗配置、公共绑定或平台分支。

完成这处已定位的残留后，后续回到三关视觉与交火体验评审，按可见弱项推进；不扩展成无边界的全引擎内存清理。Windows、作者主观品质与实际听感继续保留。

## 本地证据（不入库）

- `tmp/crossfire-lifecycle-20260919/`：build.log、baseline-gate/candidate-gate.log、两组 snapshots.txt、独立测试源码/日志/指纹。
- 原版 gate：`tmp/crossfire-gate-20260919-201041-d1katdd8/lifecycle/`；新版：`tmp/crossfire-gate-20260919-201321-2fv38wnf/lifecycle/`。完整 stdout/stderr 与 summary.json 保留；stderr 均只有既有 GL 驱动一次提示。
- 可复跑：`python3 tools/run_crossfire_gate.py --mode lifecycle --width 1280 --height 720`。后台/静音/禁止硬件输入，所有选关/重试通过正常输入分发；`HELLO_CROSSFIRE_LIFECYCLE_TEST` 只加只读快照，不创建额外对象。
