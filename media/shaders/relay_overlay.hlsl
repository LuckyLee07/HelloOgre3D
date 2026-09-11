void main_vp(
	in float4 position : POSITION,
	in float2 uv : TEXCOORD0,
	uniform float4x4 worldViewProj,
	out float4 outputPosition : POSITION,
	out float2 outputUv : TEXCOORD0)
{
	outputPosition = mul(worldViewProj, position);
	outputUv = uv;
}

float4 main_fp(
	float2 uv : TEXCOORD0,
	uniform float4 tint,
	uniform sampler2D overlayMap : register(s0)) : COLOR0
{
	return tex2D(overlayMap, uv) * tint;
}
