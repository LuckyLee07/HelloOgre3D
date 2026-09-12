#version 150
in vec4 vertex;
uniform mat4 wvpMat;
uniform mat4 shadowMat;
out vec4 shadowPosition;
void main()
{
	gl_Position = wvpMat * vertex;
	shadowPosition = shadowMat * vertex;
}
