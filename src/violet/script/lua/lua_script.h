#ifndef VIOLET_LUA_SCRIPT_H
#define VIOLET_LUA_SCRIPT_H

#include <iosfwd>
#include <string>

#include "violet/core/defines.h"
#include "violet/script/script.h"
#include "violet/utility/text_resource.h"
#include "violet/utility/shared_val.h"

struct lua_State;

namespace vlt
{
	struct VIOLET_API Lua_Script final : public Script
	{
		static void install();

		Lua_Script(shared_val<Text_Resource> source);
		Lua_Script(Lua_Script && other);
		virtual ~Lua_Script() override;

		virtual std::string const & get_filename() const override;
		virtual bool is_valid() const override;
		virtual void reload() override;

		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args && ... args) const;

	private:

		Lua_Script(const Lua_Script &) = delete;
		Lua_Script & operator=(const Lua_Script &) = delete;

		void load();
		void unload();

	private:

		shared_val<Text_Resource> m_source;
		lua_State * m_lua;
		bool m_valid;
	};
}

#include "violet/script/lua/lua_script.inl"

#endif