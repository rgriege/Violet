#include "engine/Engine.h"
#include "engine/script/system/CppScriptSystem.h"
#include "engine/serialization/file/FileDeserializerFactory.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/system/TransformSystem.h"
#include "engine/serialization/json/JsonDeserializer.h"
#include "game/pathfinding/PathfindingSystem.h"
#include "game/world/WorldSystem.h"
#include "engine/window/glut/GlutWindowSystem.h"
#include "engine/graphics/system/RenderSystem.h"
#include "engine/input/system/InputSystem.h"
#include "engine/physics/system/PhysicsSystem.h"
#include "engine/window/sdl/SDLWindowSystem.h"
#include "engine/update/system/UpdateSystem.h"

#include <iostream>

SystemFactory setup()
{
	Violet::JsonDeserializer::install();

	Violet::SystemFactory factory;
	Violet::SDLWindowSystem::install(factory);
	Violet::TransformSystem::install(factory);
	Violet::PhysicsSystem::install(factory);
	Violet::RenderSystem::install(factory);
	Violet::InputSystem::install(factory);
	Violet::CppScriptSystem::install(factory);
	Violet::UpdateSystem::install(factory);
	PathfindingSystem::install(factory);
	WorldSystem::install(factory);

	return factory;
}

int main(int /*argc*/, char ** /*argv*/)
{
	auto factory = setup();

	auto deserializer = FileDeserializerFactory::getInstance().create("config.json");
	if (deserializer == nullptr || !*deserializer)
	{
		std::cout << "failed to read config file" << std::endl;
		char c;
		std::cin >> c;
		exit(1);
	}

	auto engine = Violet::Engine::init(factory, *deserializer);
	if (engine == nullptr)
	{
		std::cout << "failed to init engine" << std::endl;
		char c;
		std::cin >> c;
		exit(1);
	}

	engine->begin();
}
