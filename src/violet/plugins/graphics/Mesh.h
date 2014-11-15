#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "violet/core/math/Vec2.h"
#include "violet/core/math/Interval.h"

namespace Violet
{
	class Mesh
	{
	public:

		explicit Mesh(std::vector<Vec2f> vertices);
		explicit Mesh(Deserializer & deserializer);

	public:

		std::vector<Vec2f> m_vertices;
	};
}

#endif
