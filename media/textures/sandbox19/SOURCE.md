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

## desert_grass.png

- 日期：2026-09-11
- 生成方式：Codex 内置 ImageGen；生成图从 1254×1254 等比重采样为 512×512 RGBA PNG。
- 用途：`Relay/Vegetation` 的 crossed-card 透明植被；材质以 alpha rejection 和低饱和 tint 控制远景边缘与亮度。
- 后处理：仅使用 Lanczos 缩放；未加入第三方素材。实机场景以透明裁切后的轮廓验收，不把生成图的低 alpha 边缘当作不透明颜色。

最终提示词：

> Use case: stylized-concept
> Asset type: transparent billboard texture for a third-person 3D tactical game
> Primary request: one isolated clump of hardy desert bunchgrass and small scrub, suitable for crossed-card vegetation in a warm arid relay-station courtyard
> Scene/backdrop: fully transparent background, no ground plane, no horizon
> Subject: a single dense asymmetrical tuft, dry straw blades mixed with a few muted sage-green leaves, roots hidden at the bottom center
> Style/medium: polished hand-authored realistic game asset, readable silhouette at medium distance, restrained detail, not cartoony
> Composition/framing: centered, full plant visible, generous transparent margin, front orthographic view, bottom edge aligned horizontally
> Lighting/mood: neutral soft studio lighting baked minimally, compatible with warm directional scene light
> Color palette: sand, ochre, dusty olive, muted sage
> Constraints: genuine alpha transparency; clean antialiased edges; no cast shadow; no soil patch; no text; no watermark; one vegetation clump only; square image
> Avoid: photographic background, white or checkerboard background, rocks, flowers, cactus, pot, frame, cropped leaves, neon saturation

## contact_shadow.png / ground_dust.png

- 日期：2026-09-11 至 2026-09-12
- 生成方式：仓库自制 SVG 源位于 `source/contact_shadow.svg` 与 `source/ground_dust.svg`，用 ImageMagick 6 的 `convert -background none ... -depth 8 PNG32:...` 栅格化为 RGBA PNG。
- 用途：`Relay/ContactShadow` 提供角色、掩体、设备和 relay 的低强度接地；`Relay/GroundDust` 提供不改变几何的地面旧化。
- 约束：两张图不含第三方素材，不提供物理、导航或视线语义。ImageMagick 6 对当前 SVG 渐变生成较宽的 alpha 场，最终强度由材质 tint 明确压低；验收以 GL 实机场景为准。
