// ============================================================================

template <typename Derived>
uint32 Violet::Component::getFlag()
{
	static const uint32 s_flag = getNextFlag();
	return s_flag;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
uint32 Violet::Component::gatherFlags()
{
	return ComponentType::getFlag();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, typename OtherComponentType, typename ... ComponentTypes>
uint32 Violet::Component::gatherFlags()
{
	return gatherFlags<ComponentType>() | gatherFlags<OtherComponentType, ComponentTypes...>();
}

// ============================================================================

template <typename Derived>
uint32 Violet::ComponentBase<Derived>::getFlag()
{
	return Component::getFlag<Derived>();
}

// ============================================================================

template <typename Derived>
Violet::Tag Violet::ComponentBase<Derived>::getTag() const
{
	return Derived::getStaticTag();
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
