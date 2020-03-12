#pragma once

#include <Primitives/Primitives.h>

#include <vector>
#include <utility>
#include <algorithm>
#include <array>

// TODO : wrap Int32 into Mesh and correct its usage across project
// TODO : rewrite this
namespace mesh
{
	using math::Mat4;
	using math::Vec3;
	using math::Vec4;
	using math::Int32;

	using math::operator "" _FL;

	using primitive::Triangle;
	using primitive::AABB;

	// TODO : remove
	struct Mesh
	{
		std::vector<Triangle> triangles{};
	};

	// TODO : remove
	struct IndicesMesh
	{
		std::vector<Vec3>  points{};
		std::vector<Int32> indices{};
		Mat4 m_transform;
		AABB m_aabb;
	};

	// TODO : remove
	Mesh indicesToTriangle(const IndicesMesh& indicesMesh);

	// TODO ; remove
	Mesh testMesh();


	// PIMPL & ABI for suckers
	class TriangleMesh
	{
	public:
		using Triangles = std::vector<Triangle>;


	private:
		AABB computeAABB();

	public:
		TriangleMesh() = default;

		TriangleMesh(const Triangles& triangles, const Mat4& transform);
		TriangleMesh(Triangles&& triangles, const Mat4& transform);

		TriangleMesh(const TriangleMesh&) = default;
		TriangleMesh(TriangleMesh&& another);

		~TriangleMesh() = default;

		TriangleMesh& operator = (const TriangleMesh&) = default;
		TriangleMesh& operator = (TriangleMesh&& another);


	public:
		const Triangles& getTriangles() const;

		const Mat4& getTransform() const;

		const AABB& getAABB() const;


	public:
		auto begin()
		{
			return m_triangles.begin();
		}

		auto end()
		{
			return m_triangles.end();
		}

		auto begin() const
		{
			return m_triangles.begin();
		}

		auto end() const
		{
			return m_triangles.end();
		}

		auto size() const
		{
			return m_triangles.size();
		}


	private:
		Triangles m_triangles{};
		Mat4 m_transform{};
		AABB m_aabb{};
	};

	class TriangleIndicesMesh
	{
	public:
		using Index = Int32;
		using Points  = std::vector<Vec3>;
		using Indices = std::vector<Index>;

		friend class Iterator;

		template<bool Const = true>
		struct TriangleView
		{
			using Ref  = std::conditional_t<
				  Const
				, std::reference_wrapper<const Vec3>
				, std::reference_wrapper<Vec3>
			>;
			using Refs = std::array<Ref, 3>;

			Refs points;
		};

		class Iterator : std::iterator<std::bidirectional_iterator_tag, TriangleView<false>>
		{
		public:
			using Ret      = TriangleView<false>;
			using ConstRet = TriangleView<true>;

			friend class TriangleIndicesMesh;

		private:
			Iterator(TriangleIndicesMesh* owner, Int32 index)
				: m_owner(owner)
			{
				m_index = index - (index % 3);
				m_index = std::min(m_index, static_cast<Int32>(m_owner->m_indices.size()));
				m_index = std::max(m_index, -1);
			}


		public:
			Iterator()  = default;

			Iterator(const Iterator&) = default;
			Iterator(Iterator&&)      = default;

			~Iterator() = default;

			Iterator& operator = (const Iterator&) = default;
			Iterator& operator = (Iterator&&)      = default;


		public:
			bool operator == (const Iterator& another) const
			{
				return m_owner == another.m_owner && m_index == another.m_index;
			}

			bool operator != (const Iterator& another) const
			{
				return !(*this == another);
			}


			Ret operator * ()
			{
				return Ret
				{
					{
						  m_owner->m_points[m_owner->m_indices[m_index    ]]
						, m_owner->m_points[m_owner->m_indices[m_index + 1]]
						, m_owner->m_points[m_owner->m_indices[m_index + 2]]
					}
				};
			}

			ConstRet operator * () const
			{
				return ConstRet
				{
					{
						  m_owner->m_points[m_owner->m_indices[m_index    ]]
						, m_owner->m_points[m_owner->m_indices[m_index + 1]]
						, m_owner->m_points[m_owner->m_indices[m_index + 2]]
					}
				};
			}

			Ret operator -> ()
			{
				return Ret
				{
					{
						  m_owner->m_points[m_index    ]
						, m_owner->m_points[m_index + 1]
						, m_owner->m_points[m_index + 2]
					}
				};
			}

