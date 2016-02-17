// ============================================================================

#include "violet/graphics/system/render_system.h"

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/graphics/component/text_component.h"
#include "violet/graphics/component/texture_component.h"
#include "violet/graphics/font/font.h"
#include "violet/graphics/shader/shader.h"
#include "violet/log/Log.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/guard.h"
#include "violet/utility/memory.h"
#include "violet/window/window_system.h"

#include <fstream>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace vlt;

namespace RenderSystemNamespace
{
	// ----------------------------------------------------------------------------

	void draw(const world_transform_component & transformComponent, const color_component & colorComponent, const m4 & view);
	void draw(const world_transform_component & transformComponent, const text_component & textComponent, const m4 & view);
	void draw(const world_transform_component & transformComponent, const texture_component & textureComponent, const m4 & view);

	// ----------------------------------------------------------------------------
}

using namespace RenderSystemNamespace;

// ============================================================================

const char * render_system::get_label_static()
{
	return "rndr";
}

// ----------------------------------------------------------------------------

void render_system::install(system_factory & factory)
{
	factory.assign(get_label_static(), &render_system::init);
}

// ----------------------------------------------------------------------------

static void init_internal(void * mem)
{
	auto c = make_unique<color>(mem);
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		log(formatted_string<1024>().sprintf("glewInit error: %s", glewGetErrorString(glewError)));
		engine::instance().stop();
		return;
	}

	log(formatted_string<32>().sprintf("GL version: %s", glGetString(GL_VERSION)));

	glClearColor(c->r, c->g, c->b, c->a);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	engine::instance().add_system(std::unique_ptr<render_system>(new render_system));
}

void render_system::init(deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());
	color * c = new color(*settingsSegment);
	add_task(init_internal, c, color_component::metadata->thread, task_type::write);
}

// ============================================================================

render_system::render_system() :
	system(get_label_static())
{
}

// ----------------------------------------------------------------------------

static void clear_caches(void *)
{
	shader_program::get_cache().clear();
	font::get_cache().clear();
	texture::get_cache().clear();
}

render_system::~render_system()
{
	add_task(clear_caches, nullptr, color_component::metadata->thread, task_type::del);
}

// ----------------------------------------------------------------------------

static void render(void*)
{
	engine::instance().get_system<window_system>()->render();
}

void update_internal(void *)
{
	auto & e = engine::instance();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const auto & windowSystem = e.get_system<window_system>();
	m4 viewMatrix = m4::Identity;
	viewMatrix[0][0] = 2.f / windowSystem->get_width();
	viewMatrix[1][1] = 2.f / windowSystem->get_height();
	viewMatrix[2][2] = 0.01f;

	for (const auto & entity : e.get_current_scene().get_entity_view<world_transform_component, color_component>())
		draw(entity.get<world_transform_component>(), entity.get<color_component>(), viewMatrix);
	for (const auto & entity : e.get_current_scene().get_entity_view<world_transform_component, texture_component>())
		draw(entity.get<world_transform_component>(), entity.get<texture_component>(), viewMatrix);
	for (const auto & entity : e.get_current_scene().get_entity_view<world_transform_component, text_component>())
		draw(entity.get<world_transform_component>(), entity.get<text_component>(), viewMatrix);

	glFlush();
	// todo: window_system's thread
	add_task(render, nullptr, color_component::metadata->thread, task_type::write);
}

void render_system::update(r32 const /*dt*/)
{
	add_task(update_internal, nullptr, color_component::metadata->thread, task_type::read);
}

// ============================================================================

void RenderSystemNamespace::draw(const world_transform_component & transformComponent, const color_component & colorComponent, const m4 & view)
{
	const m4 & transform = transformComponent.transform;

	const GLint colorAttrib = colorComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = colorComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = colorComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(colorComponent.m_vertexArrayBuffer);
	const guard<shader_program> shaderGuard(*colorComponent.m_shader);
	glUniform4fv(colorAttrib, 1, colorComponent.color.as4fv().data());
	glUniformMatrix4fv(modelAttrib, 1, true, transform.data());
	glUniformMatrix4fv(viewAttribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, colorComponent.m_mesh->m_size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const world_transform_component & transformComponent, const text_component & textComponent, const m4 & view)
{
	const m4 & transform = transformComponent.transform;

	const GLint colorAttrib = textComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = textComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textComponent.m_shader->getUniformLocation("view");

	const guard<shader_program> shaderGuard(*textComponent.m_shader);
	glUniform4fv(colorAttrib, 1, textComponent.color.as4fv().data());
	glUniformMatrix4fv(modelAttrib, 1, true, transform.data());
	glUniformMatrix4fv(viewAttribute, 1, true, view.data());

	textComponent.m_font->render(textComponent.m_text, *textComponent.m_shader);
}

// ----------------------------------------------------------------------------

void RenderSystemNamespace::draw(const world_transform_component & transformComponent, const texture_component & textureComponent, const m4 & view)
{
	const m4 & transform = transformComponent.transform;

	const GLint modelAttrib = textureComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textureComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(textureComponent.m_vertexArrayBuffer);
	const guard<shader_program> shaderGuard(*textureComponent.m_shader);
	const guard<texture> texGuard(*textureComponent.m_texture);
	glUniformMatrix4fv(modelAttrib, 1, true, transform.data());
	glUniformMatrix4fv(viewAttribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, textureComponent.m_mesh->m_size);
	glBindVertexArray(0);
}

// ============================================================================