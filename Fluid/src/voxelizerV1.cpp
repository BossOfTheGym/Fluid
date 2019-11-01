#include "voxelizer.h"


#include <set>
#include <queue>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/common.hpp>

#include <Render/Attributes.h>

namespace
{
	//consts
	const float EPS = 1e-5f;
	const float SQ2 = std::sqrt(2.0f);
	const float SQ3 = std::sqrt(3.0f);

	//all necessary structs
	using Vec3  = glm::vec3;
	using Vec3i = glm::ivec3;
	
	struct Triangle
	{
		Vec3 points[3] = {};
	};

	struct Cylinder
	{
		Vec3 heightVec     = {}; // normalized
		Vec3 radiusVecs[2] = {}; // normalized
		Vec3 origin        = {};
		float height = {};
		float radius = {};
	};

	struct Prism
	{
		enum
		{
			  LOWER_PLANE = 0
			, UPPER_PLANE = 1
			, SIDE_PLANE0 = 2
			, SIDE_PLANE1 = 3
			, SIDE_PLANE2 = 4
		};

		Vec4 planes[5];
	};

	struct Sphere
	{
		Vec3 center{};
		float radius{};
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

	struct RoundedTriangle
	{
		Sphere   vertices[3] = {};
		Cylinder edges[3]    = {};
		Prism    triangle    = {};
	};


	//utils
	//triangle points' order is ccw
	Vec3 triangleNormal(const Triangle& triangle)
	{
		auto& [p0, p1, p2] = triangle.points;

		return glm::normalize(glm::cross(p1 - p0, p2 - p0));
	}

	Prism prismFromMiddleTriangleHeight(const Triangle& triangle, float height)
	{
		Prism prism;

		auto& [p0, p1, p2] = triangle.points;

		Vec3 normal = triangleNormal(triangle);
		Vec3 half   = normal * height / 2.0f;

		Vec3 p00 = p0 - half;
		Vec3 p01 = p1 - half;
		Vec3 p02 = p2 - half;
		Vec3 p10 = p0 + half;

		Vec3 normals[5];
		normals[Prism::LOWER_PLANE] = +normal;
		normals[Prism::UPPER_PLANE] = -normal;
		normals[Prism::SIDE_PLANE0] = glm::normalize(glm::cross(normal, p01 - p00));
		normals[Prism::SIDE_PLANE1] = glm::normalize(glm::cross(normal, p02 - p01));
		normals[Prism::SIDE_PLANE2] = glm::normalize(glm::cross(normal, p00 - p02));

		prism.planes[Prism::LOWER_PLANE] = Vec4(normals[Prism::LOWER_PLANE], -glm::dot(normals[Prism::LOWER_PLANE], p00));
		prism.planes[Prism::UPPER_PLANE] = Vec4(normals[Prism::UPPER_PLANE], -glm::dot(normals[Prism::UPPER_PLANE], p10));
		prism.planes[Prism::SIDE_PLANE0] = Vec4(normals[Prism::SIDE_PLANE0], -glm::dot(normals[Prism::SIDE_PLANE0], p00));
		prism.planes[Prism::SIDE_PLANE1] = Vec4(normals[Prism::SIDE_PLANE1], -glm::dot(normals[Prism::SIDE_PLANE1], p01));
		prism.planes[Prism::SIDE_PLANE2] = Vec4(normals[Prism::SIDE_PLANE2], -glm::dot(normals[Prism::SIDE_PLANE2], p02));

		return prism;
	}

