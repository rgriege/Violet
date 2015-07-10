#ifndef VIOLET_RenderComponent_H
#define VIOLET_RenderComponent_H

#include "engine/component/Component.h"

#include "engine/graphics/Mesh.h"
#include "engine/graphics/Color.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API RenderComponent : public ComponentBase<RenderComponent>
	{
	public:

		static Tag getTag();

	public:

		RenderComponent(const Entity & owner, Deserializer & deserializer);
		RenderComponent(const Entity & owner, const Polygon & poly, Color color, std::shared_ptr<ShaderProgram> shader);
		RenderComponent(RenderComponent && other);
		//RenderComponent & operator=(RenderComponent &&);
		RenderComponent(const RenderComponent &) = delete;
		RenderComponent & operator=(const RenderComponent &) = delete;
		virtual ~RenderComponent();

	public:

		uint32 m_vertexArrayBuffer;
		Mesh m_mesh;
		Color m_color;
		std::shared_ptr<ShaderProgram> m_shader;
	};

	Deserializer & operator>>(Deserializer & deserializer, RenderComponent & component);
	Serializer & operator<<(Serializer & serializer, const RenderComponent & component);
}

#endif