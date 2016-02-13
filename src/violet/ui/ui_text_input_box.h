#ifndef VIOLET_UI_TEXT_INPUT_BOX_H
#define VIOLET_UI_TEXT_INPUT_BOX_H

#include "violet/core/handle.h"
#include "violet/window/window_system.h"

namespace vlt
{
	struct VIOLET_API ui_text_input_box final
	{
	public:

		enum Status
		{
			S_active,
			S_inactive
		};

	public:

		void activate(handle entity_id);
		Status on_key_up(handle entity_id, const window_system::key_event & event);
	};
}

#endif