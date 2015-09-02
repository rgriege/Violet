#ifndef VIOLET_WindowSystem_H
#define VIOLET_WindowSystem_H

#include "violet/Defines.h"
#include "violet/system/System.h"
#include "violet/window/MouseButton.h"

#include <memory>

namespace Violet
{
	class Deserializer;

	class VIOLET_API WindowSystem : public System
	{
	public:

		static const char * getStaticLabel() { return "wndw"; }

	public:

		enum EventType
		{
			ET_KeyDown = 1,
			ET_KeyUp = 2,
			ET_MouseDown = 4,
			ET_MouseUp = 8,
			ET_MouseMove = 16,
			ET_Quit = 32
		};

		struct KeyEvent
		{
			unsigned char code;
		};

		struct MouseEvent
		{
			int x, y;
			MouseButton button;
		};

		struct MotionEvent
		{
			int x, y, xrel, yrel;
		};

		struct Event
		{
			EventType type;

			union
			{
				KeyEvent key;
				MouseEvent mouse;
				MotionEvent motion;
			};
		};

	public:

		WindowSystem() : System(getStaticLabel()) {}

		virtual ~WindowSystem() override = default;

		virtual void render() = 0;
		virtual bool getEvent(EventType type, Event* event) = 0;
		virtual void addEvent(Event event) = 0;
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
	};
}

#endif