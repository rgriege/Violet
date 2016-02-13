// ============================================================================

template <typename Derived, typename ResultType, typename ... Args>
void vlt::script::method<Derived, ResultType(Args...)>::assign(script & script, const Handler & func)
{
	const auto it = script.m_boundMethods.find(Derived::get_identifier());
	if (it != script.m_boundMethods.end())
		script.warn(Derived::get_name(), "method already bound");

	add_hook(script, Derived::get_identifier(), func);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void vlt::script::method<Derived, ResultType(Args...)>::assign(const script & _script, const Handler & func)
{
	const auto it = _script.m_boundMethods.find(Derived::get_identifier());
	if (it != _script.m_boundMethods.end())
		_script.warn(Derived::get_name(), "method already bound");

	engine::instance().add_write_task(_script,
		[=](script & _script)
		{
			add_hook(_script, Derived::get_identifier(), func);
		});
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
bool vlt::script::method<Derived, ResultType(Args...)>::has(const script & script)
{
	return script.m_boundMethods.find(Derived::get_identifier()) != script.m_boundMethods.end();
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
ResultType vlt::script::method<Derived, ResultType(Args...)>::run(const script & script, Args && ... args)
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
void vlt::script::method<Derived, ResultType(Args...)>::remove(script & script)
{
	remove_hook(script);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void vlt::script::method<Derived, ResultType(Args...)>::remove(const script & script)
{
	engine::instance().add_write_task(script, remove_hook);
}

// ============================================================================

template <typename Derived, typename ResultType, typename ... Args>
void vlt::script::method<Derived, ResultType(Args...)>::remove_hook(script & script)
{
	const auto it = script.m_boundMethods.find(Derived::get_identifier());
	if (it != script.m_boundMethods.end())
		script.m_boundMethods.erase(it);
	else
		script.warn(Derived::get_name(), "failed to remove - method not bound");
}

// ============================================================================