	Cylinder cylinderFromPoints(const Vec3& point0, const Vec3& point1, float radius)
	{
		Vec3  heightVec = point1 - point0;
		float height  = glm::length(heightVec);

		heightVec /= height;

		int i = 0;
		while (i < 3 && std::abs(heightVec[i]) < EPS)
		{
			i++;
		}

		int j = 0;
		if (i == 0)
		{
			j = 1;
		}

		Vec3 radiusVec{};
		radiusVec[i] = +heightVec[j];
		radiusVec[j] = -heightVec[i];
		radiusVec /= glm::length(radiusVec);

		Cylinder cylinder{};

		cylinder.height    = height;
		cylinder.heightVec = heightVec;
		cylinder.origin    = point0;
		cylinder.radius    = radius;
		cylinder.radiusVecs[0] = radiusVec;
		cylinder.radiusVecs[1] = glm::normalize(glm::cross(radiusVec, heightVec));

		return cylinder;
	}

	Sphere sphereFromCenterRadius(const Vec3& center, float radius)
	{
		return Sphere{center, radius};
	}

	RoundedTriangle roundedTriangleFromTriangleRadius(const Triangle& triangle, float radius, float height)
	{
		auto& [p0, p1, p2] = triangle.points;

		RoundedTriangle result;

		result.vertices[0] = sphereFromCenterRadius(p0, radius);
		result.vertices[1] = sphereFromCenterRadius(p1, radius);
		result.vertices[2] = sphereFromCenterRadius(p2, radius);
		result.edges[0] = cylinderFromPoints(p0, p1, radius);
		result.edges[1] = cylinderFromPoints(p1, p2, radius);
		result.edges[2] = cylinderFromPoints(p2, p0, radius);
		result.triangle = prismFromMiddleTriangleHeight(triangle, height);

		return result;
	}



	//volume checks
	bool pointInCylinder(const Vec3& point, const Cylinder& cylinder)
	{
		Vec3 delta = point - cylinder.origin;
		float dh  = glm::dot(delta, cylinder.heightVec);
		float dr0 = glm::dot(delta, cylinder.radiusVecs[0]);
		float dr1 = glm::dot(delta, cylinder.radiusVecs[1]);
		float dr2 = dr0 * dr0 + dr1 * dr1;
		float rr  = (cylinder.radius * cylinder.radius);

		return 0 <= dh && dh <= cylinder.height && dr2 <= rr;
	}

	bool pointInSphere(const Vec3& point, const Sphere& sphere)
	{
		Vec3 dr = point - sphere.center;

		return glm::dot(dr, dr) <= sphere.radius * sphere.radius;
	}

	bool pointInPrism(const Vec3& point, const Prism& prism)
	{
		auto& x = point[0];
		auto& y = point[1];
		auto& z = point[2];
		for (auto& plane : prism.planes)
		{
			auto& x0 = plane[ 0];
			auto& y0 = plane[1];
			auto& z0 = plane[2];
			auto& w0 = plane[3];

			float check = x * x0 + y * y0 + z * z0 + w0;
			if (check <= 0.0f)
			{
				return false;
			}
		}

		return true;
	}

	bool pointInRoundedTriangle(const Vec3& point, const RoundedTriangle& roundedTriangle)
	{
		auto& [vertices, edges, triangle] = roundedTriangle;

		return pointInSphere(point, vertices[0])
			|| pointInSphere(point, vertices[1])
			|| pointInSphere(point, vertices[2])
			|| pointInCylinder(point, edges[0])
			|| pointInCylinder(point, edges[1])
			|| pointInCylinder(point, edges[2])
			|| pointInPrism(point, triangle);
	}



	//SVO
	//next power of 2
	template<class IntType, auto bits = 8 * sizeof(IntType)>
	IntType npo2(IntType num)
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
		using Int  = int;
		using Hash = int;
		using Set  = std::set<Hash>;
		using Point   = Vec3;
		using Indices = Vec3i;


