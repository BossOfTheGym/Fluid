#ifndef OCTREE_H
#define OCTREE_H


#include <Common.h>

#include <limits>
#include <algorithm>


template<class Type, class NodeAllocator, class TypeAllocator>
class Octree
{
public:
	//TODO: move out of class
	static const size_t NODES = static_cast<size_t>(8);

	static const Vec3 CUBE_VERTICES[NODES];


	//possible checks for Type that should maintain multiply and addition operators

	using TNodeAllocator = std::remove_cv_t<std::remove_reference_t<NodeAllocator>>;
	using TTypeAllocator = std::remove_cv_t<std::remove_reference_t<TypeAllocator>>;

	using NodeToken = typename TNodeAllocator::Token;
	using TypeToken = typename TTypeAllocator::Token;

	static const NodeToken NODE_EMPTY;
	static const TypeToken TYPE_EMPTY;


	struct Node
	{
		TypeToken data   = TTypeAllocator::EMPTY_TOKEN; // key  to data

		NodeToken parent = TNodeAllocator::EMPTY_TOKEN; // key  to parent

		NodeToken children[NODES] = {                   // keys to children
			  TNodeAllocator::EMPTY_TOKEN
			, TNodeAllocator::EMPTY_TOKEN
			, TNodeAllocator::EMPTY_TOKEN
			, TNodeAllocator::EMPTY_TOKEN
			, TNodeAllocator::EMPTY_TOKEN
			, TNodeAllocator::EMPTY_TOKEN
			, TNodeAllocator::EMPTY_TOKEN
			, TNodeAllocator::EMPTY_TOKEN
		}; 
	};


private:
	static bool inBox(const Vec3& firstCorner, const Vec3& secondCorner, const Vec3& testPoint)
	{
		auto& [x0, y0, z0] = firstCorner;
		auto& [x1, y1, z1] = secondCorner;
		auto& [x , y , z ] = testPoint;

		//TODO: rework with eps comparison
		return 
			   (x0 <= x) && (x <= x1)
			&& (y0 <= y) && (y <= y1)
			&& (z0 <= z) && (z <= z1);
	}

	static bool nodeEmpty(const NodeToken& token)
	{
		return token == NODE_EMPTY;
	}

	static bool typeEmpty(const TypeToken& token)
	{
		return token == TYPE_EMPTY;
	}

	static bool isLeaf(const Node& node)
	{
		return std::all_of(
			node.children, node.children + Octree::NODES
			, [&] (const auto& childToken)
			{
				return Octree::nodeEmpty(childToken);
			}
		);
	}


public:
	Octree(
		  const Vec3& firstCorner
		, const Vec3& secondCorner

		, NodeAllocator&& nodesAllocator
		, TypeAllocator&& leavesAllocator
		, TypeAllocator&& nodeDataAllocator

		, Float eps = std::numeric_limits<Float>::epsilon()
	)
		: m_firstCorner (firstCorner)
		, m_delta       (secondCorner - firstCorner)
		, m_secondCorner(secondCorner)

		, m_nodesAllocator  (std::forward<NodeAllocator>(nodesAllocator))
		, m_leafAllocator   (std::forward<TypeAllocator>(leavesAllocator))
		, m_nonLeafAllocator(std::forward<TypeAllocator>(nodeDataAllocator))

		, m_eps(eps)
	{
		m_root = m_nodesAllocator.alloc();
	}

	~Octree()
	{
		clear();
	}


	Octree(const Octree&) = default;
	Octree(Octree&&)      = default;

	Octree& operator = (const Octree&) = default;
	Octree& operator = (Octree&&)      = default;


public:
	bool add(const Vec3& point, const Type& elem)
	{
		//TODO
		//TODO: add from root
		for (size_t i = 0; i < Octree::CUBE_VERTICES; i++)
		{

		}
	}

	bool has(const Vec3& point)
	{
		//TODO
	}

	bool remove(const Vec3& point)
	{
		//TODO
	}


private:
	void ascendingUpdate()
	{
		//TODO
	}


public:
	void clear()
	{
		//TODO: think about its necessity
		auto& root = m_nodesAllocator.get(m_root);

		for (auto& childToken : root.children)
		{
			if (!Octree::nodeEmpty(childToken))
			{
				clearNode(childToken);
				deleteNode(childToken);
			}

			childToken = Octree::NODE_EMPTY;
		}

		root.data = Octree::TYPE_EMPTY;
	}

