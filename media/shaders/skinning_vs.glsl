#version 150

in vec4 vertex;
in vec2 uv0;
in vec3 normal;
in vec3 tangent;
in vec4 blendIndices;
in vec4 blendWeights;

uniform vec4 light_position;
uniform vec4 eye_position;
uniform mat4 worldMatrix3x4Array[60];
uniform mat4 viewProjectionMatrix;
uniform mat4 invworldmatrix;

out vec2 vUv;
out vec3 vLightVector;
out vec3 vHalfAngle;

void main()
{
    vec4 skinnedPos = vec4(0.0);
    vec3 skinnedNormal = vec3(0.0);
    vec3 skinnedTangent = vec3(0.0);

    for (int i = 0; i < 4; ++i)
    {
        int idx = int(blendIndices[i]);
        float w = blendWeights[i];
        mat4 skinMat = worldMatrix3x4Array[idx];
        skinnedPos += (skinMat * vec4(vertex.xyz, 1.0)) * w;
        skinnedNormal += (mat3(skinMat) * normal) * w;
        skinnedTangent += (mat3(skinMat) * tangent) * w;
    }

    gl_Position = viewProjectionMatrix * skinnedPos;
    vUv = uv0;

    vec3 newNormal = normalize(mat3(invworldmatrix) * skinnedNormal);
    vec3 newTangent = normalize(mat3(invworldmatrix) * skinnedTangent);
    vec3 binormal = cross(newTangent, newNormal);
    mat3 rotation = mat3(newTangent, binormal, newNormal);

    vec3 lightDir = normalize(light_position.xyz - (skinnedPos.xyz * light_position.w));
    vLightVector = normalize(rotation * lightDir);

    vec3 eyeDir = normalize((eye_position - skinnedPos).xyz);
    eyeDir = normalize(rotation * eyeDir);
    vHalfAngle = vLightVector + eyeDir;
}
