// ============================================================================

#include "violet/ui/UiTextInputBox.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/input/component/KeyInputComponent.h"

#include <cctype>

using namespace Violet;

// ============================================================================

void UiTextInputBox::activate(const EntityId entityId)
{
	const Engine & engine = Engine::getInstance();
	if (!engine.getCurrentScene().hasComponent<KeyInputComponent>(entityId))
	{
		engine.addWriteTask(engine.getCurrentScene(),
			[=](ComponentManager & manager)
			{
				manager.createComponent<KeyInputComponent>(entityId);
			});
	}
}

UiTextInputBox::Status UiTextInputBox::onKeyUp(const EntityId entityId, const WindowSystem::KeyEvent & event)
{
	const unsigned char key = event.code;
	const Engine & engine = Engine::getInstance();
	if (std::isalpha(key) || key == '.' || key == ' ')
	{
		const auto * tc = engine.getCurrentScene().getComponent<TextComponent>(entityId);
		if (tc != nullptr)
		{
			engine.addWriteTask(*tc,
				[key](TextComponent & tc)
				{
					tc.m_text += key;
				});
		}
	}
	else if (key == Key::Backspace)
	{
		const auto * tc = engine.getCurrentScene().getComponent<TextComponent>(entityId);
		if (tc != nullptr && !tc->m_text.empty())
		{
			engine.addWriteTask(*tc,
				[](TextComponent & tc)
				{
					tc.m_text.pop_back();
				});
		}
	}
	else if (key == Key::Return)
	{
		engine.addWriteTask(engine.getCurrentScene(),
			[=](ComponentManager & manager)
			{
				manager.remove<KeyInputComponent>(entityId);
			});
		return S_inactive;
	}
	else
		printf("%d\n", key);

	return S_active;
}

// ============================================================================
