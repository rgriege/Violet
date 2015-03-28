#include "violet/core/Engine.h"
#include "violet/core/script/CppScript.h"
#include "violet/core/script/system/ScriptSystem.h"
#include "violet/core/serialization/FileDeserializerFactory.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/plugins/glut/GlutWindow.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include "violet/plugins/input/system/InputSystem.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"
#include "violet/plugins/update/system/UpdateSystem.h"

#include <iostream>

SystemFactory setup()
{
	Violet::Window::install(&GlutWindow::create);
	Violet::JsonDeserializer::install();

	Violet::CppScript::install();

	Violet::SystemFactory factory;
	Violet::TransformSystem::install(factory);
	Violet::PhysicsSystem::install(factory);
	Violet::RenderSystem::install(factory);
	Violet::InputSystem::install(factory);
	Violet::ScriptSystem::install(factory);
	Violet::UpdateSystem::install(factory);

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
