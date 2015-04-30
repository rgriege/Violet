#ifndef UPDATE_SYSTEM_H
#define UPDATE_SYSTEM_H

#include "engine/system/System.h"
#include "engine/update/component/UpdateComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API UpdateSystem : public ComponentSystem<UpdateComponent>
	{
	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		UpdateSystem(UpdateSystem && other);

		virtual ~UpdateSystem() override = default;
		virtual void update(float dt, Engine & engine);

	private:

		UpdateSystem() = default;
	};
}

#endif