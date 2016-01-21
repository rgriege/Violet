// ============================================================================

#include "violet/graphics/system/RenderSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/graphics/component/TextureComponent.h"
#include "violet/log/Log.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/Guard.h"
#include "violet/window/WindowSystem.h"
#include "violet/graphics/font/Font.h"
#include "violet/graphics/shader/Shader.h"

#include <fstream>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace Violet;

namespace RenderSystemNamespace
{
	// ----------------------------------------------------------------------------

	void draw(const WorldTransformComponent & transformComponent, const ColorComponent & colorComponent, const Matrix4f & view);
	void draw(const WorldTransformComponent & transformComponent, const TextComponent & textComponent, const Matrix4f & view);
	void draw(const WorldTransformComponent & transformComponent, const TextureComponent & textureComponent, const Matrix4f & view);

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

void RenderSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());

	Color const color(*settingsSegment);

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[=](Engine & engine)
		{
			GLenum glewError = glewInit();
			if (glewError != GLEW_OK)
			{
                Log::log(FormattedString<1024>().sprintf("glewInit error: %s", glewGetErrorString(glewError)));
				engine.stop();
				return;
			}

            Log::log(FormattedString<32>().sprintf("GL version: %s", glGetString(GL_VERSION)));
	
			glClearColor(color.r, color.g, color.b, color.a);

			glEnable(GL_DEPTH_TEST);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
			engine.addSystem(std::unique_ptr<RenderSystem>(new RenderSystem));
		}, Thread::Window);
}

// ============================================================================

RenderSystem::~RenderSystem()
{
	Engine::getInstance().addDeleteTask(std::make_unique<DelegateTask>(
		[]()
		{
			ShaderProgram::getCache().clear();
			Font::getCache().clear();
		}), Thread::Window);
}

// ----------------------------------------------------------------------------

void RenderSystem::update(float const /*dt*/)
{
	Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
		[]()
		{
			const auto & engine = Engine::getInstance();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			const auto & windowSystem = engine.getSystem<WindowSystem>();
			Matrix4f viewMatrix = Matrix4f::Identity;
			viewMatrix[0][0] = 2.f / windowSystem->getWidth();
			viewMatrix[1][1] = 2.f / windowSystem->getHeight();
			viewMatrix[2][2] = 0.01f;

			for (const auto entity : engine.getCurrentScene().getEntityView<WorldTransformComponent, ColorComponent>())
				draw(std::get<0>(entity), std::get<1>(entity), viewMatrix);
			for (const auto entity : engine.getCurrentScene().getEntityView<WorldTransformComponent, TextureComponent>())
				draw(std::get<0>(entity), std::get<1>(entity), viewMatrix);
			for (const auto entity : engine.getCurrentScene().getEntityView<WorldTransformComponent, TextComponent>())
				draw(std::get<0>(entity), std::get<1>(entity), viewMatrix);

			glFlush();
			engine.addWriteTask(engine, [](Engine & engine) { engine.getSystem<WindowSystem>()->render(); }, Thread::Window);
		}), Thread::Window);
}

// ============================================================================

RenderSystem::RenderSystem() :
	System("rndr")
{
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const WorldTransformComponent & transformComponent, const ColorComponent & colorComponent, const Matrix4f & view)
{
	const Matrix4f & transform = transformComponent.m_transform;

	const GLint colorAttrib = colorComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = colorComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = colorComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(colorComponent.m_vertexArrayBuffer);
	const Guard<ShaderProgram> shaderGuard(*colorComponent.m_shader);
	glUniform4fv(colorAttrib, 1, colorComponent.m_color.as4fv().data());
	glUniformMatrix4fv(modelAttrib, 1, true, transform.data());
	glUniformMatrix4fv(viewAttribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, colorComponent.m_mesh->m_size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const WorldTransformComponent & transformComponent, const TextComponent & textComponent, const Matrix4f & view)
{
	const Matrix4f & transform = transformComponent.m_transform;

	const GLint colorAttrib = textComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = textComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textComponent.m_shader->getUniformLocation("view");

	const Guard<ShaderProgram> shaderGuard(*textComponent.m_shader);
	glUniform4fv(colorAttrib, 1, textComponent.m_color.as4fv().data());
	glUniformMatrix4fv(modelAttrib, 1, true, transform.data());
	glUniformMatrix4fv(viewAttribute, 1, true, view.data());

	textComponent.m_font->render(textComponent.m_text, *textComponent.m_shader);
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const WorldTransformComponent & transformComponent, const TextureComponent & textureComponent, const Matrix4f & view)
{
	const Matrix4f & transform = transformComponent.m_transform;

	const GLint modelAttrib = textureComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textureComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(textureComponent.m_vertexArrayBuffer);
	const Guard<ShaderProgram> shaderGuard(*textureComponent.m_shader);
	const Guard<Texture> texGuard(*textureComponent.m_texture);
	glUniformMatrix4fv(modelAttrib, 1, true, transform.data());
	glUniformMatrix4fv(viewAttribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, textureComponent.m_mesh->m_size);
	glBindVertexArray(0);
}

// ============================================================================
