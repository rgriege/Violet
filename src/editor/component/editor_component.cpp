// ============================================================================

#include <assert.h>

#include "editor/component/editor_component.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const component_metadata * editor_component::metadata;

// ============================================================================

editor_component::editor_component(const handle entity_id, const handle editId) :
	proxied_id(editId)
{
}

// ----------------------------------------------------------------------------

editor_component::editor_component(const handle entity_id, const component_deserializer & /*deserializer*/) :
	proxied_id()
{
	assert(false);
}

// ----------------------------------------------------------------------------

editor_component::editor_component(editor_component && other) :
	proxied_id(other.proxied_id)
{
}

// ============================================================================

void edt::install_editor_component()
{
	editor_component::metadata = vlt::init_component_metadata(tag('e', 'd', 't', 'r'), 0, sizeof(editor_component));
	vlt::scene::install_component<editor_component>();
}

// ----------------------------------------------------------------------------

serializer & edt::operator<<(serializer & serializer, const editor_component & /*component*/)
{
	assert(false);
	return serializer;
}

// ============================================================================
