#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "violet/core/component/Component.h"

#include "violet/core/math/Polygon.h"
#include "violet/plugins/graphics/Color.h"

#ifdef WIN32
#ifdef VIOLETGRAPHICS_EXPORT
#define VIOLET_GRAPHICS_API __declspec(dllexport)
#else
#define VIOLET_GRAPHICS_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

namespace Violet
{
	class Deserializer;

	class VIOLET_GRAPHICS_API RenderComponent : public Component
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