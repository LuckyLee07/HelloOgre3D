#version 150

in vec4 vertex;
in vec4 colour;
in vec2 uv0;

uniform mat4 worldViewProj;

out vec2 oUv;
out vec4 oColor;

void main()
{
    gl_Position = worldViewProj * vertex;
    oUv = uv0;
    oColor = colour;
}