	void resetBounds(const Vec3& firstCorner, const Vec3& secondCorner)
	{
		//TODO
	}

	bool inBounds(const Vec3& point) const
	{
		//TODO: move to AABB class

		return Octree::inBox(m_firstCorner, m_secondCorner, point);
	}


public:
	const Vec3& firstCorner() const
	{
		return m_firstCorner;
	}

	const Vec3& secondCorner() const
	{
		return m_secondCorner;
	}

	const Vec3& delta() const
	{
		return m_delta;
	}


	Float eps() const
	{
		return m_eps;
	}


private:
	void buildTree()
	{
		//TODO
	}


	void deleteNode(const NodeToken& token)
	{
		//accepts non-empty token

		auto& node = m_nodesAllocator.get(token);

		for(auto& childToken : node.children)
		{
			if (!Octree::nodeEmpty(childToken))
			{
				clearNode(childToken);
				deleteNode(childToken);
			}
		}

		m_nodesAllocator.dealloc(token);
	}

	void clearNode(const NodeToken& token)
	{
		//accepts non-empty token

		auto& node = m_nodesAllocator.get(token);

		if (Octree::isLeaf(node))
		{
			m_leafAllocator.dealloc(token);
		}
		else
		{
			m_nonLeafAllocator.dealloc(token);
		}
	}

	void createNode(const NodeToken& token)
	{
		//TODO
	}
	
	void addNode(const NodeToken& token, const Vec3& point, const Type& elem)
	{
		//TODO

		Vec3 fcNext{}; Vec3 fcPrev = m_firstCorner;
		Vec3 scNext{}; Vec3 scPrev = m_secondCorner;
		Vec3 delta = m_delta;

		NodeToken currentToken = token;


		while (true)
		{
			delta /= 2;

			for (size_t i = 0; i < Octree::NODES; i++)
			{
				fcNext = fcPrev + delta * CUBE_VERTICES[i];
				scNext = fcNext + delta;

				if (Octree::inBox())
				{
					if (Octree::isLeaf())
					{
						addLeaf();
					}
					else
					{
						fcPrev = fcNext;
						scPrev = scNext;

						currentToken;
					}
				}
			}
		}
	}

	void addLeaf()
	{
		//TODO
	}


private:
	//TODO : create separete AABB class
	Vec3 m_firstCorner;  // the first  corner of bounding box
	Vec3 m_secondCorner; // the second corner of bounding box
	Vec3 m_delta;        // vector that firstCorner + delta = secondCorner

	Float m_eps;

	TNodeAllocator m_nodesAllocator;
	TTypeAllocator m_leafAllocator;
	TTypeAllocator m_nonLeafAllocator;

	NodeToken m_root;
};


//CUBE_VERTICES initialization
template<class Type, class NodeAllocator, class TypeAllocator>
const Vec3 Octree<Type, NodeAllocator, TypeAllocator>::CUBE_VERTICES[8] = 
{
	  Vec3{0.0f, 0.0f, 0.0f}
	, Vec3{0.0f, 0.0f, 1.0f}
	, Vec3{0.0f, 1.0f, 0.0f}
	, Vec3{0.0f, 1.0f, 1.0f}
	, Vec3{1.0f, 0.0f, 0.0f}
	, Vec3{1.0f, 0.0f, 1.0f}
	, Vec3{1.0f, 1.0f, 0.0f}
	, Vec3{1.0f, 1.0f, 1.0f}
};

//NODE_EMPTY initialization
template<class Type, class NodeAllocator, class TypeAllocator>
const Octree<Type, NodeAllocator, TypeAllocator>::NodeToken NODE_EMPTY = Octree<Type, NodeAllocator, TypeAllocator>::TNodeAllocator::EMPTY_TOKEN;

//TYPE_EMPTY initialization
template<class Type, class NodeAllocator, class TypeAllocator>
const Octree<Type, NodeAllocator, TypeAllocator>::TypeToken TYPE_EMPTY = Octree<Type, NodeAllocator, TypeAllocator>::TTypeAllocator::EMPTY_TOKEN;

#endif