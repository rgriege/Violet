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

template <typename Derived, uint32 Version>
uint32 Violet::ComponentBase<Derived, Version>::getStaticFlag()
{
	return Component::getStaticFlag<Derived>();
}

// ----------------------------------------------------------------------------

template <typename Derived, uint32 Version>
uint32 Violet::ComponentBase<Derived, Version>::getStaticVersion()
{
	return Version;
}

// ============================================================================

template <typename Derived, uint32 Version>
Violet::ComponentBase<Derived, Version>::ComponentBase(const EntityId entityId) :
	Component(entityId)
{
}

// ============================================================================
