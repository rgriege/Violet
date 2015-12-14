// ============================================================================

#include "violet/graphics/Mesh.h"

#include "violet/math/Polygon.h"

#include <GL/glew.h>

using namespace Violet;

// ============================================================================

namespace MeshNamespace
{
	void setVertices(Mesh & mesh, const Polygon & poly);
}

using namespace MeshNamespace;

// ============================================================================

void Mesh::bind(const Mesh & mesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vertexBuffer);
}

// ----------------------------------------------------------------------------

void Mesh::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ============================================================================

Mesh::Mesh(Vector<Vec2f> && vertices) :
	Mesh(Polygon(std::move(vertices)))
{
}

// ----------------------------------------------------------------------------

Mesh::Mesh(Deserializer & deserializer) :
	Mesh(Polygon(deserializer))
{
}

// ----------------------------------------------------------------------------

Mesh::Mesh(const Mesh & other) :
	Mesh(other.getPolygon())
{
}

// ----------------------------------------------------------------------------

Mesh::Mesh(Mesh && other) :
	m_vertexBuffer(other.m_vertexBuffer),
	m_size(other.m_size)
{
	other.m_vertexBuffer = 0;
}

// ----------------------------------------------------------------------------

Mesh & Mesh::operator=(Mesh && other)
{
	std::swap(m_vertexBuffer, other.m_vertexBuffer);
	std::swap(m_size, other.m_size);
	return *this;
}

// ----------------------------------------------------------------------------

Mesh::Mesh(const Polygon & poly) :
	m_vertexBuffer(0),
	m_size(poly.m_vertices.size())
{
	glGenBuffers(1, &m_vertexBuffer);
	setVertices(*this, poly);
}

// ----------------------------------------------------------------------------

Mesh::~Mesh()
{
	if (m_vertexBuffer != 0)
		glDeleteBuffers(1, &m_vertexBuffer);
}

// ----------------------------------------------------------------------------

Polygon Mesh::getPolygon() const
{
	Vector<Vec2f> vertices;
	vertices.resize(m_size);
	Mesh::bind(*this);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_size * sizeof(Vec2f), vertices.data());
	Mesh::unbind();
	return Polygon(std::move(vertices));
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, Mesh & mesh)
{
	Polygon poly(deserializer);
	setVertices(mesh, poly);
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const Mesh & mesh)
{
	return serializer << mesh.getPolygon();
}

// ============================================================================

void MeshNamespace::setVertices(Mesh & mesh, const Polygon & poly)
{

	Mesh::bind(mesh);
	glBufferData(GL_ARRAY_BUFFER, poly.m_vertices.size() * sizeof(Vec2f), poly.m_vertices.data(), GL_STATIC_DRAW);
	Mesh::unbind();
}

// ============================================================================
