#ifndef VIOLET_PHYSICS_SYSTEM_H
#define VIOLET_PHYSICS_SYSTEM_H

#include "violet/core/handle.h"
#include "violet/event/event.h"
#include "violet/math/v2.h"
#include "violet/system/system.h"
#include "violet/physics/component/physics_component.h"

namespace vlt
{
	struct Deserializer;
	struct System_Factory;

	struct VIOLET_API Physics_System final : public System
	{
	public:

		static const char * get_label_static();
		static void install(System_Factory & factory);
		static void init(Deserializer & deserializer);

	public:

		Physics_System(r32 drag, v2 gravity);
		Physics_System(Physics_System && other);

		virtual ~Physics_System() override = default;
		virtual void update(r32 dt) override;

	private:
		
		r32 m_drag;
		v2 m_gravity;
	};

	DEFINE_EVENT(CollisionEvent, void(Handle e1, Handle e2));
}

#endif
 
