#ifndef UPDATE_SYSTEM_H
#define UPDATE_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/plugins/update/component/UpdateComponent.h"

namespace Violet
{
	class AlterContext;
	class Deserializer;
	class SystemFactory;

	class VIOLET_UPDATE_API UpdateSystem : public ComponentSystem<UpdateComponent>
	{
	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~UpdateSystem() override = default;
		virtual void update(float dt, AlterContext & context);

	private:

		UpdateSystem() = default;
	};
}

#endif