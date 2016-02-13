// ============================================================================

template <typename ResultType, typename... Args>
ResultType vlt::cpp_script::run(const char * method, Args&&... args) const
{
	if (m_instance != nullptr)
	{
		void * methodPtr = get_method_ptr(method);
		if (methodPtr != nullptr)
		{
			auto m = (ResultType(*)(void *, Args...)) methodPtr;
			return m(m_instance, std::forward<Args>(args)...);
		}
	}

	return ResultType();
}

// ============================================================================
