// ============================================================================

#include "violet/serialization/Serializer.h"

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
Violet::Tag Violet::ComponentBase<Derived>::getTag() const
{
	return Derived::getStaticTag();
}

// ----------------------------------------------------------------------------

template <typename Derived>
uint32 Violet::ComponentBase<Derived>::getFlag() const
{
	return getStaticFlag();
}

// ----------------------------------------------------------------------------

template <typename Derived>
void Violet::ComponentBase<Derived>::save(Serializer & serializer) const
{
	char tag[5];
	memcpy(tag, Derived::getStaticTag().asString(), 4);
	tag[4] = '\0';
	auto componentSegment = serializer.createSegment(tag);
	*componentSegment << *static_cast<const Derived *>(this);
}

// ============================================================================

template <typename Derived>
Violet::ComponentBase<Derived>::ComponentBase(Entity & owner) :
	Component(owner)
{
}

// ----------------------------------------------------------------------------

template <typename Derived>
Violet::ComponentBase<Derived>::ComponentBase(ComponentBase && other) :
	Component(std::move(other))
{
}

// ============================================================================
