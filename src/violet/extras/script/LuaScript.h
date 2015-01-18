#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "violet/core/script/Script.h"

#include <lua.hpp>

namespace Violet
{
	class Entity;

	class LuaScript : public Script
	{
	public:

		static void install();

	public:

		LuaScript(const char * filename);
		virtual ~LuaScript() override;

		virtual void run(const char * procedure, const Entity & entity) const override;

	private:

		lua_State * m_lua;
		bool m_valid;
	};
}

#endif