// ============================================================================

template <typename Derived>
Violet::Tag Violet::Component<Derived>::getTypeId()
{
	return Derived::getTypeId();
}

// ============================================================================

template <typename Derived>
Violet::Component<Derived>::Component(const Entity entity) :
	BaseComponent(entity)
{
}

// ============================================================================
