#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/plugins/graphics/component/RenderComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_GRAPHICS_API RenderSystem : public ComponentSystem<RenderComponent>
	{
	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		RenderSystem() = default;
		virtual ~RenderSystem() override = default;
		virtual void update(float dt, Engine & engine) override;

	private:

		static void display();
	};
}

#endif