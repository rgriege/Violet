#include "engine/graphics/Mesh.h"

#include "engine/math/Polygon.h"

#include <GL\glew.h>

using namespace Violet;

void Mesh::bind(const Mesh & mesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vertexBuffer);
}

void Mesh::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Mesh(std::vector<Vec2f> && vertices) :
	Mesh(Polygon(std::move(vertices)))
{
}

Mesh::Mesh(Deserializer & deserializer) :
	Mesh(Polygon(deserializer))
{
}

Mesh::Mesh(Mesh && other) :
	m_vertexBuffer(other.m_vertexBuffer),
	m_size(other.m_size)
{
	other.m_vertexBuffer = 0;
}

Mesh & Mesh::operator=(Mesh && other)
{
	std::swap(m_vertexBuffer, other.m_vertexBuffer);
	std::swap(m_size, other.m_size);
	return *this;
}

Mesh::Mesh(const Polygon & poly) :
	m_vertexBuffer(0),
	m_size(poly.m_vertices.size())
{
	glGenBuffers(1, &m_vertexBuffer);
	bind(*this);
	glBufferData(GL_ARRAY_BUFFER, poly.m_vertices.size() * sizeof(Vec2f), poly.m_vertices.data(), GL_STATIC_DRAW);
	unbind();
}

Mesh::~Mesh()
{
	if (m_vertexBuffer != 0)
		glDeleteBuffers(1, &m_vertexBuffer);
}

Serializer & Violet::operator<<(Serializer & serializer, const Mesh & mesh)
{
	std::vector<Vec2f> vertices;
	vertices.resize(mesh.m_size);
	Mesh::bind(mesh);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, mesh.m_size * sizeof(Vec2f), vertices.data());
	Mesh::unbind();
	return serializer << Polygon(std::move(vertices));
}
