# Sandbox19 表面资产来源

## supply_case_wear_v1.png

- 日期：2026-09-12。内置 ImageGen 全新生成，实际 1254×1254 RGB，原样复制到本目录；没有额外裁切/绘图或第三方输入图。
- 用途：自制补给箱的涂层磨损 albedo，乘以各 submesh 的橄榄色 tint；不是 normal/roughness 图，也不作为完整 PBR 材质。
- 原始输出：`exec-5bd163d4-8b1b-4f83-aa7e-c4e0548800f3.png`。生成图不保证严格数学无缝，按近/远实机检查接缝及尺度。
- 最终提示词：

> Use case: photorealistic-natural. Asset type: seamless square neutral-grey wear/albedo texture for painted steel supply cases in a real-time 3D desert relay outpost. Create a new 1024x1024 texture that fills the entire square, perfectly orthographic, flat diffuse illumination, no lighting baked in. This will be multiplied by an olive-green material tint, so keep the texture monochrome light grey with average sRGB approximately 0.78 and VERY LOW contrast. Fine powder-coat grain, gentle rubbed areas, sparse tiny shallow scratches, a few small paint chips exposing slightly darker grey primer, subtle dusty scuffs. Physical coverage about 2 metres. Detail should look maintained but used, not rusty or ruined. Uniform brightness, seamless horizontal and vertical edges, no distinctive large scratches, no directional highlights, no shadows, no vignette, no panels, seams, bolts, vents, borders, lettering, logos or objects. Surface material texture only, not a render of a crate.

## courtyard_concrete_normal_v1.png

- 日期：2026-09-12
- 生成方式：以仓库现有 `courtyard_concrete_diffuse.png` 为唯一编辑参照，使用 Codex 内置 ImageGen 生成 1254×1254 RGB 切线空间法线图；用 `sips -z 1024 1024` 缩为当前 1024×1024 PNG。原始生成图位于本机 Codex 输出目录，不作为游戏运行资源；仓库内最终图为真源。未叠加第三方图像。
- 用途：`Relay/Concrete` 和 `Relay/Cover` 的 `normalMap`，对应其现有 albedo。`Relay/Ground`、`Relay/MainRoute` 与 `Relay/Facade` 仍使用平法线；不要把此图套到不配对的铺地或门面上。
- 验收：macOS GL3+ 日志确认加载 1024×1024 RGB 图，实机场景与完整自然对局见 `tmp/sandbox19-normal-final-longrun-20260912/`；Windows D3D9 尚未运行。ImageGen 参照编辑不保证每条裂纹逐像素重合或严格无缝，以上仍以最终 UV 平铺的实机观感为准。

最终提示词：

> Use case: stylized-concept
> Asset type: tangent-space RGB normal map texture for a 3D tactical game's existing concrete wall and low barrier material.
> Input images: Image 1 is the edit target, the exact existing square diffuse/albedo concrete texture. Preserve its crack, chip, aggregate and wear positions as closely as possible; convert its surface relief to a matching NORMAL MAP, not to another albedo image.
> Primary request: output a restrained physically plausible OpenGL-style +Z tangent-space normal map. Mostly neutral violet-blue RGB (128,128,255), with shallow chipped/cracked regions recessed and fine concrete aggregate relief. Keep broad smooth plaster zones nearly flat.
> Composition/framing: exact square, edge-to-edge orthographic texture, same 1:1 mapping and surface features as Image 1, no borders or central focal point; opposing edges should tile.
> Lighting/mood: no lighting or shadows are baked into the map; pixels encode only surface normal direction.
> Constraints: output only the normal-map bitmap; no rendered preview, no sphere/cube, no perspective, no albedo colors, no AO, no vignetting, no typography, logos, watermarks or extra objects. Preserve input image's layout and avoid strong noisy ridges or black line artifacts.

## courtyard_paving_diffuse_v2.png

- 日期：2026-09-12
- 生成方式：Codex 内置 ImageGen 生成 1254×1254 RGB 原图；为兼容旧 D3D9 纹理尺寸限制，用 `sips -z 1024 1024` 等比重采样为当前 1024×1024 PNG，不覆盖已有 `courtyard_concrete_diffuse.png`。未进行其它图像合成或手工修饰。
- 用途：供 `Relay/Ground` 与 `Relay/MainRoute` 的整片庭院混凝土铺地使用；中路仅以较亮 tint 区分。墙体与掩体保留原贴图；碰撞、导航均不改。
- 验收：原图 2×2 平铺预览位于 `tmp/sandbox19-paving-tile-test-20260912.png`；最终 1024×1024 资源在 GL 日志确认加载，实机抓帧位于 `tmp/sandbox19-paving-gl-preview-20260912/paving_1024_03500ms.png`。平铺缝数学逐像素一致性与 Windows D3D9 材质显示仍须分别确认。

