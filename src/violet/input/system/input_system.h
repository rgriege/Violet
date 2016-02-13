#ifndef VIOLET_INPUT_SYSTEM_H
#define VIOLET_INPUT_SYSTEM_H

#include "violet/math/v2.h"
#include "violet/event/event.h"
#include "violet/core/handle.h"
#include "violet/input/input_result.h"
#include "violet/script/script.h"
#include "violet/system/system.h"
#include "violet/window/mouse_button.h"
#include "violet/window/window_system.h"

namespace vlt
{
	struct system_factory;

	struct VIOLET_API input_system final : public system
	{
		struct MouseButtonEvent
		{
			const v2 position;
			mouse_button button;
			key::modifier modifiers;
		};

		struct MouseMotionEvent
		{
			const v2 from;
			const v2 to;
		};

		static const char * get_label_static();
		static void install(system_factory & factory);
		static void init(deserializer & deserializer);

		input_system(input_system && other);

		virtual ~input_system() override = default;
		virtual void update(r32 dt) override;

		void focus(handle entity_id);
		void unfocus(handle entity_id);

	private:

		input_system();

		void process_focussed_event(const window_system::key_event & event, window_system::event_type type) thread_const;
		void process_focussed_event(const MouseButtonEvent & worldEvent, window_system::event_type type) thread_const;
		void process_focussed_event(const MouseMotionEvent & worldEvent) thread_const;

	private:

		handle m_focussedEntityId;
	};

	DEFINE_METHOD(KeyDownMethod, void(handle entity_id, const window_system::key_event &));
	DEFINE_METHOD(KeyUpMethod, void(handle entity_id, const window_system::key_event &));
	DEFINE_METHOD(MouseDownMethod, input_result(handle entity_id, const input_system::MouseButtonEvent &));
	DEFINE_METHOD(MouseUpMethod, input_result(handle entity_id, const input_system::MouseButtonEvent &));
	DEFINE_METHOD(MouseMoveMethod, void(handle entity_id, const input_system::MouseMotionEvent &));
	DEFINE_METHOD(MouseInMethod, void(handle entity_id));
	DEFINE_METHOD(MouseOutMethod, void(handle entity_id));
	DEFINE_METHOD(FocusLostMethod, void(handle entity_id));
	DEFINE_EVENT(QuitEvent, void());
}

#endif