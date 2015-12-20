#ifndef VIOLET_UiTextInputBox_H
#define VIOLET_UiTextInputBox_H

#include "violet/window/WindowSystem.h"

namespace Violet
{
	class VIOLET_API UiTextInputBox
	{
	public:

		enum Status
		{
			S_active,
			S_inactive
		};

	public:

		void activate(EntityId entityId);
		Status onKeyUp(EntityId entityId, const WindowSystem::KeyEvent & event);
	};
}

#endif