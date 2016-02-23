// ============================================================================

template <typename T>
void edt::Editor_System::register_command()
{
	register_command(T::get_usage(), &T::parse);
}

// ============================================================================
