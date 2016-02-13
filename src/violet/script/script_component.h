#ifndef VIOLET_SCRIPT_COMPONENT_H
#define VIOLET_SCRIPT_COMPONENT_H

#include <memory>

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/script/script.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct deserializer;
	struct serializer;

	struct VIOLET_API script_component final : public component_base<script_component, 0>
	{
		std::unique_ptr<script> script;

		static tag get_tag_static();
		static thread get_thread_static();

		script_component(handle entity_id, deserializer & deserializer);
		script_component(handle entity_id, const char * fileName);
		script_component(script_component && other);
		~script_component();
	};

	VIOLET_API serializer & operator<<(serializer & serializer, const script_component & component);

	DEFINE_METHOD(BindToComponentMethod, void(handle entity_id));
	DEFINE_METHOD(UnbindFromComponentMethod, void());
}

#endif
