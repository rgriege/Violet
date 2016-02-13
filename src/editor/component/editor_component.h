#ifndef EDITOR_EDITOR_COMPONENT_H
#define EDITOR_EDITOR_COMPONENT_H

#include "editor/Defines.h"
#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;
}

namespace edt
{
	struct EDITOR_API editor_component final: public vlt::component_base<editor_component, 0>
	{
		vlt::handle edit_id;

		static vlt::tag get_tag_static();
		static vlt::thread get_thread_static();

		editor_component(vlt::handle entity_id, vlt::handle editId);
		editor_component(vlt::handle entity_id, const vlt::component_deserializer & deserializer);
		editor_component(editor_component && other);
	};

	EDITOR_API vlt::serializer & operator<<(vlt::serializer & serializer, const editor_component & component);
}

#endif
