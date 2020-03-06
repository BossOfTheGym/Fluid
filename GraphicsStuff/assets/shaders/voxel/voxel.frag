#version 450 core

in OUT_GEOM
{
	vec3 normal;
	vec3 position;
	vec3 bc;
} inData;

out vec4 outColor;


uniform vec3 uEyePos;

uniform float uVoxelSize;

// I'm lazy as fuck
const vec3 lightPos     = vec3(0.5f, 3.5f, 0.5f);
const vec3 lightColor   = vec3(1.0f, 1.0f, 1.0f);
const vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f);

//emerald :D
const vec3 kA  = vec3(0.0215f, 0.1745f, 0.0215f);
const vec3 kD  = vec3(0.07568f, 0.61424f, 0.07568f);
const vec3 kSp = vec3(0.633f, 0.727811f, 0.633f);
const vec3 kE  = vec3(0.5f, 0.25f, 0.9f); // not emerald :P

const float kSh = 0.6f * 128;

const float kAtt  = 1.0f;
const float kAttL = 0.1f;
const float kAttQ = 0.1f;

const float y = 2.0f;

void main()
{
	vec3 normal  = normalize(inData.normal);
	vec3 fragPos = inData.position;

	vec3 lightDir   = lightPos - fragPos;
	float lightDist = length(lightDir);
	lightDir /= lightDist;

	vec3 viewDir = normalize(uEyePos - fragPos);
	vec3 halfDir = normalize(lightDir + viewDir);

	float d = max(dot(lightDir, normal), 0.0);            
	float s = max(dot(halfDir, normal), 0.0);

	float att = (kAtt + kAttL * lightDist + kAttQ * lightDist * lightDist);

	vec3 color = (kA * ambientColor + kD * d * lightColor + kSp * pow(s, kSh) * lightColor) / att;
	color = pow(color, vec3(1.0f / y));
	
	vec3 bc = inData.bc;
	float eps = 0.05;
	if (bc.y <= eps || bc.z <= eps)
	{
		//color = kE;
	}

	outColor = vec4(color, 1.0f);
}