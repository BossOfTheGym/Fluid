#version 450 core

in VS_OUT
{
	in vec2 texCoords;
};

out vec4 color;

layout(binding = 0) uniform sampler2D image;
layout(binding = 1) uniform sampler2D field;

const float dt  = 0.1f;
const float eps = 1e-6f;
const float pi  = acos(-1.0f);
const float pi2 = 2.0f * pi;

void main()
{
	vec2 u;
	vec2 f;
	
	float len;

	u = texCoords;
	f = texture(field, u).xy;
	
	//f = u - vec2(0.5f);
	//len = length(f);
	//if(len > eps)
	//{
	//	f /= len;
	//}

	u -= dt * f;

	color = texture(image, u);
}