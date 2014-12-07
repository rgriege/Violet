#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "violet/core/component/Component.h"

#include "violet/core/math/Polygon.h"
#include "violet/plugins/graphics/Color.h"

namespace Violet
{
	class Deserializer;

	class RenderComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		RenderComponent(const Entity & entity, Deserializer & deserializer);
		RenderComponent(RenderComponent && other);
		RenderComponent(const RenderComponent &) = delete;

	public:

		Polygon m_mesh;
		Color m_color;
	};
}

#endif