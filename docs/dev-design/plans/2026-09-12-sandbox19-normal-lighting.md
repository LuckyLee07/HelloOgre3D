# GL3+ 基础材质法线映射与中继站混凝土层次

日期：2026-09-12
状态：GL3+ 基础材质修复与 Sandbox19 配对法线切片已完成；产品级材质与 Windows 验收未完成

## 根因与范围

`base_material` 的 HLSL per-light pass 读取 `normalMap` 和网格 tangent，GLSL 150 版本却只用插值后的几何法线；纹理单元已绑定但没有参与光照。程序化 BoxGenerator 在 `TriangleBuffer::transformToMesh` 构建 tangent，现有 Nobiax 资源也以这一基础材质走 Windows 路径。GL 顶点程序现将切线与逆转置变换后的几何法线传到片元程序，片元程序正交化 TBN 并读取第三纹理单元的法线图；无有效切线时仍按原几何法线着色。HLSL 路径没有修改。

GLSL sampler 的 `diffuseMap/specMap/normalMap` 现显式绑定 0/1/2，且移除该程序未使用的自动参数。第一版仅补采样而未固定 sampler 单元时，GL 把混凝土地面误采样为深色；固定后三路纹理恢复正常，启动日志中原有的四条 `base.program` 无效参数异常也不再出现。

最初将 CC0 Nobiax `ground2Normal.tga` 试用在 Sandbox19 混凝土墙与掩体上，实机出现与 albedo 不配对的黑色横纹，已撤回。最终以现有 `courtyard_concrete_diffuse.png` 为参照生成克制的 1024×1024 切线空间法线图，只给 `Relay/Concrete`、`Relay/Cover` 配对使用。铺地、中路和金属门面保留平法线。图像来源、缩放与提示词见[资源来源](../../../media/textures/sandbox19/SOURCE.md)。

## 验证

- macOS GL3+ 实机场景同构图 `tmp/sandbox19-matched-normal-probe-20260912/matched_05000ms.png` 与近门 `tmp/sandbox19-normal-final-longrun-20260912/normal_final_25000ms.png`：墙体与掩体的表面响应比平法线有轻微变化，未见试验版黑横纹；这是局部细节，不是 PBR 或概念图级的视觉跃迁。最终图和 GLSL 正常载入，未见 shader 编译或纹理缺失异常。
- 60 秒真实 GL 内部输入回放首波 12.936 秒清除，17.919 秒触发第二波，36.762 秒 REGROUP、40.359 秒 VICTORY，无 STALEMATE，正常退出；完整日志及 5/25/40 秒抓帧在 `tmp/sandbox19-normal-final-longrun-20260912/`。这是合成输入，不代表真人手感。
- 产品 fixture `tmp/relay-product-fixture-20260912-112901-7hhpld_3/summary.json` 为 `PASS reason=evidence-complete synthetic=true`；Sandbox19/6/7/8 的 15 秒 smoke 均 PASS，日志在 `tmp/m1-smoke-20260912-112956/`，无新增 shader、纹理或运行时异常。四个 smoke 中 Sandbox19 的 `result=failed` 是任务指令的正常目标丢失/owner-down 结果，不是测试失败。
- `luac -p` 对改动 scene 脚本解析通过；文档链接、资源尺寸/格式和 `git diff --check` 在收口时复核。Windows/D3D9 路径只做静态比对，没有该平台构建或运行证据，不能标 PASS。
