#ifndef VIOLET_InputSystem_H
#define VIOLET_InputSystem_H

#include "violet/math/Vec2.h"
#include "violet/event/Event.h"
#include "violet/handle/Handle.h"
#include "violet/input/InputResult.h"
#include "violet/script/Script.h"
#include "violet/system/System.h"
#include "violet/window/MouseButton.h"
#include "violet/window/WindowSystem.h"

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
		static void init(Deserializer & deserializer);

	public:

		InputSystem(InputSystem && other);

		virtual ~InputSystem() override = default;
		virtual void update(float dt) override;

		void focus(EntityId entityId);
		void unfocus(EntityId entityId);

	private:

		InputSystem();

		void processFocussedEvent(const WindowSystem::KeyEvent & event, WindowSystem::EventType type) thread_const;
		void processFocussedEvent(const MouseButtonEvent & worldEvent, WindowSystem::EventType type) thread_const;
		void processFocussedEvent(const MouseMotionEvent & worldEvent) thread_const;

	private:

		EntityId m_focussedEntityId;
	};

	DEFINE_METHOD(KeyDownMethod, void(EntityId entityId, const WindowSystem::KeyEvent &));
	DEFINE_METHOD(KeyUpMethod, void(EntityId entityId, const WindowSystem::KeyEvent &));
	DEFINE_METHOD(MouseDownMethod, InputResult(EntityId entityId, const InputSystem::MouseButtonEvent &));
	DEFINE_METHOD(MouseUpMethod, InputResult(EntityId entityId, const InputSystem::MouseButtonEvent &));
	DEFINE_METHOD(MouseMoveMethod, void(EntityId entityId, const InputSystem::MouseMotionEvent &));
	DEFINE_METHOD(MouseInMethod, void(EntityId entityId));
	DEFINE_METHOD(MouseOutMethod, void(EntityId entityId));
	DEFINE_METHOD(FocusLostMethod, void(EntityId entityId));
	DEFINE_EVENT(QuitEvent, void());
}

#endif