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
	handle entity_id;
};

static void activate_task(void * mem)
{
	auto data = make_unique<activate_task_data>(mem);
	engine::instance().get_current_scene().create_component<key_input_component>(data->entity_id);
}

void ui_text_input_box::activate(const handle entity_id)
{
	const engine & engine = engine::instance();
	if (!engine.get_current_scene().has_component<key_input_component>(entity_id))
		add_task(activate_task, new activate_task_data{ entity_id }, key_input_component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

struct append_character_data
{
	handle entity_id;
	unsigned char character;
};

static void append_character_task(void * mem)
{
	auto data = make_unique<append_character_data>(mem);
	auto * tc = engine::instance().get_current_scene().get_component<text_component>(data->entity_id);
	if (tc != nullptr)
		tc->m_text += data->character;
}

struct remove_character_data
{
	handle entity_id;
};

void remove_character_task(void * mem)
{
	auto data = static_cast<remove_character_data*>(mem);
	auto * tc = engine::instance().get_current_scene().get_component<text_component>(data->entity_id);
	if (tc != nullptr && !tc->m_text.empty())
		tc->m_text.pop_back();
}

struct deactivate_task_data
{
	handle entity_id;
};

static void deactivate_task(void * mem)
{
	auto data = make_unique<deactivate_task_data>(mem);
	engine::instance().get_current_scene().remove<key_input_component>(data->entity_id);
}

ui_text_input_box::Status ui_text_input_box::on_key_up(const handle entity_id, const window_system::key_event & event)
{
	const unsigned char key = event.code;
	const engine & engine = engine::instance();
	if (std::isalpha(key) || key == '.' || key == ' ')
	{
		add_task(append_character_task, new append_character_data{ entity_id, key }, text_component::metadata->thread, task_type::write);
	}
	else if (key == key::Backspace)
	{
		add_task(remove_character_task, new remove_character_data{ entity_id }, text_component::metadata->thread, task_type::write);
	}
	else if (key == key::Return)
	{
		add_task(deactivate_task, new deactivate_task_data{ entity_id }, text_component::metadata->thread, task_type::del);
		return S_inactive;
	}
	else
		printf("%d\n", key);

	return S_active;
}

// ============================================================================
