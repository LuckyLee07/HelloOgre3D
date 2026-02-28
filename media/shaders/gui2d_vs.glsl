#version 150

in vec4 vertex;
in vec4 colour;
in vec2 uv0;

out vec2 oUv;
out vec4 oColor;

void main()
{
    gl_Position = vertex;
    oUv = uv0;
    oColor = colour;
}
