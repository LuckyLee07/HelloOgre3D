#version 150

in vec2 vUv;

uniform sampler2D overlayMap;
uniform vec4 tint;

out vec4 fragColor;

void main()
{
	fragColor = texture(overlayMap, vUv) * tint;
}
