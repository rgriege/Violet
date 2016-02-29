// ============================================================================

#include "editor/command/delete_command.h"
#include "editor/component/editor_component.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/log.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/random.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

static void cleanup(std::string tempFileName);

// ============================================================================

const char * Delete_Command::get_usage()
{
	return "delete <id>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Delete_Command::parse(const std::string & text)
{
	return !text.empty() ? std::make_unique<Delete_Command>(Handle(std::atoi(text.c_str()), 0)) : nullptr;
}

// ============================================================================

Delete_Command::Delete_Command(const vlt::Handle _proxy_id) :
	root_proxy_id(_proxy_id),
	temp_filename()
{
}

// ----------------------------------------------------------------------------

Delete_Command::~Delete_Command()
{
	if (!temp_filename.empty())
		cleanup(temp_filename);
}

// ----------------------------------------------------------------------------

void Delete_Command::execute()
{
	const auto & proxy_scene = Engine::instance().get_current_scene();
	root_proxy_id.version = proxy_scene.get_entity_version(root_proxy_id.id);
	const Handle root_proxied_id = proxy_scene.get_component<Editor_Component>(root_proxy_id)->proxied_id;

	// gather all children and delete them as well
	// todo: more than just 1 level
	std::vector<Handle> proxy_ids{ root_proxy_id };
	std::vector<Handle> proxied_ids{ root_proxied_id };
	for (const auto & entity : proxy_scene.get_entity_view<Local_Transform_Component>())
	{
		if (entity.get<Local_Transform_Component>().parent_id == root_proxy_id)
		{
			proxy_ids.emplace_back(entity.id);

			auto * ec = proxy_scene.get_component<Editor_Component>(entity.id);
			if (ec)
				proxied_ids.emplace_back(ec->proxied_id);
		}
	}

	// save
	const auto & editor = *Engine::instance().get_system<Editor_System>();
	const auto & proxied_scene = editor.get_scene();
	temp_filename = String_Utilities::rightOfFirst(Formatted_String<32>().sprintf("%.6f.json", Random::ms_generator.generate_0_to_1()), '.');
	proxied_scene.save(temp_filename.c_str(), proxied_ids);

	// remove
	for (const Handle proxy_id : proxy_ids)
		proxy_scene.remove_all(proxy_id);

	for (const Handle proxied_id : proxied_ids)
		proxied_scene.remove_all(proxied_id);
}

// ----------------------------------------------------------------------------

bool Delete_Command::can_undo() const
{
	return !temp_filename.empty();
}

// ----------------------------------------------------------------------------

static void propagate_add_task(void * mem)
{
	auto entity_ids = make_unique<std::vector<Handle>>(mem);
	auto & editor = Engine::instance().get_system<Editor_System>();
	for (const Handle entity_id : *entity_ids)
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
	log("Delete_Command::undo");
	auto & editor = *Engine::instance().get_system<Editor_System>();
	auto entity_ids = new std::vector<Handle>(editor.get_scene().load(data->c_str()));
	add_task(propagate_add_task, entity_ids, 0, task_type::read);
	add_task(cleanup_task, data, 0, task_type::read);
}

void Delete_Command::undo()
{
	if (!temp_filename.empty())
	{
		// todo: assign Scene tasks to Thread
		add_task(undo_task, new std::string(std::move(temp_filename)), 0, task_type::write);
		temp_filename.clear();
	}
}

// ============================================================================

void cleanup(std::string tempFileName)
{
	auto Deserializer = File_Deserializer_Factory::instance().create(tempFileName.c_str());
	if (Deserializer == nullptr)
		log(Formatted_String<128>().sprintf("Could not open Scene file '%s'", tempFileName.c_str()));
	else if (!Deserializer->is_valid())
		log(Formatted_String<128>().sprintf("Failed to parse Scene file '%s'", tempFileName.c_str()));
	else
	{
		std::remove(Deserializer->get_string("idFile"));
		while (Deserializer->is_valid())
		{
			Deserializer->get_string("cpnt");
			std::remove(Deserializer->get_string("file"));
		}
	}
	Deserializer.reset();
	std::remove(tempFileName.c_str());
}

// ============================================================================
