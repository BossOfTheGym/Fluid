#version 450 core

#define POSITION_LOCATION 0
#define TEXTURE_LOCATION  1

layout(location = POSITION_LOCATION) in vec3 position;
layout(location = TEXTURE_LOCATION)  in vec2 textureCoords;

out VS_OUT
{
	out vec2 texCoords;
};

void main()
{
	gl_Position = vec4(position, 1.0);
	texCoords = textureCoords;
}
