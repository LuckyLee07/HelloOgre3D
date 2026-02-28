#version 150

in vec4 vertex;
in vec3 normal;
in vec2 uv0;

uniform mat4 wMat;
uniform mat4 wvpMat;

out vec2 vUv;
out vec3 vWorldPos;
out vec3 vWorldNormal;

void main()
{
    vec4 worldPos = wMat * vertex;
    gl_Position = wvpMat * vertex;
    vUv = uv0;
    vWorldPos = worldPos.xyz;
    vWorldNormal = normalize((wMat * vec4(normal, 0.0)).xyz);
}
