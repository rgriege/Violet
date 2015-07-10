// ============================================================================

template <typename ResultType, typename... Args>
ResultType Violet::ScriptUtilities::run(const CppScriptComponent & component, const char * method, Args&&... args)
{
	return component.run<ResultType>(method, std::forward<Args>(args)...);
}

// ============================================================================
