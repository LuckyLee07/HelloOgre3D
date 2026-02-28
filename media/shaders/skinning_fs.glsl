#version 150

in vec2 vUv;
in vec3 vLightVector;
in vec3 vHalfAngle;

uniform vec4 ambient;
uniform vec3 lightDif0;
uniform vec3 lightSpec0;
uniform vec4 matDif;
uniform float matShininess;
uniform sampler2D diffuseMap;
uniform sampler2D specMap;
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;

out vec4 fragColor;

void main()
{
    vec3 base = texture(diffuseMap, vUv).rgb * matDif.rgb;
    vec3 bump = texture(normalMap, vUv).rgb;
    vec3 specularTex = texture(specMap, vUv).rgb;
    vec3 emissive = texture(emissiveMap, vUv).rgb;

    vec3 n = normalize((bump * 2.0) - 1.0);
    vec3 l = normalize(vLightVector);
    vec3 h = normalize(vHalfAngle);

    float ndotl = max(dot(n, l), 0.0);
    float ndoth = max(dot(n, h), 0.0);

    vec3 litColor = max(emissive, base * ambient.rgb) +
                    (base * lightDif0 * ndotl) +
                    (lightSpec0 * specularTex * pow(ndoth, max(matShininess, 1.0)));

    fragColor = vec4(litColor, 1.0);
}
