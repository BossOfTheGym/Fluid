#pragma once

#include <Primitives/Primitives.h>

#include <vector>

// TODO : wrap Int32 into Mesh and correct its usage across project
// TODO : rewrite this
namespace mesh
{
	using math::Vec3;
	using math::Int32;

	using primitive::Triangle;


	struct Mesh
	{
		std::vector<Triangle> triangles{};
	};

	struct IndicesMesh
	{
		std::vector<Vec3>  points{};
		std::vector<Int32> indices{};
	};


	Mesh indicesToTriangle(const IndicesMesh& indicesMesh);

	Mesh testMesh();
}