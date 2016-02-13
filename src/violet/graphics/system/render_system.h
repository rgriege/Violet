#ifndef VIOLET_RENDER_SYSTEM_H
#define VIOLET_RENDER_SYSTEM_H

#include "violet/math/m3.h"
#include "violet/system/system.h"

namespace vlt
{
	struct deserializer;
	struct system_factory;

	struct VIOLET_API render_system final : public system
	{
		static const char * get_label_static();
		static void install(system_factory & factory);
		static void init(deserializer & deserializer);

		virtual ~render_system() override;
		virtual void update(r32 dt) override;

	private:

		render_system();
	};
}

#endif