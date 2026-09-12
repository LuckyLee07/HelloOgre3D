struct ShadowVertex
{
	float4 position : POSITION;
	float4 projected : TEXCOORD0;
};
ShadowVertex caster_vs(float4 position : POSITION, uniform float4x4 wvpMat)
{
	ShadowVertex result;
	result.position = mul(wvpMat, position);
	result.projected = result.position;
	return result;
}
float4 caster_ps(ShadowVertex input) : COLOR0
{
	return float4(input.projected.z / input.projected.w, 0, 0, 1);
}
ShadowVertex receiver_vs(float4 position : POSITION,
	uniform float4x4 wvpMat, uniform float4x4 shadowMat)
{
	ShadowVertex result;
	result.position = mul(wvpMat, position);
	result.projected = mul(shadowMat, position);
	return result;
}
float4 receiver_ps(ShadowVertex input, uniform sampler2D shadowMap : register(s0),
	uniform float shadowBias, uniform float shadowFloor, uniform float texelSize) : COLOR0
{
	float3 projected = input.projected.xyz / input.projected.w;
	// D3D's projection already has a 0..1 depth range.
	float depth = projected.z;
	// Receiver-plane depth correction keeps PCF taps on sloped surfaces.
	float2 uvDx = ddx(projected.xy);
	float2 uvDy = ddy(projected.xy);
	float depthDx = ddx(depth);
	float depthDy = ddy(depth);
	float determinant = uvDx.x * uvDy.y - uvDx.y * uvDy.x;
	float2 gradient = float2(0.0, 0.0);
	if (abs(determinant) > 0.000000000001)
		gradient = float2(uvDy.y * depthDx - uvDx.y * depthDy,
			uvDx.x * depthDy - uvDy.x * depthDx) / determinant;
	float bias = shadowBias + min(0.005, dot(abs(gradient), float2(texelSize, texelSize)) * 0.75);
	float visibility = 1;
	if (input.projected.w > 0 && depth > 0 && depth < 1 &&
		all(projected.xy >= 0) && all(projected.xy <= 1))
	{
		// Bilinear PCF: four exact texel centres avoid point-sampled steps.
		float2 texelPosition = projected.xy / texelSize - 0.5;
		float2 base = floor(texelPosition);
		float2 fraction = texelPosition - base;
		visibility = 0.0;
		for (int y = 0; y <= 1; ++y)
			for (int x = 0; x <= 1; ++x)
			{
				float2 uv = (base + float2(x, y) + 0.5) * texelSize;
				float weight = (x == 0 ? 1.0 - fraction.x : fraction.x)
					* (y == 0 ? 1.0 - fraction.y : fraction.y);
				float storedDepth = tex2Dlod(shadowMap, float4(uv, 0, 0)).r;
				float receiverDepth = depth + dot(gradient, uv - projected.xy) - bias;
				visibility += weight * (receiverDepth <= storedDepth ? 1.0 : 0.0);
			}
	}
	return float4(lerp(shadowFloor, 1, visibility).xxx, 1);
}
