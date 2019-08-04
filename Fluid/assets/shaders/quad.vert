#version 450 core

layout(location = 1) vec3 position in;

out VS_OUT
{
	vec2 texCoord;
};

void main()
{
	gl_Position = vec4(position, 1.0);
	texCoord = position.xy;
}
