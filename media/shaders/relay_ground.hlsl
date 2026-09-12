struct GroundOut
{
	float4 position : POSITION;
	float3 worldPos : TEXCOORD0;
	float3 worldNormal : TEXCOORD1;
};
GroundOut ground_vs(float4 vertex : POSITION, float3 normal : NORMAL,
	uniform float4x4 wvpMat, uniform float4x4 wMat, uniform float4x4 iTWMat)
{
	GroundOut o;
	o.position = mul(wvpMat, vertex);
	o.worldPos = mul(wMat, vertex).xyz;
	o.worldNormal = normalize(mul(iTWMat, float4(normal, 0.0)).xyz);
	return o;
}
float hash21(float2 p)
{
	return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}
float4 ground_fs(GroundOut input, uniform sampler2D surfaceMap : register(s0),
	uniform float3 ambient, uniform float3 lightDif0, uniform float4 lightPos0,
	uniform float4 tint) : COLOR
{
	float2 slabSize = float2(2.4, 3.2);
	float2 grid = input.worldPos.xz / slabSize;
	grid.x += (floor(grid.y)-floor(floor(grid.y)*0.5)*2.0) * 0.5;
	float2 cell = floor(grid);
	float2 local = frac(grid);
	float2 edge = min(local, 1.0-local) * slabSize;
	float distanceToJoint = min(edge.x, edge.y);
	float pixel = max(length(fwidth(input.worldPos.xz)) * 0.5, 0.002);
	float joint = min(clamp((0.007+pixel-distanceToJoint)/(2.0*pixel), 0.0, 1.0), 0.007/pixel);
	float dustyEdge = 1.0-smoothstep(0.02, 0.12, distanceToJoint);
	float2 uv = local * 0.465 + 0.015;
	uv += (cell-floor(cell*0.5)*2.0) * 0.5;
	float3 albedo = tex2D(surfaceMap, uv).rgb;
	float variation = lerp(0.975, 1.025, hash21(cell));
	albedo *= variation * (1.0 - joint*0.20 - dustyEdge*0.018);
	float3 normalDir = normalize(input.worldNormal);
	float3 lightDir = normalize(lightPos0.xyz-lightPos0.w*input.worldPos);
	float lambert = max(dot(normalDir, lightDir), 0.0);
	return float4(albedo * tint.rgb * (ambient + lightDif0*lambert), 1.0);
}
