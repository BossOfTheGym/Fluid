#include "Mesh.h"


Mesh indicesToTriangle(const IndicesMesh& indicesMesh)
{
	Mesh result;

	int i = 0;
	int e = static_cast<int>(indicesMesh.indices.size());
	while (i + 2 < e)
	{
		auto i0 = indicesMesh.indices[i    ];
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