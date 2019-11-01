#ifndef VOXELIZER_H
#define VOXELIZER_H


#include <glWrap/Buffer/Buffer.h>
#include <glWrap/VertexArray/VertexArray.h>

struct VoxelData
{
	VertexArray vertexArray  = {};
	Buffer      vertexBuffer = {};
	float       voxelSize    = 0.0f;
};

VoxelData voxelize(int split);

VoxelData voxelizeV1(int split);


void testV1();

#endif