using namespace Violet;

template <typename T>
template <typename U, typename... Args>
unique_val<T> unique_val<T>::create(Args&&... args)
{
	return unique_val<T>(std::unique_ptr<T>(new U(std::forward<Args>(args)...)));
}

template <typename T>
T & unique_val<T>::operator*() const
{
	return *m_ptr;
}

template <typename T>
T * unique_val<T>::operator->() const
{
	return m_ptr.operator->();
}


template <typename T>
unique_val<T>::unique_val(std::unique_ptr<T> && ptr) :
	m_ptr(std::move(ptr))
{
}