#include "violet/core/Engine.h"
#include "violet/core/script/system/CppScriptSystem.h"
#include "violet/core/serialization/FileDeserializerFactory.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/editor/system/EngineSystem.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/extras/serialization/JsonSerializer.h"
#include "violet/plugins/glut/GlutWindowSystem.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include "violet/plugins/input/system/InputSystem.h"
#include "violet/plugins/sdl/SDLWindowSystem.h"

#include <iostream>

Violet::SystemFactory setup()
{
	Violet::JsonDeserializer::install();
	Violet::JsonSerializer::install();

	Violet::SystemFactory factory;
	Violet::SDLWindowSystem::install(factory);
	Violet::TransformSystem::install(factory);
	Violet::RenderSystem::install(factory);
	Violet::InputSystem::install(factory);
	Violet::CppScriptSystem::install(factory);
	EngineSystem::install(factory);

	return factory;
}

int main(int /*argc*/, char ** /*argv*/)
{
	auto factory = setup();

	std::unique_ptr<Violet::Engine> engine;
	{
		auto deserializer = Violet::FileDeserializerFactory::getInstance().create("editorConfig.json");
		if (deserializer == nullptr || !*deserializer)
		{
			std::cout << "failed to read config file" << std::endl;
			char c;
			std::cin >> c;
			exit(1);
		}

		engine = Violet::Engine::init(factory, *deserializer);
		if (engine == nullptr)
		{
			std::cout << "failed to init engine" << std::endl;
			char c;
			std::cin >> c;
			exit(1);
		}
	}

	engine->begin();
}
