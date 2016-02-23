#ifndef VIOLET_UI_TEXT_INPUT_BOX_H
#define VIOLET_UI_TEXT_INPUT_BOX_H

#include "violet/core/handle.h"
#include "violet/window/window_system.h"

namespace vlt
{
	struct VIOLET_API Ui_Text_Input_Box final
	{
	public:

		enum Status
		{
			S_active,
			S_inactive
		};

	public:

		void activate(Handle entity_id);
		Status on_key_up(Handle entity_id, const Window_System::Key_Event & event);
	};
}

#endif
