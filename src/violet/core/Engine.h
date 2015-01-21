#ifndef ENGINE_H
#define ENGINE_H

#include "violet/core/Defines.h"

#include <memory>
#include <vector>

namespace Violet
{
	class Deserializer;
	class Scene;
	class System;
	class SystemFactory;

	class VIOLET_API Engine
	{
	public:

		static std::unique_ptr<Engine> init(SystemFactory & factory, Deserializer & deserializer);

		void begin();

	public:

		~Engine();

	private:

		Engine(std::vector<std::unique_ptr<System>> && systems, std::unique_ptr<Scene> && firstScene);

	private:

		std::unique_ptr<Scene> m_activeScene;
		std::vector<std::unique_ptr<System>> m_systems;
	};
}

#endif