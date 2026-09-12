#version 150

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec2 uv0;

uniform mat4 wMat;
uniform mat4 wvpMat;
uniform mat4 iTWMat;

out vec2 vUv;
out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec3 vWorldTangent;

void main()
{
    vec4 worldPos = wMat * vertex;
    gl_Position = wvpMat * vertex;
    vUv = uv0;
    vWorldPos = worldPos.xyz;
    vWorldNormal = normalize((iTWMat * vec4(normal, 0.0)).xyz);
    vWorldTangent = (wMat * vec4(tangent, 0.0)).xyz;
}
