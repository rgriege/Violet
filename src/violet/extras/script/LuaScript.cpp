#include "violet/extras/script/LuaScript.h"

#include "violet/core/Defines.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/script/ScriptFactory.h"
#include "violet/core/utility/Assert.h"
#include "violet/extras/script/LuaLibrary.h"

#include <iostream>

using namespace Violet;

namespace LuaScriptNamespace
{
	std::unique_ptr<Script> create(const char *);

	int getId(lua_State * lua)
	{
		assert_equal(lua_gettop(lua), 1);
		const Entity ** p = static_cast<const Entity **>(luaL_checkudata(lua, 1, "Entity"));
		lua_pushnumber(lua, (*p)->m_id);
		return 1;
	}
}

using namespace LuaScriptNamespace;

void LuaScript::install()
{
	ScriptFactory::getInstance().assign("lua", &create);
}

LuaScript::LuaScript(const char * filename) :
	m_lua(luaL_newstate()),
	m_valid()
{
	luaL_openlibs(m_lua);

	luaL_newmetatable(m_lua, "Entity");
	lua_pushvalue(m_lua, -1);
	lua_setfield(m_lua, -2, "__index");
	lua_pushcfunction(m_lua, getId);
	lua_setfield(m_lua, -2, "getId");

	m_valid = luaL_loadfile(m_lua, filename) == 0;
}

LuaScript::~LuaScript()
{
	lua_close(m_lua);
}

CreateLuaWrapper(Entity);

void LuaScript::run(const Entity & entity) const
{
	luaV_pushudata(m_lua, entity);
	lua_setglobal(m_lua, "entity");

	lua_pcall(m_lua, 0, 0, 0);
}

std::unique_ptr<Script> LuaScriptNamespace::create(const char * filename)
{
	return std::unique_ptr<Script>(new LuaScript(filename));
}