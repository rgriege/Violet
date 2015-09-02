// ============================================================================

template <typename T>
Violet::unique_val<T>::unique_val(unique_val<T> && rhs) :
	m_ptr(std::move(rhs.m_ptr))
{
}

// ----------------------------------------------------------------------------

template <typename T>
template <typename U>
Violet::unique_val<T>::unique_val(unique_val<U> && rhs) :
	m_ptr(std::move(rhs.m_ptr))
{
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::unique_val<T> & Violet::unique_val<T>::operator=(unique_val<T> && rhs)
{
	m_ptr = std::move(rhs.m_ptr);
	return *this;
}

// ----------------------------------------------------------------------------

template <typename T>
T & Violet::unique_val<T>::get() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
T * Violet::unique_val<T>::ptr() const
{
	return m_ptr.get();
}

// ----------------------------------------------------------------------------

template <typename T>
T & Violet::unique_val<T>::operator*() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
T * Violet::unique_val<T>::operator->() const
{
	return m_ptr.operator->();
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::unique_val<T>::operator T &() const
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::unique_val<T>::unique_val(std::unique_ptr<T> && ptr) :
	m_ptr(std::move(ptr))
{
}

// ============================================================================

template <typename T, typename ... Args>
Violet::unique_val<T> Violet::make_unique_val(Args && ... args)
{
	return unique_val<T>(std::make_unique<T>(std::forward<Args>(args)...));
}

// ============================================================================
