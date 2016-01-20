#ifndef VIOLET_Component_H
#define VIOLET_Component_H

#include "violet/Defines.h"
#include "violet/utility/Tag.h"

#include <utility>

namespace Violet
{
	class Serializer;

	class VIOLET_API Component
	{
	public:

		template <typename ComponentType>
		static uint32 gatherFlags();
		template <typename ComponentType, typename OtherComponentType, typename ... ComponentTypes>
		static uint32 gatherFlags();

	protected:

		template <typename Derived>
		static uint32 getStaticFlag();

	private:

		static uint32 getNextFlag();

	public:

		EntityId getEntityId() const;
		
	protected:

		Component(EntityId entityId);

	protected:

		EntityId m_entityId;
	};

	template <typename Derived, uint32 Version>
	class ComponentBase : public Component
	{
	public:

		static uint32 getStaticFlag();
		static uint32 getStaticVersion();

	protected:

		ComponentBase(EntityId entityId);
	};
}

#include "violet/component/Component.inl"

#endif
