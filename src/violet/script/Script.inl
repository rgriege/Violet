// ============================================================================

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::assign(Script & script, const Delegate & func)
{
	const auto it = script.m_boundMethods.find(Derived::getName());
	if (it != script.m_boundMethods.end())
		script.warn(Derived::getName(), "delegate already bound");

	addHook(script, Derived::getName(), new Delegate(func));
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::assign(const Engine & engine, const Script & script, const Delegate & func)
{
	const auto it = script.m_boundMethods.find(Derived::getName());
	if (it != script.m_boundMethods.end())
		script.warn(Derived::getName(), "delegate already bound");

	engine.addWriteTask(script, addHook, Derived::getName(), new Delegate(func));
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
bool Violet::Script::Method<Derived, ResultType(Args...)>::has(const Script & script)
{
	return script.m_boundMethods.find(Derived::getName()) != script.m_boundMethods.end();
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
ResultType Violet::Script::Method<Derived, ResultType(Args...)>::run(const Script & script, Args && ... args)
{
	const auto it = script.m_boundMethods.find(Derived::getName());
	if (it != script.m_boundMethods.end())
		return (*static_cast<Delegate*>(it->second))(std::forward<Args>(args)...);
	else
	{
		script.warn(Derived::getName(), "failed to run - delegate not bound");
		return ResultType();
	}
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::remove(Script & script)
{
	removeHook(script);
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::remove(const Engine & engine, const Script & script)
{
	engine.addWriteTask(script, removeHook);
}

// ============================================================================

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::removeHook(Script & script)
{
	const auto it = script.m_boundMethods.find(Derived::getName());
	if (it != script.m_boundMethods.end())
	{
		static_cast<Delegate*>(it->second)->~function();
		script.m_boundMethods.erase(it);
	}
	else
		script.warn(Derived::getName(), "failed to remove - delegate not bound");
}

// ============================================================================