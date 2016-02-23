#ifndef EDITOR_EDITOR_COMPONENT_H
#define EDITOR_EDITOR_COMPONENT_H

#include "editor/defines.h"
#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;
}

namespace edt
{
	struct EDITOR_API Editor_Component final
	{
		vlt::Handle proxied_id;

		static const vlt::Component_Metadata * metadata;

		Editor_Component(vlt::Handle entity_id, vlt::Handle editId);
		Editor_Component(vlt::Handle entity_id, const vlt::Component_Deserializer & deserializer);
		Editor_Component(Editor_Component && other);
	};

	EDITOR_API void install_editor_component();

	EDITOR_API vlt::Serializer & operator<<(vlt::Serializer & serializer, const Editor_Component & component);
}

#endif
