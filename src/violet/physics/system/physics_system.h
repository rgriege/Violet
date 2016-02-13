#ifndef VIOLET_PHYSICS_SYSTEM_H
#define VIOLET_PHYSICS_SYSTEM_H

#include "violet/core/handle.h"
#include "violet/event/event.h"
#include "violet/math/v2.h"
#include "violet/system/system.h"
#include "violet/physics/component/physics_component.h"

namespace vlt
{
	struct deserializer;
	struct system_factory;

	struct VIOLET_API physics_system final : public system
	{
	public:

		static const char * get_label_static();
		static void install(system_factory & factory);
		static void init(deserializer & deserializer);

	public:

		physics_system(physics_system && other);

		virtual ~physics_system() override = default;
		virtual void update(r32 dt) override;

	private:

		physics_system(r32 drag, v2 gravity);

	private:
		
		r32 m_drag;
		v2 m_gravity;
	};

	DEFINE_EVENT(CollisionEvent, void(handle e1, handle e2));
}

#endif
 