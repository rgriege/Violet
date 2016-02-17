#ifndef VIOLET_UPDATE_SYSTEM_H
#define VIOLET_UPDATE_SYSTEM_H

#include "violet/core/handle.h"
#include "violet/system/system.h"
#include "violet/script/script.h"

namespace vlt
{
	struct deserializer;
	struct system_factory;

	struct VIOLET_API update_system final : public system
	{
		static const char * get_label_static();
		static void install(system_factory & factory);
		static void init(deserializer & deserializer);

		update_system();
		update_system(update_system && other);

		virtual void update(r32 dt) override;
	};

	DEFINE_METHOD(UpdateMethod, void(handle, r32));
}

#endif