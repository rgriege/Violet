// ============================================================================

template <typename ... Args>
int Violet::LuaMethod<void(Args ...)>::callFromLua(lua_State * lua)
{
	void * func = lua_touserdata(lua, lua_upvalueindex(1));
	return static_cast<Violet::LuaMethodBase *>(func)->eval(lua);
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
	lua_pushcclosure(lua, callFromLua, 1);
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
