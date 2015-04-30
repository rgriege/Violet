#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H

#include "engine/system/System.h"
#include "engine/transform/component/TransformComponent.h"

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