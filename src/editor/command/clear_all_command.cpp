// ============================================================================

#include "editor/command/clear_all_command.h"

#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/random.h"
#include "violet/utility/string_utilities.h"

using namespace edt;
using namespace vlt;

// ============================================================================

namespace ClearAllNamespace
{
	void cleanup(std::string tempFileName);
}

using namespace ClearAllNamespace;

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

	const auto & scene = engine::instance().get_system<editor_system>()->get_scene();
	const auto entityIds = scene.get_entity_ids();
	if (!entityIds.empty())
	{
		m_tempFileName = string_utilities::rightOfFirst(formatted_string<32>().sprintf("%.6f.json", random::ms_generator.generate_0_to_1()), '.');

		scene.save(m_tempFileName.c_str(), entityIds);

		engine::instance().add_read_task(std::make_unique<delegate_task>(
			[=]()
			{
				const auto & editor = *engine::instance().get_system<editor_system>();
				for (const auto entity_id : entityIds)
				{
					editor.propagated_remove(entity_id);
					editor.get_scene().remove_all(entity_id);
				}
			}));
	}
}

// ----------------------------------------------------------------------------

bool clear_all_command::can_undo() const
{
	return !m_tempFileName.empty();
}

// ----------------------------------------------------------------------------

void clear_all_command::undo()
{
	if (!m_tempFileName.empty())
	{
		const std::string tempFileName = m_tempFileName;
		engine::instance().add_write_task(engine::instance().get_system<editor_system>()->get_scene(),
			[=](scene & scene)
			{
				log("clear_all_command::undo load");
				const auto & editor = *engine::instance().get_system<editor_system>();
				const auto & entityIds = scene.load(tempFileName.c_str());
				for (const handle entity_id : entityIds)
				{
					engine::instance().add_read_task(std::make_unique<delegate_task>(
						[=]()
						{
							engine::instance().get_system<editor_system>()->propagate_add(entity_id);
						}));
				}
				cleanup(tempFileName);
			});
		m_tempFileName.clear();
	}
}

// ============================================================================

void ClearAllNamespace::cleanup(std::string tempFileName)
{
	engine::instance().add_read_task(std::make_unique<delegate_task>(
		[=]()
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
		}));
}

// ============================================================================
