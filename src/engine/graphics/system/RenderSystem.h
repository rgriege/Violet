#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "engine/system/System.h"
#include "engine/graphics/component/RenderComponent.h"
#include "engine/graphics/component/TextComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API RenderSystem : public MultiComponentSystem<RenderComponent, TextComponent>
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~RenderSystem() override;
		virtual void update(float dt, Engine & engine) override;

		void clear();

		void draw(RenderComponent & renderComponent, Engine & engine);
		void draw(TextComponent & textComponent, Engine & engine);

	private:

		RenderSystem();
	};
}

#endif