// ============================================================================

#include <fstream>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/color_component.h"
#include "violet/graphics/component/text_component.h"
#include "violet/graphics/component/texture_component.h"
#include "violet/graphics/font/font.h"
#include "violet/graphics/shader/shader.h"
#include "violet/graphics/system/render_system.h"
#include "violet/log/log.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/guard.h"
#include "violet/utility/memory.h"
#include "violet/window/window_system.h"

using namespace vlt;

// ============================================================================

static void draw(const World_Transform_Component & tsfm_cpnt, const Color_Component & color_cpnt, const m4 & view);
static void draw(const World_Transform_Component & tsfm_cpnt, const Text_Component & text_cpnt, const m4 & view);
static void draw(const World_Transform_Component & tsfm_cpnt, const Texture_Component & texu_cpnt, const m4 & view);

// ============================================================================

const char * Render_System::get_label_static()
{
	return "rndr";
}

// ----------------------------------------------------------------------------

void Render_System::install(System_Factory & Factory)
{
	Factory.assign(get_label_static(), &Render_System::init);
}

// ----------------------------------------------------------------------------

static void init_internal(void * mem)
{
	auto c = make_unique<Color>(mem);
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		log(Formatted_String<1024>().sprintf("glewInit error: %s", glewGetErrorString(glewError)));
		Engine::instance().stop();
		return;
	}

	log(Formatted_String<32>().sprintf("GL version: %s", glGetString(GL_VERSION)));

	glClearColor(c->r, c->g, c->b, c->a);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Engine::instance().add_system(std::unique_ptr<Render_System>(new Render_System));
}

void Render_System::init(Deserializer & deserializer)
{
	auto settings_segment = deserializer.enter_segment(get_label_static());
	Color * c = new Color(*settings_segment);
	add_task(init_internal, c, Color_Component::metadata->thread, task_type::write);
}

// ============================================================================

Render_System::Render_System() :
	System(get_label_static())
{
}

// ----------------------------------------------------------------------------

static void clear_caches(void *)
{
	Shader_Program::get_cache().clear();
	Font::get_cache().clear();
	Texture::get_cache().clear();
}

Render_System::~Render_System()
{
	add_task(clear_caches, nullptr, Color_Component::metadata->thread, task_type::del);
}

// ----------------------------------------------------------------------------

static void render(void*)
{
	Engine::instance().get_system<Window_System>()->render();
}

void update_internal(void *)
{
	auto & e = Engine::instance();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const auto & window_sys = e.get_system<Window_System>();
	m4 view_mat = m4::Identity;
	view_mat[0][0] = 2.f / window_sys->get_width();
	view_mat[1][1] = 2.f / window_sys->get_height();
	view_mat[2][2] = 0.01f;

	for (const auto & entity : e.get_current_scene().get_entity_view<World_Transform_Component, Color_Component>())
		draw(entity.get<World_Transform_Component>(), entity.get<Color_Component>(), view_mat);
	for (const auto & entity : e.get_current_scene().get_entity_view<World_Transform_Component, Texture_Component>())
		draw(entity.get<World_Transform_Component>(), entity.get<Texture_Component>(), view_mat);
	for (const auto & entity : e.get_current_scene().get_entity_view<World_Transform_Component, Text_Component>())
		draw(entity.get<World_Transform_Component>(), entity.get<Text_Component>(), view_mat);

	glFlush();
	// todo: Window_System's Thread
	add_task(render, nullptr, Color_Component::metadata->thread, task_type::write);
}

void Render_System::update(r32 const /*dt*/)
{
	add_task(update_internal, nullptr, Color_Component::metadata->thread, task_type::read);
}

// ============================================================================

void draw(const World_Transform_Component & tsfm_cpnt, const Color_Component & color_cpnt, const m4 & view)
{
	const m4 & transform = tsfm_cpnt.transform;

	const GLint color_attrib = color_cpnt.shader->get_uniform_loc("color");
	const GLint model_attrib = color_cpnt.shader->get_uniform_loc("model");
	const GLint view_attribute = color_cpnt.shader->get_uniform_loc("view");

	glBindVertexArray(color_cpnt.vertex_array_buffer);
	const Guard<Shader_Program> shader_guard(*color_cpnt.shader);
	glUniform4fv(color_attrib, 1, color_cpnt.color.as4fv().data());
	glUniformMatrix4fv(model_attrib, 1, true, transform.data());
	glUniformMatrix4fv(view_attribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, color_cpnt.mesh->size);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void draw(const World_Transform_Component & tsfm_cpnt, const Text_Component & text_cpnt, const m4 & view)
{
	const m4 & transform = tsfm_cpnt.transform;

	const GLint color_attrib = text_cpnt.shader->get_uniform_loc("color");
	const GLint model_attrib = text_cpnt.shader->get_uniform_loc("model");
	const GLint view_attribute = text_cpnt.shader->get_uniform_loc("view");

	const Guard<Shader_Program> shader_guard(*text_cpnt.shader);
	glUniform4fv(color_attrib, 1, text_cpnt.color.as4fv().data());
	glUniformMatrix4fv(model_attrib, 1, true, transform.data());
	glUniformMatrix4fv(view_attribute, 1, true, view.data());

	text_cpnt.font->render(text_cpnt.text, *text_cpnt.shader);
}

// ----------------------------------------------------------------------------

void draw(const World_Transform_Component & tsfm_cpnt, const Texture_Component & texu_cpnt, const m4 & view)
{
	const m4 & transform = tsfm_cpnt.transform;

	const GLint model_attrib = texu_cpnt.shader->get_uniform_loc("model");
	const GLint view_attribute = texu_cpnt.shader->get_uniform_loc("view");

	glBindVertexArray(texu_cpnt.vertex_array_buffer);
	const Guard<Shader_Program> shader_guard(*texu_cpnt.shader);
	const Guard<Texture> texGuard(*texu_cpnt.texture);
	glUniformMatrix4fv(model_attrib, 1, true, transform.data());
	glUniformMatrix4fv(view_attribute, 1, true, view.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, texu_cpnt.mesh->size);
	glBindVertexArray(0);
}

// ============================================================================
