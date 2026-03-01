#version 150

in vec2 vUv;

uniform sampler2D skyboxMap;

out vec4 fragColor;

void main()
{
    fragColor = texture(skyboxMap, vUv);
}
