#pragma once

#include <Primitives/Primitives.h>

#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <iterator>

namespace ds
{
	using math::Vec3;
	using math::Vec3i;
	using math::Vec3i32;
	using math::Vec3u32;
	using math::Vec3i64;
	using math::Vec3u64;
	using math::Int32;
	using math::Int64;	
	using math::Float;

	using primitive::AABB;


    // next power of 2
	template<class IntType, auto bits = 8 * sizeof(IntType)>
	IntType npo2(IntType num)
	{
		num -= 1;

		decltype(bits) bit = 1;
		while (bit < bits)
		{
			num = num | (num >> bit);
			bit <<= 1;
		}

		return num + 1;
	}

	// binary logarithm 
	template<class IntType>
	IntType blog(IntType num)
	{
		IntType count{};
		while (num)
		{
			num >>= 1;
			count++;
		}
		return count;
	}

	// TODO : rework this
	// TODO : move common part to base template class Grid<T>
	// TODO : add smart SFINAE check for find() method
	// TODO : bounds-safe methods
	// TODO : add support for the case Value = void(so this thing will work like std::set)
	template<class Value>
	class SparseGrid
	{
	public:
		using Int  = Int64;
		using Hash = Int64;
		using Grid = std::map<Hash, Value>;
		using Point   = Vec3;
		using Indices = Vec3i32;

	public:
		SparseGrid(const AABB& space, const Vec3i32& split)
		{
			// TODO : some error processing?

			auto [fc, sc] = space.corners;
			auto delta = sc - fc;

			m_space = space;
			m_split = split;

			m_cellSize[0] = delta[0] / m_split[0];			
			m_cellSize[1] = delta[1] / m_split[1];
			m_cellSize[2] = delta[2] / m_split[2];
			m_halfCellSize = m_cellSize / 2.0_FL;

			Float min = std::min(std::min(m_cellSize[0], m_cellSize[1]), m_cellSize[2]);
			m_sizeRatios = m_cellSize / min;

			m_biasYZ = m_split.y * m_split.z;
			m_biasZ  = m_split.z;
		}

		SparseGrid(const AABB& space, const Vec3& cellSize)
		{
			// TODO : some error processing?

			auto [fc, sc] = space.corners;
			auto delta = sc - fc;

			m_cellSize = cellSize;
			m_halfCellSize = m_cellSize / 2.0_FL;

			m_split[0] = static_cast<Int32>(std::ceil(delta / m_cellSize[0]));
			m_split[1] = static_cast<Int32>(std::ceil(delta / m_cellSize[1]));
			m_split[2] = static_cast<Int32>(std::ceil(delta / m_cellSize[2]));

			m_space.sc[0] = m_space.fc[0] + m_cellSize[0] * m_split[0];
			m_space.sc[1] = m_space.fc[1] + m_cellSize[1] * m_split[1];
			m_space.sc[2] = m_space.fc[2] + m_cellSize[2] * m_split[2];

			Float min = std::min(std::min(m_cellSize[0], m_cellSize[1]), m_cellSize[2]);
			m_sizeRatios = m_cellSize / min;

			m_biasYZ = m_split.y * m_split.z;
			m_biasZ = m_split.z;
		}

		~SparseGrid() = default;

		SparseGrid(const SparseGrid&) = default;
		SparseGrid(SparseGrid&& another)
		{
			*this = std::move(another);
		}

		SparseGrid& operator = (const SparseGrid&) = default;
		SparseGrid& operator = (SparseGrid&& another)
		{
			std::swap(m_grid, another.m_grid);
			std::swap(m_space, another.m_space);
			std::swap(m_cellSize, another.m_cellSize);
			std::swap(m_sizeRatio, another.m_sizeRatio);
			std::swap(m_split, another.m_split);
			std::swap(m_biasYZ, another.m_biasYZ);
			std::swap(m_biasZ, another.m_biasZ);

			return *this;
		}


	public:
		Indices index(const Point& item) const
		{
			Point temp = item - m_space.corners[0];

			Indices indices{};
			indices[0] = Hash(temp[0] / m_cellSize[0]);
			indices[1] = Hash(temp[1] / m_cellSize[1]);
			indices[2] = Hash(temp[2] / m_cellSize[2]);
			return indices;
		}

		Indices index(const Hash& item) const
		{
			Hash hash = item;

			Indices result;
			result[0] = hash / m_biasYZ; hash %= m_biasYZ;
			result[1] = hash / m_biasZ;	hash %= m_biasZ;
			result[2] = hash;
			return result;
		}

		Indices index(const Indices& item) const
		{
			return item;
		}


		Point point(const Point& item) const
		{
			return item;
		}

		Point point(const Hash& item) const
		{
			return point(index(item));
		}

		Point point(const Indices& item) const
		{
			Point temp(m_space.corners[0]);
			temp[0] += item[0] * m_cellSize[0];
			temp[1] += item[1] * m_cellSize[1];
			temp[2] += item[2] * m_cellSize[2];
			return temp;
		}


