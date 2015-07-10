#ifndef VIOLET_Component_H
#define VIOLET_Component_H

#include "engine/utility/Tag.h"

namespace Violet
{
	class Entity;

	class VIOLET_API Component
	{
	public:

		template <typename Derived>
		static uint32 getFlag();

		template <typename ComponentType>
		static uint32 gatherFlags();
		template <typename ComponentType, typename OtherComponentType, typename ... ComponentTypes>
		static uint32 gatherFlags();

	private:

		static uint32 getNextFlag();

	public:

		const Entity & getOwner() const;

		virtual Tag getTag() const = 0;

		virtual ~Component() = 0;

	protected:

		Component(const Entity & owner);

		Component(const Component &) = delete;
		Component & operator=(const Component &) = delete;

		Component(Component &&);
		// Component & operator=(Component &&);

	private:

		const Entity & m_owner;
	};

	template <typename Derived>
	class ComponentBase : public Component
	{
	public:

		static uint32 getFlag();

	public:

		virtual Tag getTag() const override;

	protected:

		ComponentBase(const Entity & owner);
		ComponentBase(ComponentBase && other);
	};
}

#include "engine/component/Component.inl"

#endif