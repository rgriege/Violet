#ifndef EDITOR_EDITOR_COMPONENT_H
#define EDITOR_EDITOR_COMPONENT_H

#include "editor/Defines.h"
#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;
}

namespace edt
{
	struct EDITOR_API editor_component final
	{
		vlt::handle proxied_id;

		static const vlt::component_metadata * metadata;

		editor_component(vlt::handle entity_id, vlt::handle editId);
		editor_component(vlt::handle entity_id, const vlt::component_deserializer & deserializer);
		editor_component(editor_component && other);
	};

	EDITOR_API void install_editor_component();

	EDITOR_API vlt::serializer & operator<<(vlt::serializer & serializer, const editor_component & component);
}

#endif
