#version 150

in vec2 vUv;
in vec3 vWorldPos;
in vec3 vWorldNormal;

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
