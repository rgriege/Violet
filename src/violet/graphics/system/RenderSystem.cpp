// ============================================================================

#include "violet/graphics/system/RenderSystem.h"

#include "violet/Engine.h"
#include "violet/entity/Entity.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/graphics/component/TextureComponent.h"
#include "violet/scene/SceneProcessor.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/TransformComponent.h"
#include "violet/utility/Guard.h"
#include "violet/window/WindowSystem.h"
#include "violet/graphics/font/Font.h"
#include "violet/graphics/shader/Shader.h"

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace Violet;

namespace RenderSystemNamespace
{
	// ----------------------------------------------------------------------------

	void process(const Entity & entity, const Matrix3f & view, const Matrix3f & localToWorld, float opacity);
	void draw(const TransformComponent & transformComponent, const ColorComponent & colorComponent, const Matrix3f & view, const Matrix3f & localToWorld, float opacity);
	void draw(const TransformComponent & transformComponent, const TextComponent & textComponent, const Matrix3f & view, const Matrix3f & localToWorld);
	void draw(const TransformComponent & transformComponent, const TextureComponent & textureComponent, const Matrix3f & view, const Matrix3f & localToWorld);

	// ----------------------------------------------------------------------------
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
		process(*child, viewMatrix, Matrix3f::Identity, 1.f);
	
	glFlush();
	engine.addWriteTask(engine, [](Engine & engine) { engine.getSystem<WindowSystem>()->render(); });
}

// ============================================================================

RenderSystem::RenderSystem() :
	System("rndr")
{
}

// ============================================================================

void RenderSystemNamespace::process(const Entity & entity, const Matrix3f & view, const Matrix3f & parentToWorld, const float opacity)
{
	if (entity.hasComponent<TransformComponent>())
	{
		auto const & transformComponent = *entity.getComponent<TransformComponent>();
		const Matrix3f & transform = transformComponent.m_transform;

		float newOpacity = opacity;
		if ((entity.getComponentFlags() & Component::gatherFlags<ColorComponent, TextComponent, TextureComponent>()) != 0)
		{
			if (entity.hasComponent<ColorComponent>())
			{
				const auto & colorComponent = *entity.getComponent<ColorComponent>();
				newOpacity = opacity * colorComponent.m_color.a.asFloat();
				draw(transformComponent, colorComponent, view, parentToWorld, newOpacity);
			}
			if (entity.hasComponent<TextureComponent>())
				draw(transformComponent, *entity.getComponent<TextureComponent>(), view, parentToWorld);
			if (entity.hasComponent<TextComponent>())
				draw(transformComponent, *entity.getComponent<TextComponent>(), view, parentToWorld);
		}

		const Matrix3f localToWorld = parentToWorld * transform;
		for (auto const & child : entity.getChildren())
			process(*child, view, localToWorld, newOpacity);
	}
}

// ----------------------------------------------------------------------------


void RenderSystemNamespace::draw(const TransformComponent & transformComponent, const ColorComponent & colorComponent, const Matrix3f & view, const Matrix3f & parentToWorld, const float opacity)
{
	const Matrix3f & transform = transformComponent.m_transform;

	const GLint colorAttrib = colorComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = colorComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = colorComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(colorComponent.m_vertexArrayBuffer);
	const Guard<ShaderProgram> shaderGuard(*colorComponent.m_shader);
	Color color = colorComponent.m_color;
	color.a = color.a.asFloat() * opacity;
	glUniform4fv(colorAttrib, 1, color.as4fv().data());
	glUniformMatrix3fv(modelAttrib, 1, true, (parentToWorld * transform).data());
	glUniformMatrix3fv(viewAttribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, colorComponent.m_mesh.m_size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const TransformComponent & transformComponent, const TextComponent & textComponent, const Matrix3f & view, const Matrix3f & parentToWorld)
{
	const Matrix3f & transform = transformComponent.m_transform;

	const GLint modelAttrib = textComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textComponent.m_shader->getUniformLocation("view");

	const Guard<ShaderProgram> shaderGuard(*textComponent.m_shader);
	glUniformMatrix3fv(modelAttrib, 1, true, (parentToWorld * transform).data());
	glUniformMatrix3fv(viewAttribute, 1, true, view.data());

	textComponent.m_font->render(textComponent.m_text, *textComponent.m_shader);
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const TransformComponent & transformComponent, const TextureComponent & textureComponent, const Matrix3f & view, const Matrix3f & parentToWorld)
{
	const Matrix3f & transform = transformComponent.m_transform;

	const GLint modelAttrib = textureComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textureComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(textureComponent.m_vertexArrayBuffer);
	const Guard<ShaderProgram> shaderGuard(*textureComponent.m_shader);
	const Guard<Texture> texGuard(*textureComponent.m_texture);
	glUniformMatrix3fv(modelAttrib, 1, true, (parentToWorld * transform).data());
	glUniformMatrix3fv(viewAttribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, textureComponent.m_mesh.m_size);
	glBindVertexArray(0);
}

// ============================================================================
