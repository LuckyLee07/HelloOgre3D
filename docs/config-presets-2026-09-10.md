# 配置列表覆盖与 macOS 后台验证（2026-09-10）

## 配置根因与修复

`ConfigManager.mergeTable` 原来对所有 table 递归合并。默认出生点有 20 个，Sandbox19 声明 7 个，解析结果仍有 20 个；Sandbox10 的 2 个、Sandbox12/17 的 6 个也被补上默认尾项。此前 Sandbox19 的导航自测只检查遭遇战实际使用的槽位，避开了尾项，但未修复配置根因。

现在 `spawnPoints`、`waveEnemyCounts`、`waveSpawnIndices` 采用整表深拷贝替换；显式空列表可清空。其他 table 继续递归继承，覆盖优先级仍为 default → sample → selected preset → 环境变量。不能仅根据连续数字键判断列表：Chapter9 的 `legacyForceInitialRandomAgents` 也是数字键 map，需要按 agent id 继承。

`BuildDebugSummary` 新增 `spawnPoints` 数量，Sandbox19 导航自测仍只验收实际使用的出生槽位。纯 Lua 回归直接调用真实 ConfigManager，通过加载适配器处理引擎 require 名称；环境变量层不在该单测覆盖范围。

## 后台窗口

用户要求自动验证启动不要抢占当前活动窗口。macOS 的 Ogre Cocoa 适配在创建/调整窗口时调用 `orderFrontRegardless`、`makeKeyAndOrderFront` 和 `activateIgnoringOtherApps:YES`，仅在 shell 后台运行不能阻止激活。

局部修改 vendored `OgreOSXCocoaWindow.mm` 是为了在实际发起激活的位置处理 `HELLO_WINDOW_BACKGROUND=1`：创建、重新显示、调整窗口走 `orderBack`，不主动激活应用。未设置开关时保留原交互启动行为。本文记录的 2026-09-10 修改当时没有涉及 Windows；Windows 后台 D3D9 路径于 2026-09-11 独立收口，见[运行时模块](modules/runtime.md)。

`run_m1_smoke.py` 与 `run_sandbox19_stability.py` 默认注入开关；M1 脚本新增可选 Sandbox6/7，默认四个入口保持不变。手工后台验证示例（仓库根目录）：

```bash
cd bin
HELLO_WINDOW_BACKGROUND=1 HELLO_SANDBOX_SAMPLE=Sandbox19 ./HelloOgre3D
```

## 验证证据

- Lua 5.1：`tmp/config-merge/lua51 tools/test_config_presets.lua` 通过，40 份实际预设出生点数量一致，另有 9 个合并契约用例（短表、坐标长度、空表、优先级、map 继承、嵌套 wave 列表、隔离拷贝）。修复前后日志分别为 `tmp/config-merge/before.log` / `after.log`。
- Lua 5.1 语法、Python AST、`git diff --check` 通过。
- macOS arm64 Release：`xcodebuild -project build/HelloOgre3D/HelloOgre3D.xcodeproj -target HelloOgre3D -configuration Release ARCHS=arm64 ONLY_ACTIVE_ARCH=YES build` 通过，日志 `tmp/config-merge/build.log`。初次受限构建无法写 module cache，使用桌面权限重跑通过。

- `python3 tools/run_m1_smoke.py --seconds 30`：Sandbox19/8/12/17 全部 PASS；日志 `tmp/m1-smoke-20260910-091340/`。Sandbox19 实际 `spawnPoints=7`，Sandbox17 为 6，Sandbox8 为 20，均符合声明。
- 桌面焦点：只读 AppKit/CGWindow 采样约 140 秒（每轮间隔 50ms），前台 PID 始终为 52483，`steals=0`，2039 个样本存在游戏窗口。日志 `tmp/config-merge/focus-desktop.log`，采样工具源码 `tmp/config-merge/watch_focus.m`。最初受限桌面采样只得到 PID 0，无效；采用桌面权限重跑的数据。采样覆盖后续 sample 启动及运行，不宣称覆盖每一瞬间。层序计数 `aheadOfFrontSamples=169` 也包含未枚举到前台普通窗口的情况，未据此宣称所有窗口遮挡关系均已视觉验收。
- `python3 tools/run_m1_smoke.py --samples Sandbox6 Sandbox7 --seconds 30`：两者 PASS；日志 `tmp/m1-smoke-20260910-091621/`，两者实际 `spawnPoints=20`。已读取 stdout/stderr 与 `bin/Sandbox.log`，未见此次改动引入的 Lua/断言失败；shader 历史警告见下述边界。

## 边界

运行 smoke 是限时 marker/错误检查，只终止脚本自己的进程，不代表完整对局或优雅退出验收。此次未重新验证完整对局；之前低血量僵持修复证据见 [对应记录](stalemate-2026-09-06.md)。Windows、x86_64、全屏切换与手动输入交互未执行。既有 `programs/base.program` shader 常量参数警告仍存在，本次未修改渲染资源。
