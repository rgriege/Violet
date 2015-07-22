#ifndef VIOLET_LuaScript_H
#define VIOLET_LuaScript_H

#include "engine/Defines.h"
#include "engine/utility/TextResource.h"
#include "engine/utility/shared_val.h"

#include <string>
#include <iosfwd>

struct lua_State;

namespace Violet
{
	class VIOLET_API LuaScript
	{
	public:

		LuaScript(shared_val<TextResource> source);
		LuaScript(LuaScript && other);
		~LuaScript();

		std::string const & getFilename() const;
		bool isValid() const;
		void reload();
		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args) const;

	private:

		LuaScript(const LuaScript &) = delete;
		LuaScript & operator=(const LuaScript &) = delete;

		void load();
		void unload();

	private:

		shared_val<TextResource> m_source;
		lua_State * m_lua;
		bool m_valid;
	};
}

#endif