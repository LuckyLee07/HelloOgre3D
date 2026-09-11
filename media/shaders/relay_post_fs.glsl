#version 150

in vec2 vUv;

uniform sampler2D sceneMap;

out vec4 fragColor;

void main()
{
	vec3 colour = texture(sceneMap, vUv).rgb;
	float luminance = dot(colour, vec3(0.2126, 0.7152, 0.0722));

	// A restrained S-curve and saturation lift restore material separation in
	// the deliberately bright desert lighting without crushing cover detail.
	colour = clamp((colour - 0.46) * 1.075 + 0.46, 0.0, 1.0);
	float curvedLuminance = dot(colour, vec3(0.2126, 0.7152, 0.0722));
	colour = mix(vec3(curvedLuminance), colour, 1.065);

	// Cool shadows and warm highlights reinforce team silhouettes and the amber
	// relay facade while keeping the neutral concrete close to its source hue.
	float tonalPosition = smoothstep(0.18, 0.78, luminance);
	colour *= mix(vec3(0.985, 1.002, 1.020), vec3(1.022, 1.006, 0.975), tonalPosition);

	vec2 centred = vUv * 2.0 - 1.0;
	float edge = smoothstep(0.48, 1.72, dot(centred, centred));
	colour *= 1.0 - edge * 0.065;

	fragColor = vec4(clamp(colour, 0.0, 1.0), 1.0);
}
