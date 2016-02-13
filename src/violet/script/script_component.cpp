// ============================================================================

#include "violet/script/script_component.h"

#include "violet/script/script_factory.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

tag script_component::get_tag_static()
{
	return tag('s', 'c', 'p', 't');
}

// ----------------------------------------------------------------------------

thread script_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

script_component::script_component(const handle entity_id, deserializer & deserializer) :
	script_component(entity_id, deserializer.get_string("file"))
{
}

// ----------------------------------------------------------------------------

script_component::script_component(handle entity_id, const char * const fileName) :
	component_base<script_component, 0>(),
	script(script_factory::create(fileName))
{
	BindToComponentMethod::run(*script, std::move(entity_id));
}

// ----------------------------------------------------------------------------

script_component::script_component(script_component && other) :
	component_base<script_component, 0>(std::move(other)),
	script(std::move(other.script))
{
}

// ----------------------------------------------------------------------------

script_component::~script_component()
{
	UnbindFromComponentMethod::run(*script);
}

// ============================================================================

serializer & vlt::operator<<(serializer & serializer, const script_component & component)
{
	serializer.write_string("file", component.script->get_filename().c_str());
	return serializer;
}

// ============================================================================
