#include "voxelizer.h"

#include <vector>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <utility>
#include <queue>

#include <glm/glm.hpp>
#include <glm/common.hpp>

#include <Render/Attributes.h>


namespace
{
	using Vec3  = glm::vec3;
	using Vec3i = glm::ivec3;



	struct Triangle
	{
		Vec3 points[3] = {};
	};

	struct AABB
	{
		Vec3 corners[2] = {};
	};

	struct Mesh
	{
		std::vector<Triangle> triangles;
	};

	struct VoxelMesh
	{
		std::vector<Vec3> voxels;
		float voxelSize;
	};



	struct Range
	{
		float min;
		float max;
	};

	Range getRange(const AABB& box, const Vec3& axis)
	{
		auto& [fc, sc] = box.corners;
		Vec3 vertices[8] = {
			Vec3(fc[0], fc[1], fc[2])
			, Vec3(fc[0], fc[1], sc[2])
			, Vec3(fc[0], sc[1], fc[2])
			, Vec3(fc[0], sc[1], sc[2])
			, Vec3(sc[0], fc[1], fc[2])
			, Vec3(sc[0], fc[1], sc[2])
			, Vec3(sc[0], sc[1], fc[2])
			, Vec3(sc[0], sc[1], sc[2])
		};

		Range result{};
		auto& [min, max] = result;
		min = max = glm::dot(axis, vertices[0]);
		for (int i = 1; i < 8; i++)
		{
			float dot = glm::dot(axis, vertices[i]);
			min = (dot < min) ? dot : min;
			max = (dot > max) ? dot : max;
		}

		return result;
	}

	Range getRange(const Triangle& tri, const Vec3& axis)
	{
		Range result;
		auto& [min, max] = result;
		min = max = glm::dot(axis, tri.points[0]);
		for (int i = 1; i < 3; i++)
		{
			float dot = glm::dot(axis, tri.points[i]);
			min = (dot < min) ? dot : min;
			max = (dot > max) ? dot : max;
		}

		return result;
	}

	bool overlapOnAxis(const AABB& box, const Triangle& tri, const Vec3& axis)
	{
		auto [min0, max0] = getRange(box, axis);
		auto [min1, max1] = getRange(tri, axis);

		return (min1 <= max0) && (min0 <= max1);
	}

	bool intersects(const AABB& box, const Triangle& tri)
	{
		const Vec3 EPS_VEC = Vec3(1e-6);

		Vec3 t0 = tri.points[1] - tri.points[0];
		Vec3 t1 = tri.points[2] - tri.points[1];
		Vec3 t2 = tri.points[0] - tri.points[2];

		Vec3 b0 = Vec3(1.0f, 0.0f, 0.0f);
		Vec3 b1 = Vec3(0.0f, 1.0f, 0.0f);
		Vec3 b2 = Vec3(0.0f, 0.0f, 0.0f);

		Vec3 tests[13] = {
			b0
			, b1
			, b2

			, glm::cross(t0, t1)

			, glm::cross(b0, t0)
			, glm::cross(b0, t1)
			, glm::cross(b0, t2)

			, glm::cross(b1, t0)
			, glm::cross(b1, t1)
			, glm::cross(b1, t2)

			, glm::cross(b2, t0)
			, glm::cross(b1, t1)
			, glm::cross(b1, t2)
		};
		for (auto& test : tests)
		{
			if (glm::dot(test, test) < 1e-6)
			{
				test = EPS_VEC;
			}
		}

		for (auto& test : tests)
		{
			if (!overlapOnAxis(box, tri, test))
			{
				return false;
			}
		}

		return true;
	}



	//next power of 2
	template<class IntType, auto bits = 8 * sizeof(IntType)>
	int npo2(IntType num)
	{
		num -= 1;

		decltype(bits) bit = 1;
		while(bit < bits)
		{
			num = num | (num >> bit);
			bit <<= 1;
		}

		return num + 1;
	}



	//all coords in [-1; 1] range
	//octree range - [0, 1] along each axis
	//simple sparse octree imitation
	class SVO
	{
	public:
		class FlattenedSVO
		{
		public:
			using Storage = std::vector<int>;


		private:


		private:

		};

		using Set = std::set<int>;


	public:
		SVO(const AABB& space, int split) 
			: m_set()
			, m_space(space)
			, m_spaceDelta()
		{
			m_split  = split;
			m_extent = npo2(split);
			m_depth  = std::ceil(std::log2(m_extent));
			m_depth2 = m_depth << 1;

			m_ratio = m_extent / m_split;

			auto& [fc, sc] = m_space.corners;
			auto delta = sc - fc;
			m_spaceDelta = delta / float(split);
			sc = fc + delta * m_ratio;
		}

		SVO(const SVO&) = default;
		SVO(SVO&&)      = default;

		~SVO() = default;

		SVO& operator = (const SVO&) = default;
		SVO& operator = (SVO&&)      = default;


	public:
		Vec3i index(const Vec3& point)
		{
			Vec3 temp = point - m_space.corners[0];

			Vec3i indices{};
			indices[0] = std::max(std::min(int(temp[0] / m_spaceDelta[0]), m_split - 1), 0);
			indices[1] = std::max(std::min(int(temp[1] / m_spaceDelta[1]), m_split - 1), 0);
			indices[2] = std::max(std::min(int(temp[2] / m_spaceDelta[2]), m_split - 1), 0);

			return indices;
		}

		Vec3 coord(const Vec3i& indices)
		{
			Vec3 temp{};

			temp[0] = indices[0] * m_spaceDelta[0];
			temp[1] = indices[1] * m_spaceDelta[1];
			temp[2] = indices[2] * m_spaceDelta[2];
			temp += m_space.corners[0];

			return temp;
		}

