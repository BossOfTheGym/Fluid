#version 450 core

in VS_OUT
{
	vec2 texCoords;
};

out vec4 color;

layout(binding = 0) uniform sampler2D image;
layout(binding = 1) uniform sampler2D field;

void main()
{
	
}