// ============================================================================

template <typename T>
vlt::quad_tree<T>::quad_tree(const aabb & boundary, const u32 capacity) :
	m_boundary(boundary),
	m_capacity(capacity),
	m_topLeft(),
	m_topRight(),
	m_bottomLeft(),
	m_bottomRight(),
	m_elements()
{
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::quad_tree<T>::insert(const T & elem, const aabb & box)
{
	const bool success = m_boundary.contains(box);
	if (success)
	{
		if (is_leaf())
		{
			if (m_elements.size() == m_capacity)
				subdivide();
			else
				m_elements.emplace_back(std::make_pair(elem, box));
		}
		else if (!insert_into_sub_tree(elem, box))
			m_elements.emplace_back(std::make_pair(elem, box));
	}
	return success;
}

// ----------------------------------------------------------------------------

template <typename T>
u32 vlt::quad_tree<T>::size() const
{
	u32 result = m_elements.size();
	if (!is_leaf())
	{
		result += m_topLeft->size()
			+ m_topRight->size()
			+ m_bottomLeft->size()
			+ m_bottomRight->size();
	}
	return result;
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::quad_tree<T>::is_leaf() const
{
	return m_topLeft == nullptr;
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::quad_tree<T>::empty() const
{
	return size() == 0;
}

// ----------------------------------------------------------------------------

template <typename T>
void vlt::quad_tree<T>::retrieve(std::vector<T> & elements, const aabb & box) const
{
	if (m_boundary.contains(box))
	{
		for (const auto & element : m_elements)
			elements.emplace_back(element.first);

		if (!is_leaf())
		{
			m_topLeft->retrieve(elements, box);
			m_topRight->retrieve(elements, box);
			m_bottomRight->retrieve(elements, box);
			m_bottomLeft->retrieve(elements, box);
		}
	}
}

// ============================================================================

template <typename T>
void vlt::quad_tree<T>::subdivide()
{
	const v2 center = m_boundary.get_center();
	const v2 quarterDim = m_boundary.get_half_dim() / 2.f;
	m_topLeft = std::make_unique<quad_tree>(aabb(center - quarterDim, quarterDim), m_capacity);
	m_topRight = std::make_unique<quad_tree>(aabb(v2(center.x + quarterDim.x, center.y - quarterDim.y), quarterDim), m_capacity);
	m_bottomRight = std::make_unique<quad_tree>(aabb(center + quarterDim, quarterDim), m_capacity);
	m_bottomLeft = std::make_unique<quad_tree>(aabb(v2(center.x - quarterDim.x, center.y + quarterDim.y), quarterDim), m_capacity);

	auto it = m_elements.begin(), end = m_elements.end();
	for (it; it != end;)
	{
		if (insert_into_sub_tree(it->first, it->second))
		{
			it = m_elements.erase(it);
			end = m_elements.end();
		}
		else
			++it;
	}
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::quad_tree<T>::insert_into_sub_tree(const T & elem, const aabb & box)
{
	return m_topLeft->insert(elem, box)
		|| m_topRight->insert(elem, box)
		|| m_bottomLeft->insert(elem, box)
		|| m_bottomRight->insert(elem, box);
}

// ============================================================================
