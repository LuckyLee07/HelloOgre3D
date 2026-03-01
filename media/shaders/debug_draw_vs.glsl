#version 150

in vec4 vertex;
in vec4 colour;

uniform mat4 wvpMat;

out vec4 vColor;

void main()
{
    gl_Position = wvpMat * vertex;
    vColor = colour;
}
