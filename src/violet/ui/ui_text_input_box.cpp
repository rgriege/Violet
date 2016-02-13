// ============================================================================

#include "violet/ui/ui_text_input_box.h"

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/text_component.h"
#include "violet/input/component/key_input_component.h"

#include <cctype>

using namespace vlt;

// ============================================================================

void ui_text_input_box::activate(const handle entity_id)
{
	const engine & engine = engine::instance();
	if (!engine.get_current_scene().has_component<key_input_component>(entity_id))
	{
		engine.add_write_task(engine.get_current_scene(),
			[=](scene & manager)
			{
				manager.create_component<key_input_component>(entity_id);
			});
	}
}

ui_text_input_box::Status ui_text_input_box::on_key_up(const handle entity_id, const window_system::key_event & event)
{
	const unsigned char key = event.code;
	const engine & engine = engine::instance();
	if (std::isalpha(key) || key == '.' || key == ' ')
	{
		const auto * tc = engine.get_current_scene().get_component<text_component>(entity_id);
		if (tc != nullptr)
		{
			engine.add_write_task(*tc,
				[key](text_component & tc)
				{
					tc.m_text += key;
				});
		}
	}
	else if (key == key::Backspace)
	{
		const auto * tc = engine.get_current_scene().get_component<text_component>(entity_id);
		if (tc != nullptr && !tc->m_text.empty())
		{
			engine.add_write_task(*tc,
				[](text_component & tc)
				{
					tc.m_text.pop_back();
				});
		}
	}
	else if (key == key::Return)
	{
		engine.add_write_task(engine.get_current_scene(),
			[=](scene & manager)
			{
				manager.remove<key_input_component>(entity_id);
			});
		return S_inactive;
	}
	else
		printf("%d\n", key);

	return S_active;
}

// ============================================================================
