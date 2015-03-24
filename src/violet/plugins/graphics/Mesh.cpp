#include "violet/plugins/graphics/Mesh.h"

#include "violet/core/math/Polygon.h"

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

Mesh::~Mesh()
{
	if (m_vertexBuffer != 0)
		glDeleteBuffers(1, &m_vertexBuffer);
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