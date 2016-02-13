#include "editor/editor_system.h"
#include "editor/cpp_script_exports.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/input/system/input_system.h"
#include "violet/log/log.h"
#include "violet/math/aabb.h"
#include "violet/math/poly.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/transform/transform.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"

using namespace edt;
using namespace vlt;

class instance : public cpp_script::instance
{
public:

    instance(cpp_script & script) :
        cpp_script::instance(script)
    {
        EntityDeselectedEvent::subscribe(engine::instance(), EntityDeselectedEvent::subscriber::bind<instance, &instance::onEntityDeselected>(this));
        EntitySelectedEvent::subscribe(engine::instance(), EntitySelectedEvent::subscriber::bind<instance, &instance::onEntitySelected>(this));
    }

    virtual ~instance() override
    {
        EntityDeselectedEvent::unsubscribe(engine::instance(), EntityDeselectedEvent::subscriber::bind<instance, &instance::onEntityDeselected>(this));
        EntitySelectedEvent::unsubscribe(engine::instance(), EntitySelectedEvent::subscriber::bind<instance, &instance::onEntitySelected>(this));
    }

private:

    void onEntityDeselected(const handle entityId)
    {
		log(formatted_string<128>().sprintf("<%d> deselected", entityId.id));
        const auto & engine = engine::instance();
        const auto & editor = *engine.get_system<editor_system>();
        const auto & selectedEntities = editor.get_selected_entities();
        if (!selectedEntities.empty())
        {
			/*m_selectionBox = aabb();
			for (const auto & entity : selectedEntities)
			{
				engine.add_write_task(engine.getSystem<editor_system>()->get_scene(),
					[=](scene & scene)
					{
						const auto * cc = scene.getComponent<color_component>(entityId);
						aabb box = cc->m_mesh->getPolygon().get_bounding_box();
						const Vec2f pos = transform::getPosition(scene.getComponent<world_transform_component>(entityId)->m_transform);
						box.translate(pos);
						m_selectionBox.extend(box);
						moveMutationHandle(m_minimumBoxId);
						moveMutationHandle(m_maximumBoxId);
					}, color_component::getStaticThread());
			}*/
        }
        else
        {
            // engine.getCurrentScene().removeAll(m_minimumBoxId);
            // engine.getCurrentScene().removeAll(m_maximumBoxId);
            m_minimumBoxId = handle::Invalid;
            m_maximumBoxId = handle::Invalid;
            m_selectionBox = aabb();
        }
    }

    void onEntitySelected(const handle entityId)
    {
		log(formatted_string<128>().sprintf("<%d> selected", entityId.id));
        const auto & engine = engine::instance();
        engine.add_write_task(engine.get_system<editor_system>()->get_scene(),
            [=](scene & scene)
            {
				const auto * cc = scene.get_component<color_component>(entityId);
                aabb box = cc->m_mesh->get_poly().get_bounding_box();
                const v2 pos = transform::get_position(scene.get_component<world_transform_component>(entityId)->transform);
                box.translate(pos);
                if (m_selectionBox.empty())
                {
                    m_selectionBox = box;
                    // addMutationHandle(m_minimumBoxId);
                    // addMutationHandle(m_maximumBoxId);
                }
                else
                {
                    m_selectionBox.extend(box);
                    // moveMutationHandle(m_minimumBoxId);
                    // moveMutationHandle(m_maximumBoxId);
                }
            }, color_component::get_thread_static());
    }

    void addMutationHandle(handle & entityId)
    {
        const auto & e = engine::instance();
        e.add_write_task(e.get_current_scene(),
            [&entityId, this](scene & s)
            {
                entityId = s.load("mutationHandle.json").front();
                moveMutationHandle(entityId);
            });
    }

    void moveMutationHandle(const handle entityId)
    {
        const auto & e = engine::instance();
        const v2 & offset = entityId == m_minimumBoxId
            ? m_selectionBox.get_min()
            : m_selectionBox.get_max();
        e.add_write_task(*e.get_current_scene().get_pool<world_transform_component>(),
            [=](component_pool & pool)
            {
                transform::set_position(pool.get<world_transform_component>(entityId)->transform, offset);
            });
    }

private:

    aabb m_selectionBox;
    handle m_minimumBoxId;
	handle m_maximumBoxId;
};

VIOLET_SCRIPT_EXPORT void init(cpp_script & script, std::unique_ptr<cpp_script::instance> & i)
{
    i = std::make_unique<instance>(script);
}

