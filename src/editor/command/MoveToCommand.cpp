// ============================================================================

#include "editor/command/MoveToCommand.h"

#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/log/Log.h"
#include "violet/transform/Transform.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/StringUtilities.h"

#include <cstdlib>

using namespace edt;
using namespace Violet;

// ============================================================================

const char * MoveToCommand::getUsage()
{
	return "move <id> <x> <y>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> MoveToCommand::parse(const std::string & text)
{
    std::vector<std::string> arguments;
    StringUtilities::split(text, ' ', arguments);
    if (arguments.size() == 3)
    {
        const uint32 id = std::atoi(arguments[0].c_str());
        const float x = static_cast<float>(std::atof(arguments[1].c_str()));
        const float y = static_cast<float>(std::atof(arguments[2].c_str()));
        return std::make_unique<MoveToCommand>(EntityId(id, ~0), Vec2f(x, y));
    }
    return nullptr;
}

// ============================================================================

MoveToCommand::MoveToCommand(const EntityId entityId, const Vec2f & position) :
    m_entityId(entityId),
    m_position(position)
{
}

// ----------------------------------------------------------------------------

void MoveToCommand::execute()
{
    Log::log(FormattedString<128>().sprintf("MoveTo::execute %d", m_entityId.getId()));
    const auto & engine = Engine::getInstance();
    const auto & editor = *engine.getSystem<EditorSystem>();
    const auto & scene = editor.getScene();
    if (m_entityId.getVersion() == EntityId::MaxVersion)
        m_entityId = EntityId(m_entityId.getId(), scene.getEntityVersion(m_entityId.getId()));
    const auto * ltc = scene.getComponent<LocalTransformComponent>(m_entityId);
    if (ltc != nullptr)
    {
        Log::log(FormattedString<128>().sprintf("MoveTo local <%f,%f>", m_position.x, m_position.y));
        engine.addWriteTask(*ltc,
            [=, &editor](LocalTransformComponent & ltc)
            {
                auto & transform = ltc.m_transform;
                const Vec2f oldPosition = Transform::getPosition(transform);
                Transform::setPosition(transform, m_position);
                m_position = oldPosition;
                editor.propogateChange<LocalTransformComponent, Matrix3f, &LocalTransformComponent::m_transform>(m_entityId, transform);
            });
    }
    else
    {
        const auto * wtc = scene.getComponent<WorldTransformComponent>(m_entityId);
        if (wtc != nullptr)
        {
            Log::log(FormattedString<128>().sprintf("MoveTo world <%f,%f>", m_position.x, m_position.y));
            engine.addWriteTask(*wtc,
                [=](WorldTransformComponent & wtc)
                {
                    auto & transform = wtc.m_transform;
                    const Vec2f oldPosition = Transform::getPosition(transform);
                    Transform::setPosition(transform, m_position);
                    m_position = oldPosition;
                });
            editor.propogateChange<WorldTransformComponent, Matrix3f, &WorldTransformComponent::m_transform>(m_entityId, wtc->m_transform);
        }
    }
}

// ----------------------------------------------------------------------------

bool MoveToCommand::canUndo() const
{
    return true;
}

// ----------------------------------------------------------------------------

void MoveToCommand::undo()
{
    execute();
}

// ============================================================================

