#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/core/transform/TransformComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API TransformSystem : public ComponentSystem<TransformComponent>
	{
	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~TransformSystem() override = default;
		virtual void update(float dt, Engine & engine) override;
	};
}

#endif