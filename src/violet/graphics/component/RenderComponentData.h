#ifndef VIOLET_RenderComponentData_H
#define VIOLET_RenderComponentData_H

#include "violet/graphics/Mesh.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API RenderComponentData
	{
	protected:

		RenderComponentData(Deserializer & deserializer);
		RenderComponentData(const Polygon & poly, std::shared_ptr<ShaderProgram> shader);
		RenderComponentData(RenderComponentData && other);
		~RenderComponentData();

	private:

		RenderComponentData(const RenderComponentData &) = delete;
		RenderComponentData & operator=(const RenderComponentData &) = delete;

	public:

		uint32 m_vertexArrayBuffer;
		std::unique_ptr<Mesh> m_mesh;
		std::shared_ptr<ShaderProgram> m_shader;
	};

	Deserializer & operator>>(Deserializer & deserializer, RenderComponentData & component);
	Serializer & operator<<(Serializer & serializer, const RenderComponentData & component);
}

#endif