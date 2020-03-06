#include "Mesh.h"


namespace mesh
{
	using math::operator "" _FL;

	Mesh indicesToTriangle(const IndicesMesh& indicesMesh)
	{
		Mesh result;

		Int32 i = 0;
		Int32 e = static_cast<int>(indicesMesh.indices.size());
		while (i + 2 < e)
		{
			auto i0 = indicesMesh.indices[i];
			auto i1 = indicesMesh.indices[i + 1];
			auto i2 = indicesMesh.indices[i + 2];

			auto& p0 = indicesMesh.points[i0];
			auto& p1 = indicesMesh.points[i1];
			auto& p2 = indicesMesh.points[i2];

			result.triangles.push_back({p0, p1, p2});

			i += 3;
		}
		result.triangles.shrink_to_fit();

		return result;
	}

	Mesh testMesh()
	{
		Mesh mesh;
		mesh.triangles.push_back(
			Triangle{
				  Vec3{-1.0_FL, -1.0_FL, -1.0_FL}
				, Vec3{-1.0_FL, -1.0_FL, +1.0_FL}
				, Vec3{+0.0_FL, +1.0_FL, +0.0_FL}
			}
		);
		return mesh;
	}
}
