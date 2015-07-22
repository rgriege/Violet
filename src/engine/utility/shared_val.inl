// ============================================================================

template <typename T>
Violet::shared_val<T>::shared_val(const shared_val<T> & rhs) :
	m_ptr(rhs.m_ptr)
{
}

// ----------------------------------------------------------------------------

template <typename T>
template <typename U>
Violet::shared_val<T>::shared_val(const shared_val<U> & rhs) :
	m_ptr(rhs.m_ptr)
{
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::shared_val<T> & Violet::shared_val<T>::operator=(const shared_val<T> & rhs)
{
	m_ptr = rhs.m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
T & Violet::shared_val<T>::get() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
T * Violet::shared_val<T>::ptr() const
{
	return m_ptr.get();
}

// ----------------------------------------------------------------------------

template <typename T>
T & Violet::shared_val<T>::operator*() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
T * Violet::shared_val<T>::operator->() const
{
	return m_ptr.operator->();
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::shared_val<T>::operator T &() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::shared_val<T>::shared_val(std::shared_ptr<T> ptr) :
	m_ptr(std::move(ptr))
{
}

// ============================================================================

template <typename T, typename ... Args>
Violet::shared_val<T> Violet::make_shared_val(Args && ... args)
{
	return shared_val<T>(std::make_shared<T>(std::forward<Args>(args)...));
}

// ============================================================================
