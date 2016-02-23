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
	struct System_Factory;

	struct VIOLET_API Input_System final : public System
	{
		struct Mouse_Button_Event
		{
			const v2 position;
			Mouse_Button button;
			Key::Modifier modifiers;
		};

		struct Mouse_Motion_Event
		{
			const v2 from;
			const v2 to;
		};

		static const char * get_label_static();
		static void install(System_Factory & Factory);
		static void init(Deserializer & deserializer);

		Input_System();
		Input_System(Input_System && other);

		virtual ~Input_System() override = default;
		virtual void update(r32 dt) override;

		void focus(Handle entity_id);
		void unfocus(Handle entity_id);
		
		void process_focussed_event(const Window_System::Key_Event & Event, Window_System::Event_Type type) thread_const;
		void process_focussed_event(const Mouse_Button_Event & worldEvent, Window_System::Event_Type type) thread_const;
		void process_focussed_event(const Mouse_Motion_Event & worldEvent) thread_const;

		Handle focussed_entity_id;
	};

	DEFINE_METHOD(KeyDownMethod, void(Handle entity_id, const Window_System::Key_Event &));
	DEFINE_METHOD(KeyUpMethod, void(Handle entity_id, const Window_System::Key_Event &));
	DEFINE_METHOD(MouseDownMethod, Input_Result(Handle entity_id, const Input_System::Mouse_Button_Event &));
	DEFINE_METHOD(MouseUpMethod, Input_Result(Handle entity_id, const Input_System::Mouse_Button_Event &));
	DEFINE_METHOD(MouseMoveMethod, void(Handle entity_id, const Input_System::Mouse_Motion_Event &));
	DEFINE_METHOD(MouseInMethod, void(Handle entity_id));
	DEFINE_METHOD(MouseOutMethod, void(Handle entity_id));
	DEFINE_METHOD(FocusLostMethod, void(Handle entity_id));
	DEFINE_EVENT(QuitEvent, void());
}

#endif
