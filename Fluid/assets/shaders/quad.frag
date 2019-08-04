#version 450 core


in VS_OUT
{
	in vec2 texCoord;
};

out	vec4 color;

uniform sampler2D testTexture;

void main()
{
	color = texture2D(testTexture, texCoord);
}