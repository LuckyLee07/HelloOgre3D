#version 150
in vec4 vertex;
uniform mat4 wvpMat;
void main()
{
	gl_Position = wvpMat * vertex;
}
