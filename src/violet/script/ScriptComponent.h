#ifndef VIOLET_ScriptComponent_H
#define VIOLET_ScriptComponent_H

#include "violet/component/Component.h"

#include "violet/script/Script.h"

#include <memory>

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API ScriptComponent : public ComponentBase<ScriptComponent>
	{
	public:

		static Tag getStaticTag();

	public:

		ScriptComponent(const Entity & owner, Deserializer & deserializer);
		ScriptComponent(ScriptComponent && other);

	public:

		std::unique_ptr<Script> m_script;
	};

	Serializer & operator<<(Serializer & serializer, const ScriptComponent & component);
}

#endif