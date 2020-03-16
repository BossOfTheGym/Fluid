#pragma once

#include <queue>
#include <algorithm>
#include <set>

#include <Primitives/Primitives.h>

namespace voxel
{
	using math::Float;
	using math::Int32;
	using math::Vec3;

	using math::operator "" _FL;

	using primitive::Triangle;
	using primitive::AABB;
	using primitive::RoundedTriangle;

	namespace detail
	{
		// default handlers
		struct DoNothing
		{
			template<class ... Args>
			void operator () (Args&& ... args)
			{}
		};

		struct SimpleAdd
		{
			template
			<
				  class Mesh
				, class Grid
				, class Triangle
				, class Number
				, class Hash
			>
			void operator () 
			(
				  Mesh&& mesh
				, Grid&& grid
				, Triangle&& triangle
				, Number&& triNumber
				, Hash&& cellHash
			)
			{
				grid.add(cellHash);
			}
		};

		// 1
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
			Set cellQueue;
			for (auto& p : tri.points)
			{
				auto indices = grid.index(p);

				for (Hash i = -1; i < 2; i++)
				for (Hash j = -1; j < 2; j++)
				for (Hash k = -1; k < 2; k++)
				{
					auto neighbour = grid.clampToBoundaries(indices + Indices{i, j, k});

					auto hash = grid.hash(neighbour);
					//cellQueue.push_back(hash);
					cellQueue.insert(hash);
				}
			}

