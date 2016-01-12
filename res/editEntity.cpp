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
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_dragging(false),
        m_mouseDownPos(),
        m_startDragPos()
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
            if (engine.getCurrentScene().hasComponent<LocalTransformComponent>(entityId))
            {
                const auto * ltc = engine.getCurrentScene().getComponent<LocalTransformComponent>(entityId);
                m_startDragPos = Transform::getPosition(ltc->m_transform);
            }
            else
            {
                const auto * wtc = engine.getCurrentScene().getComponent<WorldTransformComponent>(entityId);
                m_startDragPos = Transform::getPosition(wtc->m_transform);
            }

            const bool multiSelect = (event.modifiers & Key::M_SHIFT) != 0;
            engine.addWriteTask(*engine.getSystem<EditorSystem>(),
                [=](EditorSystem & editor)
                {
                    const EntityId editId = Engine::getInstance().getCurrentScene().getComponent<EditorComponent>(entityId)->m_editId;
                    const auto & entityIds = editor.getSelectedEntities();
                    auto it = entityIds.find(editId);
                    const bool selected = it != entityIds.end();

					if (!selected)
					{
						editor.execute(createSelectCommand(editId));

						if (!multiSelect && !entityIds.empty())
							editor.execute(createDeselectCommand(std::vector<EntityId>(entityIds.begin(), entityIds.end())));
					}
					else if (multiSelect)
						editor.execute(createDeselectCommand(editId));
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
            const auto & newPosition = m_startDragPos + event.to - m_mouseDownPos;
            if (engine.getCurrentScene().hasComponent<LocalTransformComponent>(entityId))
                move<LocalTransformComponent>(entityId, newPosition);
            else
                move<WorldTransformComponent>(entityId, newPosition);
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
					const auto * ec = engine.getCurrentScene().getComponent<EditorComponent>(entityId);
					const Vec2f pos = m_startDragPos + event.position - m_mouseDownPos;
					engine.addWriteTask(*engine.getSystem<EditorSystem>(),
						[=](EditorSystem & editor)
						{
							editor.execute(createMoveToCommand(ec->m_editId, pos));
						});
				}
				m_dragging = false;
                m_startDragPos = Vec2f::ZERO;
                m_mouseDownPos = Vec2f::ZERO;
            }
            return InputResult::Block;
        }

        return InputResult::Pass;
    }

    template <typename ComponentType>
    void move(const EntityId entityId, const Vec2f & position)
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
    Vec2f m_startDragPos;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

