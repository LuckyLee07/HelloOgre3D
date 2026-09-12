#version 150
in vec4 shadowPosition;
uniform sampler2D shadowMap;
uniform float shadowBias;
uniform float shadowFloor;
uniform float texelSize;
out vec4 fragColor;
void main()
{
	vec3 projected = shadowPosition.xyz / shadowPosition.w;
	// Ogre maps texture XY, but retains OpenGL's clip Z range.
	float depth = projected.z * 0.5 + 0.5;
	// Receiver-plane depth correction keeps PCF taps on sloped surfaces.
	vec2 uvDx = dFdx(projected.xy);
	vec2 uvDy = dFdy(projected.xy);
	float depthDx = dFdx(depth);
	float depthDy = dFdy(depth);
	float determinant = uvDx.x * uvDy.y - uvDx.y * uvDy.x;
	vec2 gradient = vec2(0.0, 0.0);
	if (abs(determinant) > 0.000000000001)
		gradient = vec2(uvDy.y * depthDx - uvDx.y * depthDy,
			uvDx.x * depthDy - uvDy.x * depthDx) / determinant;
	float bias = shadowBias + min(0.005, dot(abs(gradient), vec2(texelSize, texelSize)) * 0.75);
	float visibility = 1.0;
	if (shadowPosition.w > 0.0 && depth > 0.0 && depth < 1.0 &&
		all(greaterThanEqual(projected.xy, vec2(0.0))) &&
		all(lessThanEqual(projected.xy, vec2(1.0))))
	{
		// Bilinear PCF: four exact texel centres avoid point-sampled steps.
		vec2 texelPosition = projected.xy / texelSize - 0.5;
		vec2 base = floor(texelPosition);
		vec2 fraction = texelPosition - base;
		visibility = 0.0;
		for (int y = 0; y <= 1; ++y)
			for (int x = 0; x <= 1; ++x)
			{
				vec2 uv = (base + vec2(x, y) + 0.5) * texelSize;
				float weight = (x == 0 ? 1.0 - fraction.x : fraction.x)
					* (y == 0 ? 1.0 - fraction.y : fraction.y);
				float storedDepth = textureLod(shadowMap, uv, 0.0).r;
				float receiverDepth = depth + dot(gradient, uv - projected.xy) - bias;
				visibility += weight * (receiverDepth <= storedDepth ? 1.0 : 0.0);
			}
	}
	fragColor = vec4(vec3(mix(shadowFloor, 1.0, visibility)), 1.0);
}
