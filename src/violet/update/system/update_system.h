#ifndef VIOLET_UPDATE_SYSTEM_H
#define VIOLET_UPDATE_SYSTEM_H

#include "violet/core/handle.h"
#include "violet/system/system.h"
#include "violet/script/script.h"

namespace vlt
{
	struct Deserializer;
	struct System_Factory;

	struct VIOLET_API Update_System final : public System
	{
		static const char * get_label_static();
		static void install(System_Factory & factory);
		static void init(Deserializer & deserializer);

		Update_System();
		Update_System(Update_System && other);

		virtual void update(r32 dt) override;
	};

	DEFINE_METHOD(UpdateMethod, void(Handle, r32));
}

#endif
