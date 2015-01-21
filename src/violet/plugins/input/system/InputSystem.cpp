#include "violet/plugins/input/system/InputSystem.h"

#include "violet/core/AlterContext.h"
#include "violet/core/component/ComponentFactory.h"
#include "violet/core/transform/TransformSystem.h"

#include <GL/freeglut.h>

using namespace Violet;

namespace InputSystemNamespace
{
	InputSystem * ms_inputSystem;

	AlterContext * ms_alterContext;

	void runActions(InputComponent & inputComponent);
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
	glutKeyboardFunc(onKeyboard);
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
			component.m_script->run("onMouse", component.m_entity, *ms_alterContext);
	}
}

void InputSystem::onKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
	for (auto const & component : *ms_inputSystem->m_components)
	{
		auto const & transform = ms_alterContext->fetch<TransformSystem>(component.m_entity);
		component.m_script->run("onKeyboard", component.m_entity, *ms_alterContext);
	}
}