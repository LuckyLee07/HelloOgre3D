#version 150

in vec2 oUv;
in vec4 oColor;

uniform sampler2D atlas;
out vec4 fragColor;

void main()
{
    fragColor = texture(atlas, oUv) * oColor;
}