		Point center(const Point& item) const
		{
			return point(item) + m_halfCellSize;
		}

		Point center(const Hash& item) const
		{
			return point(item) + m_halfCellSize;
		}

		Point center(const Indices& item) const
		{
			return point(item) + m_halfCellSize;
		}


		Hash hash(const Point& item) const
		{
			return hash(index(item));
		}

		Hash hash(const Hash& item) const
		{
			// TODO : no checks provided, maybe should?
			return item;
		}

		Hash hash(const Indices& item) const
		{
			return item.x * m_biasYZ + item.y * m_biasZ + item.z;
		}


		bool inBounds(const Point& item) const
		{
			return primitive::pointInAABB(item, m_space);
		}

		bool inBounds(const Hash& item) const
		{
			return inBounds(index(item));
		}

		bool inBounds(const Indices& item) const
		{
			return 0 <= item[0] && item[0] <= m_split[0] - 1
				&& 0 <= item[1] && item[1] <= m_split[1] - 1
				&& 0 <= item[2] && item[2] <= m_split[2] - 1
		}

		bool boundary(const Point& item) const
		{
			auto indices = index(item);

			return indices[0] == 0
				|| indices[1] == 0 
				|| indices[2] == 0
				|| indices[0] == m_split[0] - 1 
				|| indices[1] == m_split[1] - 1
				|| indices[2] == m_split[2] - 1;
		}

		bool boundary(const Hash& item) const
		{
			auto indices = index(item);

			return indices[0] == 0
				|| indices[1] == 0 
				|| indices[2] == 0
				|| indices[0] == m_split[0] - 1 
				|| indices[1] == m_split[1] - 1
				|| indices[2] == m_split[2] - 1;
		}

		bool boundary(const Indices& item) const
		{
			auto indices = index(item);

			return indices[0] == 0
				|| indices[1] == 0 
				|| indices[2] == 0
				|| indices[0] == m_split[0] - 1 
				|| indices[1] == m_split[1] - 1
				|| indices[2] == m_split[2] - 1;
		}


		Indices clampToBoundaries(const Indices& item) const
		{
			Indices inBoundaries = item;
			inBoundaries.x = std::max(std::min(inBoundaries.x, m_split[0] - 1), 0);
			inBoundaries.y = std::max(std::min(inBoundaries.y, m_split[1] - 1), 0);
			inBoundaries.z = std::max(std::min(inBoundaries.z, m_split[2] - 1), 0);
			return inBoundaries;
		}


	public:
		const AABB& getSpace() const
		{
			return m_space;
		}

		const Vec3& getCellSize() const
		{
			return m_cellSize;
		}

		const Vec3& getSizeRatios() const
		{
			return m_sizeRatios;
		}

		const Vec3i32& getSplit() const
		{
			return m_split;
		}


	public:
		bool has(const Point& item) const
		{
			return m_grid.find(hash(item)) != m_grid.end();
		}

		bool has(const Hash& item) const
		{
			return m_grid.find(hash(item)) != m_grid.end();
		}

		bool has(const Indices& item) const
		{
			return m_grid.find(hash(item)) != m_grid.end();
		}


		Value& operator [] (const Point& item)
		{
			return m_grid[hash(item)];
		}

		Value& operator [] (const Hash& item)
		{
			return m_grid[hash(item)];
		}

		Value& operator [] (const Indices& item)
		{
			return m_grid[hash(item)];
		}


		void add(const Point& item, const Value& value = Value())
		{
			m_grid[hash(item)] = value;
		}

		void add(const Hash& item, const Value& value = Value())
		{
			m_grid[hash(item)] = value;
		}

		void add(const Indices& item, const Value& value = Value())
		{
			m_grid[hash(item)] = value;
		}


		Value& get(const Point& item)
		{
			auto i = m_grid.find(hash(item));

			return *i;
		}

		Value& get(const Hash& item)
		{
			auto i = m_grid.find(hash(item));

			return *i;
		}

		Value& get(const Indices& item)
		{
			auto i = m_grid.find(hash(item));

			return *i;
		}


		const Value& get(const Point& item) const
		{
			auto i = m_grid.find(hash(item));

			return *i;
		}

		const Value& get(const Hash& item) const
		{
			auto i = m_grid.find(hash(item));

			return *i;
		}

		const Value& get(const Indices& item) const
		{
			auto i = m_grid.find(hash(item));

			return *i;
		}


		void remove(const Point& item)
		{
			m_grid.erase(hash(item));
		}

		void remove(const Hash& item)
		{
			m_grid.erase(hash(item));
		}

		void remove(const Indices& indices)
		{
			m_grid.erase(hash(indices));
		}


	public:
		auto begin()
		{
			return m_grid.begin();
		}

		auto end()
		{
			return m_grid.end();
		}


