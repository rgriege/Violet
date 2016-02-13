// ============================================================================

template <typename Derived>
u32 vlt::component::get_flag()
{
	static const u32 flag = get_next_flag();
	return flag;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
u32 vlt::component::gather_flags()
{
	return ComponentType::get_flag();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, typename OtherComponentType, typename ... ComponentTypes>
u32 vlt::component::gather_flags()
{
	return gather_flags<ComponentType>() | gather_flags<OtherComponentType, ComponentTypes...>();
}

// ============================================================================

template <typename Derived, u32 Version>
u32 vlt::component_base<Derived, Version>::get_flag()
{
	return component::get_flag<Derived>();
}

// ----------------------------------------------------------------------------

template <typename Derived, u32 Version>
u32 vlt::component_base<Derived, Version>::get_version()
{
	return Version;
}

// ============================================================================

