// ============================================================================

template <typename ResultType, typename... Args>
ResultType Violet::ScriptUtilities::run(CppScriptComponent & component, const char * method, Args&&... args)
{
	return component.run<ResultType>(method, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename ResultType, typename... Args>
ResultType Violet::ScriptUtilities::run(Engine & engine, const Entity & entity, const char * method, Args&&... args)
{
	CppScriptComponent * component = engine.getCurrentScene().m_componentManager.get<CppScriptComponent>(entity);
	return component != nullptr ? run<ResultType>(component, method, std::forward<Args>(args)...) : ResultType();
}

// ============================================================================
