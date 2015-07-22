#ifndef VIOLET_LuaScriptComponent_H
#define VIOLET_LuaScriptComponent_H

#include "engine/component/Component.h"

#include "engine/script/lua/LuaScript.h"

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

	public:

		LuaScript m_script;
	};

	Serializer & operator<<(Serializer & serializer, const LuaScriptComponent & component);
}

#endif