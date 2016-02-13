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
	struct VIOLET_API lua_script final : public script
	{
		static void install();

		lua_script(shared_val<text_resource> source);
		lua_script(lua_script && other);
		virtual ~lua_script() override;

		virtual std::string const & get_filename() const override;
		virtual bool is_valid() const override;
		virtual void reload() override;

		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args && ... args) const;

	private:

		lua_script(const lua_script &) = delete;
		lua_script & operator=(const lua_script &) = delete;

		void load();
		void unload();

	private:

		shared_val<text_resource> m_source;
		lua_State * m_lua;
		bool m_valid;
	};
}

#include "violet/script/lua/lua_script.inl"

#endif