#include "editor/cpp_script_exports.h"
#include "editor/editor_system.h"
#include "editor/component/editor_component.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/input/system/input_system.h"
#include "violet/log/log.h"
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

struct Instance : public Cpp_Script::Instance
{
private:

	struct DraggedEntity
	{
		DraggedEntity(const Handle id, const v2 & startPos) :
			m_id(id),
			m_startPos(startPos)
		{
		}

		Handle m_id;
		v2 m_startPos;
	};

public:

    Instance(Cpp_Script & script) :
        Cpp_Script::Instance(script),
        m_dragging(false),
        m_mouseDownPos(),
		m_draggedEntities()
    {
        MouseDownMethod::assign(script, MouseDownMethod::Handler::bind<Instance, &Instance::onMouseDown>(this));
        MouseMoveMethod::assign(script, MouseMoveMethod::Handler::bind<Instance, &Instance::onMouseMove>(this));
        MouseUpMethod::assign(script, MouseUpMethod::Handler::bind<Instance, &Instance::onMouseUp>(this));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(script);
        MouseMoveMethod::remove(script);
        MouseUpMethod::remove(script);
    }

private:

    Input_Result onMouseDown(const Handle entity_id, const Input_System::Mouse_Button_Event & event)
    {
        if (event.button == MB_Left)
        {
            auto & engine = Engine::instance();

            m_dragging = true;
            m_mouseDownPos = event.position;
			drag(entity_id);

            const bool multiSelect = (event.modifiers & Key::M_SHIFT) != 0;

			// todo: async
			{
				auto & editor = *engine.get_system<Editor_System>();
				const Handle proxied_id = engine.get_current_scene().get_component<Editor_Component>(entity_id)->proxied_id;
				const auto & selectedEditIds = editor.get_selected_entities();
				const auto & selectedProxyIds = get_selected_proxies(selectedEditIds);
				auto it = selectedEditIds.find(proxied_id);
				const bool selected = it != selectedEditIds.end();

				if (!selected)
				{
					std::vector<std::unique_ptr<Command>> commands;
					commands.emplace_back(create_select_command(proxied_id));

					if (!multiSelect && !selectedEditIds.empty())
						commands.emplace_back(create_deselect_command(std::vector<Handle>(selectedEditIds.begin(), selectedEditIds.end())));
					else
					{
						for (const Handle id : selectedProxyIds)
							drag(id);
					}

					editor.execute(create_chain_command(std::move(commands)));
				}
				else if (multiSelect)
					editor.execute(create_deselect_command(proxied_id));
				else
				{
					for (const Handle id : selectedProxyIds)
						drag(id);
				}
			}

            return Input_Result::Block;
        }

        return Input_Result::Pass;
    }

    void onMouseMove(const Handle entity_id, const Input_System::Mouse_Motion_Event & event)
    {
        if (m_dragging)
        {
            const auto & engine = Engine::instance();
			const v2 & delta = event.to - m_mouseDownPos;
			for (const auto & draggedEntity : m_draggedEntities)
				move(draggedEntity.m_id, draggedEntity.m_startPos + delta);
        }
    }

	Input_Result onMouseUp(const Handle entity_id, const Input_System::Mouse_Button_Event & event)
    {
        if (event.button == MB_Left)
        {
            if (m_dragging)
            {
				if (event.position != m_mouseDownPos)
				{
					auto & engine = Engine::instance();
					std::vector<std::pair<Handle, v2>> moveData;
					for (const auto & draggedEntity : m_draggedEntities)
					{
						const auto * ec = engine.get_current_scene().get_component<Editor_Component>(draggedEntity.m_id);
						const v2 pos = draggedEntity.m_startPos + event.position - m_mouseDownPos;
						moveData.emplace_back(ec->proxied_id, pos);
					}
					
					// todo: async
					auto & editor = *engine.get_system<Editor_System>();
					{
						std::vector<std::unique_ptr<Command>> commands;
						for (const auto & d : moveData)
							commands.emplace_back(create_move_to_command(d.first, d.second));
						editor.execute(create_chain_command(std::move(commands)));
					}
				}
				m_dragging = false;
                m_mouseDownPos = v2::Zero;
				m_draggedEntities.clear();
            }
            return Input_Result::Block;
        }

        return Input_Result::Pass;
    }

	std::vector<Handle> get_selected_proxies(const std::set<Handle> & selectedEntities) const
	{
		std::vector<Handle> result;
		for (const auto & entity : Engine::instance().get_current_scene().get_entity_view<Editor_Component>())
			if (selectedEntities.find(entity.get<Editor_Component>().proxied_id) != selectedEntities.end())
				result.emplace_back(entity.id);
		return result;
	}

	void drag(const Handle entity_id)
	{
		const auto & engine = Engine::instance();
		if (engine.get_current_scene().has_component<Local_Transform_Component>(entity_id))
		{
			if (std::find_if(m_draggedEntities.begin(), m_draggedEntities.end(), [=](const DraggedEntity & d) { return isAncestor(entity_id, d.m_id); }) == m_draggedEntities.end())
			{
				const auto * ltc = engine.get_current_scene().get_component<Local_Transform_Component>(entity_id);
				m_draggedEntities.emplace_back(entity_id, Transform::get_position(ltc->transform));
			}

			m_draggedEntities.erase(std::remove_if(m_draggedEntities.begin(), m_draggedEntities.end(),
				[=](const DraggedEntity & d)
				{
					return isAncestor(d.m_id, entity_id);
				}), m_draggedEntities.end());
		}
		else
		{
			const auto * wtc = engine.get_current_scene().get_component<World_Transform_Component>(entity_id);
			m_draggedEntities.emplace_back(entity_id, Transform::get_position(wtc->transform));
		}
	}

	bool isAncestor(const Handle childId, const Handle parentId) const
	{
		const auto & engine = Engine::instance();
		const auto * ltc = engine.get_current_scene().get_component<Local_Transform_Component>(childId);
		if (ltc != nullptr)
		{
			if (ltc->parent_id == parentId)
				return true;
			else if (ltc->parent_id.is_valid())
				return isAncestor(ltc->parent_id, parentId);
		}
		return false;
	}

	void move(const Handle entity_id, const v2 & position) const
	{
		if (Engine::instance().get_current_scene().has_component<Local_Transform_Component>(entity_id))
			moveT<Local_Transform_Component>(entity_id, position);
		else
			moveT<World_Transform_Component>(entity_id, position);
	}

    template <typename ComponentType>
    void moveT(const Handle entity_id, const v2 & position) const
    {
		// todo: async
		{
			auto component = Engine::instance().get_current_scene().get_component<ComponentType>(entity_id);
			Transform::set_position(component->transform, position);
		}
    }

private:

    bool m_dragging;
    v2 m_mouseDownPos;
	std::vector<DraggedEntity> m_draggedEntities;
};

VIOLET_SCRIPT_EXPORT void init(Cpp_Script & script, std::unique_ptr<Cpp_Script::Instance> & i)
{
    i = std::make_unique<Instance>(script);
}

