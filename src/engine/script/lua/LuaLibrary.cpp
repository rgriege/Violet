// ============================================================================

#include "engine/script/lua/LuaLibrary.h"

using namespace Violet;

// ============================================================================

template <>
void Violet::luaV_push(lua_State * lua, const bool & value)
{
	lua_pushboolean(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void Violet::luaV_push(lua_State * lua, const int & value)
{
	lua_pushinteger(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void Violet::luaV_push(lua_State * lua, const double & value)
{
	lua_pushnumber(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void Violet::luaV_push(lua_State * lua, const std::string & value)
{
	lua_pushstring(lua, value.c_str());
}

// ----------------------------------------------------------------------------

template <>
bool Violet::luaV_pop(lua_State * lua)
{
	const bool result = lua_toboolean(lua, -1) == 1;
	lua_pop(lua, 1);
	return result;
}

// ----------------------------------------------------------------------------

template <>
int Violet::luaV_pop(lua_State * lua)
{
	const int result = luaL_checkint(lua, -1);
	lua_pop(lua, 1);
	return result;
}

// ----------------------------------------------------------------------------

template <>
double Violet::luaV_pop(lua_State * lua)
{
	const double result = luaL_checknumber(lua, -1);
	lua_pop(lua, 1);
	return result;
}

// ----------------------------------------------------------------------------

template <>
std::string Violet::luaV_pop(lua_State * lua)
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
	return static_cast<Violet::LuaMethodBase *>(func)->eval(lua);
}

// ============================================================================
