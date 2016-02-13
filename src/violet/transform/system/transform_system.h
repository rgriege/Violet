#ifndef VIOLET_TRANSFORM_SYSTEM_H
#define VIOLET_TRANSFORM_SYSTEM_H

#include <map>

#include "violet/core/handle.h"
#include "violet/math/m4.h"
#include "violet/system/system.h"

namespace vlt
{
	struct deserializer;
	struct system_factory;

	struct VIOLET_API transform_system : public system
	{
		static const char * get_label_static();
		static void install(system_factory & factory);
		static void init(deserializer & deserializer);

		transform_system(transform_system && other);

		virtual ~transform_system() override;
		virtual void update(r32 dt) override;

	private:

		transform_system();

		std::map<handle, m4> m_entityWorldtransformCache;
	};
}

#endif