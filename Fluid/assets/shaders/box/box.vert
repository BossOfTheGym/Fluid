#version 450 core

#define POSITION_LOCATION 0

layout(location = POSITION_LOCATION) in vec3 position;

uniform mat4 uPVM;

void main()
{
	gl_Position = uPVM * vec4(position, 1.0f);
}