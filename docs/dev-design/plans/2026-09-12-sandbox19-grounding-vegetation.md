# Sandbox19 地表纵深与接地收口

日期：2026-09-12
状态：已完成

## 目标

在 P3 已完成的相机、荒漠远景、relay 门面和交火反馈上，继续解决正常游玩画面中的三项资产级缺口：大面积地面仍像单张平板、角色与建筑缺少稳定接地、场地没有与目标稿一致的干旱植被层。此次保持现有指挥、碰撞、视线与两条导航路线合同，不把视觉装饰伪装成玩法实体。

## 关键设计

- `ObjectFactory::CreateVisualPlane(width, height)` 创建由 `ObjectManager` 持有的渲染平面，但不创建 Bullet 刚体；`OBJ_TYPE_PLANE` 使它被 navmesh 固定几何收集排除，无刚体则使它不进入物理射线和 AI 视线阻挡。Lua 只获得 non-owning userdata，不承担销毁。
- GL3+ core profile 没有可供这些材质回退的固定渲染管线，因此新增一对 GLSL 150 与 HLSL 2.0 的最小 unlit alpha shader；植被、接触阴影和地表旧化共用同一 program，并分别控制裁切、深度写入和最终透明强度。
- 植被采用两张互相垂直的 crossed card，放在外围服务带与掩体肩部；24 组均避开中央主路和西侧绕行。角色阴影只跟随真实脚点，最多复用七个平面，角色死亡或槽位空闲时移到地下，不跨帧保存 agent userdata。
- 六组掩体从约 `x=±10` 内移到约 `x=±8`，让正常跟随镜头能读到近、中、远三层掩体，同时保留约 10 米中央突击走廊与 `x=-18` 绕行路线。地表旧化、静态接触阴影和植被都不参与物理或导航。

## 实施结果

- 新增荒漠草丛透明纹理、接触阴影与地表旧化纹理，来源与转换记录位于 `media/textures/sandbox19/SOURCE.md`；最终材质收紧 alpha 裁切并压低植被色阶，避免白边和高亮纸片感。
- 九块低对比地表旧化打散 16 米地板网格；掩体、设备、relay 主体和七个战斗角色获得接触阴影，减少悬浮感。
- 实机调试经历“GL core 不显示固定管线材质”和“SVG 栅格 alpha 过强形成黑块”两次失败；前者以显式 shader 修复，后者以材质级最终 alpha 收敛。失败截图只保留在本地 `tmp/`，最终 1280×720 证据纳入视觉复盘。

## 验证

- `/usr/local/bin/luac` 5.3 对 `Sandbox19.lua` 与 `sandbox19_scene.lua` 解析通过；当前机器仍无独立 Lua 5.1 解析器，但游戏内嵌 Lua 5.1 已实际加载执行。
- macOS arm64 Release 构建通过；Apple M1 Pro / OpenGL 4.1 真实窗口加载 overlay program、三张 RGBA 纹理并完成最终 1280×720 抓帧，无黑面、材质缺失或 shader 错误。HLSL/D3D9 本轮未在 macOS 验证。
- `python3 tools/run_sandbox19_stability.py --product-fixture --timeout 90` 返回 `PASS reason=evidence-complete`：七个出生点、主路/侧路、掩体射线、地面/relay 碰撞、真实路径到达、暂停、命令与重开合同全部通过。
- `python3 tools/run_m1_smoke.py --samples Sandbox19 Sandbox6 Sandbox7 Sandbox8 --seconds 20` 四个 sample 全部 PASS；`git diff --check` 通过。
- 额外自然战斗回放不改血、不传送、不强制清敌：现有 80 秒配方推进到第二段守卫并正常退出，但未在时限内结算；延长配方因第一段仍有一名守卫时让无武器指挥官前压，于 53.823 秒自然战败。两局都未出现导航/碰撞错误，但不能提供本轮自然胜利证据；这一策略时序边界不由合成 fixture 覆盖。

## 保留边界

本轮完成的是当前资产和渲染能力内的地表纵深、植被与接地层；scene-only 后处理随后由[场景色调与 UI 合成隔离](2026-09-12-sandbox19-scene-grade.md)补齐。角色资产、PBR 高阶材质、动态窗口、人工持续输入手感或扬声器听感仍未完成。产品 fixture 是显式合成验收，不冒充自然战斗或外部输入证据。
