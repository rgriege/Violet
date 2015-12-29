// ============================================================================

#include "editor/command/ClearAllCommand.h"

#include "editor/EditorSystem.h"
#include "editor/component/EditorComponent.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/Random.h"
#include "violet/utility/StringUtilities.h"

using namespace edt;
using namespace Violet;

// ============================================================================

namespace ClearAllNamespace
{
	void cleanup(std::string tempFileName);
}

using namespace ClearAllNamespace;

// ============================================================================

const char * ClearAllCommand::getUsage()
{
	return "clear";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> ClearAllCommand::parse(const std::string & text)
{
	return text.empty() ? std::make_unique<ClearAllCommand>() : nullptr;
}

// ============================================================================

ClearAllCommand::ClearAllCommand() :
	m_tempFileName()
{
}

// ----------------------------------------------------------------------------

ClearAllCommand::~ClearAllCommand()
{
	if (!m_tempFileName.empty())
		cleanup(m_tempFileName);
}

// ----------------------------------------------------------------------------

void ClearAllCommand::execute()
{
	const auto & scene = Engine::getInstance().getCurrentScene();

	std::vector<EntityId> entityIds;
	for (const auto & entity : scene.getEntityView<EditorComponent>())
		entityIds.emplace_back(std::get<0>(entity).getEntityId());

	if (!entityIds.empty())
	{
		m_tempFileName = StringUtilities::rightOfFirst(FormattedString<32>().sprintf("%.6f.json", Random::ms_generator.generate0to1()), '.');

		scene.save(m_tempFileName.c_str(), entityIds, EditorSystem::ms_tagMap);

		Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
			[=]()
			{
				for (const auto & entityId : entityIds)
					Engine::getInstance().getCurrentScene().removeAll(entityId);
			}));
	}
}

// ----------------------------------------------------------------------------

bool ClearAllCommand::canUndo() const
{
	return !m_tempFileName.empty();
}

// ----------------------------------------------------------------------------

void ClearAllCommand::undo()
{
	if (!m_tempFileName.empty())
	{
		const std::string tempFileName = m_tempFileName;
		Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
			[=](ComponentManager & scene)
			{
				const auto & entityIds = scene.load(tempFileName.c_str(), EditorSystem::ms_tagMap);
				for (const auto entityId : entityIds)
					Engine::getInstance().getSystem<EditorSystem>()->addEditBehavior(scene, entityId);
				cleanup(tempFileName);
			});
		m_tempFileName.clear();
	}
}

// ============================================================================

void ClearAllNamespace::cleanup(std::string tempFileName)
{
	Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
		[=]()
		{
			auto deserializer = FileDeserializerFactory::getInstance().create(tempFileName.c_str());
			if (deserializer == nullptr)
				Log::log(FormattedString<128>().sprintf("Could not open scene file '%s'", tempFileName.c_str()));
			else if (!*deserializer)
				Log::log(FormattedString<128>().sprintf("Failed to parse scene file '%s'", tempFileName.c_str()));
			else
			{
				std::remove(deserializer->getString("idFile"));
				while (*deserializer)
				{
					deserializer->getString("cpnt");
					std::remove(deserializer->getString("file"));
				}
			}
			deserializer.reset();
			std::remove(tempFileName.c_str());
		}));
}

// ============================================================================
