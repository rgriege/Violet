#ifndef VIOLET_RENDER_SYSTEM_H
#define VIOLET_RENDER_SYSTEM_H

#include "violet/system/system.h"

namespace vlt
{
	struct Deserializer;
	struct System_Factory;

	struct VIOLET_API Render_System final : public System
	{
		static const char * get_label_static();
		static void install(System_Factory & Factory);
		static void init(Deserializer & deserializer);

		Render_System();
		virtual ~Render_System() override;
		virtual void update(r32 dt) override;
	};
}

#endif
