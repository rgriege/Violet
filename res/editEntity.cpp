#include "editor/cpp_script_exports.h"
#include "editor/editor_system.h"
#include "editor/component/editor_component.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/input/system/input_system.h"
#include "violet/log/Log.h"
#include "violet/math/poly.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/script/script_component.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/transform/transform.h"
#include "violet/utility/formatted_string.h"

using namespace edt;
using namespace vlt;

struct instance : public cpp_script::instance
{
private:

	struct DraggedEntity
	{
		DraggedEntity(const handle id, const v2 & startPos) :
			m_id(id),
			m_startPos(startPos)
		{
		}

		handle m_id;
		v2 m_startPos;
	};

public:

    instance(cpp_script & script) :
        cpp_script::instance(script),
        m_dragging(false),
        m_mouseDownPos(),
		m_draggedEntities()
    {
        MouseDownMethod::assign(script, MouseDownMethod::Handler::bind<instance, &instance::onMouseDown>(this));
        MouseMoveMethod::assign(script, MouseMoveMethod::Handler::bind<instance, &instance::onMouseMove>(this));
        MouseUpMethod::assign(script, MouseUpMethod::Handler::bind<instance, &instance::onMouseUp>(this));
    }

    virtual ~instance() override
    {
        MouseDownMethod::remove(script);
        MouseMoveMethod::remove(script);
        MouseUpMethod::remove(script);
    }

private:

    input_result onMouseDown(const handle entity_id, const input_system::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            const auto & engine = engine::instance();

            m_dragging = true;
            m_mouseDownPos = event.position;
			drag(entity_id);

            const bool multiSelect = (event.modifiers & key::M_SHIFT) != 0;
            engine.add_write_task(*engine.get_system<editor_system>(),
                [=](editor_system & editor)
                {
					const handle editId = engine::instance().get_current_scene().get_component<editor_component>(entity_id)->edit_id;
					const auto & selectedEditIds = editor.get_selected_entities();
					const auto & selectedProxyIds = getSelectedProxies(selectedEditIds);
                    auto it = selectedEditIds.find(editId);
                    const bool selected = it != selectedEditIds.end();

					if (!selected)
					{
						std::vector<std::unique_ptr<command>> commands;
						commands.emplace_back(create_select_command(editId));

						if (!multiSelect && !selectedEditIds.empty())
							commands.emplace_back(create_deselect_command(std::vector<handle>(selectedEditIds.begin(), selectedEditIds.end())));
						else
						{
							for (const handle id : selectedProxyIds)
								drag(id);
						}

						editor.execute(create_chain_command(std::move(commands)));
					}
					else if (multiSelect)
						editor.execute(create_deselect_command(editId));
					else
					{
						for (const handle id : selectedProxyIds)
							drag(id);
					}
                });

            return input_result::block;
        }

        return input_result::Pass;
    }

    void onMouseMove(const handle entity_id, const input_system::MouseMotionEvent & event)
    {
        if (m_dragging)
        {
            const auto & engine = engine::instance();
			const v2 & delta = event.to - m_mouseDownPos;
			for (const auto & draggedEntity : m_draggedEntities)
				move(draggedEntity.m_id, draggedEntity.m_startPos + delta);
        }
    }

    input_result onMouseUp(const handle entity_id, const input_system::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            if (m_dragging)
            {
				if (event.position != m_mouseDownPos)
				{
					const auto & engine = engine::instance();
					std::vector<std::pair<handle, v2>> moveData;
					for (const auto & draggedEntity : m_draggedEntities)
					{
						const auto * ec = engine.get_current_scene().get_component<editor_component>(draggedEntity.m_id);
						const v2 pos = draggedEntity.m_startPos + event.position - m_mouseDownPos;
						moveData.emplace_back(ec->edit_id, pos);
					}
					
					engine.add_write_task(*engine.get_system<editor_system>(),
						[moveData](editor_system & editor) mutable
						{
							std::vector<std::unique_ptr<command>> commands;
							for (const auto & d : moveData)
								commands.emplace_back(create_move_to_command(d.first, d.second));
							editor.execute(create_chain_command(std::move(commands)));
						});
				}
				m_dragging = false;
                m_mouseDownPos = v2::Zero;
				m_draggedEntities.clear();
            }
            return input_result::block;
        }

        return input_result::Pass;
    }

	std::vector<handle> getSelectedProxies(const std::set<handle> & selectedEntities) const
	{
		std::vector<handle> result;
		for (const auto & entity : engine::instance().get_current_scene().get_entity_view<editor_component>())
			if (selectedEntities.find(entity.get<editor_component>().edit_id) != selectedEntities.end())
				result.emplace_back(entity.id);
		return result;
	}

	void drag(const handle entity_id)
	{
		const auto & engine = engine::instance();
		if (engine.get_current_scene().has_component<local_transform_component>(entity_id))
		{
			if (std::find_if(m_draggedEntities.begin(), m_draggedEntities.end(), [=](const DraggedEntity & d) { return isAncestor(entity_id, d.m_id); }) == m_draggedEntities.end())
			{
				const auto * ltc = engine.get_current_scene().get_component<local_transform_component>(entity_id);
				m_draggedEntities.emplace_back(entity_id, transform::get_position(ltc->transform));
			}

			m_draggedEntities.erase(std::remove_if(m_draggedEntities.begin(), m_draggedEntities.end(),
				[=](const DraggedEntity & d)
				{
					return isAncestor(d.m_id, entity_id);
				}), m_draggedEntities.end());
		}
		else
		{
			const auto * wtc = engine.get_current_scene().get_component<world_transform_component>(entity_id);
			m_draggedEntities.emplace_back(entity_id, transform::get_position(wtc->transform));
		}
	}

	bool isAncestor(const handle childId, const handle parentId) const
	{
		const auto & engine = engine::instance();
		const auto * ltc = engine.get_current_scene().get_component<local_transform_component>(childId);
		if (ltc != nullptr)
		{
			if (ltc->parent_id == parentId)
				return true;
			else if (ltc->parent_id.is_valid())
				return isAncestor(ltc->parent_id, parentId);
		}
		return false;
	}

	void move(const handle entity_id, const v2 & position) const
	{
		if (engine::instance().get_current_scene().has_component<local_transform_component>(entity_id))
			moveT<local_transform_component>(entity_id, position);
		else
			moveT<world_transform_component>(entity_id, position);
	}

    template <typename ComponentType>
    void moveT(const handle entity_id, const v2 & position) const
    {
        const auto & engine = engine::instance();
        engine.add_write_task(*engine.get_current_scene().get_pool<ComponentType>(),
            [=](component_pool & pool)
            {
                auto & transform = pool.get<ComponentType>(entity_id)->transform;
                transform::set_position(transform, position);
            });
    }

private:

    bool m_dragging;
    v2 m_mouseDownPos;
	std::vector<DraggedEntity> m_draggedEntities;
};

VIOLET_SCRIPT_EXPORT void init(cpp_script & script, std::unique_ptr<cpp_script::instance> & i)
{
    i = std::make_unique<instance>(script);
}

