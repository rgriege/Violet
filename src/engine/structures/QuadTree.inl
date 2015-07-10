// ============================================================================

template <typename T>
Violet::QuadTree<T>::QuadTree(const AABB & boundary, const uint32 capacity) :
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
bool Violet::QuadTree<T>::insert(const T & elem, const AABB & box)
{
	const bool success = m_boundary.contains(box);
	if (success)
	{
		if (isLeaf())
		{
			if (m_elements.size() == m_capacity)
				subdivide();
			else
				m_elements.emplace_back(std::make_pair(elem, box));
		}
		else if (!insertIntoSubTree(elem, box))
			m_elements.emplace_back(std::make_pair(elem, box));
	}
	return success;
}

// ----------------------------------------------------------------------------

template <typename T>
uint32 Violet::QuadTree<T>::size() const
{
	uint32 result = m_elements.size();
	if (!isLeaf())
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
bool Violet::QuadTree<T>::isLeaf() const
{
	return m_topLeft == nullptr;
}

// ----------------------------------------------------------------------------

template <typename T>
bool Violet::QuadTree<T>::empty() const
{
	return size() == 0;
}

// ----------------------------------------------------------------------------

template <typename T>
void Violet::QuadTree<T>::retrieve(std::vector<T> & elements, const AABB & box) const
{
	if (m_boundary.contains(box))
	{
		for (const auto & element : m_elements)
			elements.emplace_back(element.first);

		if (!isLeaf())
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
void Violet::QuadTree<T>::subdivide()
{
	const Vec2f center = m_boundary.getCenter();
	const Vec2f quarterDim = m_boundary.getHalfDim() / 2.f;
	m_topLeft = std::make_unique<QuadTree>(AABB(center - quarterDim, quarterDim), m_capacity);
	m_topRight = std::make_unique<QuadTree>(AABB(Vec2f(center.x + quarterDim.x, center.y - quarterDim.y), quarterDim), m_capacity);
	m_bottomRight = std::make_unique<QuadTree>(AABB(center + quarterDim, quarterDim), m_capacity);
	m_bottomLeft = std::make_unique<QuadTree>(AABB(Vec2f(center.x - quarterDim.x, center.y + quarterDim.y), quarterDim), m_capacity);

	auto it = m_elements.begin(), end = m_elements.end();
	for (it; it != end;)
	{
		if (insertIntoSubTree(it->first, it->second))
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
bool Violet::QuadTree<T>::insertIntoSubTree(const T & elem, const AABB & box)
{
	return m_topLeft->insert(elem, box)
		|| m_topRight->insert(elem, box)
		|| m_bottomLeft->insert(elem, box)
		|| m_bottomRight->insert(elem, box);
}

// ============================================================================
