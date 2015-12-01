#ifndef VIOLET_InputSystem_H
#define VIOLET_InputSystem_H

#include "violet/math/Vec2.h"
#include "violet/input/InputResult.h"
#include "violet/script/Script.h"
#include "violet/system/System.h"
#include "violet/window/MouseButton.h"

namespace Violet
{
	class Entity;
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
		static void init(Deserializer & deserializer);

	public:

		InputSystem(InputSystem && other);

		virtual ~InputSystem() override = default;
		virtual void update(float dt, const Engine & engine) override;

	private:

		InputSystem();
	};

	DEFINE_METHOD(KeyDownMethod, void(const Entity &, const Engine &, unsigned char));
	DEFINE_METHOD(KeyUpMethod, void(const Entity &, const Engine &, unsigned char));
	DEFINE_METHOD(MouseDownMethod, InputResult(const Entity &, const Engine &, const InputSystem::MouseButtonEvent &));
	DEFINE_METHOD(MouseUpMethod, InputResult(const Entity &, const Engine &, const InputSystem::MouseButtonEvent &));
	DEFINE_METHOD(MouseMoveMethod, void(const Entity &, const Engine &, const InputSystem::MouseMotionEvent &));
	DEFINE_METHOD(MouseInMethod, void(const Entity &, const Engine &));
	DEFINE_METHOD(MouseOutMethod, void(const Entity &, const Engine &));
}

#endif