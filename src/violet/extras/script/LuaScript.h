#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "violet/core/script/Script.h"

#include <iosfwd>
#include <lua.hpp>

namespace Violet
{
	class Entity;

	class LuaScript : public Script
	{
	public:

		static void install();

	public:

		LuaScript(std::istream & stream);
		virtual ~LuaScript() override;

		virtual void run(const Entity & entity) const override;

	private:

		lua_State * m_lua;
		bool m_valid;
	};
}

#endif