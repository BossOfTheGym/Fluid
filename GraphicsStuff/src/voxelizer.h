#pragma once

#include <queue>
#include <algorithm>
#include <set>

#include <Primitives/Primitives.h>
#include <Mesh/Mesh.h>
#include <DataStructures/SVO.h>


namespace voxel
{
	/*template<class Value>
	using SVO = octree::SparseVoxelOctree<Value>;

	template<class Value>
	using FVO = octree::FullVoxelOctree<Value>;*/


	using math::Float;
	using math::Int32;
	using math::Vec3;

	using math::operator "" _FL;


	using mesh::Mesh;
	using mesh::IndicesMesh;


	using primitive::Triangle;
	using primitive::AABB;
	using primitive::RoundedTriangle;

	template<class Value>
	using SG = ds::SparseGrid<Value>;

	template<class Value>
	using FG = ds::FullGrid<Value>;


	namespace detail
	{
		// 1.
		template<class Grid, class Volume>
		auto traverseCells(const Grid& grid, const Volume& volume, const Triangle& tri)
		{
			using Indices = typename Grid::Indices;
			using Point   = typename Grid::Point;
			using Hash    = typename Grid::Hash;
			using Set     = std::set<Hash>;

			// in fact not a queue(more like stack) as we do not care about the processing order
			using Queue = std::vector<Hash>;

			//add all neighbouring cells of triangle points
			Queue cellQueue;
			for (auto& p : tri.points)
			{
				auto indices = grid.index(p);

				for (Hash i = -1; i < 2; i++)
				for (Hash j = -1; j < 2; j++)
				for (Hash k = -1; k < 2; k++)
				{
					auto neighbour = grid.clampToBoundaries(indices + Indices{i, j, k});

					auto hash = grid.hash(neighbour);
					cellQueue.push_back(hash);
				}
			}

			//traverse all volume cells
			Set traversedCells;
			while (!cellQueue.empty())
			{
				auto hash    = cellQueue.back();
				auto indices = grid.index(hash);
				auto cell    = grid.cell(indices);
				cellQueue.pop_back();

				bool notTraversed = traversedCells.find(hash) == traversedCells.end(); 
				if (notTraversed && primitive::pointInVolume(cell, volume))
				{
					traversedCells.insert(hash);

					for (Hash i = -1; i < 2; i++)
					for (Hash j = -1; j < 2; j++)
					for (Hash k = -1; k < 2; k++)
					{
						auto neighbour = grid.clampToBoundaries(indices + Indices{i, j, k});

						auto hash = grid.hash(neighbour);
						if (traversedCells.find(hash) == traversedCells.end())
						{
							cellQueue.push_back(hash);
						}
					}
				}
			}

			return traversedCells;
		}

		// 2. mark boundary
		template<class Grid>
		auto getBoundaryCells(const Grid& grid)
		{
			using Indices = typename Grid::Indices;
			using Point   = typename Grid::Point;
			using Hash    = typename Grid::Hash;
			using Set     = std::set<Hash>;

			Set prev; // cells traversed on the previous step(so we don't add them )
			Set curr; // cells that we will traverse

			// add grid bounds to begin
			auto split = grid.split();
			for (Hash i = 0; i < split[0])
		}

		// 3. remove inner cells
		template<class Grid, class Boundary>
		auto removeInnerCells(const Grid& grid, Boundary boundaryCells)
		{
			
		}
	}

	// Mesh : mesh class
	// Grid : grid class
	// Handler : functor accepting Grid, Triangle, TriangleNumber, cellIndex,
	// used to add some additional logic when adding new cell to grid
	template<class Mesh, class Grid, class Handler>
	auto meshToCells(Mesh&& mesh, Grid& grid, Handler handler)
	{
		return 0;
	}

