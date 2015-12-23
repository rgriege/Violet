// ============================================================================

template <typename T>
void edt::EditorSystem::registerCommand()
{
	registerCommand(T::getUsage(), &T::parse);
}

// ============================================================================
