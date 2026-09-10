# P0 目标画面生成记录

使用内置 imagegen；图片仅为概念稿，不是游戏运行证据。所有选用图片已保存至本目录，未购买或下载外部素材。实际基线截图由当前 Release 的 RuntimeRenderCapture 生成。

## 选用版本

| 用途 | 文件 | 来源与检查 |
|---|---|---|
| 正常战斗 | target-combat-v2.png | 统一平层侧路、普通交战与中性命令栏；检查文字、角色数量、环境和UI一致性 |
| 双人集火 | target-command.png | 同一场地与HUD，增加选择环、目标框、队友状态和接受提示 |
| 任务结算 | target-result-v2.png | 同一视觉语言，统计明确为示例；修正前景指挥官误持枪 |

`target-combat.png` 与 `target-result.png` 是未选用草稿，分别存在楼梯/指挥官持枪差异，不作为实施目标。

## 初稿

```text
Use case: stylized-concept / game ui-mockup.
Create ONE widescreen 16:9 target gameplay image for a small indie tactical squad game in an older Ogre 3D forward renderer. This is a P0 visual design proposal, NOT a real screenshot. The supplied image is a technical baseline reference only: retain the practical idea of one unarmed white-armored commander and two white-armored rifle allies, simple humanoid rigs, dark-armored enemies, a small navigable arena. Redesign presentation substantially; do not retain the debug lines, sky dominance, concrete cubes, oversized help panels or old HUD.

Recommended art direction: a coherent modest sunlit concrete relay outpost, restrained stylized 3D game rendering, warm off-white concrete, pale sand ground, muted sage metal panels, charcoal equipment, subtle wear, clean readable silhouettes. Reusable modular walls, low waist-high barriers, a few boxy supply crates, simple antenna behind a low control building at the far end. Clear central approach and a sheltered left route; avoid clutter, foliage, huge architecture or floating platform edges. Ground plane fills ~80% of world view, distant low walls/background fill the rest. One directional warm sunlight with believable crisp simple shadows, cool ambient fill. Modest diffuse/specular materials, no photorealistic PBR, no volumetric fog, ray tracing, depth of field, bloom or cinematic lens tricks.

Camera: elevated third-person tactical follow, looking down roughly 38 degrees, wide enough to see two allies and 2 dark enemy soldiers ahead; not overhead orthographic/isometric. Commander small in lower middle, two allies in middle left and middle right, enemies near far entrance. White/off-white armor resembles practical low-poly existing humanoids, cyan small identification plates and compact cyan friend indicators; enemy markers orange-red diamonds. Minimal muzzle flashes and 1-2 thin bullet streaks indicate normal combat. No drawn debug paths, navmesh grid, wireframes, visibility cones or long text over heads.

UI: polished, deliberately restrained, sharp legible typography, modern condensed headings with clean sans-serif text. Flat deep charcoal translucent panels (#17242B), warm off-white text (#F1EDDF), muted cyan (#65D1CF) for friend/selection and amber (#E7AE61) for mission. Squared corners, fine separators, small generous margins, no neon sci-fi ornament or bevels. UI takes at most ~20% of screen. Top-left compact objective block: small "RELAY OUTPOST", heading "SECURE THE COURTYARD", subline "Hostiles remaining 2". Top-right tiny "01:24" and pause icon. Bottom-left two compact horizontal soldier cards with tiny helmet portraits, labels "1  ALPHA" and "2  BRAVO", readable HP bars and state "ENGAGING"; not huge decorative portraits. Bottom-center modest command strip: "F  FOCUS", "T  FALL BACK", "G  RALLY", with clean simple monochrome icons. Bottom-right small commander status "COMMANDER  100" and help hint "TAB  Select squad". No minimap, no crosshair, no enormous tutorial. Keep the center battlefield unobstructed. Put the exact small label "P0 CONCEPT / NOT IN-GAME" at the very bottom left. English UI is intentional for the current font pipeline.
The final image should feel like an attainable carefully art-directed indie game screenshot, not concept illustration of an impossible AAA game. High quality composition and UI typography; one coherent full-screen image, no collage, no border outside the game image.
```

## 集火状态