最终提示词：

> Use case: stylized-concept
> Asset type: square, seamless tileable albedo/diffuse texture for the paved central courtyard of an existing 3D tactical game; intended to repeat every ~4 metres on a ground plane.
> Primary request: a convincing aged warm-gray concrete paving surface like a practical desert relay outpost, with broad offset rectangular slab joints, subtle chipped edges, sparse hairline cracks, sand gathered lightly in a few joints, fine aggregate and scuffed foot traffic. Moderately varied value so it remains readable behind moving soldiers; material detail at several scales, not noisy decoration.
> Composition/framing: perfectly top-down orthographic surface scan filling the square edge-to-edge, with no central focal point; match patterns across opposite edges for truly seamless repeat.
> Lighting/mood: even neutral ambient albedo, no directional highlights, cast shadows, vignettes, atmospheric perspective or baked lighting.
> Palette: warm pale concrete beige/gray, muted tan dust; not saturated orange, not dark brown.
> Constraints: pure square texture, no people, props, buildings, grass, weapons, markings, typography, logos, borders, perspective, or raised geometry. Four sides must tile cleanly; realistic wear at game scale, production-ready hand-authored PBR albedo look.

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

## signal_glow.png

- 日期：2026-09-12
- 生成方式：用 ImageMagick 6 的 `convert -size 256x256 radial-gradient:'rgba(255,255,255,0.96)-rgba(255,255,255,0)' -depth 8 PNG32:signal_glow.png` 直接生成 256×256 RGBA 径向渐变；命令即完整可复现来源。
- 用途：中继站门面、桅杆、庭院入口与集合区的状态光晕；白色径向亮度与 alpha 只提供软遮罩，最终琥珀/青色及强度由 `Relay/Signal*` 材质控制。
- 约束：不含第三方素材，不提供灯光、碰撞、导航或 AI 语义；只作为现有任务状态的克制渲染反馈。

## 2026-09-12 视觉 goal：克制铺地 v3

- 文件：`courtyard_paving_diffuse_v3.png`。通过内置 ImageGen 为本项目生成，保留原始输出，未覆盖 v2；实际尺寸为 1254×1254 RGB（请求1024×1024，工具返回原生尺寸不同），需以实际运行日志验证加载。无外部品牌/照片/授权素材。
- 用途：中继站庭院与中路共用低对比混凝土 albedo，减少旧铺地的重复深格和黄色底色；不修改物理或导航。法线继续平法线，不将 albedo 当作高度或 PBR 完整材质。
- 工具：内置 `image_gen`；原始输出 `exec-607d920a-aa88-4cc6-b0b4-5d0dc79f6a95.png`。视觉取舍、三阶段实机和验证见 [视觉 goal 计划](../../../docs/dev-design/plans/2026-09-12-sandbox19-visual-goal.md)。

生成提示词：

> Use case: photorealistic-natural. Asset type: square seamless repeating base-color/albedo texture for a real-time 3D tactical game courtyard, covering a physical 6 metre by 6 metre area. Generate a new 1024 x 1024 image, perfectly orthographic top-down scanned surface, fills entire square, uniform flat neutral illumination without directional shadows or AO baked in. Material: weathered pale warm-grey poured concrete paving in a dry industrial desert outpost; fine matte aggregate and restrained pores, a few tiny chips and hairline cracks, light irregular dusty deposits that are subtle rather than dirty stains. Four large concrete panels in a simple 2x2 arrangement, extremely thin desaturated expansion joints, edges seamlessly tile horizontally and vertically. Keep brightness even across the whole image and low overall contrast. Palette approximately neutral limestone grey/beige, average albedo sRGB around 0.66 0.65 0.61, NO strong orange/yellow tint. Panel joints should not form dark thick grids when repeated. Realistic understated physically plausible surface detail, no repeating decorative motif, no large distinctive cracks or blobs, no bricks, no cobblestones, no grass, no objects, no scene perspective, no highlights, no vignetting, no text or watermark. This is a texture asset only, not a rendered scene or concept illustration.
