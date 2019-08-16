#version 450 core

in VS_OUT
{
	in vec2 texCoords;
};

out	vec4 color;

layout(binding = 0) uniform sampler2D testTexture;

uniform float t;

void main()
{
	//color = vec4(sin(t) * texture(testTexture, texCoords).xyz, 1.0f);
	color = texture(testTexture, texCoords);
}