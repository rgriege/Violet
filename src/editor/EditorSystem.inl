// ============================================================================

#include "editor/component/EditorComponent.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"

#include "violet/log/Log.h"
#include "violet/utility/FormattedString.h"

// ============================================================================

template <typename T>
void edt::EditorSystem::registerCommand()
{
	registerCommand(T::getUsage(), &T::parse);
}

// ============================================================================

template <typename ComponentType, typename MemberType, MemberType ComponentType::*Member>
void edt::EditorSystem::propogateChange(const Violet::EntityId entityId, const MemberType & member) thread_const
{
    const auto & engine = Violet::Engine::getInstance();
	auto copyableMember = to_copyable(member);
    engine.addWriteTask(engine.getCurrentScene(),
		[entityId, copyableMember](Violet::ComponentManager & scene)
        {
            Violet::EntityId copyId;
            for (const auto & entity : scene.getEntityView<edt::EditorComponent>())
                if (entity.get<edt::EditorComponent>().m_editId == entityId)
                    copyId = entity.getId();
            Violet::Log::log(Violet::FormattedString<128>().sprintf("propogate %s %d -> %d", ComponentType::getStaticTag().asString().c_str(), entityId.getId(), copyId.getId()));
            if (copyId.isValid())
                scene.getComponent<ComponentType>(copyId)->*Member = static_cast<MemberType>(copyableMember);
        }, ComponentType::getStaticThread());
}

// ============================================================================
