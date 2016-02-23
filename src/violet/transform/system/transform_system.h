#ifndef VIOLET_TRANSFORM_SYSTEM_H
#define VIOLET_TRANSFORM_SYSTEM_H

#include <map>

#include "violet/core/handle.h"
#include "violet/math/m4.h"
#include "violet/system/system.h"

namespace vlt
{
	struct Deserializer;
	struct System_Factory;

	struct VIOLET_API Transform_System final : public System
	{
		static const char * get_label_static();
		static void install(System_Factory & factory);
		static void init(Deserializer & deserializer);

		Transform_System();
		Transform_System(Transform_System && other);

		virtual void update(r32 dt) override;

	private:

		std::map<Handle, m4> m_entityWorldtransformCache;
	};
}

#endif
