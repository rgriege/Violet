// ============================================================================

#include <GL/glew.h>

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/texture_component.h"
#include "violet/graphics/shader/shader.h"
#include "violet/math/aabb.h"
#include "violet/math/poly.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/guard.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Texture_Component::metadata;

// ============================================================================

static Vector<v2> create_text_coords_from_mesh(const Mesh & mesh);

// ============================================================================

Texture_Component::Texture_Component(const Handle entity_id, Component_Deserializer & deserializer) :
	Render_Component_Data(deserializer),
	texture(Texture::get_cache().fetch(deserializer.get_string("texture"))),
	tex_coords(std::make_unique<Mesh>(create_text_coords_from_mesh(*mesh)))
{
	glBindVertexArray(vertex_array_buffer);
	const Guard<Mesh> tex_coord_guard(*tex_coords);
	const GLuint tex_coord_attrib = shader->get_attrib_loc("texCoord");
	glEnableVertexAttribArray(tex_coord_attrib);
	glVertexAttribPointer(tex_coord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

Texture_Component::Texture_Component(const Handle entity_id, const Poly & p, std::shared_ptr<Shader_Program> shader, std::shared_ptr<Texture> _texture, const Poly & _tex_coords) :
	Render_Component_Data(p, shader),
	texture(std::move(_texture)),
	tex_coords(std::make_unique<Mesh>(_tex_coords))
{
}

// ----------------------------------------------------------------------------

Texture_Component::Texture_Component(Texture_Component && other) :
	Render_Component_Data(std::move(other)),
	texture(),
	tex_coords(std::move(other.tex_coords))
{
	texture.swap(other.texture);
}

// ============================================================================

void vlt::install_texture_component()
{
	Texture_Component::metadata = init_component_metadata(Tag('t', 'e', 'x', 'u'), 0, sizeof(Texture_Component));
	Scene::install_component<Texture_Component>();
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Texture_Component & component)
{
	operator>>(deserializer, static_cast<Render_Component_Data &>(component));
	component.texture = Texture::get_cache().fetch(deserializer.get_string("texture"));
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Texture_Component & component)
{
	operator<<(serializer, static_cast<const Render_Component_Data &>(component));
	serializer.write_string("texture", component.texture->filename.c_str());
	return serializer;
}

// ============================================================================

Vector<v2> create_text_coords_from_mesh(const Mesh & mesh)
{
	aabb extent;
	const auto poly = mesh.get_poly();
	for (const auto & vertex : poly.vertices)
		extent.extend(vertex);
	const v2 & minimum = extent.get_min();
	const v2 & dimension = extent.get_max() - minimum;

	Vector<v2> result;
	result.reserve(poly.vertices.size());
	for (const v2 & vertex : poly.vertices)
		result.emplace_back(v2((vertex.x - minimum.x) / dimension.x, (vertex.y - minimum.y) / dimension.y));
	return result;
}

	// ============================================================================
