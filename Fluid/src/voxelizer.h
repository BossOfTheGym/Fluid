#ifndef VOXELIZER_H
#define VOXELIZER_H


#include <queue>
#include <algorithm>
#include <set>

#include "primitives.h"
#include "Mesh.h"
#include "SVO.h"


namespace voxel
{
	template<class Value>
	using SVO = SparseVoxelOctree<Value>;

	template<class Value>
	using FVO = FullVoxelOctree<Value>;


	//Voxelizer. Traverses voxels belonging to some volume
	namespace detail
	{
		//voxelization algorithm
		template<class VO, class Value>
		class Voxelizer
		{
		public:
			using Indices = typename VO::Indices;
			using Point   = typename VO::Point;
			using Hash    = typename VO::Hash;
			using Set     = std::set<Hash>;

			using Queue = std::queue<Indices>;


		public:
			Voxelizer() = default;

			Voxelizer(const Voxelizer&) = default;
			Voxelizer(Voxelizer&&)      = default;

			~Voxelizer() = default;

			Voxelizer& operator = (const Voxelizer&) = default;
			Voxelizer& operator = (Voxelizer&&)      = default;


		public:
			void traverseVoxels(VO& output, const RoundedTriangle& volume, Value primIndex)
			{
				//add all voxels from vertices
				auto& [vertices, edges, triangle] = volume;

				//initial push & bounding indices
				auto split = output.split();
				for (auto& vertex : vertices)
				{
					auto indices = output.index(vertex.center);

					for (int i = -1; i < 2; i++)
					{
						for (int j = -1; j < 2; j++)
						{
							for (int k = -1; k < 2; k++)
							{
								auto toPush = output.clampToBoundaries(indices + Indices{i, j, k});

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

						for (int i = -1; i < 2; i++)
						{
							for (int j = -1; j < 2; j++)
							{
								for (int k = -1; k < 2; k++)
								{
									auto toPush = output.clampToBoundaries(indices + Indices{i, j, k});

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
					output[hash] = primIndex;
				}
			}


		private:
			Queue m_queue;
		};
	}


	//Creates only voxelized boundary of volume
	namespace detail
	{
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

		template<class Value>
		SVO<Value> svoVoxelize(const Mesh& mesh, int split)
		{
			SVO<Value> voxels(
				AABB{Vec3{-1.0f}, Vec3{1.0f}}
				, split
			);

			float voxelSize     = 2.0f / split;
			float voxelHalfSize = 1.0f / split;
			float radius  = voxelSize * SQ3 / 2;
			float radius2 = voxelSize * SQ3;

			Voxelizer<SVO<Value>, Value> voxelizer;
			for (Value i = 0; i < mesh.triangles.size(); i++)
			{
				auto triangle = mesh.triangles[i];

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
				auto volume  = roundedTriangleFromTriangleRadius(triangle, radius, 2.0f * height);
				voxelizer.traverseVoxels(voxels, volume, i);
			}

			return voxels;
		}
	}

	template<class Value>
	SVO<Value> svoVoxelize(const Mesh& mesh, int split)
	{
		return detail::svoVoxelize<Value>(mesh, split);
	}


	//Creates fully voxelized volume
	namespace detail
	{
		//TODO : rework this
		template<class Value>
		void initialVoxelization(FVO<Value>& voxels, const Mesh& mesh)
		{
			float voxelSize = 2.0f / voxels.split();
			float radius  = voxelSize * SQ3 / 2;
			float radius2 = voxelSize * SQ3;

			Voxelizer<FVO<Value>, Value> voxelizer;
			for (Value i = 0; i < mesh.triangles.size(); i++)
			{
				auto triangle = mesh.triangles[i];

				//translate triangle
				auto& [p0, p1, p2] = triangle.points;

				auto volume = roundedTriangleFromTriangleRadius(triangle, radius, radius2);
				voxelizer.traverseVoxels(voxels, volume, i);
			}
		}

		template<class Value>
		void mark(FVO<Value>& voxels, const Vec3& innerPoint, Value empty, Value interior, Value exterior)
		{
			using Indices = typename FVO<Value>::Indices;
			using Hash    = typename FVO<Value>::Hash;

			using Set   = std::set<Hash>;
			using Queue = std::queue<Indices>;

			Set queue;
			Hash split;

			split = voxels.split();

			//mark all interior points
			queue.insert(voxels.hash(innerPoint));
			while (!queue.empty())
			{
				auto currHash = *queue.begin();
				auto indices  = voxels.index(currHash);
				
				voxels[indices] = interior;

				for (Hash i = -1; i < 2; i++)
				{
					for (Hash j = -1; j < 2; j++)
					{
						for (Hash k = -1; k < 2; k++)
						{
							auto toPush = voxels.clampToBoundaries(indices + Indices{i, j, k});

							if (voxels[toPush] == empty)
							{
								queue.insert(voxels.hash(toPush));
							}
						}
					}
				}

				queue.erase(currHash);
			}

			//mark all exterior points
			for(auto& voxel : voxels)
			{
				if (voxel == empty)
				{
					voxel = exterior;
				}
			}
		}

		template<class Value, class Boundary, class GoodBoundary>
		void refineLabel(FVO<Value>& voxels, Value replacement, Boundary boundary, GoodBoundary goodBoundary)
		{
			using Hash    = typename FVO<Value>::Hash;
			using Indices = typename FVO<Value>::Indices;

			for(Hash currHash = 0; currHash < voxels.size(); currHash++)
			{
				if (boundary(currHash) && !goodBoundary(currHash))
				{
					auto& voxel = voxels[currHash];

					voxel = replacement;
				}
			}
		}

		template<class Value>
		void refine(FVO<Value>& voxels, const Value& empty, const Value& interior, const Value& exterior)
		{
			using Hash    = typename FVO<Value>::Hash;
			using Indices = typename FVO<Value>::Indices;
			

			//common lambdas
			auto boundary = [&] (const auto& hash)
			{
				const auto& voxel = voxels[hash];

				return voxel != empty && voxel != interior && voxel != exterior;
			};

			/*auto has6ConnectivityExteriorNeighbour = [&] (const auto& hash, const Value& neighbour)
			{
				Indices checks[6] =
				{
					  Indices{+1, +0, +0}
					, Indices{-1, +0, +0}
					, Indices{+0, +1, +0}
					, Indices{+0, -1, +0}
					, Indices{+0, +0, +1}
					, Indices{+0, +0, -1}
				};
				
				const auto& voxel = voxels[hash];

				auto indices = voxels.index(hash);
				for (auto& check : checks)
				{
					auto index = voxels.clampToBoundaries(indices + check);

					if (voxels[index] == neighbour)
					{
						return true;
					}
				}

				return false;
			};*/


			//delete inner voxels
			auto hasExteriorNeighbour = [&] (const auto& hash)
			{
				Indices checks[6] =
				{
					Indices{+1, +0, +0}
					, Indices{-1, +0, +0}
					, Indices{+0, +1, +0}
					, Indices{+0, -1, +0}
					, Indices{+0, +0, +1}
					, Indices{+0, +0, -1}
				};

				const auto& voxel = voxels[hash];

				auto indices = voxels.index(hash);
				for (auto& check : checks)
				{
					auto index = voxels.clampToBoundaries(indices + check);

					if (voxels.boundaryVoxel(index) || voxels[index] == exterior)
					{
						return true;
					}
				}

				return false;
			};

			refineLabel(voxels, interior, boundary, hasExteriorNeighbour);


			//delete outer voxels
			auto hasInteriorNeighbour = [&] (const auto& hash)
			{
				Indices checks[6] =
				{
					Indices{+1, +0, +0}
					, Indices{-1, +0, +0}
					, Indices{+0, +1, +0}
					, Indices{+0, -1, +0}
					, Indices{+0, +0, +1}
					, Indices{+0, +0, -1}
				};

				const auto& voxel = voxels[hash];

				auto indices = voxels.index(hash);
				for (auto& check : checks)
				{
					auto index = voxels.clampToBoundaries(indices + check);

					if (voxels[index] == interior)
					{
						return true;
					}
				}

				return false;
			};

			refineLabel(voxels, exterior, boundary, hasInteriorNeighbour);
		}

		template<class Value>
		FVO<Value> fvoVoxelize(
			  const Mesh& mesh
			, int split
			, const Vec3& innerPoint
			, Value empty
			, Value interior
			, Value exterior
		)
		{	
			FVO<Value> voxels(
				AABB{Vec3{-1.0f}, Vec3{1.0f}}
				, split
				, empty
			);

			//get intial approximation
			initialVoxelization(voxels, mesh);

			//mark interior and exterior
			mark(voxels, innerPoint, empty, interior, exterior);

			//refine mesh
			refine(voxels, empty, interior, exterior);

			return voxels;
		}
	}

	template<class Value>
	FVO<Value> fvoVoxelize(const Mesh& mesh, int split, const Vec3& innerPoint, Value empty = -1, Value interior = -2, Value exterior = -3)
	{	
		return detail::fvoVoxelize<Value>(mesh, split, innerPoint, empty, interior, exterior);
	}
}
#endif