		Vec3 voxel(int voxelHash)
		{
			int mask = (1 << m_depth) - 1;

			int x = voxelHash & mask;

			voxelHash >>= m_depth;
			int y = (voxelHash & mask);

			voxelHash >>= m_depth;
			int z = (voxelHash & mask);

			return coord(Vec3i{x, y, z}) + Vec3(m_spaceDelta / 2.0f);
		}

		int hash(const Vec3& item)
		{
			auto i = index(item);

			int xc = i.x;
			int yc = i.y;
			int zc = i.z;

			return (xc) | (yc << m_depth) | (zc << m_depth2);
		}


	public:
		bool has(const Vec3& item)
		{
			return m_set.find(hash(item)) != m_set.end();
		}

		void add(const Vec3& item)
		{
			m_set.insert(hash(item));
		}

		void remove(const Vec3& item)
		{
			m_set.erase(hash(item));
		}


		auto begin() const
		{
			return m_set.begin();
		}

		auto end() const
		{
			return m_set.end();
		}

		auto size() const
		{
			return m_set.size();
		}


	private:
		Set m_set; // stores hashes of 

		AABB m_space;      // space[a,b]x[c,d] that will be split
		Vec3 m_spaceDelta; // extents of a single piece to that space[a,b]x[c,d] was split

		float m_ratio; //ratio of m_extent to m_split

		int m_split;  // count of pieces that space was split to
		int m_extent; // next power of 2 of m_split
		int m_depth;  // depth of an octree
		int m_depth2; // depth * depth
	};

	VoxelMesh voxelizeMesh(const Mesh& mesh, int split)
	{
		SVO voxels(
			AABB{
				  Vec3{-1.0f, -1.0f, -1.0f}
				, Vec3{+1.0f, +1.0f, +1.0f}
			}
			, split
		);
		//std::queue<Vec3i> queue;
		for (auto& triangle : mesh.triangles)
		{
			Vec3 cornerMin = triangle.points[0];
			Vec3 cornerMax = triangle.points[0];
			for (int i = 1; i < 3; i++)
			{
				auto& vertex = triangle.points[i];

				cornerMin[0] = (vertex[0] > cornerMin[0]) ? cornerMin[0] : vertex[0];
				cornerMin[1] = (vertex[1] > cornerMin[1]) ? cornerMin[1] : vertex[1];
				cornerMin[2] = (vertex[2] > cornerMin[2]) ? cornerMin[2] : vertex[2];

				cornerMax[0] = (vertex[0] < cornerMax[0]) ? cornerMax[0] : vertex[0];
				cornerMax[1] = (vertex[1] < cornerMax[1]) ? cornerMax[1] : vertex[1];
				cornerMax[2] = (vertex[2] < cornerMax[2]) ? cornerMax[2] : vertex[2];
			}

			auto min = voxels.index(cornerMin);
			auto max = voxels.index(cornerMax);

			//later create more optimized version
			for (int i = min.x; i <= max.x; i++)
			{
				for(int j = min.y; j <= max.y; j++)
				{
					for (int k = min.z; k <= max.z; k++)
					{
						Vec3 firstCorner  = voxels.coord(Vec3i{i    , j    , k    });
						Vec3 secondCorner = voxels.coord(Vec3i{i + 1, j + 1, k + 1});
						Vec3 center = (firstCorner + secondCorner) / 2.0f;

						AABB voxel{firstCorner, secondCorner};

						if (intersects(voxel, triangle))
						{
							voxels.add(center);
						}
					}
				}
			}
		}

		VoxelMesh result{};
		
		result.voxels.reserve(voxels.size());
		result.voxels.clear();
		for (auto& hash : voxels)
		{
			result.voxels.push_back(voxels.voxel(hash));
		}
		result.voxelSize = 2.0f / split;

		return result;
	}
}

VoxelData voxelize(int split)
{
	Mesh mesh{};

	mesh.triangles = {
		/*{
			  Vec3(0.5f, 0.0f, 0.0f)
			, Vec3(0.0f, 0.5f, 0.0f)
		    , Vec3(0.0f, 0.0f, 0.5f)
		}*/
		 {
			  Vec3(-1.0f, 0.0f, -1.0f)
			, Vec3(-1.0f, 1.0f, +1.0f)
		    , Vec3(+1.0f, 0.0f, +1.0f)
		}
		//,
		/*{
			  Vec3(+1.0f, +1.0f, 0.0f)
			, Vec3(-1.0f, +1.0f, 0.0f)
		    , Vec3(-1.0f, -1.0f, 0.0f)
		}*/
	};

	auto[voxels, voxelSize] = voxelizeMesh(mesh, split);


	VertexArray vertexArray;
	Buffer     vertexBuffer;

	vertexBuffer.genBuffer();
	vertexBuffer.bufferData(static_cast<GLsizeiptr>(sizeof(Vec3) * voxels.size()), voxels.data(), BufferUsage::StaticDraw);
	
	vertexArray.info() = VertexArray::DrawInfo{
		  DrawMode::Points
		, static_cast<GLint>(0)
		, static_cast<GLsizei>(voxels.size())
		, IndicesType::None
		, static_cast<GLvoid*>(nullptr)
	};

	vertexArray.genArray();
	vertexArray.bind();
	vertexArray.setAttribPointerInBuffer(
		vertexBuffer
		, VertexArray::PointerInfo{
			  static_cast<GLuint>(VertexAttributes::Position)
			, AttributeSize::Three
		    , DataType::Float
		    , GLBool::True
		    , static_cast<GLsizei>(0)
		    , nullptr
		}
	);
	vertexArray.unbind();

	return VoxelData{std::move(vertexArray), std::move(vertexBuffer), voxelSize};
}