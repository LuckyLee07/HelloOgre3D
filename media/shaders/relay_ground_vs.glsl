#version 150
in vec4 vertex;
in vec3 normal;
uniform mat4 wvpMat;
uniform mat4 wMat;
uniform mat4 iTWMat;
out vec3 worldPos;
out vec3 worldNormal;
void main()
{
	gl_Position = wvpMat * vertex;
	worldPos = (wMat * vertex).xyz;
	worldNormal = normalize((iTWMat * vec4(normal, 0.0)).xyz);
}
