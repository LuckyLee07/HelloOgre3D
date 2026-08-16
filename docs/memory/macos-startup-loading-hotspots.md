# macOS 启动卡顿的两个热点

类型：project

2026-08-16 在 Apple M1 Pro、Release arm64、`Sandbox19` 热缓存启动上用 `/usr/bin/sample` 以 1 ms 间隔采样 8 秒。Lua 初始化所在主线程的主要同步阻塞是：

- `NavigationService::CreateNavigationMesh -> NavBuilder::Build -> rcBuildRegions` 约 2.39 秒。默认边界 `[-100.05, 100.05]`、cell size `0.1` 形成约 `2001 x 2001` 网格，且 sample 在启动帧同步构建。
- 首个 soldier 创建触发 skeleton load 约 2.23 秒。`SkeletonSerializer::readAnimationTrack` 每个 track 都会 `backpedalChunkHeader`；`ObfuscatedZipDataStream::skip(-6)` 映射到 `zzip_seek`，而 zzip 对 deflate 流的负向 seek 会 rewind 后从头 inflate 到目标位置。867,909 字节的 skeleton 因此被反复解压；采样中该链路 2,100 个样本落在 `backpedalChunkHeader -> zzip_seek`，XOR 读取只有十几个样本。

同次采样里 `GL3PlusTexture::prepareImpl` 约 0.13 秒；日志中的全部 resource group script parse/create 在同一秒完成。排查启动资源慢时，先区分上述导航构建和 skeleton seek amplification，不要先优化 `initialiseAllResourceGroups()`、磁盘扫描或贴图上传。