	public:
		SVO(const AABB& space, Int split) 
			: m_set()
			, m_space(space)
			, m_spaceDelta()
		{
			m_split  = split;
			m_extent = npo2(split);
			m_depth  = std::ceil(std::log2(m_extent));
			m_depth2 = m_depth << 1;

			m_ratio = float(m_extent) / m_split;

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
		Indices index(const Point& point) const
		{
			Point temp = point - m_space.corners[0];

			Indices indices{};
			/*indices[0] = std::max(std::min(Hash(temp[0] / m_spaceDelta[0]), m_split - 1), 0);
			indices[1] = std::max(std::min(Hash(temp[1] / m_spaceDelta[1]), m_split - 1), 0);
			indices[2] = std::max(std::min(Hash(temp[2] / m_spaceDelta[2]), m_split - 1), 0);*/
			indices[0] = Hash(temp[0] / m_spaceDelta[0]);
			indices[1] = Hash(temp[1] / m_spaceDelta[1]);
			indices[2] = Hash(temp[2] / m_spaceDelta[2]);
			return indices;
		}

		Point coord(const Indices& indices) const
		{
			Point temp{};

			/*temp[0] = std::max(std::min(indices[0], m_split - 1), 0) * m_spaceDelta[0];
			temp[1] = std::max(std::min(indices[1], m_split - 1), 0) * m_spaceDelta[1];
			temp[2] = std::max(std::min(indices[2], m_split - 1), 0) * m_spaceDelta[2];*/
			temp[0] = indices[0] * m_spaceDelta[0];
			temp[1] = indices[1] * m_spaceDelta[1];
			temp[2] = indices[2] * m_spaceDelta[2];

			temp += m_space.corners[0];

			return temp;
		}

		Point voxel(Hash voxelHash) const
		{
			Hash mask = (1 << m_depth) - 1;

			Hash x = voxelHash & mask;

			voxelHash >>= m_depth;
			Hash y = (voxelHash & mask);

			voxelHash >>= m_depth;
			Hash z = (voxelHash & mask);

			return coord(Indices{x, y, z}) + Point(m_spaceDelta / 2.0f);
		}

		Point voxel(const Indices& indices) const
		{
			return coord(indices) + Point(m_spaceDelta / 2.0f);
		}

		Hash hash(const Point& item) const
		{
			auto i = index(item);

			return hash(i);
		}

		Hash hash(const Indices& indices) const
		{
			/*Hash xc = std::max(std::min(indices.x, m_split - 1), 0);
			Hash yc = std::max(std::min(indices.y, m_split - 1), 0);
			Hash zc = std::max(std::min(indices.z, m_split - 1), 0);*/
			Hash xc = indices.x;
			Hash yc = indices.y;
			Hash zc = indices.z;

			return (xc) | (yc << m_depth) | (zc << m_depth2);
		}
		

	public:
		const Point& spaceDelta() const
		{
			return m_spaceDelta;
		}

		Int split() const
		{
			return m_split;
		}


	public:
		bool has(const Point& item) const
		{
			return m_set.find(hash(item)) != m_set.end();
		}

		bool has(const Hash& hash) const
		{
			return m_set.find(hash) != m_set.end();
		}

		bool has(const Indices& indices) const
		{
			return m_set.find(hash(indices)) == m_set.end();
		}


		void add(const Point& item)
		{
			m_set.insert(hash(item));
		}

		void add(const Hash& hash)
		{
			m_set.insert(hash);
		}

		void add(const Indices& indices)
		{
			m_set.insert(hash(indices));
		}


		void remove(const Point& item)
		{
			m_set.erase(hash(item));
		}

		void remove(const Hash& hash)
		{
			m_set.erase(hash);
		}

		void remove(const Indices& indices)
		{
			m_set.erase(hash(indices));
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


		void clear()
		{
			m_set.clear();
		}


	private:
		Set m_set; // stores hashes of 

		AABB m_space;       // space[a,b]x[c,d] that will be split
		Point m_spaceDelta; // extents of a single piece to that space[a,b]x[c,d] was split

		float m_ratio; //ratio of m_extent to m_split

		Int m_split;  // count of pieces that space was split to
		Int m_extent; // next power of 2 of m_split
		Int m_depth;  // depth of an octree
		Int m_depth2; // 2 * depth
	};



	//voxelization algorithm
	class Voxelizer
	{
	public:
		using Indices = SVO::Indices;
		using Point   = SVO::Point;
		using Hash    = SVO::Hash;
		using Set     = SVO::Set;

		using Queue = std::queue<Indices>;


	public:
		Voxelizer()
		{}

		Voxelizer(const Voxelizer&) = default;
		Voxelizer(Voxelizer&&)      = default;

		~Voxelizer() = default;

		Voxelizer& operator = (const Voxelizer&) = default;
		Voxelizer& operator = (Voxelizer&&)      = default;


	public:
		void traverseVoxels(SVO& output, const RoundedTriangle& volume)
		{
			//add all voxels from vertices
			auto& [vertices, edges, triangle] = volume;

			//initial push & bounding indices
			auto split = output.split();
			for (auto& vertex : vertices)
			{
				auto indices = output.index(vertex.center);

				indices -= 1;
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						for (int k = 0; k < 3; k++)
						{
							auto toPush = indices + Indices{i, j, k};

							toPush.x = std::max(std::min(toPush.x, split - 1), 0);
							toPush.y = std::max(std::min(toPush.y, split - 1), 0);
							toPush.z = std::max(std::min(toPush.z, split - 1), 0);

							m_queue.push(toPush);
						}
					}
				}
			}

			//traverse all voxels
			Set traversedVoxels;
			while (!m_queue.empty())
			{
				auto indices = m_queue.front();
				auto voxel   = output.voxel(indices);
				auto hash    = output.hash(indices);

				if (
					traversedVoxels.find(hash) == traversedVoxels.end() 
					&& pointInRoundedTriangle(voxel, volume)
				)
				{
					traversedVoxels.insert(hash);

					indices -= 1;
					for (int i = 0; i < 3; i++)
					{
						for (int j = 0; j < 3; j++)
						{
							for (int k = 0; k < 3; k++)
							{
								auto toPush = indices + Indices{i, j, k};
								toPush.x = std::max(std::min(toPush.x, split - 1), 0);
								toPush.y = std::max(std::min(toPush.y, split - 1), 0);
								toPush.z = std::max(std::min(toPush.z, split - 1), 0);

								auto hash = output.hash(toPush);
								if (traversedVoxels.find(hash) == traversedVoxels.end())
								{
									m_queue.push(toPush);
								}
							}
						}
					}
				}

				m_queue.pop();
			}

			//push traversed voxels into output
			for (auto& hash : traversedVoxels)
			{
				output.add(hash);
			}
		}


