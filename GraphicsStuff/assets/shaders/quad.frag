#version 450 core

in VS_OUT
{
	in vec2 texCoords;
};

out	vec4 color;

layout(binding = 0) uniform sampler2D testTexture;

void main()
{
	color = texture(testTexture, texCoords);
}