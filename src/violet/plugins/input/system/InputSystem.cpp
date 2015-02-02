#include "violet/plugins/input/system/InputSystem.h"

#include "violet/core/AlterContext.h"
#include "violet/core/component/ComponentFactory.h"
#include "violet/core/script/Procedure.h"
#include "violet/core/script/system/ScriptSystem.h"
#include "violet/core/transform/TransformSystem.h"

#include <GL/freeglut.h>
#include <iostream>

using namespace Violet;

namespace InputSystemNamespace
{
	InputSystem * ms_inputSystem;

	AlterContext * ms_alterContext;
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
	ComponentFactory::getInstance().assign(InputComponent::getLabel(), std::bind(&InputSystem::create, ms_inputSystem, std::placeholders::_1, std::placeholders::_2));
	return std::unique_ptr<System>(ms_inputSystem);
}

void InputSystem::update(float /*dt*/, AlterContext & context)
{
	ms_alterContext = &context;
}

void InputSystem::onMouse(int button, int state, int x, int y)
{
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	Vec2f point(x, height - y);
	for (auto const & component : *ms_inputSystem->m_components)
	{
		auto const & transform = ms_alterContext->fetch<TransformSystem>(component.m_entity);
		if (component.m_mesh.contains(point - transform.m_position))
		{
			auto const & scriptComponent = ms_alterContext->fetch<ScriptSystem>(component.m_entity);
			scriptComponent.m_script->run(Procedure::create(state == GLUT_DOWN ? "onMouseDown" : "onMouseUp", component.m_entity, *ms_alterContext));
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
		auto const & scriptComponent = ms_alterContext->fetch<ScriptSystem>(component.m_entity);
		scriptComponent.m_script->run(Procedure::create("onKeyDown", component.m_entity, *ms_alterContext, key));
	}
}

void InputSystem::onKeyboardUp(const unsigned char key, int /*x*/, int /*y*/)
{
	static int i = 0;
	for (auto const & component : *ms_inputSystem->m_components)
	{
		auto const & scriptComponent = ms_alterContext->fetch<ScriptSystem>(component.m_entity);
		scriptComponent.m_script->run(Procedure::create("onKeyUp", component.m_entity, *ms_alterContext, key));
	}
}