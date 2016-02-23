// ============================================================================

template <typename Derived, typename ResultType, typename ... Args>
void vlt::Script::Method<Derived, ResultType(Args...)>::assign(Script & script, const Handler & func)
{
	const auto it = script.m_boundMethods.find(Derived::get_identifier());
	if (it != script.m_boundMethods.end())
		script.warn(Derived::get_name(), "method already bound");

	add_hook(script, Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void vlt::Script::Method<Derived, ResultType(Args...)>::assign(const Script & script, const Handler & func)
{
	const auto it = script.m_boundMethods.find(Derived::get_identifier());
	if (it != script.m_boundMethods.end())
		script.warn(Derived::get_name(), "method already bound");

	add_hook(script, Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
bool vlt::Script::Method<Derived, ResultType(Args...)>::has(const Script & script)
{
	return script.m_boundMethods.find(Derived::get_identifier()) != script.m_boundMethods.end();
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
ResultType vlt::Script::Method<Derived, ResultType(Args...)>::run(const Script & script, Args && ... args)
{
	const auto it = script.m_boundMethods.find(Derived::get_identifier());
	if (it != script.m_boundMethods.end())
		return Handler(it->second)(std::forward<Args>(args)...);
	else
	{
		script.warn(Derived::get_name(), "failed to run - method not bound");
		return ResultType();
	}
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void vlt::Script::Method<Derived, ResultType(Args...)>::remove(Script & script)
{
	remove_hook(script, Derived::get_identifier(), Derived::get_name());
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void vlt::Script::Method<Derived, ResultType(Args...)>::remove(const Script & script)
{
	remove_hook(script, Derived::get_identifier(), Derived::get_name());
}

// ============================================================================
