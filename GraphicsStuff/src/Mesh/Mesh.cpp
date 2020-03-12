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


	// TriangleMesh
	AABB TriangleMesh::computeAABB()
	{
		AABB aabb{};
		if (!m_triangles.empty())
		{
			//auto preTransform()
			auto i = m_triangles.begin();
			auto e = m_triangles.end();

			auto& [fc, sc] = aabb.corners;
			fc = i->points[0];
			sc = i->points[0];
			while (i != e)
			{
				auto& points = i->points;
				for (int j = 0; j < 3; j++)
				{
					auto& curr = points[j];
					fc[0] = std::min(fc[0], curr[0]);
					fc[1] = std::min(fc[1], curr[1]);
					fc[2] = std::min(fc[2], curr[2]);
					sc[0] = std::max(sc[0], curr[0]);
					sc[1] = std::max(sc[1], curr[1]);
					sc[2] = std::max(sc[2], curr[2]);
				}

				++i;
			}
		}
		return aabb;
	}


	TriangleMesh::TriangleMesh(const Triangles& triangles, const Mat4& transform)
		: m_triangles(triangles)
		, m_transform(transform)
	{
		m_aabb = computeAABB();
	}

	TriangleMesh::TriangleMesh(Triangles&& triangles, const Mat4& transform)
		: m_triangles(std::move(triangles))
		, m_transform(transform)
	{
		m_aabb = computeAABB();
	}

	TriangleMesh::TriangleMesh(TriangleMesh&& another)
	{
		*this = std::move(another);
	}

	TriangleMesh& TriangleMesh::operator = (TriangleMesh&& another)
	{
		std::swap(m_triangles, another.m_triangles);
		std::swap(m_transform, another.m_transform);
		std::swap(m_aabb, another.m_aabb);

		return *this;
	}


	const TriangleMesh::Triangles& TriangleMesh::getTriangles() const
	{
		return m_triangles;
	}

	const Mat4& TriangleMesh::getTransform() const
	{
		return m_transform;
	}

	const AABB& TriangleMesh::getAABB() const
	{
		return m_aabb;
	}



	// TringleIndicesMesh

	AABB TriangleIndicesMesh::computeAABB()
	{
		AABB aabb{};
		if (!m_points.empty())
		{
			//auto preTransform()
			auto i = m_points.begin();
			auto e = m_points.end();

			auto& [fc, sc] = aabb.corners;
			fc = *i;
			sc = *i;
			while (i != e)
			{
				auto& curr = *i;
				fc[0] = std::min(fc[0], curr[0]);
				fc[1] = std::min(fc[1], curr[1]);
				fc[2] = std::min(fc[2], curr[2]);
				sc[0] = std::max(sc[0], curr[0]);
				sc[1] = std::max(sc[1], curr[1]);
				sc[2] = std::max(sc[2], curr[2]);
				++i;
			}
		}
		return aabb;
	}


	TriangleIndicesMesh::TriangleIndicesMesh(const Points& points, const Indices& indices, const Mat4& transform)
		: m_points(points)
		, m_indices(indices)
		, m_transform(transform)
	{
		m_aabb = computeAABB();
	}

	TriangleIndicesMesh::TriangleIndicesMesh(Points&& points, Indices&& indices, const Mat4& transform)
		: m_points(std::move(points))
		, m_indices(std::move(indices))
		, m_transform(transform)
	{
		m_aabb = computeAABB();
	}

	TriangleIndicesMesh::TriangleIndicesMesh(TriangleIndicesMesh&& another)
	{
		*this = std::move(another);
	}

	TriangleIndicesMesh& TriangleIndicesMesh::operator = (TriangleIndicesMesh&& another)
	{
		std::swap(m_points, another.m_points);
		std::swap(m_indices, another.m_indices);
		std::swap(m_transform, another.m_transform);
		std::swap(m_aabb, another.m_aabb);

		return *this;
	}


	const TriangleIndicesMesh::Points& TriangleIndicesMesh::getPoints() const
	{
		return m_points;
	}

	const TriangleIndicesMesh::Indices& TriangleIndicesMesh::getIndices() const
	{
		return m_indices;
	}

	const Mat4& TriangleIndicesMesh::getTransform() const
	{
		return m_transform;
	}

	const AABB& TriangleIndicesMesh::getAABB() const
	{
		return m_aabb;
	}
}