			//traverse all volume cells
			Set traversedCells;
			while (!cellQueue.empty())
			{
				//auto hash    = cellQueue.back();
				auto hash    = *cellQueue.begin();
				auto indices = grid.index(hash);
				auto cell    = grid.center(indices);
				//cellQueue.pop_back();
				cellQueue.erase(hash);

				auto notTraversed = traversedCells.find(hash) == traversedCells.end(); 
				auto inVolume     = primitive::pointInVolume(cell, volume);
				if (notTraversed && inVolume)
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
							//cellQueue.push_back(hash);
							cellQueue.insert(hash);
						}
					}
				}
			}

			return traversedCells;
		}

		// 2. mark boundary
		// TODO : check neighbours while initializing prev
		template<class Grid>
		auto getBoundaryCells(const Grid& grid)
		{
			using Indices = typename Grid::Indices;
			using Point   = typename Grid::Point;
			using Hash    = typename Grid::Hash;
			using Set     = std::set<Hash>;

			static const Indices NEIGHBOURS[] = 
			{
				  Indices{+1,  0,  0}
				, Indices{-1,  0,  0}
				, Indices{ 0, +1,  0}
				, Indices{ 0, -1,  0}
				, Indices{ 0,  0, +1}
				, Indices{ 0,  0, -1}
			};

			Set prev; // cells traversed on the previous step(so we don't add them )
			Set curr; // cells that we will traverse to find ones we will traverse next
			Set next; // cells that we will traverse
			Set boundary; // boundary cells

			// add grid bounds to begin
			auto insertConditionally = [&] (auto condition, auto value, auto& set1, auto& set2)
			{
				if (condition)
				{
					set1.insert(value);
				}
				else
				{
					set2.insert(value);
				}
			};

			auto split = grid.getSplit();
			for (Hash i = 0; i < split[0]; i++)
			for (Hash j = 0; j < split[1]; j++)
			{
				auto hash = grid.hash(Indices{i, j, 0});
				insertConditionally(!grid.has(hash), hash, prev, boundary);

				hash = grid.hash(Indices{i, j, split[2] - 1});
				insertConditionally(!grid.has(hash), hash, prev, boundary);
			}

			for (Hash j = 0; j < split[1]; j++)
			for (Hash k = 0; k < split[2]; k++)
			{
				auto hash = grid.hash(Indices{0, j, k});
				insertConditionally(!grid.has(hash), hash, prev, boundary);

				hash = grid.hash(Indices{split[0] - 1, j, k});
				insertConditionally(!grid.has(hash), hash, prev, boundary);
			}

			for (Hash i = 0; i < split[0]; i++)
			for (Hash k = 0; k < split[2]; k++)
			{
				auto hash = grid.hash(Indices{i, 0, k});
				insertConditionally(!grid.has(hash), hash, prev, boundary);

				hash = grid.hash(Indices{i, split[1] - 1, k});
				insertConditionally(!grid.has(hash), hash, prev, boundary);
			}

			// initialize curr cells set
			auto inSet = [&] (auto value, auto& set)
			{
				return set.find(value) != set.end();
			};

			for (auto& hash : prev)
			{
				auto indices = grid.index(hash);
				for (auto& n : NEIGHBOURS)
				{
					auto ni = grid.clampToBoundaries(indices + n);
					auto nh = grid.hash(ni);

					auto inGrid = grid.has(nh);
					if (!inSet(nh, prev) && !inGrid)
					{
						curr.insert(nh);
					}
					if (inGrid)
					{
						boundary.insert(nh);
					}
				}
			}

			// traverse grid
			while(!curr.empty())
			{
				for (auto& ch : curr)
				{
					// traverse neighbours of each cell
					auto indices = grid.index(ch);
					for (auto& n : NEIGHBOURS)
					{
						auto ni = grid.clampToBoundaries(indices + n);
						auto nh = grid.hash(ni);

						auto inPrev = inSet(nh, prev);
						auto inCurr = inSet(nh, curr);
						auto inGrid = grid.has(nh);
						if (!inPrev && !inCurr && !inGrid)
						{
							next.insert(nh);
						}
						if (inGrid)
						{
							boundary.insert(nh);
						}
					}
				}

				std::swap(prev, curr); // prev <- curr
				std::swap(curr, next); // curr <- next
				next.clear();
			}

			return boundary;
		}

		// 3. remove inner cells
		template<class Grid, class Boundary>
		auto getInnerCells(const Grid& grid, Boundary boundaryCells)
		{
			using Indices = typename Grid::Indices;
			using Point   = typename Grid::Point;
			using Hash    = typename Grid::Hash;
			using Set     = std::set<Hash>;

			Set innerCells;
			for (auto& [hash, value] : grid)
			{
				if (boundaryCells.find(hash) == boundaryCells.end())
				{
					innerCells.insert(hash);
				}
			}
			return innerCells;
		}
	}

	// TODO : improve this with if constexpr() to remove unneccessary Donothing call
	// TODO : add some type check support so it can correctly handle case when 
	// handlers are not default-contructable
	// TODO : rearrange a bit so post-processing is made after inner cells removed
	// TODO : add proper checking if triangle lies in-between the grid cells
	// Mesh : mesh class
	// Grid : grid class
	// WhenTraversed : functor takes Mesh, Grid, Triangle, TriangleNumber, cellIndex(handles event when voxel is added)
	// PostProcess   : functor takes Mesh, Grid, cellIndex(used for post-processing) 
	template
	<
		  class Mesh
		, class Grid
		, class WhenAdded   = detail::SimpleAdd
		, class PostProcess = detail::DoNothing
	>
	void meshToCells
	(
		  Mesh&& mesh
		, Grid& grid
		, WhenAdded   whenAdded   = WhenAdded()
		, PostProcess postProcess = PostProcess()
	)
	{
		using Set   = std::set<typename Grid::Hash>;
		using Index = size_t;

		const Vec3 halfCellSize = grid.getCellSize() / 2.0_FL;
		const Vec3 DIAGS[] = 		
		{
			  Vec3{-1.0_FL, -1.0_FL, -1.0_FL} * halfCellSize
			, Vec3{-1.0_FL, -1.0_FL, +1.0_FL} * halfCellSize
			, Vec3{-1.0_FL, +1.0_FL, -1.0_FL} * halfCellSize
			, Vec3{-1.0_FL, +1.0_FL, +1.0_FL} * halfCellSize
		};
		auto getLargestProjection = [&](const auto& normal)
		{
			Float proj = 0.0_FL;
			for (auto& diag : DIAGS)
			{
				proj = std::max(proj, std::abs(glm::dot(diag, normal)));
			}
			return proj;
		};

		const Float radius  = glm::length(halfCellSize) * (1.0_FL - math::EPS);
		const Float radius2 = radius * 2.0_FL;

		Set allTraversed;
		Index i = 0;
		for(auto& triangle : mesh)
		{
			auto [normal, length] = primitive::triangleNormalLength(triangle);

			auto proj = getLargestProjection(normal) * (1.0_FL - math::EPS);

			auto volume = primitive::roundedTriangleFromRadius(triangle, radius, radius, radius);

			Set traversed = detail::traverseCells(grid, volume, triangle);
			for(auto& hash : traversed)
			{
				whenAdded(mesh, grid, triangle, i, hash);
			}
			allTraversed.merge(traversed);

			++i;
		}
		for (auto& hash : allTraversed)
		{
			postProcess(mesh, grid, hash);
		}

		Set boundaryCells = detail::getBoundaryCells(grid);
		Set innerCells    = detail::getInnerCells(grid, boundaryCells);
		for(auto& hash : innerCells)
		{
			grid.remove(hash);
		}
	}
}
