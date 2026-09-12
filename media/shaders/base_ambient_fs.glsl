#version 150

in vec2 vUv;

uniform sampler2D diffuseMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;
uniform vec3 ambient;
uniform vec4 matDif;

out vec4 fragColor;

void main()
{
    vec4 diffuseTex = texture(diffuseMap, vUv);
    vec4 aoTex = texture(aoMap, vUv);
    fragColor = diffuseTex * aoTex * vec4(ambient, 1.0) * vec4(matDif.rgb, 1.0);
    // Add once per surface, independent of scene lighting and diffuse tint.
    fragColor.rgb += texture(emissiveMap, vUv).rgb;
}