		auto begin() const
		{
			return m_grid.begin();
		}

		auto end() const
		{
			return m_grid.end();
		}


		auto size() const
		{
			return m_grid.size();
		}


		void clear()
		{
			m_grid.clear();
		}


	private:
		Grid m_grid{};

		AABB m_space{};

		Vec3 m_cellSize{};
		Vec3 m_halfCellSize{};
		Vec3 m_sizeRatios{};

		Vec3i32 m_split{};

		Int32 m_biasYZ{};
		Int32 m_biasZ{};
	};


	template<class Value>
	class FullGrid
	{
	public:
		using Int  = Int64;
		using Hash = Int64;
		using Grid   = std::vector<Value>;
		using Status = std::vector<bool>;
		using Point   = Vec3;
		using Indices = Vec3i32;


		class Iterator 
			: public std::iterator<std::bidirectional_iterator_tag, std::tuple<Hash, Value&>>
		{
		public:
			using Ret      = std::tuple<Hash, Value&>;
			using ConstRet = Ret;//std::tuple<Hash, const Value&>;

			friend class FullGrid;


		private:
			Iterator(FullGrid* owner, Int32 index)
				: m_owner(owner)
				, m_index(index)
			{
				m_index = std::max(m_index, static_cast<Int32>(-1));
				m_index = std::min(m_index, static_cast<Int32>(m_owner->size()));
				while(m_index < m_owner->size() && !m_owner->has(m_index))
				{
					m_index++;
				}
			}

		public:
			Iterator() = default;

			Iterator(const Iterator& another) = default;
			Iterator(Iterator&&)              = default;

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
				return {m_index, m_owner->get(m_index)};
			}

			ConstRet operator * () const
			{
				return {m_index, m_owner->get(m_index)};
			}

			Value& operator -> ()
			{
				return m_owner->get(m_index);
			}

			const Value& operator -> () const
			{
				return m_owner->get(m_index);
			}


			Iterator& operator ++ ()
			{
				if (m_index < m_owner->size())
				{
					m_index++;
				}
				while(m_index < m_owner->size() && !m_owner->has(m_index))
				{
					m_index++;
				}
				return *this;
			}

			Iterator& operator -- ()
			{
				if (m_index >= 0)
				{
					m_index--;
				}
				while(m_index >= 0 && !m_owner->has(m_index))
				{
					m_index--;
				}
				return *this;
			}


		private:
			Int32 m_index{-1};
			FullGrid* m_owner{};
		};

		class ConstIterator 
			: public std::iterator<std::bidirectional_iterator_tag, std::tuple<Hash, const Value&>>
		{
		public:
			using ConstRet = std::tuple<Hash, const Value&>;

			friend class FullGrid;


		private:
			ConstIterator(const FullGrid* owner, Int32 index)
				: m_owner(owner)
				, m_index(index)
			{
				m_index = std::max(m_index, static_cast<Int32>(-1));
				m_index = std::min(m_index, static_cast<Int32>(m_owner->size()));
				while(m_index < m_owner->size() && !m_owner->has(m_index))
				{
					m_index++;
				}
			}

		public:
			ConstIterator() = default;

			ConstIterator(const ConstIterator& another) = default;
			ConstIterator(ConstIterator&&)              = default;

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
				return {m_index, m_owner->get(m_index)};
			}

			const Value& operator -> () const
			{
				return m_owner->get(m_index);
			}


			ConstIterator& operator ++ ()
			{
				if (m_index < m_owner->size())
				{
					m_index++;
				}
				while(m_index < m_owner->size() && !m_owner->has(m_index))
				{
					m_index++;
				}
				return *this;
			}

			ConstIterator& operator -- ()
			{
				if (m_index >= 0)
				{
					m_index--;
				}
				while(m_index >= 0 && !m_owner->has(m_index))
				{
					m_index--;
				}
				return *this;
			}


