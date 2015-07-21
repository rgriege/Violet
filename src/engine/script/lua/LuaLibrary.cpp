// ============================================================================

#include "engine/script/lua/LuaLibrary.h"

using namespace Violet;

// ============================================================================

template <>
void Violet::luaV_push(lua_State * lua, const int & value)
{
	lua_pushinteger(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void Violet::luaV_push(lua_State * lua, const bool & value)
{
	lua_pushboolean(lua, value);
}

// ----------------------------------------------------------------------------

template <>
void Violet::luaV_push(lua_State * lua, const std::string & value)
{
	lua_pushstring(lua, value.c_str());
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

// ============================================================================
