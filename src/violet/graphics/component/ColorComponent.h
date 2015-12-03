#ifndef VIOLET_ColorComponent_H
#define VIOLET_ColorComponent_H

#include "violet/component/Component.h"
#include "violet/graphics/component/RenderComponentData.h"

#include "violet/graphics/Color.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API ColorComponent : public ComponentBase<ColorComponent>, public RenderComponentData
	{
	public:

		static Tag getStaticTag();

	public:

		ColorComponent(Entity & owner, Deserializer & deserializer);
		ColorComponent(Entity & owner, const Polygon & poly, std::shared_ptr<ShaderProgram> shader, Color color);
		ColorComponent(ColorComponent && other);

	private:

		ColorComponent(const ColorComponent &) = delete;
		ColorComponent & operator=(const ColorComponent &) = delete;

	public:

		Color m_color;
	};

	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, ColorComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const ColorComponent & component);
}

#endif