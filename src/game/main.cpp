#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/graphics/component/TextureComponent.h"
#include "violet/graphics/system/RenderSystem.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/physics/system/PhysicsSystem.h"
#include "violet/script/ScriptComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/script/lua/LuaScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/json/JsonDeserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/TransformComponent.h"
#include "violet/ui/UiStateComponent.h"
#include "violet/update/component/UpdateComponent.h"
#include "violet/update/system/UpdateSystem.h"
#include "violet/window/glut/GlutWindowSystem.h"
#include "violet/window/sdl/SDLWindowSystem.h"
#include "game/pathfinding/MapComponent.h"
#include "game/pathfinding/PathComponent.h"
#include "game/pathfinding/PathfindingSystem.h"
#include "game/world/WorldSystem.h"

#include <iostream>

Violet::SystemFactory setup()
{
	Violet::Entity::installComponent<Violet::TransformComponent>();
	Violet::Entity::installComponent<Violet::ColorComponent>();
	Violet::Entity::installComponent<Violet::TextComponent>();
	Violet::Entity::installComponent<Violet::TextureComponent>();
	Violet::Entity::installComponent<Violet::MouseInputComponent>();
	Violet::Entity::installComponent<Violet::KeyInputComponent>();
	Violet::Entity::installComponent<Violet::PhysicsComponent>();
	Violet::Entity::installComponent<Violet::ScriptComponent>();
	Violet::Entity::installComponent<Violet::UiStateComponent>();
	Violet::Entity::installComponent<Violet::UpdateComponent>();
	Violet::Entity::installComponent<MapComponent>();
	Violet::Entity::installComponent<PathComponent>();
	Violet::Entity::installComponent<PathfindingComponent>();

	Violet::JsonDeserializer::install();
	// Violet::BinaryDeserializer::install();

	Violet::LuaScript::install();
	Violet::CppScript::install();

	Violet::SystemFactory factory;
	// Violet::GlutWindowSystem::install(factory);
	Violet::SDLWindowSystem::install(factory);
	Violet::PhysicsSystem::install(factory);
	Violet::RenderSystem::install(factory);
	Violet::InputSystem::install(factory);
	Violet::UpdateSystem::install(factory);
	Violet::EditorSystem::install(factory);
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
