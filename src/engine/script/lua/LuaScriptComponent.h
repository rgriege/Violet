#ifndef VIOLET_LuaScriptComponent_H
#define VIOLET_LuaScriptComponent_H

#include "engine/component/Component.h"

#include <string>

struct lua_State;

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API LuaScriptComponent : public ComponentBase<LuaScriptComponent>
	{
	public:

		static Tag getTag();

	public:

		LuaScriptComponent(const Entity & owner, Deserializer & deserializer);
		LuaScriptComponent(LuaScriptComponent && other);
		~LuaScriptComponent();

		std::string const & getFilename() const;
		void reload();
		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args) const;

	private:

		LuaScriptComponent(const LuaScriptComponent &) = delete;
		LuaScriptComponent & operator=(const LuaScriptComponent &) = delete;

		void load();
		void unload();

	private:

		std::string m_filename;
		lua_State * m_lua;
		bool m_valid;
	};

	Serializer & operator<<(Serializer & serializer, const LuaScriptComponent & component);
}

#endif