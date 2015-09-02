#ifndef VIOLET_QuadTree_H
#define VIOLET_QuadTree_H

#include "violet/math/AABB.h"
#include "violet/math/Vec2.h"

#include <array>

namespace Violet
{
	template <typename T>
	class QuadTree
	{
	public:

		class Iterator
		{
		public:


		};

	public:

		QuadTree(const AABB & boundary, uint32 capacity);

		bool insert(const T & elem, const AABB & box);

		uint32 size() const;
		bool isLeaf() const;
		bool empty() const;

		void retrieve(std::vector<T> & elements, const AABB & box) const;

	private:

		void subdivide();
		bool insertIntoSubTree(const T & elem, const AABB & box);

	private:

		const AABB m_boundary;
		const uint32 m_capacity;
		std::unique_ptr<QuadTree> m_topLeft;
		std::unique_ptr<QuadTree> m_topRight;
		std::unique_ptr<QuadTree> m_bottomLeft;
		std::unique_ptr<QuadTree> m_bottomRight;
		std::vector<std::pair<T, AABB>> m_elements;
	};
}

#include "violet/structures/QuadTree.inl"

#endif
