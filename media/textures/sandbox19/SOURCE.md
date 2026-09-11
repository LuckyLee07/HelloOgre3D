# Sandbox19 表面资产来源

## relay_facade_diffuse.png

- 日期：2026-09-11
- 生成方式：Codex 内置 ImageGen；生成图从 1254×1254 等比重采样为 1024×1024 PNG，供 Ogre 纹理资源使用。
- 用途：`Relay/Facade` 的漫反射/albedo；只用于中继站主体面板，不作为法线、粗糙度或几何信息。
- 后处理：仅缩放到 1024×1024；未添加文字、标志或第三方素材。

最终提示词：

> Use case: stylized-concept
> Asset type: seamless tileable game texture for a legacy Ogre 3D relay-station facade
> Primary request: a square diffuse/albedo texture of weathered warm gray-green industrial relay wall panels, with restrained panel seams, sparse recessed bolts, subtle edge wear, fine dust and a few muted ochre service accents
> Style/medium: realistic game-environment material, crisp enough for mid-distance third-person gameplay, restrained military-industrial design
> Composition/framing: perfectly orthographic flat surface filling the entire square; evenly distributed detail with no focal object
> Lighting/mood: neutral flat albedo reference, no directional light, no cast shadows, no baked highlights or ambient occlusion
> Color palette: warm limestone gray, desaturated sage/olive green, very small muted ochre accents
> Materials/textures: painted metal and fiber-cement panel character, fine grain and believable wear, medium and high frequency detail
> Constraints: genuinely seamless on all four edges; no perspective; no scene objects; no doors or windows; no text; no numbers; no logos; no symbols; no watermark; no large cracks; no strong contrast; no PBR preview sphere; output only the flat square texture

视觉生成不能证明边缘数学上逐像素一致；场景验收应以实际 UV 重复后的接缝和缩放为准。
