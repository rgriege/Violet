// ============================================================================

#include "editor/command/delete_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/random.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

static void cleanup(std::string tempFileName);

// ============================================================================

const char * delete_command::get_usage()
{
	return "delete <id>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> delete_command::parse(const std::string & text)
{
	return !text.empty() ? std::make_unique<delete_command>(handle(std::atoi(text.c_str()), 0)) : nullptr;
}

// ============================================================================

delete_command::delete_command(const vlt::handle _entity_id) :
	entity_id(_entity_id),
	temp_filename()
{
}

// ----------------------------------------------------------------------------

delete_command::~delete_command()
{
	if (!temp_filename.empty())
		cleanup(temp_filename);
}

// ----------------------------------------------------------------------------

void delete_command::execute()
{
	const auto & proxy_scene = engine::instance().get_current_scene();
	entity_id.version = proxy_scene.get_entity_version(entity_id.id);

	const handle proxied_id = proxy_scene.get_component<editor_component>(entity_id)->proxied_id;

	const auto & editor = *engine::instance().get_system<editor_system>();
	const auto & proxied_scene = editor.get_scene();
	temp_filename = string_utilities::rightOfFirst(formatted_string<32>().sprintf("%.6f.json", random::ms_generator.generate_0_to_1()), '.');

	proxied_scene.save(temp_filename.c_str(), { proxied_id });

	proxy_scene.remove_all(entity_id);
	proxied_scene.remove_all(proxied_id);
}

// ----------------------------------------------------------------------------

bool delete_command::can_undo() const
{
	return !temp_filename.empty();
}

// ----------------------------------------------------------------------------

static void propagate_add_task(void * mem)
{
	auto entity_ids = make_unique<std::vector<handle>>(mem);
	auto & editor = engine::instance().get_system<editor_system>();
	for (const handle entity_id : *entity_ids)
		editor->propagate_add(entity_id);
}

static void cleanup_task(void * mem)
{
	auto temp_filename = make_unique<std::string>(mem);
	cleanup(*temp_filename);
}

static void undo_task(void * mem)
{
	auto data = static_cast<std::string*>(mem);
	log("delete_command::undo");
	auto & editor = *engine::instance().get_system<editor_system>();
	auto entity_ids = new std::vector<handle>(editor.get_scene().load(data->c_str()));
	add_task(propagate_add_task, entity_ids, 0, task_type::read);
	add_task(cleanup_task, data, 0, task_type::read);
}

void delete_command::undo()
{
	if (!temp_filename.empty())
	{
		// todo: assign scene tasks to thread
		add_task(undo_task, new std::string(std::move(temp_filename)), 0, task_type::write);
		temp_filename.clear();
	}
}

// ============================================================================

void cleanup(std::string tempFileName)
{
	auto deserializer = file_deserializer_factory::instance().create(tempFileName.c_str());
	if (deserializer == nullptr)
		log(formatted_string<128>().sprintf("Could not open scene file '%s'", tempFileName.c_str()));
	else if (!deserializer->is_valid())
		log(formatted_string<128>().sprintf("Failed to parse scene file '%s'", tempFileName.c_str()));
	else
	{
		std::remove(deserializer->get_string("idFile"));
		while (deserializer->is_valid())
		{
			deserializer->get_string("cpnt");
			std::remove(deserializer->get_string("file"));
		}
	}
	deserializer.reset();
	std::remove(tempFileName.c_str());
}

// ============================================================================
