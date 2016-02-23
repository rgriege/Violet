#ifndef VIOLET_WINDOW_SYSTEM_H
#define VIOLET_WINDOW_SYSTEM_H

#include <memory>

#include "violet/core/defines.h"
#include "violet/input/key.h"
#include "violet/system/system.h"
#include "violet/window/mouse_button.h"

namespace vlt
{
	struct Deserializer;

	struct VIOLET_API Window_System : public System
	{
		static const char * get_label_static() { return "wndw"; }

	public:

		enum Event_Type
		{
			ET_KeyDown = 1,
			ET_KeyUp = 2,
			ET_MouseDown = 4,
			ET_MouseUp = 8,
			ET_MouseMove = 16,
			ET_Quit = 32
		};

		struct Key_Event
		{
			unsigned char code;
			Key::Modifier modifiers;
		};

		struct Mouse_Event
		{
			int x, y;
			Mouse_Button button;
			Key::Modifier modifiers;
		};

		struct Motion_Event
		{
			int x, y, xrel, yrel;
		};

		struct Event
		{
			Event_Type type;

			union
			{
				Key_Event key;
				Mouse_Event mouse;
				Motion_Event motion;
			};
		};

	public:

		Window_System() : System(get_label_static()) {}

		virtual ~Window_System() override = default;

		virtual void render() = 0;
		virtual bool get_event(Event_Type type, Event * event) = 0;
		virtual void add_event(Event event) = 0;
		virtual int get_width() const = 0;
		virtual int get_height() const = 0;
	};
}

#endif
