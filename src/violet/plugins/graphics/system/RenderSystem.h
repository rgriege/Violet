#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/plugins/graphics/component/RenderComponent.h"
#include "violet/plugins/graphics/component/TextComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_GRAPHICS_API RenderSystem : public MultiComponentSystem<RenderComponent, TextComponent>
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		RenderSystem();
		virtual ~RenderSystem() override = default;
		virtual void update(float dt, Engine & engine) override;

	private:

		static void display();
	};
}

#endif