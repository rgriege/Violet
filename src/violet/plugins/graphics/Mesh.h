#ifndef MESH_H
#define MESH_H

#include "violet/core/math/Polygon.h"
#include "violet/plugins/graphics/Config.h"

namespace Violet
{
	class VIOLET_GRAPHICS_API Mesh : public Polygon
	{
	public:

		explicit Mesh(std::vector<Vec2f> && vertices);
		explicit Mesh(Deserializer & deserializer);

	public:

		uint32 m_vertexBuffer;
	};
}

#endif
