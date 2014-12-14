#ifndef SETUP_H
#define SETUP_H

#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/extras/script/LuaScript.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"
#include "violet/plugins/input/system/InputSystem.h"

SystemFactory setup(int argc, char ** argv)
{
	Violet::JsonDeserializer::install();

	Violet::LuaScript::install();

	Violet::SystemFactory factory;
	Violet::TransformSystem::install(factory);
	Violet::PhysicsSystem::install(factory);
	Violet::RenderSystem::install(factory);
	Violet::InputSystem::install(factory);

	return factory;
}

#endif