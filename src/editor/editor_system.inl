// ============================================================================

template <typename T>
void edt::editor_system::register_command()
{
	register_command(T::get_usage(), &T::parse);
}

// ============================================================================
