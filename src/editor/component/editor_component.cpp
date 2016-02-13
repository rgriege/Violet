// ============================================================================

#include "editor/component/editor_component.h"

#include "violet/serialization/serializer.h"

#include <assert.h>

using namespace edt;
using namespace vlt;

// ============================================================================

tag editor_component::get_tag_static()
{
	return tag('e', 'd', 't', 'r');
}

// ----------------------------------------------------------------------------

thread editor_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

editor_component::editor_component(const handle entity_id, const handle editId) :
	component_base<editor_component, 0>(),
	edit_id(editId)
{
}

// ----------------------------------------------------------------------------

editor_component::editor_component(const handle entity_id, const component_deserializer & /*deserializer*/) :
	component_base<editor_component, 0>()
{
	assert(false);
}

// ----------------------------------------------------------------------------

editor_component::editor_component(editor_component && other) :
	component_base<editor_component, 0>(std::move(other)),
	edit_id(other.edit_id)
{
}

// ============================================================================

serializer & edt::operator<<(serializer & serializer, const editor_component & /*component*/)
{
	assert(false);
	return serializer;
}

// ============================================================================
