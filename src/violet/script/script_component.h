#ifndef VIOLET_SCRIPT_COMPONENT_H
#define VIOLET_SCRIPT_COMPONENT_H

#include <memory>

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/script/script.h"

namespace vlt
{
	struct deserializer;
	struct serializer;

	struct VIOLET_API script_component final
	{
		std::unique_ptr<script> script;

		static const component_metadata * metadata;

		script_component(handle entity_id, deserializer & deserializer);
		script_component(handle entity_id, const char * fileName);
		script_component(script_component && other);
		~script_component();
	};

	VIOLET_API void install_script_component();

	VIOLET_API serializer & operator<<(serializer & serializer, const script_component & component);

	DEFINE_METHOD(BindToComponentMethod, void(handle entity_id));
	DEFINE_METHOD(UnbindFromComponentMethod, void());
}

#endif
