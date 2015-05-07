#ifndef VIOLET_Component_H
#define VIOLET_Component_H

#include "engine/entity/Entity.h"
#include "engine/utility/Tag.h"

namespace Violet
{
	class VIOLET_API BaseComponent
	{
	public:

		Entity getEntity() const;

		virtual ~BaseComponent() = 0;

	protected:

		BaseComponent(Entity entity);

		BaseComponent(const BaseComponent &) = default;
		BaseComponent & operator=(const BaseComponent &) = default;

		BaseComponent(BaseComponent &&);
		BaseComponent & operator=(BaseComponent &&);

	private:

		Entity m_entity;
	};

	template <typename Derived>
	class Component : public BaseComponent
	{
	public:

		static Tag getTypeId();

	protected:

		Component(Entity entity);
	};
}

#include "engine/component/Component.inl"

#endif