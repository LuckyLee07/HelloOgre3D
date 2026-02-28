#version 150

in vec4 vertex;
in vec2 uv0;

uniform mat4 wvpMat;

out vec2 vUv;

void main()
{
    gl_Position = wvpMat * vertex;
    vUv = uv0;
}