			ConstRet operator -> () const
			{
				return ConstRet
				{
					{
						  m_owner->m_points[m_index    ]
						, m_owner->m_points[m_index + 1]
						, m_owner->m_points[m_index + 2]
					}
				};
			}


			Iterator& operator ++ ()
			{
				if (m_index < m_owner->m_indices.size())
				{
					m_index += 3;
					m_index = std::min(m_index, static_cast<Int32>(m_owner->m_indices.size()));
				}
				return *this;
			}

			Iterator& operator -- ()
			{
				if (m_index >= 0)
				{
					m_index -= 3;
					m_index = std::max(m_index, -1);
				}
				return *this;
			}

		private:
			TriangleIndicesMesh* m_owner{};
			Int32 m_index{-1};
		};

		class ConstIterator : std::iterator<std::bidirectional_iterator_tag, TriangleView<true>>
		{
		public:
			using ConstRet = TriangleView<true>;

			friend class TriangleIndicesMesh;

		private:
			ConstIterator(const TriangleIndicesMesh* owner, Int32 index)
				: m_owner(owner) 
			{
				m_index = index - (index % 3);
				m_index = std::min(m_index, static_cast<Int32>(m_owner->m_indices.size()));
				m_index = std::max(m_index, -1);
			}


		public:
			ConstIterator()  = default;

			ConstIterator(const ConstIterator&) = default;
			ConstIterator(ConstIterator&&)      = default;

			~ConstIterator() = default;

			ConstIterator& operator = (const ConstIterator&) = default;
			ConstIterator& operator = (ConstIterator&&)      = default;


		public:
			bool operator == (const ConstIterator& another) const
			{
				return m_owner == another.m_owner && m_index == another.m_index;
			}

			bool operator != (const ConstIterator& another) const
			{
				return !(*this == another);
			}


			ConstRet operator * () const
			{
				return ConstRet
				{
					{
						m_owner->m_points[m_owner->m_indices[m_index    ]]
						, m_owner->m_points[m_owner->m_indices[m_index + 1]]
					, m_owner->m_points[m_owner->m_indices[m_index + 2]]
					}
				};
			}

			ConstRet operator -> () const
			{
				return ConstRet
				{
					{
						m_owner->m_points[m_index    ]
						, m_owner->m_points[m_index + 1]
					, m_owner->m_points[m_index + 2]
					}
				};
			}


			ConstIterator& operator ++ ()
			{
				if (m_index < m_owner->m_indices.size())
				{
					m_index += 3;
					m_index = std::min(m_index, static_cast<Int32>(m_owner->m_indices.size()));
				}
				return *this;
			}

			ConstIterator& operator -- ()
			{
				if (m_index >= 0)
				{
					m_index -= 3;
					m_index = std::max(m_index, -1);
				}
				return *this;
			}

		private:
			const TriangleIndicesMesh* m_owner{};
			Int32 m_index{-1};
		};


	private:
		AABB computeAABB();

	public:
		TriangleIndicesMesh() = default;

		TriangleIndicesMesh(const Points& points, const Indices& indices, const Mat4& transform);
		TriangleIndicesMesh(Points&& points, Indices&& indices, const Mat4& transform);

		TriangleIndicesMesh(const TriangleIndicesMesh&) = default;
		TriangleIndicesMesh(TriangleIndicesMesh&& another);

		~TriangleIndicesMesh() = default;

		TriangleIndicesMesh& operator = (const TriangleIndicesMesh&) = default;
		TriangleIndicesMesh& operator = (TriangleIndicesMesh&& another);


	public:
		const Points& getPoints() const;

		const Indices& getIndices() const;

		const Mat4& getTransform() const;

		const AABB& getAABB() const;


	public:
		auto begin()
		{
			return Iterator(this, 0);
		}

		auto end()
		{
			return Iterator(this, m_indices.size());
		}

		auto begin() const
		{
			return ConstIterator(this, 0);
		}

		auto end() const
		{
			return ConstIterator(this, m_indices.size());
		}

		auto size() const
		{
			return m_indices.size() / 3;
		}


	private:
		std::vector<Vec3>  m_points{};
		std::vector<Int32> m_indices{};
		Mat4 m_transform{};
		AABB m_aabb{};
	};
}