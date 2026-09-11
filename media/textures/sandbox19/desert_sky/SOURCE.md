# Desert relay sky source

Generated on 2026-09-11 with the built-in OpenAI image generation tool for this project.

## Final prompt

```text
Use case: stylized-concept
Asset type: production game environment skybox source panorama for Ogre3D, intended to be converted into six cubemap faces
Primary request: create a seamless 360-degree equirectangular panorama of a warm arid high-desert horizon that matches a tactical relay outpost courtyard
Scene/backdrop: pale blue afternoon sky with thin high clouds, layered ochre sandstone mountain ranges and dry atmospheric haze around the full horizon; distant terrain only, no nearby ground plane
Style/medium: polished realistic game environment backdrop, grounded military sci-fi visual tone, natural rather than painterly
Composition/framing: strict 2:1 equirectangular 360 panorama; continuous level horizon around the full width, horizon around 58 percent from the top; left and right edges must join seamlessly; zenith remains sky and nadir is muted dusty haze
Lighting/mood: warm late-afternoon desert light, clear readable contrast, no dramatic sunset
Color palette: sand, ochre, muted brown, pale cyan-blue, warm off-white cloud
Constraints: sky and far mountains only; no buildings, antennas, roads, people, vehicles, text, logos, borders, watermark, duplicated sun, close foreground objects, or visible panorama seam; preserve enough low-frequency detail for a real-time D3D9 skybox
```

The generated 1774×887 panorama was blended across its horizontal wrap boundary and saved as `desert_panorama.png`. The six `DesertRelay*1024.png` faces were sampled from that panorama with a 90-degree cubemap projection. `sandbox19_relay.material` consumes the faces in Ogre's front, back, right, left, up, down order.
