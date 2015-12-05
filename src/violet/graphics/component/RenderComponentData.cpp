// ============================================================================

#include "violet/graphics/component/RenderComponentData.h"

#include "violet/Engine.h"
#include "violet/graphics/shader/Shader.h"
#include "violet/math/Polygon.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"
#include "violet/utility/Guard.h"

#include <GL/glew.h>

using namespace Violet;

// ============================================================================

RenderComponentData::RenderComponentData(Deserializer & deserializer) :
	m_vertexArrayBuffer(-1),
	m_mesh(),
	m_shader()
{
	const std::string shaderName = deserializer.getString("shader");
	const Polygon poly(deserializer);

	Engine::getInstance().addWriteTask(*this,
		[=](RenderComponentData & data)
		{
			data.m_shader = ShaderProgram::getCache().fetch(shaderName.c_str());
			data.m_mesh = std::make_unique<Mesh>(poly);

			glGenVertexArrays(1, &data.m_vertexArrayBuffer);
			glBindVertexArray(data.m_vertexArrayBuffer);
			const Guard<Mesh> meshGuard(*data.m_mesh);
			const GLint positionAttribute = data.m_shader->getAttributeLocation("position");
			glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(positionAttribute);

			glBindVertexArray(0);
		}, Engine::Thread::Window);
}

// ----------------------------------------------------------------------------

RenderComponentData::RenderComponentData(const Polygon & poly, std::shared_ptr<ShaderProgram> shader) :
	m_vertexArrayBuffer(-1),
	m_mesh(),
	m_shader(std::move(shader))
{
	Engine::getInstance().addWriteTask(*this,
		[=](RenderComponentData & data)
		{
			data.m_mesh = std::make_unique<Mesh>(poly);
			glGenVertexArrays(1, &data.m_vertexArrayBuffer);
			glBindVertexArray(data.m_vertexArrayBuffer);
			const Guard<Mesh> meshGuard(*data.m_mesh);
			const GLint positionAttribute = data.m_shader->getAttributeLocation("position");
			glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(positionAttribute);

			glBindVertexArray(0);
		}, Engine::Thread::Window);
}

// ----------------------------------------------------------------------------

RenderComponentData::RenderComponentData(RenderComponentData && other) :
	m_vertexArrayBuffer(other.m_vertexArrayBuffer),
	m_mesh(std::move(other.m_mesh)),
	m_shader(std::move(other.m_shader))
{
	other.m_vertexArrayBuffer = 0;
}

// ----------------------------------------------------------------------------

RenderComponentData::~RenderComponentData()
{
	if (m_vertexArrayBuffer != 0)
	{
		const auto vertexArrayBuffer = m_vertexArrayBuffer;
		Engine::getInstance().addDeleteTask(std::make_unique<DelegateTask>(
			[=]()
			{
				glDeleteVertexArrays(1, &vertexArrayBuffer);
			}), Engine::Thread::Window);
	}

	const auto mesh = m_mesh.release();
	Engine::getInstance().addDeleteTask(std::make_unique<DelegateTask>(
		[=]()
		{
			delete mesh;
		}), Engine::Thread::Window);
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, RenderComponentData & component)
{
	component.m_shader = ShaderProgram::getCache().fetch(deserializer.getString("shader"));
	deserializer >> *component.m_mesh;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const RenderComponentData & component)
{
	serializer.writeString("shader", component.m_shader->getName().c_str());
	serializer << *component.m_mesh;
	return serializer;
}

// ============================================================================
