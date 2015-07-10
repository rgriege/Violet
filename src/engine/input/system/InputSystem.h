#ifndef VIOLET_InputSystem_H
#define VIOLET_InputSystem_H

#include "engine/math/Vec2.h"
#include "engine/input/InputResult.h"
#include "engine/system/System.h"
#include "engine/window/MouseButton.h"

namespace Violet
{
	class SystemFactory;

	class VIOLET_API InputSystem : public System
	{
	public:

		struct MouseButtonEvent
		{
			const Vec2f position;
			MouseButton button;
		};

		struct MouseMotionEvent
		{
			const Vec2f from;
			const Vec2f to;
		};

	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		InputSystem(InputSystem && other);

		virtual ~InputSystem() override = default;
		virtual void update(float dt, const Engine & engine) override;

	private:

		InputSystem();
	};
}

#endif