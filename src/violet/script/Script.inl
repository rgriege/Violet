// ============================================================================

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::assign(Script & script, const Delegate & func)
{
	const auto it = script.m_boundMethods.find(Derived::getIdentifier());
	if (it != script.m_boundMethods.end())
		script.warn(Derived::getName(), "delegate already bound");

	addHook(script, Derived::getIdentifier(), new Delegate(func));
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::assign(const Script & script, const Delegate & func)
{
	const auto it = script.m_boundMethods.find(Derived::getIdentifier());
	if (it != script.m_boundMethods.end())
		script.warn(Derived::getName(), "delegate already bound");

	const auto delegate = new Delegate(func);
	Engine::getInstance().addWriteTask(script,
		[=](Script & script)
		{
			addHook(script, Derived::getIdentifier(), delegate);
		});
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
bool Violet::Script::Method<Derived, ResultType(Args...)>::has(const Script & script)
{
	return script.m_boundMethods.find(Derived::getIdentifier()) != script.m_boundMethods.end();
}

// ----------------------------------------------------------------------------

template <typename Derived, typename ResultType, typename ... Args>
ResultType Violet::Script::Method<Derived, ResultType(Args...)>::run(const Script & script, Args && ... args)
{
	const auto it = script.m_boundMethods.find(Derived::getIdentifier());
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
void Violet::Script::Method<Derived, ResultType(Args...)>::remove(const Script & script)
{
	Engine::getInstance().addWriteTask(script, removeHook);
}

// ============================================================================

template <typename Derived, typename ResultType, typename ... Args>
void Violet::Script::Method<Derived, ResultType(Args...)>::removeHook(Script & script)
{
	const auto it = script.m_boundMethods.find(Derived::getIdentifier());
	if (it != script.m_boundMethods.end())
	{
		static_cast<Delegate*>(it->second)->~function();
		script.m_boundMethods.erase(it);
	}
	else
		script.warn(Derived::getName(), "failed to remove - delegate not bound");
}

// ============================================================================
