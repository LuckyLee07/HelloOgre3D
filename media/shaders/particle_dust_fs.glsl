#version 150
in vec2 oUv;
in vec4 oColor;
out vec4 fragColor;
void main()
{
	vec2 p = oUv * 2.0 - 1.0;
	float mask = max(0.0, exp(-dot(p, p) * 4.0) - 0.018) / 0.982;
	fragColor = vec4(oColor.rgb, oColor.a * mask);
}
