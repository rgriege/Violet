// ============================================================================

template <typename ResultType, typename... Args>
ResultType Violet::CppScript::run(const char * method, Args&&... args) const
{
	void * methodPtr = getMethodPtr(method);
	if (methodPtr != nullptr)
	{
		if (m_memory == nullptr)
		{
			auto m = (ResultType(*)(Args...)) methodPtr;
			return m(std::forward<Args>(args)...);
		}
		else
		{
			auto m = (ResultType(*)(Args..., std::unique_ptr<Memory> &)) methodPtr;
			return m(std::forward<Args>(args)..., m_memory);
		}
	}

	return ResultType();
}

// ============================================================================
