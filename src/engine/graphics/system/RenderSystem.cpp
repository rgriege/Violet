// ============================================================================

#include "engine/graphics/system/RenderSystem.h"

#include "engine/Engine.h"
#include "engine/entity/Entity.h"
#include "engine/scene/SceneUtilities.h"
#include "engine/serialization/Deserializer.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/utility/Guard.h"
#include "engine/window/WindowSystem.h"
#include "engine/graphics/font/Font.h"
#include "engine/graphics/shader/Shader.h"

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace Violet;

namespace RenderSystemNamespace
{
	class RenderTask : public Engine::Task
	{
	public:

		RenderTask(const Engine & engine);

		virtual void execute() const override;
	};
}

using namespace RenderSystemNamespace;

// ============================================================================

const char * RenderSystem::getStaticLabel()
{
	return "rndr";
}

// ----------------------------------------------------------------------------

void RenderSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &RenderSystem::init);
}

// ----------------------------------------------------------------------------

std::unique_ptr<System> RenderSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());

	Color const color(*settingsSegment);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << "glewInit error: " << glewGetErrorString(glewError);
		return nullptr;
	}

	std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
	
	glClearColor(color.r, color.g, color.b, color.a);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	return std::unique_ptr<System>(new RenderSystem);
}

// ============================================================================

RenderSystem::~RenderSystem()
{
	ShaderProgram::getCache().clear();
	Font::getCache().clear();
}

void process(const Entity &, float);

// ----------------------------------------------------------------------------

void RenderSystem::update(float const dt, const Engine & engine)
{
	glClear(GL_COLOR_BUFFER_BIT);
	const auto & windowSystem = engine.getSystem<WindowSystem>();
	Matrix3f viewMatrix = Matrix3f::Identity;
	viewMatrix[0][0] = 2.f / windowSystem->getWidth();
	viewMatrix[1][1] = 2.f / windowSystem->getHeight();

	for (auto const & child : engine.getCurrentScene().getRoot().getChildren())
		process(child, viewMatrix, Matrix3f::Identity);
	
	glFlush();
	engine.addTask(std::make_unique<RenderTask>(engine));
}

// ----------------------------------------------------------------------------

void RenderSystem::process(const Entity & entity, const Matrix3f & view, const Matrix3f & parentToWorld)
{
	if (entity.hasComponent<TransformComponent>())
	{
		auto const & transformComponent = *entity.getComponent<TransformComponent>();
		const Matrix3f transform = {
			1.f, 0.f, transformComponent.m_position.x,
			0.f, 1.f, transformComponent.m_position.y,
			0.f, 0.f, 1.f
		};

		if (entity.hasComponent<RenderComponent>())
			draw(transformComponent, *entity.getComponent<RenderComponent>(), view, parentToWorld);
		if (entity.hasComponent<TextComponent>())
			draw(transformComponent, *entity.getComponent<TextComponent>(), view, parentToWorld);

		const Matrix3f localToWorld = parentToWorld * transform;
		for (auto const & child : entity.getChildren())
			process(child, view, localToWorld);
	}
}

// ============================================================================

RenderSystem::RenderSystem() :
	System("rndr")
{
}

// ----------------------------------------------------------------------------

void RenderSystem::draw(const TransformComponent & transformComponent, const RenderComponent & renderComponent, const Matrix3f & view, const Matrix3f & parentToWorld)
{
	const Matrix3f transform = {
		1.f, 0.f, transformComponent.m_position.x,
		0.f, 1.f, transformComponent.m_position.y,
		0.f, 0.f, 1.f
	};

	const GLint colorAttrib = renderComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = renderComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = renderComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(renderComponent.m_vertexArrayBuffer);
	const Guard<ShaderProgram> shaderGuard(*renderComponent.m_shader);
	glUniform4fv(colorAttrib, 1, renderComponent.m_color.as4fv().data());
	glUniformMatrix3fv(modelAttrib, 1, true, (parentToWorld * transform).data());
	glUniformMatrix3fv(viewAttribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, renderComponent.m_mesh.m_size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void RenderSystem::draw(const TransformComponent & transformComponent, const TextComponent & textComponent, const Matrix3f & view, const Matrix3f & parentToWorld)
{
	const float scale = static_cast<float>(textComponent.m_size) / Font::getFontImageSize();
	const Matrix3f transform = {
		scale, 0.f, transformComponent.m_position.x,
		0.f, scale, transformComponent.m_position.y,
		0.f, 0.f, 1.f
	};

	const GLint modelAttrib = textComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textComponent.m_shader->getUniformLocation("view");

	const Guard<ShaderProgram> shaderGuard(*textComponent.m_shader);
	glUniformMatrix3fv(modelAttrib, 1, true, (parentToWorld * transform).data());
	glUniformMatrix3fv(viewAttribute, 1, true, view.data());

	textComponent.m_font->render(textComponent.m_text, *textComponent.m_shader);
}

// ============================================================================

RenderSystemNamespace::RenderTask::RenderTask(const Engine & engine) :
	Engine::Task(engine, 64)
{
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::RenderTask::execute() const
{
	m_engine.getSystem<WindowSystem>()->render();
}

// ============================================================================