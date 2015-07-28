#ifndef VIOLET_ColorComponent_H
#define VIOLET_ColorComponent_H

#include "engine/component/Component.h"
#include "engine/graphics/component/RenderComponentData.h"

#include "engine/graphics/Color.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API ColorComponent : public ComponentBase<ColorComponent>, public RenderComponentData
	{
	public:

		static Tag getTag();

	public:

		ColorComponent(const Entity & owner, Deserializer & deserializer);
		ColorComponent(const Entity & owner, const Polygon & poly, std::shared_ptr<ShaderProgram> shader, Color color);
		ColorComponent(ColorComponent && other);

	private:

		ColorComponent(const ColorComponent &) = delete;
		ColorComponent & operator=(const ColorComponent &) = delete;

	public:

		Color m_color;
	};

	Deserializer & operator>>(Deserializer & deserializer, ColorComponent & component);
	Serializer & operator<<(Serializer & serializer, const ColorComponent & component);
}

#endif