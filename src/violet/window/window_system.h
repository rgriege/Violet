#ifndef VIOLET_WINDOW_SYSTEM_H
#define VIOLET_WINDOW_SYSTEM_H

#include <memory>

#include "violet/core/defines.h"
#include "violet/input/key.h"
#include "violet/system/system.h"
#include "violet/window/mouse_button.h"

namespace vlt
{
	struct deserializer;

	struct VIOLET_API window_system : public system
	{
		static const char * get_label_static() { return "wndw"; }

	public:

		enum event_type
		{
			ET_KeyDown = 1,
			ET_KeyUp = 2,
			ET_MouseDown = 4,
			ET_MouseUp = 8,
			ET_MouseMove = 16,
			ET_Quit = 32
		};

		struct key_event
		{
			unsigned char code;
			key::modifier modifiers;
		};

		struct mouse_event
		{
			int x, y;
			mouse_button button;
			key::modifier modifiers;
		};

		struct motion_event
		{
			int x, y, xrel, yrel;
		};

		struct event
		{
			event_type type;

			union
			{
				key_event key;
				mouse_event mouse;
				motion_event motion;
			};
		};

	public:

		window_system() : system(get_label_static()) {}

		virtual ~window_system() override = default;

		virtual void render() = 0;
		virtual bool get_event(event_type type, event* event) = 0;
		virtual void add_event(event event) = 0;
		virtual int get_width() const = 0;
		virtual int get_height() const = 0;
	};
}

#endif