		private:
			Int32 m_index{-1};
			const FullGrid* m_owner{};
		};


	public:
		FullGrid(const AABB& space, const Vec3i32& split)
		{
			// TODO : some error processing?

			auto [fc, sc] = space.corners;
			auto delta = sc - fc;

			m_space = space;
			m_split = split;

			m_cellSize[0] = delta[0] / m_split[0];			
			m_cellSize[1] = delta[1] / m_split[1];
			m_cellSize[2] = delta[2] / m_split[2];
			m_halfCellSize = m_cellSize / 2.0_FL;

			Float min = std::min(std::min(m_cellSize[0], m_cellSize[1]), m_cellSize[2]);
			m_sizeRatios = m_cellSize / min;

			m_biasYZ = m_split.y * m_split.z;
			m_biasZ  = m_split.z;

			m_grid.resize(m_split.x * m_split.y * m_split.z);
			m_status.resize(m_split.x * m_split.y * m_split.z);
		}

		FullGrid(const AABB& space, const Vec3& cellSize)
		{
			// TODO : some error processing?

			auto [fc, sc] = space.corners;
			auto delta = sc - fc;

			m_cellSize = cellSize;
			m_halfCellSize = m_cellSize / 2.0_FL;

			m_split[0] = static_cast<Int32>(std::ceil(delta / m_cellSize[0]));
			m_split[1] = static_cast<Int32>(std::ceil(delta / m_cellSize[1]));
			m_split[2] = static_cast<Int32>(std::ceil(delta / m_cellSize[2]));

			m_space.sc[0] = m_space.fc[0] + m_cellSize[0] * m_split[0];
			m_space.sc[1] = m_space.fc[1] + m_cellSize[1] * m_split[1];
			m_space.sc[2] = m_space.fc[2] + m_cellSize[2] * m_split[2];

			Float min = std::min(std::min(m_cellSize[0], m_cellSize[1]), m_cellSize[2]);
			m_sizeRatios = m_cellSize / min;

			m_biasYZ = m_split.y * m_split.z;
			m_biasZ = m_split.z;

			m_grid.resize(m_split.x * m_split.y * m_split.z);
			m_status.resize(m_split.x * m_split.y * m_split.z);
		}

		~FullGrid() = default;

		FullGrid(const FullGrid&) = default;
		FullGrid(FullGrid&& another)
		{
			*this = std::move(another);
		}

		FullGrid& operator = (const FullGrid&) = default;
		FullGrid& operator = (FullGrid&& another)
		{
			std::swap(m_grid, another.m_grid);
			std::swap(m_space, another.m_space);
			std::swap(m_cellSize, another.m_cellSize);
			std::swap(m_sizeRatio, another.m_sizeRatio);
			std::swap(m_status, another.m_status);
			std::swap(m_split, another.m_split);
			std::swap(m_biasYZ, another.m_biasYZ);
			std::swap(m_biasZ, another.m_biasZ);

			return *this;
		}


	public:
		Indices index(const Point& item) const
		{
			Point temp = item - m_space.corners[0];

			Indices indices{};
			indices[0] = Hash(temp[0] / m_cellSize[0]);
			indices[1] = Hash(temp[1] / m_cellSize[1]);
			indices[2] = Hash(temp[2] / m_cellSize[2]);
			return indices;
		}

		Indices index(const Hash& item) const
		{
			Hash hash = item;

			Indices result;
			result[0] = hash / m_biasYZ; hash %= m_biasYZ;
			result[1] = hash / m_biasZ;	 hash %= m_biasZ;
			result[2] = hash;
			return result;
		}

		Indices index(const Indices& item) const
		{
			return item;
		}


		Point point(const Point& item) const
		{
			return item;
		}

		Point point(const Hash& item) const
		{
			return point(index(item));
		}

		Point point(const Indices& item) const
		{
			Point temp(m_space.corners[0]);
			temp[0] += item[0] * m_cellSize[0];
			temp[1] += item[1] * m_cellSize[1];
			temp[2] += item[2] * m_cellSize[2];
			return temp;
		}


		Point center(const Point& item) const
		{
			return point(item) + m_halfCellSize;
		}

		Point center(const Hash& item) const
		{
			return point(item) + m_halfCellSize;
		}

		Point center(const Indices& item) const
		{
			return point(item) + m_halfCellSize;
		}


		Hash hash(const Point& item) const
		{
			return hash(index(item));
		}

		Hash hash(const Hash& item) const
		{
			// TODO : no checks provided, maybe should?
			return item;
		}

		Hash hash(const Indices& item) const
		{
			return item.x * m_biasYZ + item.y * m_biasZ + item.z;
		}


		bool inBounds(const Point& item) const
		{
			return primitive::pointInAABB(item, m_space);
		}

		bool inBounds(const Hash& item) const
		{
			return inBounds(index(item));
		}

		bool inBounds(const Indices& item) const
		{
			return 0 <= item[0] && item[0] <= m_split[0] - 1
				&& 0 <= item[1] && item[1] <= m_split[1] - 1
				&& 0 <= item[2] && item[2] <= m_split[2] - 1
		}

		bool boundary(const Point& item) const
		{
			auto indices = index(item);

			return indices[0] == 0
				|| indices[1] == 0 
				|| indices[2] == 0
				|| indices[0] == m_split[0] - 1 
				|| indices[1] == m_split[1] - 1
				|| indices[2] == m_split[2] - 1;
		}

		bool boundary(const Hash& item) const
		{
			auto indices = index(item);

			return indices[0] == 0
				|| indices[1] == 0 
				|| indices[2] == 0
				|| indices[0] == m_split[0] - 1 
				|| indices[1] == m_split[1] - 1
				|| indices[2] == m_split[2] - 1;
		}

		bool boundary(const Indices& item) const
		{
			auto indices = index(item);

			return indices[0] == 0
				|| indices[1] == 0 
				|| indices[2] == 0
				|| indices[0] == m_split[0] - 1 
				|| indices[1] == m_split[1] - 1
				|| indices[2] == m_split[2] - 1;
		}



		Indices clampToBoundaries(const Indices& item) const
		{
			Indices inBoundaries = item;
			inBoundaries.x = std::max(std::min(inBoundaries.x, m_split[0] - 1), 0);
			inBoundaries.y = std::max(std::min(inBoundaries.y, m_split[1] - 1), 0);
			inBoundaries.z = std::max(std::min(inBoundaries.z, m_split[2] - 1), 0);
			return inBoundaries;
		}


	public:
		const AABB& getSpace() const
		{
			return m_space;
		}

		const Vec3& getCellSize() const
		{
			return m_cellSize;
		}

		const Vec3& getSizeRatios() const
		{
			return m_sizeRatios;
		}

		const Vec3i32& getSplit() const
		{
			return m_split;
		}


	public:
		bool has(const Point& item) const
		{
			return m_status[hash(item)];
		}

		bool has(const Hash& item) const
		{
			return m_status[hash(item)];
		}

		bool has(const Indices& item) const
		{
			return m_status[hash(item)];
		}


		Value& operator [] (const Point& item)
		{
			auto h = hash(item);
			m_status[h] = true;
			return m_grid[h];
		}

		Value& operator [] (const Hash& item)
		{
			auto h = hash(item);
			m_status[h] = true;
			return m_grid[h];
		}

		Value& operator [] (const Indices& item)
		{
			auto h = hash(item);
			m_status[h] = true;
			return m_grid[h];
		}


		void add(const Point& item, const Value& value = Value())
		{
			auto h = hash(item);
			m_status[h] = true;
			m_grid[h] = value;
		}

		void add(const Hash& item, const Value& value = Value())
		{
			auto h = hash(item);
			m_status[h] = true;
			m_grid[h] = value;
		}

		void add(const Indices& item, const Value& value = Value())
		{
			auto h = hash(item);
			m_status[h] = true;
			m_grid[h] = value;
		}


		Value& get(const Point& item)
		{
			return m_grid[hash(item)];
		}

		Value& get(const Hash& item)
		{
			return m_grid[hash(item)];
		}

		Value& get(const Indices& item)
		{
			return m_grid[hash(item)];
		}


		const Value& get(const Point& item) const
		{
			return m_grid[hash(item)];
		}

		const Value& get(const Hash& item) const
		{
			return m_grid[hash(item)];
		}

		const Value& get(const Indices& item) const
		{
			return m_grid[hash(item)];
		}


		void remove(const Point& item)
		{
			m_status[hash(item)] = false;
		}

		void remove(const Hash& item)
		{
			auto h = hash(item);
			m_status[h] = false;
			m_grid[h]   = Value();
		}

		void remove(const Indices& indices)
		{
			m_status[hash(item)] = false;
		}


	public:
		auto begin()
		{
			return Iterator(this, 0);
		}

		auto end()
		{
			return Iterator(this, size());
		}


		auto begin() const
		{
			return ConstIterator(this, 0);
		}

		auto end() const
		{
			return ConstIterator(this, size());
		}


		auto size() const
		{
			return m_grid.size();
		}


		void clear()
		{
			for (Int32 i = 0; i < m_status.size(); i++)
			{
				m_status[i] = false;
			}
			for (Int32 i = 0; i < m_grid.size(); i++)
			{
				m_grid[i] = Value();
			}
		}


	private:
		Grid m_grid{};
		Status m_status{};

		AABB m_space{};

		Vec3 m_cellSize{};
		Vec3 m_halfCellSize{};
		Vec3 m_sizeRatios{};

		Vec3i32 m_split{};

		Int32 m_biasYZ{};
		Int32 m_biasZ{};
	};


	/*
	//// simple sparse octree imitation
	//template<class Value>
	//class SparseVoxelOctree
	//{
	//public:
	//	using Int  = Int32;
	//	using Hash = Int32;
	//	using Map  = std::map<Hash, Value>;
	//	using Point   = Vec3;
	//	using Indices = Vec3i;


	//public:
	//	SparseVoxelOctree(const AABB& space, Int split)
	//		: m_map()
	//		, m_space(space)
	//		, m_spaceDelta()
	//	{
	//		m_split  = split;
	//		m_extent = npo2(split);
	//		m_depth  = blog(m_extent);//std::ceil(std::log2(m_extent));
	//		m_depth2 = m_depth << 1;

	//		m_ratio = static_cast<Float>(m_extent) / m_split;

	//		auto& [fc, sc] = m_space.corners;
	//		auto delta = sc - fc;
	//		m_spaceDelta = delta / static_cast<Float>(split);
	//		sc = fc + delta * m_ratio;
	//	}

	//	SparseVoxelOctree(const SparseVoxelOctree&) = default;
	//	SparseVoxelOctree(SparseVoxelOctree&&)      = default;

	//	~SparseVoxelOctree() = default;

	//	SparseVoxelOctree& operator = (const SparseVoxelOctree&) = default;
	//	SparseVoxelOctree& operator = (SparseVoxelOctree&&)      = default;


	//public:		
	//	Indices index(const Point& item) const
	//	{
	//		Point temp = item - m_space.corners[0];

	//		Indices indices{};

	//		indices[0] = Hash(temp[0] / m_spaceDelta[0]);
	//		indices[1] = Hash(temp[1] / m_spaceDelta[1]);
	//		indices[2] = Hash(temp[2] / m_spaceDelta[2]);

	//		return indices;
	//	}

	//	Indices index(const Hash& item) const
	//	{
	//		Hash hash = item;

	//		Hash mask = (1 << m_depth) - 1;

	//		Hash x = hash & mask;

	//		hash >>= m_depth;
	//		Hash y = (hash & mask);

	//		hash >>= m_depth;
	//		Hash z = (hash & mask);

	//		return Indices{x, y, z};
	//	}

	//	Indices index(const Indices& item) const
	//	{
	//		return item;
	//	}

	//	
	//	Point coord(const Point& point) const
	//	{
	//		return coord(index(point));
	//	}

	//	Point coord(const Hash& hash) const
	//	{
	//		return coord(index(hash));
	//	}

	//	Point coord(const Indices& indices) const
	//	{
	//		Point temp{};

	//		temp[0] = indices[0] * m_spaceDelta[0];
	//		temp[1] = indices[1] * m_spaceDelta[1];
	//		temp[2] = indices[2] * m_spaceDelta[2];

	//		temp += m_space.corners[0];

	//		return temp;
	//	}


	//	Point voxel(const Point& point) const
	//	{
	//		return coord(point) + Point(m_spaceDelta / 2.0_FL);
	//	}

	//	Point voxel(const Hash& hash) const
	//	{
	//		return coord(hash) + Point(m_spaceDelta / 2.0_FL);
	//	}

	//	Point voxel(const Indices& indices) const
	//	{
	//		return coord(indices) + Point(m_spaceDelta / 2.0_FL);
	//	}


	//	Hash hash(const Point& item) const
	//	{
	//		return hash(index(item));
	//	}

	//	Hash hash(const Hash& hash) const
	//	{
	//		return hash;
	//	}

	//	Hash hash(const Indices& indices) const
	//	{
	//		Hash xc = indices.x;
	//		Hash yc = indices.y;
	//		Hash zc = indices.z;

	//		return (zc) | (yc << m_depth) | (xc << m_depth2);
	//	}


	//	bool has(const Point& item) const
	//	{
	//		return m_map.find(hash(item)) != m_map.end();
	//	}

	//	bool has(const Hash& item) const
	//	{
	//		return m_map.find(hash(item)) != m_map.end();
	//	}

	//	bool has(const Indices& item) const
	//	{
	//		return m_map.find(hash(item)) != m_map.end();
	//	}


	//	Value& operator [] (const Point& item)
	//	{
	//		return m_map[hash(item)];
	//	}

	//	Value& operator [] (const Hash& item)
	//	{
	//		return m_map[hash(item)];
	//	}

	//	Value& operator [] (const Indices& item)
	//	{
	//		return m_map[hash(item)];
	//	}


	//	

	//	bool boundaryVoxel(const Point& item) const
	//	{
	//		auto indices = index(item);

	//		auto min = 0;
	//		auto max = m_split - 1;

	//		return indices.x == min || indices.y == min || indices.z == min
	//			|| indices.x == max || indices.y == max || indices.z == max;
	//	}

	//	bool boundaryVoxel(const Hash& item) const
	//	{
	//		auto indices = index(item);

	//		auto min = 0;
	//		auto max = m_split - 1;

	//		return indices.x == min || indices.y == min || indices.z == min
	//			|| indices.x == max || indices.y == max || indices.z == max;
	//	}

	//	bool boundaryVoxel(const Indices& item) const
	//	{
	//		auto indices = index(item);

	//		auto min = 0;
	//		auto max = m_split - 1;

	//		return indices.x == min || indices.y == min || indices.z == min
	//			|| indices.x == max || indices.y == max || indices.z == max;
	//	}

	//	Indices clampToBoundaries(const Indices& item) const
	//	{
	//		Indices inBoundaries = item;

	//		inBoundaries.x = std::max(std::min(inBoundaries.x, m_split - 1), 0);
	//		inBoundaries.y = std::max(std::min(inBoundaries.y, m_split - 1), 0);
	//		inBoundaries.z = std::max(std::min(inBoundaries.z, m_split - 1), 0);

	//		return inBoundaries;
	//	}


	//public:
	//	const Point& spaceDelta() const
	//	{
	//		return m_spaceDelta;
	//	}

	//	Int split() const
	//	{
	//		return m_split;
	//	}


	//public:
	//	void add(const Point& item, const Value& value)
	//	{
	//		m_map[hash(item)] = value;
	//	}

	//	void add(const Hash& item, const Value& value)
	//	{
	//		m_map[hash(item)] = value;
	//	}

	//	void add(const Indices& item, const Value& value)
	//	{
	//		m_map[hash(item)] = value;
	//	}


	//	Value& get(const Point& item)
	//	{
	//		auto i = m_map.find(hash(item));

	//		return *i;
	//	}

	//	Value& get(const Hash& item)
	//	{
	//		auto i = m_map.find(hash(item));

	//		return *i;
	//	}

	//	Value& get(const Indices& item)
	//	{
	//		auto i = m_map.find(hash(item));

	//		return *i;
	//	}


	//	const Value& get(const Point& item) const
	//	{
	//		auto i = m_map.find(hash(item));

	//		return *i;
	//	}

	//	const Value& get(const Hash& item) const
	//	{
	//		auto i = m_map.find(hash(item));

	//		return *i;
	//	}

	//	const Value& get(const Indices& item) const
	//	{
	//		auto i = m_map.find(hash(item));

	//		return *i;
	//	}


	//	void remove(const Point& item)
	//	{
	//		m_map.erase(hash(item));
	//	}

	//	void remove(const Hash& item)
	//	{
	//		m_map.erase(hash(item));
	//	}

	//	void remove(const Indices& indices)
	//	{
	//		m_map.erase(hash(indices));
	//	}


	//public:
	//	auto begin()
	//	{
	//		return m_map.begin();
	//	}

	//	auto end()
	//	{
	//		return m_map.end();
	//	}


	//	auto begin() const
	//	{
	//		return m_map.begin();
	//	}

	//	auto end() const
	//	{
	//		return m_map.end();
	//	}


	//	auto size() const
	//	{
	//		return m_map.size();
	//	}


	//	void clear()
	//	{
	//		m_map.clear();
	//	}


	//private:
	//	Map m_map; // stores hashes of 

	//	AABB m_space;       // space[a,b]x[c,d] that will be split
	//	Point m_spaceDelta; // extents of a single piece to that space[a,b]x[c,d] was split

	//	Float m_ratio; //ratio of m_extent to m_split

	//	Int m_split;  // count of pieces that space was split to
	//	Int m_extent; // next power of 2 of m_split
	//	Int m_depth;  // depth of an octree
	//	Int m_depth2; // 2 * depth
	//};

	////simple sparse octree imitation
	//template<class Value>
	//class FullVoxelOctree
	//{
	//public:
	//	using Int  = Int32;
	//	using Hash = Int32;
	//	using Map  = std::vector<Value>;
	//	using Point   = Vec3;
	//	using Indices = Vec3i;


	//public:
	//	FullVoxelOctree(const AABB& space, Int split, const Value& empty)
	//		: m_map()
	//		, m_empty(empty)
	//		, m_space(space)
	//		, m_spaceDelta()
	//	{
	//		m_split = split;
	//		m_biasX = split * split;
	//		m_biasY = split;

	//		m_map.resize(split * split * split);
	//		for (auto& val : m_map)
	//		{
	//			val = empty;
	//		}

	//		auto& [fc, sc] = m_space.corners;
	//		m_spaceDelta = (sc - fc) / static_cast<Float>(m_split);
	//	}

	//	FullVoxelOctree(const FullVoxelOctree&) = default;
	//	FullVoxelOctree(FullVoxelOctree&&)      = default;

	//	FullVoxelOctree& operator = (const FullVoxelOctree&) = default;
	//	FullVoxelOctree& operator = (FullVoxelOctree&&)      = default;

	//	~FullVoxelOctree() = default;


	//public:
	//	Indices index(const Point& item) const
	//	{
	//		Point temp = item - m_space.corners[0];

	//		Indices indices{};

	//		indices[0] = Hash(temp[0] / m_spaceDelta[0]);
	//		indices[1] = Hash(temp[1] / m_spaceDelta[1]);
	//		indices[2] = Hash(temp[2] / m_spaceDelta[2]);

	//		return indices;
	//	}

	//	Indices index(const Hash& item) const
	//	{
	//		Hash hash = item;

	//		Hash x = hash / m_biasX;
	//		hash %= m_biasX;

	//		Hash y = hash / m_biasY;
	//		hash %= m_biasY;

	//		Hash z = hash;

	//		return Indices{x, y, z};
	//	}

	//	Indices index(const Indices& item) const
	//	{
	//		return item;
	//	}


	//	Point coord(const Point& point) const
	//	{
	//		return coord(index(point));
	//	}

	//	Point coord(const Hash& hash) const
	//	{
	//		return coord(index(hash));
	//	}

	//	Point coord(const Indices& indices) const
	//	{
	//		Point temp{};

	//		temp[0] = indices[0] * m_spaceDelta[0];
	//		temp[1] = indices[1] * m_spaceDelta[1];
	//		temp[2] = indices[2] * m_spaceDelta[2];

	//		temp += m_space.corners[0];

	//		return temp;
	//	}


	//	Point voxel(const Point& point) const
	//	{
	//		return coord(point) + Point(m_spaceDelta / 2.0f);
	//	}

	//	Point voxel(const Hash& hash) const
	//	{
	//		return coord(hash) + Point(m_spaceDelta / 2.0f);
	//	}

	//	Point voxel(const Indices& indices) const
	//	{
	//		return coord(indices) + Point(m_spaceDelta / 2.0f);
	//	}


	//	Hash hash(const Point& item) const
	//	{
	//		return hash(index(item));
	//	}

	//	Hash hash(const Hash& hash) const
	//	{
	//		return hash;
	//	}

	//	Hash hash(const Indices& indices) const
	//	{
	//		Hash xc = indices.x;
	//		Hash yc = indices.y;
	//		Hash zc = indices.z;

	//		return (zc) + (yc * m_biasY) + (xc * m_biasX);
	//	}


	//	bool has(const Point& item) const
	//	{
	//		return m_map[hash(item)] != m_empty;
	//	}

	//	bool has(const Hash& item) const
	//	{
	//		return m_map[hash(item)] != m_empty;
	//	}

	//	bool has(const Indices& item) const
	//	{
	//		return m_map[hash(item)] != m_empty;
	//	}


	//	Value& operator [] (const Point& item)
	//	{
	//		return m_map[hash(item)];
	//	}

	//	Value& operator [] (const Hash& item)
	//	{
	//		return m_map[hash(item)];
	//	}

	//	Value& operator [] (const Indices& item)
	//	{
	//		return m_map[hash(item)];
	//	}


	//	bool boundaryVoxel(const Point& item) const
	//	{
	//		auto indices = index(item);

	//		auto min = 0;
	//		auto max = m_split - 1;

	//		return indices.x == min || indices.y == min || indices.z == min
	//			|| indices.x == max || indices.y == max || indices.z == max;
	//	}

	//	bool boundaryVoxel(const Hash& item) const
	//	{
	//		auto indices = index(item);

	//		auto min = 0;
	//		auto max = m_split - 1;

	//		return indices.x == min || indices.y == min || indices.z == min
	//			|| indices.x == max || indices.y == max || indices.z == max;
	//	}

	//	bool boundaryVoxel(const Indices& item) const
	//	{
	//		auto indices = index(item);

	//		auto min = 0;
	//		auto max = m_split - 1;

	//		return indices.x == min || indices.y == min || indices.z == min
	//			|| indices.x == max || indices.y == max || indices.z == max;
	//	}


	//	Indices clampToBoundaries(const Indices& item) const
	//	{
	//		Indices inBoundaries = item;

	//		inBoundaries.x = std::max(std::min(inBoundaries.x, m_split - 1), 0);
	//		inBoundaries.y = std::max(std::min(inBoundaries.y, m_split - 1), 0);
	//		inBoundaries.z = std::max(std::min(inBoundaries.z, m_split - 1), 0);

	//		return inBoundaries;
	//	}


	//public:
	//	const Point& spaceDelta() const
	//	{
	//		return m_spaceDelta;
	//	}

	//	Int split() const
	//	{
	//		return m_split;
	//	}

	//	Value empty() const
	//	{
	//		return m_empty;
	//	}


	//public:
	//	const Value& operator [] (const Point& item) const
	//	{
	//		return m_map[hash(item)];
	//	}

	//	const Value& operator [] (const Hash& item) const
	//	{
	//		return m_map[hash(item)];
	//	}

	//	const Value& operator [] (const Indices& item) const
	//	{
	//		return m_map[hash(item)];
	//	}


	//public:
	//	auto begin()
	//	{
	//		return m_map.begin();
	//	}

	//	auto end()
	//	{
	//		return m_map.end();
	//	}


	//	auto begin() const
	//	{
	//		return m_map.begin();
	//	}

	//	auto end() const
	//	{
	//		return m_map.end();
	//	}


	//	auto size() const
	//	{
	//		return m_map.size();
	//	}


	//	void clear()
	//	{
	//		for (auto& elem : m_map)
	//		{
	//			elem = Value();
	//		}
	//	}


	//private:
	//	Map m_map; // stores hashes of 

	//	AABB m_space;       // space[a,b]x[c,d] that will be split
	//	Point m_spaceDelta; // extents of a single piece to that space[a,b]x[c,d] was split

	//	Float m_ratio; //ratio of m_extent to m_split

	//	Value m_empty; // empty value

	//	Int m_split; // count of pieces that space was split to
	//	Int m_biasX; // index = m_biasX * x + m_bias * y + z
	//	Int m_biasY; // index = m_biasX * x + m_bias * y + z
	//};
	*/
}