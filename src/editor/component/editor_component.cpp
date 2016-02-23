// ============================================================================

#include <assert.h>

#include "editor/component/editor_component.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const Component_Metadata * Editor_Component::metadata;

// ============================================================================

Editor_Component::Editor_Component(const Handle entity_id, const Handle _proxied_id) :
	proxied_id(_proxied_id)
{
}

// ----------------------------------------------------------------------------

Editor_Component::Editor_Component(const Handle entity_id, const Component_Deserializer & /*deserializer*/) :
	proxied_id()
{
	assert(false);
}

// ----------------------------------------------------------------------------

Editor_Component::Editor_Component(Editor_Component && other) :
	proxied_id(other.proxied_id)
{
}

// ============================================================================

void edt::install_editor_component()
{
	Editor_Component::metadata = vlt::init_component_metadata(Tag('e', 'd', 't', 'r'), 0, sizeof(Editor_Component));
	vlt::Scene::install_component<Editor_Component>();
}

// ----------------------------------------------------------------------------

Serializer & edt::operator<<(Serializer & serializer, const Editor_Component & /*component*/)
{
	assert(false);
	return serializer;
}

// ============================================================================
