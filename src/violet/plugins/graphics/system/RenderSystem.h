#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <vector>
#include <map>

#include "violet/plugins/graphics/component/RenderComponent.h"

namespace Violet
{
	class Deserializer;
	class Entity;

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

		static bool init(Settings & settings);

		static void update(float dt);

		static void create(Entity & entity, Deserializer & deserializer);

	private:

		static void display();

	private:

		std::vector<RenderComponent> m_components;
		std::map<uint32, uint32> m_entityComponentMap;
		int m_window;
	};
}

#endif