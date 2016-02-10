// ============================================================================

#include "editor/command/ClearAllCommand.h"

#include "editor/EditorSystem.h"
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
	std::vector<EntityId> copiedEntityIds;
	for (const auto & entity : Engine::getInstance().getCurrentScene().getEntityView<EditorComponent>())
		copiedEntityIds.emplace_back(entity.getId());

	const auto & scene = Engine::getInstance().getSystem<EditorSystem>()->getScene();
	const auto entityIds = scene.getEntityIds();
	if (!entityIds.empty())
	{
		m_tempFileName = StringUtilities::rightOfFirst(FormattedString<32>().sprintf("%.6f.json", Random::ms_generator.generate0to1()), '.');

		scene.save(m_tempFileName.c_str(), entityIds);

		Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
			[=]()
			{
				const auto & editor = *Engine::getInstance().getSystem<EditorSystem>();
				for (const auto entityId : entityIds)
				{
					editor.propogateRemove(entityId);
					editor.getScene().removeAll(entityId);
				}
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
		Engine::getInstance().addWriteTask(Engine::getInstance().getSystem<EditorSystem>()->getScene(),
			[=](ComponentManager & scene)
			{
				Log::log("ClearAllCommand::undo load");
				const auto & editor = *Engine::getInstance().getSystem<EditorSystem>();
				const auto & entityIds = scene.load(tempFileName.c_str());
				for (const EntityId entityId : entityIds)
				{
					Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
						[=]()
						{
							Engine::getInstance().getSystem<EditorSystem>()->propogateAdd(entityId);
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
