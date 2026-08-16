# macOS 启动卡顿修复的实测结果

类型：project

2026-08-16 在 Apple M1 Pro、Release arm64、`Sandbox19` 上完成两处启动热点修复并实测：

- 自定义 `ObfuscatedZipDataStream` 补齐 Ogre `ZipDataStream` 同款 2×临时块回退缓存后，4 秒、1 ms 间隔的启动采样中 `SkeletonSerializer` 由修复前约 2,100 个样本降到 19 个，`zzip_seek` 不再出现在调用图。以后不要用整包预解压或资源格式迁移绕过这个问题；根因是 serializer 的 6 字节 backpedal 未在内存缓存内命中。
- Recast bounds 从固定 2001×2001 收紧到当前 transformed fixed geometry 的 920×840 后，不带 profiler 的冷构建由约 2.39 秒降到 1.07–1.11 秒。单 tile Detour 缓存第二次启动命中约 1.01–1.16 毫秒；`HELLO_NAVMESH_CACHE=0` 时仍能正确回退同步构建。
- 缓存默认写到从 `bin/` 运行时的 `../tmp/navmesh-cache`；键包含完整 Recast config、变换后几何与 Detour/cache 版本。诊断时可用 `HELLO_NAVMESH_CACHE_DIR` 指向独立目录做冷/热 A/B，不能只看已有默认缓存的热启动。

同轮 `Sandbox5/6/7/8/19` smoke 均完成 sample 选择，无 Ogre/Lua fatal 模式；`Sandbox19CommandSelfTest`、`Sandbox19IntentSelfTest`、`Sandbox19MatchSelfTest` 均 PASS。