	private:
		Queue m_queue;
	};

	float getLargestDiagonalProj(const Vec3& normal, float halfVoxelSize)
	{
		static const Vec3 voxelDiagonals[4] =
		{
			  Vec3{-halfVoxelSize, -halfVoxelSize, -halfVoxelSize}
			, Vec3{-halfVoxelSize, -halfVoxelSize, +halfVoxelSize}
			, Vec3{-halfVoxelSize, +halfVoxelSize, -halfVoxelSize}
			, Vec3{-halfVoxelSize, +halfVoxelSize, +halfVoxelSize}
		};

		float proj = 0.0f;
		for (auto& diag : voxelDiagonals)
		{
			proj = std::max(proj, std::abs(glm::dot(diag, normal)));
		}

		return proj;
	};

	VoxelMesh voxelizeMesh(const Mesh& mesh, int split)
	{
		SVO voxels(
			  AABB{Vec3{-1.0f}, Vec3{1.0f}}
		    , split
		);

		float voxelHalfSize = 1.0f / split;
		float radius = voxelHalfSize * SQ3;

		Voxelizer voxelizer;
		for (auto triangle : mesh.triangles)
		{
			//translate triangle
			auto& [p0, p1, p2] = triangle.points;

			auto i0 = voxels.index(p0);
			auto i1 = voxels.index(p1);
			auto i2 = voxels.index(p2);
			
			if (i0.x == i1.x && i0.x == i2.x)
			{
				p0.x += EPS; p1.x += EPS; p2.x += EPS;
			}
			if (i0.y == i1.y && i0.y == i2.y)
			{
				p0.y += EPS; p1.y += EPS; p2.y += EPS;
			}
			if (i0.z == i1.z && i0.z == i2.z)
			{
				p0.z += EPS; p1.z += EPS; p2.z += EPS;
			}

			float height = getLargestDiagonalProj(triangleNormal(triangle), voxelHalfSize);
			auto volume  = roundedTriangleFromTriangleRadius(triangle, height, 2.0f * height);
			voxelizer.traverseVoxels(voxels, volume);
		}


		VoxelMesh result{};
		result.voxels.reserve(voxels.size());
		for (auto& hash : voxels)
		{
			result.voxels.push_back(voxels.voxel(hash));
		}
		result.voxelSize = 2.0f / split;

		return result;
	}
}


