// ============================================================================

#include "violet/serialization/Serializer.h"

#include <cstring>

// ============================================================================

template <typename Derived>
uint32 Violet::Component::getStaticFlag()
{
	static const uint32 s_flag = getNextFlag();
	return s_flag;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
uint32 Violet::Component::gatherFlags()
{
	return ComponentType::getStaticFlag();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, typename OtherComponentType, typename ... ComponentTypes>
uint32 Violet::Component::gatherFlags()
{
	return gatherFlags<ComponentType>() | gatherFlags<OtherComponentType, ComponentTypes...>();
}

// ============================================================================

template <typename Derived>
uint32 Violet::ComponentBase<Derived>::getStaticFlag()
{
	return Component::getStaticFlag<Derived>();
}

// ============================================================================

template <typename Derived>
Violet::ComponentBase<Derived>::ComponentBase(const EntityId entityId) :
	Component(entityId)
{
}

// ----------------------------------------------------------------------------

template <typename Derived>
Violet::ComponentBase<Derived>::ComponentBase(ComponentBase && other) :
	Component(std::move(other))
{
}

// ============================================================================
