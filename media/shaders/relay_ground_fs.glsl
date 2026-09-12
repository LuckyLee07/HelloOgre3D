#version 150
in vec3 worldPos;
in vec3 worldNormal;
uniform sampler2D surfaceMap;
uniform vec3 ambient;
uniform vec3 lightDif0;
uniform vec4 lightPos0;
uniform vec4 tint;
out vec4 fragColor;

float hash21(vec2 p)
{
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main()
{
	// Metre-sized slabs continue across the twelve separate floor meshes.
	vec2 slabSize = vec2(2.4, 3.2);
	vec2 grid = worldPos.xz / slabSize;
	grid.x += mod(floor(grid.y), 2.0) * 0.5;
	vec2 cell = floor(grid);
	vec2 local = fract(grid);
	vec2 edge = min(local, 1.0-local) * slabSize;
	float distanceToJoint = min(edge.x, edge.y);
	float pixel = max(length(fwidth(worldPos.xz)) * 0.5, 0.002);
	float joint = min(clamp((0.007+pixel-distanceToJoint)/(2.0*pixel), 0.0, 1.0), 0.007/pixel);
	float dustyEdge = 1.0-smoothstep(0.02, 0.12, distanceToJoint);
	// Sample inside one panel of the existing albedo, avoiding its baked grid.
	vec2 uv = local * 0.465 + 0.015;
	uv += vec2(mod(cell.x, 2.0), mod(cell.y, 2.0)) * 0.5;
	vec3 albedo = texture(surfaceMap, uv).rgb;
	float variation = mix(0.975, 1.025, hash21(cell));
	albedo *= variation * (1.0 - joint*0.20 - dustyEdge*0.018);
	vec3 normalDir = normalize(worldNormal);
	vec3 lightDir = normalize(lightPos0.xyz-lightPos0.w*worldPos);
	float lambert = max(dot(normalDir, lightDir), 0.0);
	fragColor = vec4(albedo * tint.rgb * (ambient + lightDif0*lambert), 1.0);
}
