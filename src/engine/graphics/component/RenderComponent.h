#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "engine/component/Component.h"

#include "engine/graphics/Mesh.h"
#include "engine/graphics/Color.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API RenderComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		RenderComponent(const Entity & entity, Deserializer & deserializer);
		RenderComponent(const Entity & entity, const Polygon & poly, Color color, std::shared_ptr<ShaderProgram> shader);
		RenderComponent(RenderComponent && other);
		RenderComponent & operator=(RenderComponent &&);
		RenderComponent(const RenderComponent &) = delete;
		virtual ~RenderComponent();

	public:

		uint32 m_vertexArrayBuffer;
		Mesh m_mesh;
		Color m_color;
		std::shared_ptr<ShaderProgram> m_shader;
	};

	Serializer & operator<<(Serializer & serializer, const RenderComponent & component);
}

#endif