// ============================================================================

template <typename T, typename ... Args>
T * Violet::CppScriptComponent::Allocator::allocate(Args && ... args)
{
	m_memory = new T(std::forward<Args>(args)...);
	return (T *) m_memory;
}

// ----------------------------------------------------------------------------

template <typename T>
void Violet::CppScriptComponent::Allocator::deallocate()
{
	static_cast<T *>(m_memory)->~T();
}

// ============================================================================

template <typename ResultType, typename... Args>
ResultType Violet::CppScriptComponent::run(const char * method, Args&&... args) const
{
	void * methodPtr = getMethodPtr(method);
	if (methodPtr != nullptr)
	{
		if (getMemoryPtr() == nullptr)
		{
			auto m = (ResultType(*)(Args...)) methodPtr;
			return m(std::forward<Args>(args)...);
		}
		else
		{
			void * memory = getMemoryPtr();
			auto m = (ResultType(*)(Args..., int &)) methodPtr;
			return m(std::forward<Args>(args)..., *(int *)memory);
		}
	}

	return ResultType();
}

// ============================================================================
