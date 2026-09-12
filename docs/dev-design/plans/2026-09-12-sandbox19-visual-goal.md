# Sandbox19 实机视觉持续收敛

日期：2026-09-12
状态：本轮实现与当前平台验证完成；多个游玩视角已有可验证改善，P3 整体验收仍开放。

## 目标与基线

按用户授权持续缩小与 `../specs/assets/sandbox19-p0/target-combat-v2.png` 的视觉差距。起点 HEAD 为 `719f2f7`，工作区干净；此前院区、铺地与法线改动已提交，均作为本轮基线保留。

当前版本在后台真实 macOS GL 窗口运行；内部输入固定 Enter、前进、射击、停止和退出，1280×720、仿真时钟 5/20/35 秒抓帧，原始证据在 `tmp/visual-goal-20260912/`。记录 binary hash、HEAD、环境、工作区 diff、stdout 与 Sandbox.log。受限会话首轮因无法创建 GL 3.0 上下文未获得画面，不能计为通过；后续在真实桌面会话重跑。

## 实施与依赖

1. 重建三阶段实机基线，检查实际材质/光照和资产链，按画面选收益最大的缺陷。
2. 优先验证渲染假设：GL ambient 纹理单元、真实投影链与角色明暗。共享材质变动需要 Sandbox6/7/8 回归；需要 C++ 时当前平台 Release 构建。
3. 基于已修正光照再调整场景尺度、材质、资产和前中后景；不以装饰数量验收，不破坏出生点/主路/绕行路径。
4. 每轮同条件三阶段抓帧，保留收益明确的改动。新增资源记录来源与许可，失败实验仅撤回本轮内容。
5. 多角度/近距离、自然两波任务、产品 fixture、导航 debug/碰撞与命令生命周期回归；性能敏感项采用同配置日志比较。最终同步 cycle/backlog/modules。

## 边界

内部回放不代表硬件键鼠或真人手感；产品 fixture 会干预状态，只验证合同。Windows/D3D9 无本机运行环境时单列未运行。无法达到的高阶角色/武器/材质/动画效果需说明具体限制与下一条件，不将概念图或合成画面标为实机。

## 首轮根因与实现

- GL ambient pass 的 diffuse/AO samplers 现显式指定 0/1；原先两个默认采样单元使颜色贴图被重复相乘。同条件画面对比证实亮度有改善。
- 当前 Ogre `Vector3::perpendicular` 与旧 Procedural `BoxGenerator` 的面轴假设不一致，±X 面高度/深度交换。0.55×2.4×64 的墙旧渲染 bounds 为 0.55×64×64，Bullet 仍是输入尺寸。ObjectFactory 现按六个面实际轴构造 PlaneGenerator，等密度 UV，同时保持正方体原输出。新增 smoke 对实际生成 mesh bounds（扣除 Ogre culling padding）与物理尺寸的检查。
- 曾针对 Ogre stencil extrusion 的 GL3+ auto-constant 绑定做命名参数探针，构建时核对主程序实际链接的 `libogre3d_d.a`；后续画面不合格，相关 vendored 改动全部撤回，见下一节。
- `shape-no-shadow` 实机图确认巨柱/悬空长条消失，长墙、压顶和掩体高度恢复。真实阴影单独 A/B 验证中。

## 阴影路线调整

GL stencil 命名参数和 indexed range 两轮探针虽能恢复绘制，却仍出现贯穿地面的三角与动画角色拉丝，短对照帧耗时约50ms。两处 vendored 改动已完整撤回并保留试验 patch/截图，不作为最终修复交付；不用“已经有阴影”替代视觉正确性。最终改为场景显式启用的单张方向光深度图：runtime 管 Ogre 配置，SceneService 提供最小非拥有 Light 包装与局部 Lua 绑定，独立 GLSL/HLSL caster/PCF receiver。其它章节保留原默认配置，HELLO_RENDER_SHADOWS=0 可作同条件无阴影对照。

## 最终候选与保留的失败

