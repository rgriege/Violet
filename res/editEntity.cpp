#include "editor/CppScriptExports.h"
#include "editor/EditorSystem.h"
#include "editor/component/EditorComponent.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/math/Polygon.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/script/ScriptComponent.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/transform/Transform.h"
#include "violet/utility/FormattedString.h"

using namespace edt;
using namespace Violet;

class Instance : public CppScript::Instance
{
private:

	struct DraggedEntity
	{
		DraggedEntity(const EntityId id, const Vec2f & startPos) :
			m_id(id),
			m_startPos(startPos)
		{
		}

		EntityId m_id;
		Vec2f m_startPos;
	};

public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_dragging(false),
        m_mouseDownPos(),
		m_draggedEntities()
    {
        MouseDownMethod::assign(m_script, MouseDownMethod::Handler::bind<Instance, &Instance::onMouseDown>(this));
        MouseMoveMethod::assign(m_script, MouseMoveMethod::Handler::bind<Instance, &Instance::onMouseMove>(this));
        MouseUpMethod::assign(m_script, MouseUpMethod::Handler::bind<Instance, &Instance::onMouseUp>(this));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(m_script);
        MouseMoveMethod::remove(m_script);
        MouseUpMethod::remove(m_script);
    }

private:

    InputResult onMouseDown(const EntityId entityId, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            const auto & engine = Engine::getInstance();

            m_dragging = true;
            m_mouseDownPos = event.position;
			drag(entityId);

            const bool multiSelect = (event.modifiers & Key::M_SHIFT) != 0;
            engine.addWriteTask(*engine.getSystem<EditorSystem>(),
                [=](EditorSystem & editor)
                {
					const EntityId editId = Engine::getInstance().getCurrentScene().getComponent<EditorComponent>(entityId)->m_editId;
					const auto & selectedEditIds = editor.getSelectedEntities();
					const auto & selectedProxyIds = getSelectedProxies(selectedEditIds);
                    auto it = selectedEditIds.find(editId);
                    const bool selected = it != selectedEditIds.end();

					if (!selected)
					{
						std::vector<std::unique_ptr<Command>> commands;
						commands.emplace_back(createSelectCommand(editId));

						if (!multiSelect && !selectedEditIds.empty())
							commands.emplace_back(createDeselectCommand(std::vector<EntityId>(selectedEditIds.begin(), selectedEditIds.end())));
						else
						{
							for (const EntityId id : selectedProxyIds)
								drag(id);
						}

						editor.execute(createChainCommand(std::move(commands)));
					}
					else if (multiSelect)
						editor.execute(createDeselectCommand(editId));
					else
					{
						for (const EntityId id : selectedProxyIds)
							drag(id);
					}
                });

            return InputResult::Block;
        }

        return InputResult::Pass;
    }

    void onMouseMove(const EntityId entityId, const InputSystem::MouseMotionEvent & event)
    {
        if (m_dragging)
        {
            const auto & engine = Engine::getInstance();
			const Vec2f & delta = event.to - m_mouseDownPos;
			for (const auto & draggedEntity : m_draggedEntities)
				move(draggedEntity.m_id, draggedEntity.m_startPos + delta);
        }
    }

    InputResult onMouseUp(const EntityId entityId, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            if (m_dragging)
            {
				if (event.position != m_mouseDownPos)
				{
					const auto & engine = Engine::getInstance();
					std::vector<std::pair<EntityId, Vec2f>> moveData;
					for (const auto & draggedEntity : m_draggedEntities)
					{
						const auto * ec = engine.getCurrentScene().getComponent<EditorComponent>(draggedEntity.m_id);
						const Vec2f pos = draggedEntity.m_startPos + event.position - m_mouseDownPos;
						moveData.emplace_back(ec->m_editId, pos);
					}
					
					engine.addWriteTask(*engine.getSystem<EditorSystem>(),
						[moveData](EditorSystem & editor) mutable
						{
							std::vector<std::unique_ptr<Command>> commands;
							for (const auto & d : moveData)
								commands.emplace_back(createMoveToCommand(d.first, d.second));
							editor.execute(createChainCommand(std::move(commands)));
						});
				}
				m_dragging = false;
                m_mouseDownPos = Vec2f::ZERO;
				m_draggedEntities.clear();
            }
            return InputResult::Block;
        }

        return InputResult::Pass;
    }

	std::vector<EntityId> getSelectedProxies(const std::set<EntityId> & selectedEntities) const
	{
		std::vector<EntityId> result;
		for (const auto & entity : Engine::getInstance().getCurrentScene().getEntityView<EditorComponent>())
			if (selectedEntities.find(std::get<0>(entity).m_editId) != selectedEntities.end())
				result.emplace_back(std::get<0>(entity).getEntityId());
		return result;
	}

	void drag(const EntityId entityId)
	{
		const auto & engine = Engine::getInstance();
		if (engine.getCurrentScene().hasComponent<LocalTransformComponent>(entityId))
		{
			if (std::find_if(m_draggedEntities.begin(), m_draggedEntities.end(), [=](const DraggedEntity & d) { return isAncestor(entityId, d.m_id); }) == m_draggedEntities.end())
			{
				const auto * ltc = engine.getCurrentScene().getComponent<LocalTransformComponent>(entityId);
				m_draggedEntities.emplace_back(entityId, Transform::getPosition(ltc->m_transform));
			}

			m_draggedEntities.erase(std::remove_if(m_draggedEntities.begin(), m_draggedEntities.end(),
				[=](const DraggedEntity & d)
				{
					return isAncestor(d.m_id, entityId);
				}), m_draggedEntities.end());
		}
		else
		{
			const auto * wtc = engine.getCurrentScene().getComponent<WorldTransformComponent>(entityId);
			m_draggedEntities.emplace_back(entityId, Transform::getPosition(wtc->m_transform));
		}
	}

	bool isAncestor(const EntityId childId, const EntityId parentId) const
	{
		const auto & engine = Engine::getInstance();
		const auto * ltc = engine.getCurrentScene().getComponent<LocalTransformComponent>(childId);
		if (ltc != nullptr)
		{
			if (ltc->m_parentId == parentId)
				return true;
			else if (ltc->m_parentId.isValid())
				return isAncestor(ltc->m_parentId, parentId);
		}
		return false;
	}

	void move(const EntityId entityId, const Vec2f & position) const
	{
		if (Engine::getInstance().getCurrentScene().hasComponent<LocalTransformComponent>(entityId))
			moveT<LocalTransformComponent>(entityId, position);
		else
			moveT<WorldTransformComponent>(entityId, position);
	}

    template <typename ComponentType>
    void moveT(const EntityId entityId, const Vec2f & position) const
    {
        const auto & engine = Engine::getInstance();
        engine.addWriteTask(*engine.getCurrentScene().getPool<ComponentType>(),
            [=](ComponentPool & pool)
            {
                auto & transform = pool.get<ComponentType>(entityId)->m_transform;
                Transform::setPosition(transform, position);
            });
    }

private:

    bool m_dragging;
    Vec2f m_mouseDownPos;
	std::vector<DraggedEntity> m_draggedEntities;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

