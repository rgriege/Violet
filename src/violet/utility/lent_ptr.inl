// ============================================================================

template <typename T>
vlt::Lent_Ptr<T>::Lent_Ptr(T * const ptr) :
	m_ptr(ptr)
{
}

// ----------------------------------------------------------------------------

template <typename T>
vlt::Lent_Ptr<T>::Lent_Ptr(const std::unique_ptr<T> & ptr) :
	m_ptr(ptr.get())
{
}

// ----------------------------------------------------------------------------

template <typename T>
T & vlt::Lent_Ptr<T>::operator*() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
T * vlt::Lent_Ptr<T>::operator->() const
{
	return m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::Lent_Ptr<T>::operator==(const T * const rhs) const
{
	return m_ptr == rhs;
}

// ----------------------------------------------------------------------------

template <typename T>
bool vlt::Lent_Ptr<T>::operator!=(const T * const rhs) const
{
	return m_ptr != rhs;
}

// ============================================================================
