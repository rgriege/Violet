// ============================================================================

template <typename T>
vlt::lent_ptr<T>::lent_ptr(T * const ptr) :
	m_ptr(ptr)
{
}

// ----------------------------------------------------------------------------

template <typename T>
vlt::lent_ptr<T>::lent_ptr(const std::unique_ptr<T> & ptr) :
	m_ptr(ptr.get())
{
}

// ----------------------------------------------------------------------------

template <typename T>
T & vlt::lent_ptr<T>::operator*() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
T * vlt::lent_ptr<T>::operator->() const
{
	return m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::lent_ptr<T>::operator==(const T * const rhs) const
{
	return m_ptr == rhs;
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::lent_ptr<T>::operator!=(const T * const rhs) const
{
	return m_ptr != rhs;
}

// ============================================================================