- 方向光使用单张 1536² R32F、far=32、clip=0.1..220；与初始 2048²/far45 相比保持相近世界 texel 密度，覆盖只面向当前游玩镜头。Receiver-plane 深度梯度修正 PCF 各 tap，使用四个 texel 中心的双线性权重，避免点采样阶梯；投影底色 0.62。GL/HLSL 显式 LOD0，避免 PS3 动态分支隐式梯度风险。它仍是 modulative 阴影，会压低 ambient，非完整物理光照/级联阴影。
- 屋顶由四块原生 3.17m 面板构成，连接横梁与四根柱；位于主路外并保持 x=-18 绕行通道。第一版后柱刚好挡在 z=4 侧墙验证射线上，fixture 明确 FAIL，后将柱移到 z=3.45，保留原射线与精确对象 ID 检查。
- 铺地 v3 使用新生成的低对比混凝土面板，原图不重采样；来源、完整 prompt、尺寸与 hash 见 `media/textures/sandbox19/SOURCE.md`。Trim/设备盒使用均匀涂层，避免把 Nobiax 原生模型 UV 图集硬套到程序盒体；原生模块保留原贴图和 CC0 许可。
- 灯光恢复较中性日光与略冷 ambient；发光贴图仅恢复既有头盔/武器指示器，不能当作定制角色或高阶角色材质完成。
- 原始基线 `baseline-desktop` 在 40.722 秒自然胜利；后续同路线 `canopy-full`/`final-route` 在第二波 32.043 秒指挥官阵亡，停止仿真后没有 35 秒抓帧。`refined-combat` 的停步持续射击也在 29.436 秒阵亡。以上全部是 director=none 的实战结果，既不以 fixture 冒充通关，也不删除失败局；镜头/输入相同不保证改动后的导航几何与战斗结果相同。

## 验证记录

最终测试结果在本节收口，静态、构建、运行与视觉分别记录。原始日志、截图和回放仅本地保留，不入库；本节保留验证方法、结果和限制。

- 静态：Lua **5.1.4** `loadfile` 解析通过（`tmp/config-merge/lua51`；系统 luac 是 5.3，不能以它替代）；头文件、`.pkg` `$cfile`、局部绑定与 Lua 调用匹配。材质/程序资源引用、1254² RGB 铺地图尺寸与来源核对通过；`git diff --check` 通过。
- 构建：macOS arm64 应用 Release `build-final.log` 为 BUILD SUCCEEDED；最终不包含 vendored 引擎改动。Windows/D3D9 新 HLSL 与 R32F 路径未在本机运行，不标通过。
- 章节/合同：`tmp/m1-smoke-20260912-131518/` 的 Sandbox6/7/8 均 PASS；其中 Sandbox19 因柱遮挡射线 FAIL，已保留。修正后 `tmp/m1-smoke-20260912-131843/Sandbox19.log` PASS，包含实际非等边 mesh bounds、全部 spawn/主路/绕行连通、原始墙/掩体/地面射线、产品 fixture all=true（命令、失败/目标丢失、死亡、重开等合同）。fixture 有 synthetic 状态设置，不当自然通关。
- 自然战斗：`final-complete` 没有产品 fixture，director=none。通过前进、射击、后撤、等待与再推进，47.157 秒进入第二波，58.773 秒清场，62.997 秒 VICTORY，2/2 队友存活；72 秒 Enter 重开回第一波，76 秒正常退出 code=0。回放里的 F 输入发生在无可接受敌人的时段，结算 accepted orders=0，不能声称本局执行了集火命令。
- 视觉：最终 `final-pair` 匹配正面 5/20 秒（前20秒输入与基线完全一致，22秒正常退出）、900p 滚轮近景/左右转镜头、F3 导航面与路径绘制已实际查看。旋转暴露天空接缝：当前 Ogre `BoxPlane` 顺序为 front/back/**left/right**/up/down，材质原顺序把左右面反置；现两种 technique 一并修正，`sky-seam-views` 复核断层消失，未改天空像素。
- 性能：相同候选/1280×720/静止机位/perf.txt 18 秒，1 秒周期快照，排除前四条启动样本和截图导致的 frameDelta>=100ms 样本。阴影 on 的 cpuFrame 中位数 **33.34ms**、均值 **30.97ms**（15 样本）；off 中位数 **17.52ms**、均值 **17.50ms**（14 样本）。on/off 的 updateCall 中位数仅 2.56/2.21ms，新增成本主要是渲染。小样本且 off 双峰明显，不宣称稳定 FPS、p95 或仅 8ms 成本；截图原始约410ms阻塞另存日志。未修改 Tracy 埋点。当前真实阴影有明显成本，后续若要求更高帧率，应把接收整合到基础材质单次光照，减少 modulative 的额外场景 pass，而非继续堆贴图。

## 交付与剩余差距

本地目录 `docs/dev-design/specs/assets/sandbox19-visual-20260912/` 保存正面前后对照、左右近景、导航图、最终胜利原图和回放说明；这些验收产物不入库。3阶段基线完整；同路线最终局在32秒阵亡，因此没有伪造对应35秒仿真图，后段通关图明确来自另一条自然输入路线。

本轮主要缩小几何正确性、接地、天空连续性和场景结构差距；角色/枪械仍为旧资产，黑色敌人细节、地表近景层次、材质真实感和阴影性能距离目标稿仍有差距。没有完整 PBR、定制角色/动画、环境植被重建或多级级联阴影，也没有 Windows 新分支、人工持续键鼠或扬声器听感证据。P3 保持未完成。