	/*
	//Voxelizer. Traverses voxels belonging to some volume
	namespace detail
	{
		//voxelization algorithm
		template<class VO>
		auto traverseVoxels(const VO& voxelTree, const RoundedTriangle& volume)
		{
			using Indices = typename VO::Indices;
			using Point   = typename VO::Point;
			using Hash    = typename VO::Hash;
			using Set     = std::set<Hash>;

			// in fact not a queue but we do not care about the incoming order
			using Queue   = std::vector<Indices>;


			auto& [vertices, edges, triangle] = volume;

			Queue voxelQueue;

			//add all voxels from vertices
			auto split = voxelTree.split();
			for (auto& vertex : vertices)
			{
				auto indices = voxelTree.index(vertex.center);

				for (Hash i = -1; i < 2; i++)
				for (Hash j = -1; j < 2; j++)
				for (Hash k = -1; k < 2; k++)
				{
					auto toPush = voxelTree.clampToBoundaries(indices + Indices{i, j, k});

					voxelQueue.push_back(toPush);
				}
			}

			//traverse all voxels
			Set traversedVoxels;
			while (!voxelQueue.empty())
			{
				auto indices = voxelQueue.back();
				voxelQueue.pop_back();

				auto voxel   = voxelTree.voxel(indices);
				auto hash    = voxelTree.hash(indices);

				bool notTraversed = traversedVoxels.find(hash) == traversedVoxels.end(); 
				if (notTraversed && primitive::pointInRoundedTriangle(voxel, volume))
				{
					traversedVoxels.insert(hash);

					for (Hash i = -1; i < 2; i++)
					for (Hash j = -1; j < 2; j++)
					for (Hash k = -1; k < 2; k++)
					{
						auto toPush = voxelTree.clampToBoundaries(indices + Indices{i, j, k});

						auto hash = voxelTree.hash(toPush);
						if (traversedVoxels.find(hash) == traversedVoxels.end())
						{
							voxelQueue.push_back(toPush);
						}
					}
				}
			}

			return traversedVoxels;
		}
	}


	//Creates only voxelized boundary of volume
	namespace detail
	{
		Float getLargestDiagonalProj(const Vec3& normal, Float halfVoxelSize)
		{
			static const Vec3 voxelDiagonals[4] =
			{
				  Vec3{-halfVoxelSize, -halfVoxelSize, -halfVoxelSize}
				, Vec3{-halfVoxelSize, -halfVoxelSize, +halfVoxelSize}
				, Vec3{-halfVoxelSize, +halfVoxelSize, -halfVoxelSize}
				, Vec3{-halfVoxelSize, +halfVoxelSize, +halfVoxelSize}
			};

			Float proj = 0.0_FL;
			for (auto& diag : voxelDiagonals)
			{
				proj = std::max(proj, std::abs(glm::dot(diag, normal)));
			}

			return proj;
		};

		template<class Value>
		SVO<Value> svoVoxelize(const mesh::Mesh& mesh, Int32 split)
		{
			SVO<Value> voxels(
				AABB{Vec3{-1.0_FL}, Vec3{1.0_FL}}
				, split
			);

			Float voxelSize     = 2.0_FL / split;
			Float voxelHalfSize = 1.0_FL / split;
			Float radius  = voxelSize * math::SQ3 / 2;
			Float radius2 = voxelSize * math::SQ3;
			for (Value i = 0; i < mesh.triangles.size(); i++)
			{
				auto triangle = mesh.triangles[i];

				// TODO : normal check here possible, check if triangle plane is parallel to xy or yz or xz plane

				Float height = getLargestDiagonalProj(primitive::triangleNormal(triangle), voxelHalfSize);
				
				// TEST
				height = voxelHalfSize;
				radius = height;
				// END TEST

				auto volume  = primitive::roundedTriangleFromTriangleRadius(triangle, radius, 2.0_FL * height);
				auto traversed = traverseVoxels(voxels, volume);
				for(auto& voxel : traversed)
				{
					voxels[voxel] = i;
				}
			}

			return voxels;
		}
	}

	template<class Value>
	SVO<Value> svoVoxelize(const Mesh& mesh, Int32 split)
	{
		return detail::svoVoxelize<Value>(mesh, split);
	}

	template<class Value>
	SVO<Value> svoVoxelize(const IndicesMesh& mesh, Int32 split)
	{
		return detail::svoVoxelize<Value>(mesh::indicesToTriangle(mesh), split);
	}



	//Creates fully voxelized volume
	namespace detail
	{
		// TODO : rework this
		// TODO : change algorithm to mark volume starting from the volume bounds
		template<class Value>
		void initialVoxelization(FVO<Value>& voxels, const Mesh& mesh)
		{
			Float voxelSize = 2.0_FL / voxels.split();
			Float radius  = voxelSize * math::SQ3 / 2;
			Float radius2 = voxelSize * math::SQ3;
			for (Value i = 0; i < mesh.triangles.size(); i++)
			{
				auto triangle = mesh.triangles[i];

				//translate triangle
				auto& [p0, p1, p2] = triangle.points;

				auto volume = primitive::roundedTriangleFromTriangleRadius(triangle, radius, radius2);
				auto traversed = traverseVoxels(voxels, volume);
				for(auto& voxel : traversed)
				{
					voxels[voxel] = i;
				}
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

				for (int i = -1; i < 2; i++)
				{
					for (int j = -1; j < 2; j++)
					{
						for (int k = -1; k < 2; k++)
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
			, Int32 split
			, const Vec3& innerPoint
			, Value empty
			, Value interior
			, Value exterior
		)
		{	
			FVO<Value> voxels(
				AABB{Vec3{-1.0_FL}, Vec3{1.0_FL}}
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
	FVO<Value> fvoVoxelize(const Mesh& mesh, Int32 split, const Vec3& innerPoint, Value empty, Value interior, Value exterior)
	{	
		return detail::fvoVoxelize<Value>(mesh, split, innerPoint, empty, interior, exterior);
	}

	template<class Value>
	FVO<Value> fvoVoxelize(const IndicesMesh& mesh, Int32 split, const Vec3& innerPoint, Value empty, Value interior, Value exterior)
	{
		return detail::fvoVoxelize<Value>(mesh::indicesToTriangle(mesh), split, innerPoint, empty, interior, exterior);
	}
	*/
}
