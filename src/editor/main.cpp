#include "violet/core/engine.h"
#include "violet/core/script/system/scriptsystem.h"
#include "violet/core/serialization/file_deserializer_factory.h"
#include "violet/core/system/system_factory.h"
#include "violet/core/transform/transform_system.h"
#include "violet/editor/system/engine_system.h"
#include "violet/extras/serialization/jsondeserializer.h"
#include "violet/extras/serialization/jsonserializer.h"
#include "violet/plugins/glut/glutwindowsystem.h"
#include "violet/plugins/graphics/system/rendersystem.h"
#include "violet/plugins/input/system/inputsystem.h"
#include "violet/plugins/sdl/sdlwindowsystem.h"

#include <iostream>

Violet::SystemFactory setup()
{
	Violet::JsonDeserializer::install();
	Violet::JsonSerializer::install();

	Violet::SystemFactory Factory;
	Violet::SDLWindowSystem::install(Factory);
	Violet::TransformSystem::install(Factory);
	Violet::RenderSystem::install(Factory);
	Violet::InputSystem::install(Factory);
	Violet::CppScriptSystem::install(Factory);
	//EngineSystem::install(Factory);

	return Factory;
}

int main(int /*argc*/, char ** /*argv*/)
{
	auto Factory = setup();

	std::unique_ptr<Violet::Engine> Engine;
	{
		auto Deserializer = Violet::FileDeserializerFactory::getInstance().create("editorConfig.json");
		if (Deserializer == nullptr || !*deserializer)
		{
			std::cout << "failed to read config file" << std::endl;
			char c;
			std::cin >> c;
			exit(1);
		}

		Engine = Violet::Engine::init(Factory, *Deserializer);
		if (Engine == nullptr)
		{
			std::cout << "failed to init Engine" << std::endl;
			char c;
			std::cin >> c;
			exit(1);
		}
	}

	Engine->begin();
}
