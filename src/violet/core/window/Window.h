#ifndef WINDOW
#define WINDOW

#include "violet/core/Defines.h"

#include <memory>

namespace Violet
{
	class Deserializer;

	class VIOLET_API Window
	{
	public:

		enum EventType
		{
			ET_KeyDown = 1,
			ET_KeyUp = 2,
			ET_MouseDown = 4,
			ET_MouseUp = 8,
			ET_MouseMove = 16,
		};

		struct KeyEvent
		{
			unsigned char code;
		};

		struct MouseEvent
		{
			int x, y;
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

		static void install(std::unique_ptr<Window>(*creator)(Deserializer &));
		static Window & create(Deserializer & deserializer);
		static Window & getCurrent();

	public:

		virtual ~Window() = default;

		virtual bool update() = 0;
		virtual void render() = 0;
		virtual bool getEvent(EventType type, Event* event) = 0;
		virtual void addEvent(Event event) = 0;
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
	};
}

#endif