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

struct Instance final : public Cpp_Script::Instance
{
	struct Dragged_Proxy
	{
		Handle id;
		v2 start_pos;

		Dragged_Proxy(const Handle _id, const v2 & _start_pos) :
			id(_id),
			start_pos(_start_pos)
		{
		}
	};

	bool dragging;
	v2 mouse_down_pos;
	std::vector<Dragged_Proxy> dragged_proxies;

    Instance(Cpp_Script & script) :
        Cpp_Script::Instance(script),
        dragging(false),
        mouse_down_pos(),
		dragged_proxies()
    {
        MouseDownMethod::assign(script, MouseDownMethod::Handler::bind<Instance, &Instance::on_mouse_down>(this));
        MouseMoveMethod::assign(script, MouseMoveMethod::Handler::bind<Instance, &Instance::on_mouse_move>(this));
        MouseUpMethod::assign(script, MouseUpMethod::Handler::bind<Instance, &Instance::on_mouse_up>(this));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(script);
        MouseMoveMethod::remove(script);
        MouseUpMethod::remove(script);
    }

    Input_Result on_mouse_down(const Handle entity_id, const Input_System::Mouse_Button_Event & event)
    {
        if (event.button == MB_Left)
        {
            auto & engine = Engine::instance();

            dragging = true;
            mouse_down_pos = event.position;
			drag(entity_id);

            const bool multi_select = (event.modifiers & Key::M_SHIFT) != 0;

			// todo: async
			{
				auto & editor = *engine.get_system<Editor_System>();
				const auto & selected_proxy_ids = editor.get_selected_proxies();
				auto it = selected_proxy_ids.find(entity_id);
				const bool selected = it != selected_proxy_ids.end();

				if (!selected)
				{
					std::vector<std::unique_ptr<Command>> commands;
					commands.emplace_back(create_select_command(entity_id));

					if (!multi_select && !selected_proxy_ids.empty())
						commands.emplace_back(create_deselect_command(std::vector<Handle>(selected_proxy_ids.begin(), selected_proxy_ids.end())));
					else
					{
						for (const Handle id : selected_proxy_ids)
							drag(id);
					}

					editor.execute(create_chain_command(std::move(commands)));
				}
				else if (multi_select)
					editor.execute(create_deselect_command(entity_id));
				else
				{
					for (const Handle id : selected_proxy_ids)
						drag(id);
				}
			}

            return Input_Result::Block;
        }

        return Input_Result::Pass;
    }

    void on_mouse_move(const Handle entity_id, const Input_System::Mouse_Motion_Event & event)
    {
        if (dragging)
        {
            const auto & engine = Engine::instance();
			const v2 & delta = event.to - mouse_down_pos;
			for (const auto & dragged_proxy : dragged_proxies)
				move(dragged_proxy.id, dragged_proxy.start_pos + delta);
        }
    }

	Input_Result on_mouse_up(const Handle entity_id, const Input_System::Mouse_Button_Event & event)
    {
        if (event.button == MB_Left)
        {
            if (dragging)
            {
				if (event.position != mouse_down_pos)
				{
					auto & engine = Engine::instance();
					std::vector<std::pair<Handle, v2>> move_data;
					for (const auto & dragged_proxy : dragged_proxies)
					{
						const v2 end_pos = dragged_proxy.start_pos + event.position - mouse_down_pos;
						move_data.emplace_back(dragged_proxy.id, end_pos);
					}
					
					// todo: async
					auto & editor = *engine.get_system<Editor_System>();
					{
						std::vector<std::unique_ptr<Command>> commands;
						for (const auto & d : move_data)
							commands.emplace_back(create_move_to_command(d.first, d.second));
						editor.execute(create_chain_command(std::move(commands)));
					}
				}
				dragging = false;
                mouse_down_pos = v2::Zero;
				dragged_proxies.clear();
            }
            return Input_Result::Block;
        }

        return Input_Result::Pass;
    }

	void drag(const Handle proxy_id)
	{
		const auto & engine = Engine::instance();
		if (engine.get_current_scene().has_component<Local_Transform_Component>(proxy_id))
		{
			if (std::find_if(dragged_proxies.begin(), dragged_proxies.end(), [=](const Dragged_Proxy & d) { return is_ancestor(proxy_id, d.id); }) == dragged_proxies.end())
			{
				const auto * ltc = engine.get_current_scene().get_component<Local_Transform_Component>(proxy_id);
				dragged_proxies.emplace_back(proxy_id, Transform::get_position(ltc->transform));
			}

			dragged_proxies.erase(std::remove_if(dragged_proxies.begin(), dragged_proxies.end(),
				[=](const Dragged_Proxy & d)
				{
					return is_ancestor(d.id, proxy_id);
				}), dragged_proxies.end());
		}
		else
		{
			const auto * wtc = engine.get_current_scene().get_component<World_Transform_Component>(proxy_id);
			dragged_proxies.emplace_back(proxy_id, Transform::get_position(wtc->transform));
		}
	}

	bool is_ancestor(const Handle child_id, const Handle parent_id) const
	{
		const auto & engine = Engine::instance();
		const auto * ltc = engine.get_current_scene().get_component<Local_Transform_Component>(child_id);
		if (ltc != nullptr)
		{
			if (ltc->parent_id == parent_id)
				return true;
			else if (ltc->parent_id.is_valid())
				return is_ancestor(ltc->parent_id, parent_id);
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
};

VIOLET_SCRIPT_EXPORT void init(Cpp_Script & script, std::unique_ptr<Cpp_Script::Instance> & i)
{
    i = std::make_unique<Instance>(script);
}

