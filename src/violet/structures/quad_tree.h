#ifndef VIOLET_QUAD_TREE_H
#define VIOLET_QUAD_TREE_H

#include "violet/math/aabb.h"
#include "violet/math/v2.h"

#include <array>

namespace vlt
{
	template <typename T>
	struct quad_tree
	{
		quad_tree(const aabb & boundary, u32 capacity);

		bool insert(const T & elem, const aabb & box);

		u32 size() const;
		bool is_leaf() const;
		bool empty() const;

		void retrieve(std::vector<T> & elements, const aabb & box) const;

	private:

		void subdivide();
		bool insert_into_sub_tree(const T & elem, const aabb & box);

	private:

		const aabb m_boundary;
		const u32 m_capacity;
		std::unique_ptr<quad_tree> m_topLeft;
		std::unique_ptr<quad_tree> m_topRight;
		std::unique_ptr<quad_tree> m_bottomLeft;
		std::unique_ptr<quad_tree> m_bottomRight;
		std::vector<std::pair<T, aabb>> m_elements;
	};
}

#include "violet/structures/quad_tree.inl"

#endif
