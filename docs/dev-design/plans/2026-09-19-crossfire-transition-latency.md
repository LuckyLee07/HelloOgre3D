# Crossfire 切关响应优化（2026-09-19）

基线：`1b4598d`。上一轮三关质量已统一，本轮解决选关和过关时的同步停顿。

## 目标与边界

- 基于同机、同场景和同输入的 warm-cache 测量降低切关耗时，保持模型、碰撞、导航和操作规则。
- 先分解清场、场景创建、导航、表现对象和角色创建的开销，再落地局部修复。
- 不更换引擎或第三方、不扩展 Lua 绑定、不改变场景几何；保留已有 Tracy 埋点。
- 完成 Release 构建、相关碰撞/查询与三关回归、真实窗口交互、对照测量及本地提交。

## 实施记录

1. 使用隔离包中的临时 Lua `os.clock` 标记粗分 CPU 耗时；热缓存场景实体创建约 340–520 ms，清场约 1 ms、角色重建约 1–2 ms、导航约 4 ms。该诊断不是墙钟/GPU测量。首轮临时替换误改函数声明导致语法错误，已修正并语法检查后重跑，失败输出不作为性能证据。
2. 源码确认 vendored Bullet `addPoint` 每次执行六方向全点扫描。仅改 `PhysicsFactory::CreateSimplifiedConvexHull` 两次构造：用对齐数组一次传入所有点；保持源壳 margin 设置后的非空 AABB 和最终壳 margin 设置前的 AABB。源点、点顺序、简化算法、碰撞体独占所有权与脚本接口保持。
3. 未引入形状共享、资源预加载或异步切关。视觉面唯一网格重复生成及 compound 子壳释放是本次只读发现的后续候选，不混入这次耗时对照。原始输出仅保存在 `tmp/crossfire-transition-20260919/`。

## 验证

- PASS：macOS arm64 Release 增量构建；无 ABI、工程生成或 Lua 绑定变更。Windows 未运行，共享 C++ 无平台专属新分支。
- PASS：链接仓库现有 Release Bullet 的独立旧/新算法比较，9 组人工/退化点集 + 33 个实际 Crossfire 资产点集，共 42 组。点序列、简化索引、margin、199 个支持方向、13 种变换的 AABB、复合体 AABB 和 0/1/12 质量惯量均严格差 0。对照中“只批量、不校正源壳缓存”41 组中间 AABB 不同，验证保留 margin 顺序的必要性。
- PASS：相同 45 事件三关回放，新旧均正常退出、三关胜利、每关两机存活，elapsedMs / damage / medal / flankHits / blocked 逐字段相同；热缓存切关 547.31/433.84/376.35 → 68.03/56.90/54.32 ms，详见[完整对照](../../perf/crossfire-transition-20260919.md)。
- PASS：`run_crossfire_gate.py --mode queries`（23 项）与 `--mode physics`（7 项及重试/退出）在 v10 独立包通过。两者为 synthetic fixture，不冒充真实输入。
- PASS：`run_m1_smoke.py --samples Sandbox6 Sandbox7 Sandbox8 Sandbox19 --seconds 12`，四个入口 smoke 无异常，Sandbox19 地面/连接/静态碰撞/产品夹具 marker 齐全；此为有界入口与夹具观察，不宣称旧 sample 完整通关。
- PASS：Computer Use 操作非后台、无 InputReplay 的 v10。鼠标 2→3→1 选关，点击开始、两机路线和 E 两秒推进；确认自动暂停后继续，首关 10.164 s 双机存活、60 损伤、三星。结算点下一关进入冷却区，重试、设置返回选关、退出均有效，退出日志无错误。场景、机器人和规划路径截图与窗口核对一致；未修改导航算法/debug 绘制。
- PASS：v10 与 v9 的 media 和 Lua 文件逐文件 SHA256 一致；ZIP 不含日志/试玩成绩，二进制与验收包相同。`git diff --check`、文档引用和 registry JSON 核对通过。

## 运行命令与产物

```bash
xcodebuild -project build/HelloOgre3D/HelloOgre3D.xcodeproj -target HelloOgre3D -configuration Release ARCHS=arm64 ONLY_ACTIVE_ARCH=YES build
bash tools/package_macos_playtest.sh "$PWD/tmp/Crossfire-fast-transition-20260919-v10" Sandbox20
python3 tools/run_crossfire_gate.py --mode queries --executable "$PWD/tmp/Crossfire-fast-transition-20260919-v10/Crossfire.app/Contents/MacOS/HelloOgre3DLauncher" --cwd "$PWD/tmp/Crossfire-fast-transition-20260919-v10/bin" --launcher-default --width 1280 --height 720
# 同参数另跑 --mode physics；三关同路线与计时配置见性能对照。
python3 tools/run_m1_smoke.py --samples Sandbox6 Sandbox7 Sandbox8 Sandbox19 --seconds 12
```

本地试玩包：`tmp/Crossfire-fast-transition-20260919-v10/Crossfire.app` 与同名 ZIP。默认普通启动，测试档案和日志已移到 `tmp/crossfire-transition-20260919/` 保留；不提交二进制、截图或原始测量文件。

仍保留 Windows 未运行和作者主观/扬声器听感项。这轮只证明切关响应改善和已有操作/碰撞回归，不代表整体美术已被作者认可。
