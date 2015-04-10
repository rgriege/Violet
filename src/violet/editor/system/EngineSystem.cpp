#include "violet/editor/system/EngineSystem.h"

#include "violet/core/script/system/ScriptSystem.h"
#include "violet/core/serialization/FileDeserializerFactory.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/editor/system/SubWindowSystem.h"
#include "violet/editor/system/EditorSystem.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/game/pathfinding/PathfindingSystem.h"
#include "violet/game/world/WorldSystem.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include "violet/plugins/input/system/InputSystem.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"
#include "violet/plugins/sdl/SDLWindowSystem.h"
#include "violet/plugins/update/system/UpdateSystem.h"

#include <iostream>

void EngineSystem::install(Violet::SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &EngineSystem::init);
}

std::unique_ptr<Violet::System> EngineSystem::init(Violet::Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());
	settingsSegment->getUint("width");
	settingsSegment->getUint("height");
	Violet::SystemFactory factory;
	SubWindowSystem::install(factory);
	Violet::TransformSystem::install(factory);
	EditorSystem<Violet::PhysicsSystem>::install(factory);
	Violet::RenderSystem::install(factory);
	EditorSystem<Violet::InputSystem>::install(factory);
	EditorSystem<Violet::ScriptSystem>::install(factory);
	EditorSystem<Violet::UpdateSystem>::install(factory);
	EditorSystem<PathfindingSystem>::install(factory);
	EditorSystem<WorldSystem>::install(factory);

	auto configDeserializer = Violet::FileDeserializerFactory::getInstance().create("config.json");
	if (configDeserializer == nullptr || !*configDeserializer)
	{
		std::cout << "failed to read config file" << std::endl;
		char c;
		std::cin >> c;
		exit(1);
	}

	auto engine = Violet::Engine::init(factory, *configDeserializer);
	if (engine == nullptr)
	{
		std::cout << "failed to init engine" << std::endl;
		char c;
		std::cin >> c;
		exit(1);
	}

	return std::unique_ptr<Violet::System>(new EngineSystem(std::move(engine)));
}

const char * EngineSystem::getStaticLabel()
{
	return "engn";
}

EngineSystem::EngineSystem(std::unique_ptr<Violet::Engine> && engine) :
	System(getStaticLabel()),
	m_engine(std::move(engine))
{
}

void EngineSystem::loadScene(const char * filename)
{
	m_engine->switchScene(filename);
}

void EngineSystem::update(float dt, Violet::Engine & /*engine*/)
{
	m_engine->runFrame(dt);
}