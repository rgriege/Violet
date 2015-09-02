// ============================================================================

template <typename T>
T Violet::luaV_pop(lua_State * lua)
{
	lua_pop(lua, -1);
	return T{};
}

// ============================================================================

template <typename ResultType, typename ... Args>
Violet::LuaMethod<ResultType(Args ...)>::LuaMethod(const char * name, std::function<ResultType(Args...)> func) :
	m_name(name),
	m_func(func)
{
}

// ----------------------------------------------------------------------------

template <typename ResultType, typename ... Args>
void Violet::LuaMethod<ResultType(Args ...)>::install(lua_State * lua)
{
	lua_pushlightuserdata(lua, this);
	lua_pushcclosure(lua, LuaLibrary::callFromLua, 1);
	lua_setglobal(lua, m_name);
}

// ----------------------------------------------------------------------------

template <typename ResultType, typename ... Args>
int Violet::LuaMethod<ResultType(Args ...)>::eval(lua_State * lua)
{
	luaV_push(lua, m_func(luaV_pop<Args>(lua)...));
	return 1;
}

// ============================================================================

template <typename ... Args>
Violet::LuaMethod<void(Args ...)>::LuaMethod(const char * name, std::function<void(Args...)> func) :
	m_name(name),
	m_func(func)
{
}

// ----------------------------------------------------------------------------

template <typename ... Args>
void Violet::LuaMethod<void(Args ...)>::install(lua_State * lua)
{
	lua_pushlightuserdata(lua, this);
	lua_pushcclosure(lua, LuaLibrary::callFromLua, 1);
	lua_setglobal(lua, m_name);
}

// ----------------------------------------------------------------------------

template <typename ... Args>
int Violet::LuaMethod<void(Args ...)>::eval(lua_State * lua)
{
	m_func();
	return 0;
}

// ============================================================================
