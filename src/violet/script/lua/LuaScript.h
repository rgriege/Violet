#ifndef VIOLET_LuaScript_H
#define VIOLET_LuaScript_H

#include "violet/Defines.h"
#include "violet/script/Script.h"
#include "violet/utility/TextResource.h"
#include "violet/utility/shared_val.h"

#include <string>
#include <iosfwd>

struct lua_State;

namespace Violet
{
	class VIOLET_API LuaScript : public Script
	{
	public:

		static void install();

	public:

		LuaScript(shared_val<TextResource> source);
		LuaScript(LuaScript && other);
		virtual ~LuaScript() override;

		virtual std::string const & getFileName() const override;
		virtual bool isValid() const override;
		virtual void reload() override;

		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args && ... args) const;

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

#include "violet/script/lua/LuaScript.inl"

#endif