#include "engine/Engine.h"
#include "engine/graphics/system/RenderSystem.h"
#include "engine/input/component/KeyInputComponent.h"
#include "engine/input/component/MouseInputComponent.h"
#include "engine/input/system/InputSystem.h"
#include "engine/physics/system/PhysicsSystem.h"
#include "engine/script/cpp/CppScriptComponent.h"
#include "engine/script/lua/LuaScriptComponent.h"
#include "engine/serialization/file/FileDeserializerFactory.h"
#include "engine/serialization/json/JsonDeserializer.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/update/component/UpdateComponent.h"
#include "engine/update/system/UpdateSystem.h"
#include "engine/window/glut/GlutWindowSystem.h"
#include "engine/window/sdl/SDLWindowSystem.h"
#include "game/pathfinding/MapComponent.h"
#include "game/pathfinding/PathComponent.h"
#include "game/pathfinding/PathfindingSystem.h"
#include "game/world/WorldSystem.h"

#include <iostream>

Violet::SystemFactory setup()
{
	Violet::Entity::installComponent<Violet::TransformComponent>();
	Violet::Entity::installComponent<Violet::RenderComponent>();
	Violet::Entity::installComponent<Violet::TextComponent>();
	Violet::Entity::installComponent<Violet::MouseInputComponent>();
	Violet::Entity::installComponent<Violet::KeyInputComponent>();
	Violet::Entity::installComponent<Violet::PhysicsComponent>();
	Violet::Entity::installComponent<Violet::CppScriptComponent>();
	Violet::Entity::installComponent<Violet::LuaScriptComponent>();
	Violet::Entity::installComponent<Violet::UpdateComponent>();
	Violet::Entity::installComponent<MapComponent>();
	Violet::Entity::installComponent<PathComponent>();
	Violet::Entity::installComponent<PathfindingComponent>();

	Violet::JsonDeserializer::install();

	Violet::SystemFactory factory;
	// Violet::GlutWindowSystem::install(factory);
	Violet::SDLWindowSystem::install(factory);
	Violet::PhysicsSystem::install(factory);
	Violet::RenderSystem::install(factory);
	Violet::InputSystem::install(factory);
	Violet::UpdateSystem::install(factory);
	PathfindingSystem::install(factory);
	WorldSystem::install(factory);

	return factory;
}

int main(int /*argc*/, char ** /*argv*/)
{
	auto factory = setup();

	auto deserializer = Violet::FileDeserializerFactory::getInstance().create("editorConfig.json");
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
