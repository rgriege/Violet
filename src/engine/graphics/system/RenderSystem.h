#ifndef VIOLET_RenderSystem_H
#define VIOLET_RenderSystem_H

#include "engine/math/Matrix3.h"
#include "engine/system/System.h"
#include "engine/graphics/component/RenderComponent.h"
#include "engine/graphics/component/TextComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;
	class TransformComponent;

	class VIOLET_API RenderSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~RenderSystem() override;
		virtual void update(float dt, const Engine & engine) override;

	private:

		RenderSystem();

		void process(const Entity & entity, const Matrix3f & view, const Matrix3f & localToWorld);
		void draw(const TransformComponent & transformComponent, const RenderComponent & renderComponent, const Matrix3f & view, const Matrix3f & localToWorld);
		void draw(const TransformComponent & transformComponent, const TextComponent & textComponent, const Matrix3f & view, const Matrix3f & localToWorld);
	};
}

#endif