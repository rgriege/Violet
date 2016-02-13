// ============================================================================

#include "violet/script/lua/lua_library.h"

using namespace vlt;

// ============================================================================

template <>
void vlt::luaV_push(lua_State * lua, const bool & value)
{
	lua_pushboolean(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void vlt::luaV_push(lua_State * lua, const int & value)
{
	lua_pushinteger(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void vlt::luaV_push(lua_State * lua, const r64 & value)
{
	lua_pushnumber(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void vlt::luaV_push(lua_State * lua, const std::string & value)
{
	lua_pushstring(lua, value.c_str());
}

// ----------------------------------------------------------------------------

template <>
bool vlt::luaV_pop(lua_State * lua)
{
	const bool result = lua_toboolean(lua, -1) == 1;
	lua_pop(lua, 1);
	return result;
}

// ----------------------------------------------------------------------------

template <>
int vlt::luaV_pop(lua_State * lua)
{
	const int result = luaL_checkint(lua, -1);
	lua_pop(lua, 1);
	return result;
}

// ----------------------------------------------------------------------------

template <>
r64 vlt::luaV_pop(lua_State * lua)
{
	const r64 result = luaL_checknumber(lua, -1);
	lua_pop(lua, 1);
	return result;
}

// ----------------------------------------------------------------------------

template <>
std::string vlt::luaV_pop(lua_State * lua)
{
	const std::string result = luaL_checkstring(lua, -1);
	lua_pop(lua, 1);
	return result;
}

// ============================================================================

std::vector<unique_val<LuaMethodBase>> LuaLibrary::ms_methods;

// ============================================================================

void LuaLibrary::addMethod(unique_val<LuaMethodBase> && method)
{
	ms_methods.emplace_back(std::move(method));
}

// ----------------------------------------------------------------------------

void LuaLibrary::open(lua_State * const lua)
{
	for (const auto & method : ms_methods)
		method->install(lua);
}

// ----------------------------------------------------------------------------

int LuaLibrary::callFromLua(lua_State * lua)
{
	void * func = lua_touserdata(lua, lua_upvalueindex(1));
	return static_cast<vlt::LuaMethodBase *>(func)->eval(lua);
}

// ============================================================================
