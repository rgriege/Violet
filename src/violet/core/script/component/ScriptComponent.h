#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "violet/core/component/Component.h"

#include "violet/core/script/Script.h"

#include <memory>

namespace Violet
{
	class Deserializer;

	class VIOLET_API ScriptComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		ScriptComponent(const Entity & entity, Deserializer & deserializer);
		ScriptComponent(ScriptComponent && other);
		ScriptComponent(const ScriptComponent &) = delete;

	public:

		std::unique_ptr<Script> m_script;
	};
}

#endif