void main_vp(
	in float4 position : POSITION,
	in float2 uv : TEXCOORD0,
	out float4 outputPosition : POSITION,
	out float2 outputUv : TEXCOORD0)
{
	outputPosition = position;
	outputUv = uv;
}

float4 main_fp(
	float2 uv : TEXCOORD0,
	uniform sampler2D sceneMap : register(s0)) : COLOR0
{
	float3 colour = tex2D(sceneMap, uv).rgb;
	float luminance = dot(colour, float3(0.2126, 0.7152, 0.0722));

	colour = saturate((colour - 0.46) * 1.075 + 0.46);
	float curvedLuminance = dot(colour, float3(0.2126, 0.7152, 0.0722));
	colour = lerp(float3(curvedLuminance, curvedLuminance, curvedLuminance), colour, 1.065);

	float tonalPosition = smoothstep(0.18, 0.78, luminance);
	colour *= lerp(float3(0.985, 1.002, 1.020), float3(1.022, 1.006, 0.975), tonalPosition);

	float2 centred = uv * 2.0 - 1.0;
	float edge = smoothstep(0.48, 1.72, dot(centred, centred));
	colour *= 1.0 - edge * 0.065;

	return float4(saturate(colour), 1.0);
}
