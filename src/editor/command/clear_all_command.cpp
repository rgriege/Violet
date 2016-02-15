// ============================================================================

#include "editor/command/clear_all_command.h"
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

void cleanup(std::string tempFileName);

// ============================================================================

const char * clear_all_command::get_usage()
{
	return "clear";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> clear_all_command::parse(const std::string & text)
{
	return text.empty() ? std::make_unique<clear_all_command>() : nullptr;
}

// ============================================================================

clear_all_command::clear_all_command() :
	m_tempFileName()
{
}

// ----------------------------------------------------------------------------

clear_all_command::~clear_all_command()
{
	if (!m_tempFileName.empty())
		cleanup(m_tempFileName);
}

// ----------------------------------------------------------------------------

void clear_all_command::execute()
{
	std::vector<handle> copiedEntityIds;
	for (const auto & entity : engine::instance().get_current_scene().get_entity_view<editor_component>())
		copiedEntityIds.emplace_back(entity.id);

	const auto & editor = *engine::instance().get_system<editor_system>();
	const auto & scene = editor.get_scene();
	const auto entityIds = scene.get_entity_ids();
	if (!entityIds.empty())
	{
		m_tempFileName = string_utilities::rightOfFirst(formatted_string<32>().sprintf("%.6f.json", random::ms_generator.generate_0_to_1()), '.');

		scene.save(m_tempFileName.c_str(), entityIds);

		for (const auto entity_id : entityIds)
		{
			editor.propagated_remove(entity_id);
			editor.get_scene().remove_all(entity_id);
		}
	}
}

// ----------------------------------------------------------------------------

bool clear_all_command::can_undo() const
{
	return !m_tempFileName.empty();
}

// ----------------------------------------------------------------------------

struct undo_task_data
{
	std::string filename;
};

static void undo_task(void * mem)
{
	auto data = make_unique<undo_task_data>(mem);
	log("clear_all_command::undo load");
	auto & editor = *engine::instance().get_system<editor_system>();
	const auto & entityIds = editor.get_scene().load(data->filename.c_str());
	for (const handle entity_id : entityIds)
		editor.propagate_add(entity_id);
	cleanup(data->filename);
}

void clear_all_command::undo()
{
	if (!m_tempFileName.empty())
	{
		// todo: assign scene tasks to thread
		add_task(undo_task, new undo_task_data{ std::move(m_tempFileName) }, 0, task_type::write);
		m_tempFileName.clear();
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
