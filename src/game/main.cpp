#include "editor/Editor.h"
#include "editor/command/ClearAllCommand.h"
#include "editor/command/file/OpenCommand.h"
#include "editor/command/file/SaveAllCommand.h"
#include "editor/component/EditorComponent.h"
#include "editor/component/EditorComponentWrapper.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/component/MappedDataComponent.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/graphics/component/TextureComponent.h"
#include "violet/graphics/system/RenderSystem.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/log/ConsoleLogTarget.h"
#include "violet/log/FileLogTarget.h"
#include "violet/physics/system/PhysicsSystem.h"
#include "violet/script/ScriptComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/script/lua/LuaScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/json/JsonDeserializer.h"
#include "violet/serialization/json/JsonSerializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/transform/system/TransformSystem.h"
#include "violet/update/component/UpdateComponent.h"
#include "violet/update/system/UpdateSystem.h"
#include "violet/window/glut/GlutWindowSystem.h"
#include "violet/window/sdl/SDLWindowSystem.h"

using namespace edt;

Violet::SystemFactory setup()
{
	Violet::ComponentManager::installComponent<Violet::ColorComponent>();
	Violet::ComponentManager::installComponent<Violet::KeyInputComponent>();
	Violet::ComponentManager::installComponent<Violet::LocalTransformComponent>();
	Violet::ComponentManager::installComponent<Violet::MappedDataComponent>();
	Violet::ComponentManager::installComponent<Violet::MouseInputComponent>();
	Violet::ComponentManager::installComponent<Violet::PhysicsComponent>();
	Violet::ComponentManager::installComponent<Violet::ScriptComponent>();
	Violet::ComponentManager::installComponent<Violet::TextComponent>();
	Violet::ComponentManager::installComponent<Violet::TextureComponent>();
	Violet::ComponentManager::installComponent<Violet::UpdateComponent>();
	Violet::ComponentManager::installComponent<Violet::WorldTransformComponent>();
	Violet::ComponentManager::installComponent<EditorComponent>();
	Violet::ComponentManager::installComponent<EditorComponentWrapper<Violet::ScriptComponent>>();
	Violet::ComponentManager::installComponent<EditorComponentWrapper<Violet::KeyInputComponent>>();
	Violet::ComponentManager::installComponent<EditorComponentWrapper<Violet::MouseInputComponent>>();
	Violet::ComponentManager::installComponent<EditorComponentWrapper<Violet::UpdateComponent>>();

	Violet::JsonDeserializer::install();
	Violet::JsonSerializer::install();
	// Violet::BinaryDeserializer::install();

	Violet::LuaScript::install();
	Violet::CppScript::install();

	Violet::SystemFactory factory;
	// Violet::GlutWindowSystem::install(factory);
	Violet::SDLWindowSystem::install(factory);
	Violet::PhysicsSystem::install(factory);
	Violet::RenderSystem::install(factory);
	Violet::InputSystem::install(factory);
	Violet::TransformSystem::install(factory);
	Violet::UpdateSystem::install(factory);

	Editor::registerCommand<ClearAllCommand>();
	Editor::registerCommand<OpenCommand>();
	Editor::registerCommand<SaveAllCommand>();

	return factory;
}

int main(int /*argc*/, char ** /*argv*/)
{
	const Violet::LogTarget::Guard consoleLogGuard(Violet::Log::installTarget(std::make_unique<Violet::ConsoleLogTarget>()));
	const Violet::LogTarget::Guard fileLogGuard(Violet::Log::installTarget(Violet::FileLogTarget::create("log.txt")));

	auto factory = setup();
	if (!Violet::Engine::bootstrap(factory, "editorConfig.json"))
	{
		Violet::Log::log("failed to init engine");
		char c;
		std::cin >> c;
		exit(1);
	}
}
