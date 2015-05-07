// ============================================================================

template <typename T>
T * Violet::CppScriptComponent::Allocator::allocate()
{
	return static_cast<T*>(allocate(sizeof(T)));
}

// ============================================================================

template <typename ResultType, typename... Args>
ResultType Violet::CppScriptComponent::run(const char * method, Args&&... args)
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
			auto m = (ResultType(*)(Args..., void *)) methodPtr;
			return m(std::forward<Args>(args)..., getMemoryPtr());
		}
	}

	return ResultType();
}

// ============================================================================
