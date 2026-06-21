# Tracy profiler 采样方式

类型：project

事实：`tools/tracy-viewer/tracy-profiler.exe --help` 只提供打开本地 `.tracy` 文件，或通过 `-a address [-p port]` 连接运行中 client 的 GUI 用法；仓库当前没有 bundled `tracy-capture` 这类无界面采集 CLI。

影响：需要沉淀 Tracy capture 基线时，不能在普通 headless smoke 里直接生成 `.tracy` 文件；应在可显示桌面会话中启动目标 sample，再用 Tracy profiler GUI 连接并保存 trace。
