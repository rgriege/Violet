#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <vector>

#include "violet/core/entity/Entity.h"
#include "violet/plugins/graphics/component/RenderComponent.h"

namespace Violet
{
	class RenderSystem
	{
	public:

		class Settings
		{
		public:

			int argc;
			char ** argv;
			int x, y, width, height;
			const char * title;
			Color screenColor;
		};

	public:

		static RenderSystem * init(Settings & settings);

	public:

		void update(float dt);

		//void create(Entity & entity, Deserializer & deserializer);

	private:

		static void display();

	private:

		std::vector<RenderComponent> m_components;
		int m_window;
	};
}

#endif