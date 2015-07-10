// ============================================================================

template <typename T>
T * Violet::CppScriptComponent::Allocator::allocate()
{
	void * buffer = allocate(sizeof(T));
	T * data = new (buffer) T;
	return data;
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
			auto m = (ResultType(*)(Args..., void *)) methodPtr;
			return m(std::forward<Args>(args)..., getMemoryPtr());
		}
	}

	return ResultType();
}

// ============================================================================
