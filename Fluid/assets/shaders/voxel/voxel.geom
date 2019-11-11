#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;


uniform float uVoxelSize;
uniform mat4  uPVM;


void emitFace(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
	gl_Position = v0;
	EmitVertex();
	gl_Position = v1;
	EmitVertex();
	gl_Position = v2;
	EmitVertex();
	gl_Position = v3;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	vec3 center = gl_in[0].gl_Position.xyz;

	vec3 cubeVertices[8] = {
		  vec3(-1.0f, -1.0f, -1.0f)
		, vec3(-1.0f, -1.0f, +1.0f)
		, vec3(-1.0f, +1.0f, -1.0f)
		, vec3(-1.0f, +1.0f, +1.0f)
		, vec3(+1.0f, -1.0f, -1.0f)
		, vec3(+1.0f, -1.0f, +1.0f)
		, vec3(+1.0f, +1.0f, -1.0f)
		, vec3(+1.0f, +1.0f, +1.0f)
	};

	vec4 vertices[8] = {
		  uPVM * vec4(center + cubeVertices[0] * uVoxelSize / 2, 1.0f)
		, uPVM * vec4(center + cubeVertices[1] * uVoxelSize / 2, 1.0f)
		, uPVM * vec4(center + cubeVertices[2] * uVoxelSize / 2, 1.0f)
		, uPVM * vec4(center + cubeVertices[3] * uVoxelSize / 2, 1.0f)
		, uPVM * vec4(center + cubeVertices[4] * uVoxelSize / 2, 1.0f)
		, uPVM * vec4(center + cubeVertices[5] * uVoxelSize / 2, 1.0f)
		, uPVM * vec4(center + cubeVertices[6] * uVoxelSize / 2, 1.0f)
		, uPVM * vec4(center + cubeVertices[7] * uVoxelSize / 2, 1.0f)
	};

	emitFace(vertices[3], vertices[7], vertices[2], vertices[6]); // ok
	emitFace(vertices[6], vertices[7], vertices[4], vertices[5]); // ok
	emitFace(vertices[7], vertices[3], vertices[5], vertices[1]); // ok
	emitFace(vertices[3], vertices[2], vertices[1], vertices[0]); // ok
	emitFace(vertices[2], vertices[6], vertices[0], vertices[4]); // ok
	emitFace(vertices[4], vertices[5], vertices[0], vertices[1]); // ok
}