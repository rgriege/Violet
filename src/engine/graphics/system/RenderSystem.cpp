// ============================================================================

#include "engine/graphics/system/RenderSystem.h"

#include "engine/Engine.h"
#include "engine/entity/Entity.h"
#include "engine/graphics/component/ColorComponent.h"
#include "engine/graphics/component/TextComponent.h"
#include "engine/graphics/component/TextureComponent.h"
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
	// ----------------------------------------------------------------------------

	class RenderTask : public Engine::Task
	{
	public:

		RenderTask(const Engine & engine);

		virtual void execute() const override;
	};

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
	engine.addTask(std::make_unique<RenderTask>(engine));
}

// ============================================================================

RenderSystem::RenderSystem() :
	System("rndr")
{
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
void RenderSystemNamespace::process(const Entity & entity, const Matrix3f & view, const Matrix3f & parentToWorld, const float opacity)
{
	if (entity.hasComponent<TransformComponent>())
	{
		auto const & transformComponent = *entity.getComponent<TransformComponent>();
		const Matrix3f transform = {
			1.f, 0.f, transformComponent.m_position.x,
			0.f, 1.f, transformComponent.m_position.y,
			0.f, 0.f, 1.f
		};

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
	const Matrix3f transform = {
		1.f, 0.f, transformComponent.m_position.x,
		0.f, 1.f, transformComponent.m_position.y,
		0.f, 0.f, 1.f
	};

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

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const TransformComponent & transformComponent, const TextureComponent & textureComponent, const Matrix3f & view, const Matrix3f & parentToWorld)
{
	const Matrix3f transform = {
		1.f, 0.f, transformComponent.m_position.x,
		0.f, 1.f, transformComponent.m_position.y,
		0.f, 0.f, 1.f
	};

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
