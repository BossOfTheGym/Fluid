#ifndef MESH_H
#define MESH_H

#include "primitives.h"

struct Mesh
{
	std::vector<Triangle> triangles{};
};

struct IndicesMesh
{
	std::vector<Vec3> points{};
	std::vector<int> indices{};
};

Mesh indicesToTriangle(const IndicesMesh& indicesMesh);

#endif