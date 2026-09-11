#version 150

in vec4 vertex;
in vec2 uv0;

out vec2 vUv;

void main()
{
	gl_Position = vertex;
	vUv = uv0;
}
