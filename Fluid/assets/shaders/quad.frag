#version 450 core


in VS_OUT
{
	in vec2 texCoord;
};

out	vec4 color;

uniform sampler2D testTexture;

void main()
{
	color = texture2D(testTexture, texCoord);//vec4(1.0f, 0.5f, 0.0f, 1.0f);
}