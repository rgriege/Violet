// ============================================================================

template <typename T, typename Allocator>
template <bool IsConst>
vlt::vector<T, Allocator>::Iterator<IsConst>::Iterator(pointer ptr) :
	m_ptr(ptr)
{
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
template <bool IsConst>
typename vlt::vector<T, Allocator>::TEMPLATE_HINT Iterator<IsConst> & vlt::vector<T, Allocator>::Iterator<IsConst>::operator++()
{
	++m_ptr;
	return *this;
}

// ----------------------------------------------------------------------------


template <typename T, typename Allocator>
template <bool IsConst>
typename vlt::vector<T, Allocator>::TEMPLATE_HINT Iterator<IsConst>::reference vlt::vector<T, Allocator>::Iterator<IsConst>::operator*()
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
template <bool IsConst>
typename vlt::vector<T, Allocator>::TEMPLATE_HINT Iterator<IsConst>::pointer vlt::vector<T, Allocator>::Iterator<IsConst>::operator->()
{
	return m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
template <bool IsConst>
bool vlt::vector<T, Allocator>::Iterator<IsConst>::operator==(const Iterator<IsConst> & other) const
{
	return m_ptr == other.m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
template <bool IsConst>
bool vlt::vector<T, Allocator>::Iterator<IsConst>::operator!=(const Iterator<IsConst> & other) const
{
	return !(m_ptr == other.m_ptr);
}

// ============================================================================

template <typename T, typename Allocator>
vlt::vector<T, Allocator>::vector() :
	m_data(nullptr),
	m_allocator(),
	m_size(0),
	m_capacity(0)
{
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
vlt::vector<T, Allocator>::vector(std::initializer_list<T> list) :
	vector()
{
	reserve(list.size());
	for (const value_type & element : list)
		emplace_back(element);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
vlt::vector<T, Allocator>::vector(const vector & rhs) :
	vector()
{
	reserve(rhs.size());
	for (const value_type & element : rhs)
		emplace_back(element);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
vlt::vector<T, Allocator>::vector(vector && rhs) :
	vector()
{
	swap(rhs);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
vlt::vector<T, Allocator>::~vector()
{
	clear();
	if (m_capacity > 0)
		m_allocator.deallocate(m_data, m_capacity);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
vlt::vector<T, Allocator> & vlt::vector<T, Allocator>::operator=(const vector & rhs)
{
	clear();
	reserve(rhs.size());
	for (const value_type & element : rhs)
		emplace_back(element);
	return *this;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
vlt::vector<T, Allocator> & vlt::vector<T, Allocator>::operator=(vector && rhs)
{
	swap(rhs);
	return *this;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::reference vlt::vector<T, Allocator>::operator[](const size_type index)
{
	assert(index < m_size);
	return *(m_data + index);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::const_reference vlt::vector<T, Allocator>::operator[](const size_type index) const
{
	assert(index < m_size);
	return *(m_data + index);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::reference vlt::vector<T, Allocator>::front()
{
	return *m_data;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::const_reference vlt::vector<T, Allocator>::front() const
{
	return *m_data;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::reference vlt::vector<T, Allocator>::back()
{
	assert(!empty());
	return *(m_data + m_size - 1);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::const_reference vlt::vector<T, Allocator>::back() const
{
	assert(!empty());
	return *(m_data + m_size - 1);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::pointer vlt::vector<T, Allocator>::data()
{
	return m_data;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::const_pointer vlt::vector<T, Allocator>::data() const
{
	return m_data;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::iterator vlt::vector<T, Allocator>::begin()
{
	return iterator(m_data);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::const_iterator vlt::vector<T, Allocator>::begin() const
{
	return const_iterator(m_data);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::iterator vlt::vector<T, Allocator>::end()
{
	return iterator(m_data + m_size);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::const_iterator vlt::vector<T, Allocator>::end() const
{
	return const_iterator(m_data + m_size);
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
bool vlt::vector<T, Allocator>::empty() const
{
	return m_size == 0;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::size_type vlt::vector<T, Allocator>::size() const
{
	return m_size;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
void vlt::vector<T, Allocator>::reserve(const size_type capacity)
{
	if (capacity > m_capacity)
	{
		pointer newData = m_allocator.allocate(capacity);

		for (size_type i = 0, end = m_size; i != end; ++i)
		{
			pointer p = m_data + i;
			m_allocator.construct(newData + i, std::move(*p));
			m_allocator.destroy(p);
		}

		if (m_capacity > 0)
			m_allocator.deallocate(m_data, m_capacity);

		m_data = newData;
		m_capacity = capacity;
	}
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
typename vlt::vector<T, Allocator>::size_type vlt::vector<T, Allocator>::capacity() const
{
	return m_capacity;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
void vlt::vector<T, Allocator>::clear()
{
	for (pointer p = m_data, end = m_data + m_size; p != end; ++p)
		m_allocator.destroy(p);
	m_size = 0;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
template <typename ... Args>
void vlt::vector<T, Allocator>::emplace_back(Args && ... args)
{
	if (m_size == m_capacity)
		grow();
	m_allocator.construct(m_data + m_size, std::forward<Args>(args)...);
	++m_size;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
void vlt::vector<T, Allocator>::resize(const size_type size)
{
	if (size < m_size)
	{
		for (pointer p = m_data + size, end = m_data + m_size; p != end; ++p)
			m_allocator.destroy(p);
	}
	else if (size > m_size)
	{
		reserve(size);
		for (pointer p = m_data + m_size, end = m_data + size; p != end; ++p)
			m_allocator.construct(p);
	}
	m_size = size;
}

// ----------------------------------------------------------------------------

template <typename T, typename Allocator>
void vlt::vector<T, Allocator>::swap(vector & rhs)
{
	const pointer data = m_data;
	allocator_type allocator = std::move(m_allocator);
	const size_type size = m_size;
	const size_type capacity = m_capacity;

	m_data = rhs.m_data;
	m_allocator = std::move(rhs.m_allocator);
	m_size = rhs.m_size;
	m_capacity = rhs.m_capacity;

	rhs.m_data = data;
	rhs.m_allocator = std::move(allocator);
	rhs.m_size = size;
	rhs.m_capacity = capacity;
}

// ============================================================================

template <typename T, typename Allocator>
void vlt::vector<T, Allocator>::grow()
{
	const size_type newCapacity = m_capacity * 3 / 2;
	reserve(newCapacity > m_capacity ? newCapacity : m_capacity + 1);
}

// ============================================================================