```text
Use case: precise-object-edit / game ui-mockup.
Edit the supplied concept image into the SELECTED SQUAD / FOCUS ORDER state of the exact same indie tactical game. Retain the same scene, camera, three white armored friendly figures, two dark enemies, warm off-white concrete, sage equipment, typography, palette and all HUD positions. This is a designed concept, never an actual game screenshot.
Small scene feasibility correction: turn the stairs on the left into a flat paved side passage at the SAME ground level as the courtyard, with low wall occlusion; no raised accessible route. Keep the rest of the composition.
Both rifle allies are selected: thin cyan segmented ground rings under the ally left and ally right, and a modest cyan leading edge on both ALPHA and BRAVO cards. The commander remains a separate unarmed figure. Both allies visibly aim toward the dark enemy on the center-left near the doorway. Give that one enemy a clean small orange corner-bracket target marker and a tiny "FOCUS" label. Do not draw long paths or giant circles, no x-ray silhouettes through walls.
Bottom center F FOCUS command becomes active with a restrained amber edge, other controls neutral. Above the command strip add a compact transient charcoal confirmation "FOCUS CONFIRMED  /  2 UNITS". ALPHA and BRAVO states change from ENGAGING to FOCUS, retain actual-looking HP bars. Top objective text remains "SECURE THE COURTYARD" and "Hostiles remaining 2". Timer "01:28".
Show a small precise pointer near the focused enemy. Keep UI readable and unobtrusive, no modal tutorial. Bottom-left must retain exact label "P0 CONCEPT / NOT IN-GAME". Widescreen, one image. Match the reference visual language closely, do not introduce a new art style.
```

## 正常战斗选用稿

```text
Use case: precise-object-edit / game ui-mockup.
Create the NORMAL COMBAT state of the same game concept shown in the reference. Preserve exactly the art direction, lighting, flat left side passage, outpost structures, all 3 friendly soldiers, 2 enemies, HUD rectangles and typeface. Keep the camera consistent. This image is the canonical normal combat concept.
Remove the orange enemy focus brackets and the FOCUS label. Remove the "FOCUS CONFIRMED / 2 UNITS" toast. Turn F FOCUS button back to the same neutral charcoal treatment as the other two buttons. Remove cyan highlight edges on the two squad cards and remove selected ground rings from the two rifle allies, but keep small cyan friendly triangles over them. Change the state text on BOTH ALPHA and BRAVO cards to "ENGAGING". Both independently engage different enemies. Preserve the "Hostiles remaining 2" mission. Timer should read "01:24". Preserve commander status and command shortcuts. The commander should remain unarmed, distinguishable by a small subdued circle at his feet.
Retain exact small footer "P0 CONCEPT / NOT IN-GAME".
Constraints: the left route stays flat, no staircase or additional vertical navigation. Keep the empty approach in front of the commander. Do not add fog, bloom, fancy shaders, extra HUD or a minimap. One wide image, clean production design mockup that matches the reference, not a rendered-in-game claim.
```

## 结算初稿

```text
Use case: precise-object-edit / game ui-mockup.
Create the MISSION COMPLETE screen for the same small tactical game in the reference. Preserve exactly the warm off-white concrete relay outpost, flat left passage, subdued sage equipment, same camera and overall environment, same white-armored commander and two teammates. The fight has ended, all three friendlies are standing calmly with weapons lowered, enemies and muzzle flashes/tracers/target markers gone. No gore. Do not change it into a cinematic or another location.
Replace ALL combat HUD blocks with one polished centered mission debrief panel over a simple uniform dark translucent scrim; background remains recognizably the same actual scene, no depth-of-field or blur requirement.
Panel art: deep charcoal #17242B, thin subtle borders, warm off-white text, one restrained cyan completion icon at top. Consistent condensed headings and clean body text from reference. Spacious hierarchy, no neon, no decorative sci-fi frames, no medals, stars or fake progression.
Exact panel text:
small eyebrow "RELAY OUTPOST"
large title "MISSION COMPLETE"
subtitle "Courtyard secured. Squad regrouped."
thin separator
three neatly aligned result columns:
"TIME" "03:42"
"SQUAD SAFE" "2 / 2"
"HOSTILES" "4 / 4"
below a small section:
"YOUR ORDERS"
"9 issued  /  7 completed  /  2 replaced"
small muted line "Illustrative results"
bottom two clear flat buttons: primary cyan "PLAY AGAIN", secondary outlined "MAIN MENU".
Do not add extra statistics. All text perfectly readable, all numbers are illustrative concept values.
Preserve a small footer outside the panel, bottom left, exact text "P0 CONCEPT / NOT IN-GAME".
One widescreen 16:9 image. This is a product visual design proposal rather than a real game capture. Match the reference style and material detail closely.
```

## 结算修正

```text
Use case: precise-object-edit.
Make ONE correction to this game concept image. The central foreground commander MUST be unarmed. Remove the dark firearm visible at the commander's right hand/hip, leaving both hands relaxed and empty. Keep his white armor, position and size unchanged. The two rifle teammates at left and right may keep their lowered rifles.
Preserve absolutely everything else: composition, camera, background, lighting, colors, debrief panel, all exact text and illustrative numbers, both buttons, and bottom-left "P0 CONCEPT / NOT IN-GAME" label. Do not move or redesign any UI. Output the same widescreen image with only that weapon removed.
```
