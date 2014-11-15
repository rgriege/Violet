#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "violet/plugins/graphics/Mesh.h"
#include "violet/plugins/graphics/Color.h"

#include <memory>

namespace Violet
{
	class Deserializer;

	class RenderComponent
	{
	public:

		RenderComponent(Deserializer & deserializer);

	public:

		Mesh m_mesh;
		Color m_color;
	};
}

#endif