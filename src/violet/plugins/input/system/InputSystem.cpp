#include "violet/plugins/input/system/InputSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/script/Procedure.h"
#include "violet/core/script/system/ScriptSystem.h"
#include "violet/core/transform/TransformSystem.h"

#include <GL/freeglut.h>
#include <iostream>

using namespace Violet;

namespace InputSystemNamespace
{
	InputSystem * ms_inputSystem;

	Engine * ms_engine;
}

using namespace InputSystemNamespace;

void InputSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &InputSystem::init);
}

std::unique_ptr<System> InputSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	glutMouseFunc(onMouse);
	glutKeyboardFunc(onKeyboardDown);
	glutKeyboardUpFunc(onKeyboardUp);
	ms_inputSystem = new InputSystem();
	return std::unique_ptr<System>(ms_inputSystem);
}

void InputSystem::update(float /*dt*/, Engine & engine)
{
	ms_engine = &engine;
}

void InputSystem::onMouse(int button, int state, int x, int y)
{
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	Vec2f point(x, height - y);
	for (auto const & component : *ms_inputSystem->m_components)
	{
		auto const & transform = ms_engine->fetch<TransformSystem>(component.m_entity);
		if (component.m_mesh.contains(point - transform.m_position))
		{
			auto const & scriptComponent = ms_engine->fetch<ScriptSystem>(component.m_entity);
			scriptComponent.m_script->run(Procedure::create(state == GLUT_DOWN ? "onMouseDown" : "onMouseUp", component.m_entity, *ms_engine));
		}
	}
}

/*class KeyboardDownEvent
{
public:

	typedef std::function<void(const Entity & entity, AlterContext & context, unsigned char key)> Listener;

public:

	static void subscribe(const Entity & entity, Listener listener)
	{
		m_listeners[entity] = listener;
	}

	static void send(const Entity & entity, AlterContext & context, unsigned char key)
	{
		auto it = m_listeners.find(entity);
		if (it != m_listeners.end())
			it->second(entity, context, key);
	}

private:

	std::map<Entity, Listener> m_listeners;
};*/

void InputSystem::onKeyboardDown(const unsigned char key, int /*x*/, int /*y*/)
{
	for (auto const & component : *ms_inputSystem->m_components)
	{
		auto const & scriptComponent = ms_engine->fetch<ScriptSystem>(component.m_entity);
		scriptComponent.m_script->run(Procedure::create("onKeyDown", component.m_entity, *ms_engine, key));
	}
}

void InputSystem::onKeyboardUp(const unsigned char key, int /*x*/, int /*y*/)
{
	static int i = 0;
	for (auto const & component : *ms_inputSystem->m_components)
	{
		auto const & scriptComponent = ms_engine->fetch<ScriptSystem>(component.m_entity);
		scriptComponent.m_script->run(Procedure::create("onKeyUp", component.m_entity, *ms_engine, key));
	}
}