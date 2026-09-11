# Sandbox19 视觉收敛复盘（2026-09-11）

用户在查看 2026-09-10 的实机图后指出，当前效果与已批准目标稿仍有很大差距。这个判断成立：此前 P1 的 `[x]` 证明了场景、HUD 和交互的游戏内第一版已经可运行，没有证明画面已经接近目标完成度。本轮停止继续接入新的 AI 实验，先收敛相机、空间构图、材质和 HUD 比例。

## 对照结果

目标仍以 [设计稿](dev-design/specs/2026-09-10-sandbox19-product-experience-design.md) 中的构图、配色、信息层级和操作状态为准，不要求复制其中的新角色模型、远山、景深或 PBR 细节。

![目标集火构图](dev-design/specs/assets/sandbox19-p0/target-command.png)

![2026-09-11 当前 1080p 实机集火](dev-design/specs/assets/sandbox19-runtime/combat.png)

本轮实机画面相较 2026-09-10 版本完成了这些结构调整与环境深化：

- 跟随相机由 12 米后距、9 米高度、6 米前视改为 6.5 米后距、3.2 米相对高度、0 米前视；主角全身、两名队友、入口敌人和 relay 轮廓可同时出现，角色不再只是远处小点。
- 专用荒漠六面天空替换通用蓝天，远山、暖色地平线和空气透视形成不参与导航/射线的背景层；目标相机范围无黑面、明显接缝或上下颠倒。
- 连续双层重复墙片改为 2.4 米低边界，西侧实体遮挡与东侧服务墙改成长体块；近景保留单侧开放棚架，并用六组低矮长掩体替换等大立方体群。
- relay 终点改为低翼楼、中央门厅、屋顶设备和桅杆的分层轮廓，主路、西侧绕行和集合区保持开放。
- 将巨大的青色安全区方框换为四角短标记；路线和集合区只保留低强度短线，避免地面标识压过人物。
- 地面、墙体和掩体使用项目内风化混凝土表面，主材收敛为浅砂、暖灰和低饱和灰绿；降低环境光并保留暖色方向光和可读阴影。
- 队友头顶大卡改为小型青色三角；任务框、队友卡、命令栏和指挥官框缩短，并按屏幕宽度居中命令栏。1280×720 与 1920×1080 都保留至少 20 像素安全边距。

## 当前验收口径

| 目标 | 本轮结果 | 证据与边界 |
|---|---|---|
| 视野结构 | PASS | 1080p 集火图中场地占据主体，主角、两名队友、两名敌人与 relay 同屏；720p 主角全身与底部命令栏分离。 |
| 环境纵深与轮廓 | PASS | 1080p/720p 均可见荒漠远山、低墙、长条掩体和分层 relay；重复双层窗片与等大方块群已移除。 |
| 敌我与选择可读性 | PASS | 友军青色三角/圆环、敌方橙色菱形/生命条、目标框和短确认提示同时可见，形状与颜色双重区分。 |
| 材质与光照统一 | PARTIAL | 风化混凝土表面、三类主材和暖色日光已统一，D3D9 真实加载通过；旧低多边形模块、平直几何、贴图密度与目标稿仍有品质差距。 |
| HUD 层级与点击 | PASS | 720p 回放覆盖开始、选择/拖框、右键、暂停设置、三种命令、重试和退出；1080p 无裁切。 |
| 空间与玩法一致 | PASS | 七个出生点、目标/撤回点、主路/侧路共 18 项路径通过；侧墙两高度射线、主路无遮挡和地面碰撞通过。 |
| 目标画面整体完成度 | IN PROGRESS | 核心构图和信息层级已经显著接近；定制建筑/道具、环境远景、材质细节、角色资产和最终声音/手感仍属于 P3。 |

## 运行证据

- 视觉基线：`tmp/product-runtime/visual-gap-baseline-20260911/`；相机/HUD 迭代：`visual-gap-pass1-20260911/` 至 `visual-gap-pass5-20260911/`；环境迭代：`environment-visual-pass1-20260911/` 至 `environment-visual-pass6-background-20260911/`。
- 最终实机截图：1920×1080 位于 `tmp/product-runtime/environment-visual-pass6-background-20260911/`，1280×720 位于 `environment-visual-pass6-background-720-20260911/`；两次均加载完整环境并正常关闭 D3D9。
- Release x64 构建为 0 错误；环境实现与 offscreen 修正后的 `run_sandbox_smoke.ps1 -Sample Sandbox19 -Seconds 40 -NoTail` 均返回 `status=PASS`。最终后台日志记录 1280×800 窗口实际位于 `-3264,-864`，`hidden=false noActivate=true offscreen=true foregroundUnchanged=true` 且物理输入禁用；`[Sandbox19ArenaSelfTest]` 导航/碰撞（含新增长条掩体射线）全部 PASS，`[Sandbox19ProductSelfTest] PASS all=true synthetic=true`。
- 扩展的真实战斗输入回放没有 fixture、改血或传送：82.236 秒清敌进入 REGROUP，99.297 秒自然 VICTORY，两名队友存活，`director=none`，随后 D3D9 正常关闭。日志在 `tmp/product-runtime/visual-gap-current-natural-extended-20260911-runtime.log`。
- Lua 5.1 语法、材质/模型/天空六面加载与 `git diff --check` 通过。后台窗口适配涉及 C++，已重新构建 Windows Release x64。

本轮环境深化已经完成，后续视觉工作继续留在 P3。下一步只在定制资产或渲染能力能实际缩小角色、建筑、植被与 PBR 细节差距时增加范围；人工手感/听感、动态窗口和 macOS 仍按独立证据验收。
