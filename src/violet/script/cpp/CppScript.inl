// ============================================================================

template <typename ResultType, typename... Args>
ResultType Violet::CppScript::run(const char * method, Args&&... args) const
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
