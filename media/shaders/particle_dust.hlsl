float4 main_fp(float2 uv : TEXCOORD0, float4 color : COLOR0) : COLOR
{
	float2 p = uv * 2.0 - 1.0;
	float mask = max(0.0, exp(-dot(p, p) * 4.0) - 0.018) / 0.982;
	return float4(color.rgb, color.a * mask);
}
