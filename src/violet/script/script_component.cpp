// ============================================================================

#include "violet/component/scene.h"
#include "violet/script/script_component.h"
#include "violet/script/script_factory.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const component_metadata * script_component::metadata;

// ============================================================================

script_component::script_component(const handle entity_id, deserializer & deserializer) :
	script_component(entity_id, deserializer.get_string("file"))
{
}

// ----------------------------------------------------------------------------

script_component::script_component(handle entity_id, const char * const fileName) :
	script(script_factory::create(fileName))
{
	BindToComponentMethod::run(*script, std::move(entity_id));
}

// ----------------------------------------------------------------------------

script_component::script_component(script_component && other) :
	script(std::move(other.script))
{
}

// ----------------------------------------------------------------------------

script_component::~script_component()
{
	UnbindFromComponentMethod::run(*script);
}

// ============================================================================

void vlt::install_script_component()
{
	script_component::metadata = init_component_metadata(tag('s', 'c', 'p', 't'), 0, sizeof(script_component));
	scene::install_component<script_component>();
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const script_component & component)
{
	serializer.write_string("file", component.script->get_filename().c_str());
	return serializer;
}

// ============================================================================