void testV1()
{
	Prism prism = prismFromMiddleTriangleHeight(
		Triangle{
			  Vec3{0.0f, 0.0f, 0.0f}
			, Vec3{1.0f, 1.0f, 0.0f}
			, Vec3{0.0f, 1.0f, 0.0f}
		}
		, 6.0f
	);

	Vec3 p00{+0.5f, -0.5f, +0.0f};
	Vec3 p01{+0.5f, +0.5f, +0.0f};
	Vec3 p02{+0.5f, +1.0f, +0.0f};
	Vec3 p03{+0.5f, +1.0f, +1.0f};
	Vec3 p04{+0.0f, -1.0f, +1.0f};
	Vec3 p05{+0.0f, +0.5f, +2.0f};
	Vec3 p06{+1.0f, +2.0f, +3.0f};

	std::cout << "test00. got: " << pointInPrism(p00, prism) << " expected: " << 0 << std::endl;
	std::cout << "test01. got: " << pointInPrism(p01, prism) << " expected: " << 1 << std::endl;
	std::cout << "test02. got: " << pointInPrism(p02, prism) << " expected: " << 1 << std::endl;
	std::cout << "test03. got: " << pointInPrism(p03, prism) << " expected: " << 1 << std::endl;
	std::cout << "test04. got: " << pointInPrism(p04, prism) << " expected: " << 0 << std::endl;
	std::cout << "test05. got: " << pointInPrism(p05, prism) << " expected: " << 1 << std::endl;
	std::cout << "test06. got: " << pointInPrism(p06, prism) << " expected: " << 0 << std::endl;
}

VoxelData voxelizeV1(int split)
{
	Mesh mesh{};

	/*
	{
	Vec3(0.0f, 0.0f, 0.0f)
	, Vec3(1.0f, 0.0f, 0.0f)
	, Vec3(0.0f, 1.0f, 0.0f)
	}
	*/
	mesh.triangles = 
	{
		/*{
			   Vec3(+1.0f, +0.0f, +0.0f)
			 , Vec3(-0.2f, -1.0f, -1.0f)
		     , Vec3(-1.0f, +1.0f, +1.0f)
		}*/
		{
			  Vec3(+0.0f, +0.0f, +0.0f)
			, Vec3(+1.0f, +0.0f, +0.0f)
		    , Vec3(+0.0f, +0.0f, +1.0f)
		}
		, 
        {
			  Vec3(+0.0f, +0.0f, +0.0f)
			, Vec3(+1.0f, +0.0f, +0.0f)
		    , Vec3(+0.0f, +1.0f, +0.0f)
		}
		, 
		{
			  Vec3(+0.0f, +0.0f, +0.0f)
			, Vec3(+0.0f, +1.0f, +0.0f)
		    , Vec3(+0.0f, +0.0f, +1.0f)
		}
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

	return VoxelData{std::move(vertexArray), std::move(vertexBuffer), std::move(voxelSize)};
}