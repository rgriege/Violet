#ifndef VIOLET_SCRIPT_COMPONENT_H
#define VIOLET_SCRIPT_COMPONENT_H

#include <memory>

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/script/script.h"

namespace vlt
{
	struct Deserializer;
	struct Serializer;

	struct VIOLET_API Script_Component final
	{
		std::unique_ptr<Script> script;

		static const Component_Metadata * metadata;

		Script_Component(Handle entity_id, Deserializer & deserializer);
		Script_Component(Handle entity_id, const char * filename);
		Script_Component(Script_Component && other);
		~Script_Component();
	};

	VIOLET_API void install_script_component();

	VIOLET_API Serializer & operator<<(Serializer & serializer, const Script_Component & component);

	DEFINE_METHOD(BindToComponentMethod, void(Handle entity_id));
	DEFINE_METHOD(UnbindFromComponentMethod, void());
}

#endif
