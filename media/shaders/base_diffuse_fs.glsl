#version 150

in vec2 vUv;
in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec3 vWorldTangent;

uniform vec3 lightDif0;
uniform vec4 lightPos0;
uniform vec4 lightAtt0;
uniform vec3 lightSpec0;
uniform vec4 matDif;
uniform vec4 matSpec;
uniform float matShininess;
uniform vec3 camPos;
uniform sampler2D diffuseMap;
uniform sampler2D specMap;
uniform sampler2D normalMap;

out vec4 fragColor;

void main()
{
    vec3 normalDir = normalize(vWorldNormal);
    vec3 tangentDir = vWorldTangent - normalDir * dot(vWorldTangent, normalDir);
    // Some legacy meshes have no tangent stream. Leave their old geometric
    // lighting intact instead of normalising a zero vector.
    if (dot(tangentDir, tangentDir) > 0.000001)
    {
        tangentDir = normalize(tangentDir);
        vec3 bitangentDir = normalize(cross(tangentDir, normalDir));
        vec3 texNormal = texture(normalMap, vUv).rgb * 2.0 - 1.0;
        normalDir = normalize(tangentDir * texNormal.x + bitangentDir * texNormal.y + normalDir * texNormal.z);
    }
    vec3 lightDir = normalize(lightPos0.xyz - (lightPos0.w * vWorldPos));
    float ndotl = max(dot(normalDir, lightDir), 0.0);

    float lightDist = length(lightPos0.xyz - vWorldPos) / max(lightAtt0.x, 0.0001);
    float attenuation = max(1.0 - (lightDist * lightDist), 0.0);

    float spot = 1.0;

    vec3 viewDir = normalize(camPos - vWorldPos);
    vec3 halfVec = normalize(lightDir + viewDir);
    float specAmount = pow(max(dot(normalDir, halfVec), 0.0), max(matShininess, 1.0));

    vec4 diffuseTex = texture(diffuseMap, vUv);
    vec4 specTex = texture(specMap, vUv);
    vec3 diffuseContrib = ndotl * lightDif0 * diffuseTex.rgb * matDif.rgb;
    vec3 specContrib = specAmount * lightSpec0 * specTex.rgb * matSpec.rgb;

    vec3 lit = (diffuseContrib + specContrib) * attenuation * spot;
    fragColor = vec4(lit, diffuseTex.a);
}
