// ============================================================================

#include "engine/graphics/system/RenderSystem.h"

#include "engine/Engine.h"
#include "engine/entity/Entity.h"
#include "engine/math/Matrix3.h"
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

// ============================================================================

namespace RenderSystemNamespace
{
	Matrix3f ms_viewMatrix;
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

// ----------------------------------------------------------------------------

void RenderSystem::update(float const dt, Engine & engine)
{
	auto windowSystem = engine.fetch<WindowSystem>();
	ms_viewMatrix = Matrix3f::Identity;
	ms_viewMatrix[0][0] = 2.f / windowSystem->getWidth();
	ms_viewMatrix[1][1] = 2.f / windowSystem->getHeight();

	for (auto & component : engine.getCurrentScene().getView<RenderComponent>())
		draw(get<RenderComponent&>(component), engine);
	for (auto & component : engine.getCurrentScene().getView<TextComponent>())
		draw(get<TextComponent&>(component), engine);

	glFlush();
	windowSystem->render();
}

// ----------------------------------------------------------------------------

void RenderSystem::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

// ============================================================================

void RenderSystem::draw(RenderComponent & renderComponent, Engine & engine)
{
	const TransformComponent * transform = engine.getCurrentScene().getComponent<TransformComponent>(renderComponent.getEntity());
	const float modelMat[9] = {
		1.f, 0.f, transform->m_position.x,
		0.f, 1.f, transform->m_position.y,
		0.f, 0.f, 1.f
	};

	const GLint colorAttrib = renderComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = renderComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = renderComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(renderComponent.m_vertexArrayBuffer);
	const Guard<ShaderProgram> shaderGuard(*renderComponent.m_shader);
	glUniform4fv(colorAttrib, 1, renderComponent.m_color.as4fv().data());
	glUniformMatrix3fv(modelAttrib, 1, true, modelMat);
	glUniformMatrix3fv(viewAttribute, 1, true, ms_viewMatrix.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, renderComponent.m_mesh.m_size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void RenderSystem::draw(TextComponent & textComponent, Engine & engine)
{
	const TransformComponent * transform = engine.getCurrentScene().getComponent<TransformComponent>(textComponent.getEntity());
	const float scale = static_cast<float>(textComponent.m_size) / Font::getFontImageSize();
	float modelMat[9] = {
		scale, 0.f, transform->m_position.x,
		0.f, scale, transform->m_position.y,
		0.f, 0.f, 1.f
	};

	const GLint modelAttrib = textComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textComponent.m_shader->getUniformLocation("view");

	const Guard<ShaderProgram> shaderGuard(*textComponent.m_shader);
	glUniformMatrix3fv(modelAttrib, 1, true, modelMat);
	glUniformMatrix3fv(viewAttribute, 1, true, ms_viewMatrix.data());

	textComponent.m_font->render(textComponent.m_text, *textComponent.m_shader);
}

// ----------------------------------------------------------------------------

RenderSystem::RenderSystem() :
	System("rndr")
{
}

// ============================================================================
