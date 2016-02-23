// ============================================================================

#include "violet/component/scene.h"
#include "violet/script/script_component.h"
#include "violet/script/script_factory.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Script_Component::metadata;

// ============================================================================

Script_Component::Script_Component(const Handle entity_id, Deserializer & deserializer) :
	Script_Component(entity_id, deserializer.get_string("file"))
{
}

// ----------------------------------------------------------------------------

Script_Component::Script_Component(Handle entity_id, const char * const fileName) :
	script(Script_Factory::create(fileName))
{
	BindToComponentMethod::run(*script, std::move(entity_id));
}

// ----------------------------------------------------------------------------

Script_Component::Script_Component(Script_Component && other) :
	script(std::move(other.script))
{
}

// ----------------------------------------------------------------------------

Script_Component::~Script_Component()
{
	UnbindFromComponentMethod::run(*script);
}

// ============================================================================

void vlt::install_script_component()
{
	Script_Component::metadata = init_component_metadata(Tag('s', 'c', 'p', 't'), 0, sizeof(Script_Component));
	Scene::install_component<Script_Component>();
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Script_Component & component)
{
	serializer.write_string("file", component.script->get_filename().c_str());
	return serializer;
}

// ============================================================================
