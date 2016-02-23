// ============================================================================

#include <cctype>

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/text_component.h"
#include "violet/input/component/key_input_component.h"
#include "violet/ui/ui_text_input_box.h"
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

struct activate_task_data
{
	Handle entity_id;
};

static void activate_task(void * mem)
{
	auto data = make_unique<activate_task_data>(mem);
	Engine::instance().get_current_scene().create_component<Key_Input_Component>(data->entity_id);
}

void Ui_Text_Input_Box::activate(const Handle entity_id)
{
	const Engine & Engine = Engine::instance();
	if (!Engine.get_current_scene().has_component<Key_Input_Component>(entity_id))
		add_task(activate_task, new activate_task_data{ entity_id }, Key_Input_Component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

struct append_character_data
{
	Handle entity_id;
	unsigned char character;
};

static void append_character_task(void * mem)
{
	auto data = make_unique<append_character_data>(mem);
	auto * tc = Engine::instance().get_current_scene().get_component<Text_Component>(data->entity_id);
	if (tc != nullptr)
		tc->text += data->character;
}

struct remove_character_data
{
	Handle entity_id;
};

void remove_character_task(void * mem)
{
	auto data = static_cast<remove_character_data*>(mem);
	auto * tc = Engine::instance().get_current_scene().get_component<Text_Component>(data->entity_id);
	if (tc != nullptr && !tc->text.empty())
		tc->text.pop_back();
}

struct deactivate_task_data
{
	Handle entity_id;
};

static void deactivate_task(void * mem)
{
	auto data = make_unique<deactivate_task_data>(mem);
	Engine::instance().get_current_scene().remove<Key_Input_Component>(data->entity_id);
}

Ui_Text_Input_Box::Status Ui_Text_Input_Box::on_key_up(const Handle entity_id, const Window_System::Key_Event & event)
{
	const unsigned char key = event.code;
	const Engine & engine = Engine::instance();
	if (std::isprint(key) || key == '.' || key == ' ')
	{
		add_task(append_character_task, new append_character_data{ entity_id, key }, Text_Component::metadata->thread, task_type::write);
	}
	else if (key == Key::Backspace)
	{
		add_task(remove_character_task, new remove_character_data{ entity_id }, Text_Component::metadata->thread, task_type::write);
	}
	else if (key == Key::Return)
	{
		add_task(deactivate_task, new deactivate_task_data{ entity_id }, Text_Component::metadata->thread, task_type::del);
		return S_inactive;
	}
	else
		printf("%d\n", key);

	return S_active;
}

// ============================================================================
