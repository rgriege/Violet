// ============================================================================

#include "violet/handle/versioned_handle_manager.h"

using namespace vlt;

// ============================================================================

Versioned_Handle_Manager::Versioned_Handle_Manager() :
	manager(),
	versions()
{
}

// ----------------------------------------------------------------------------

Handle Versioned_Handle_Manager::create()
{
	const Handle h = manager.create();
	if (h.id >= versions.size())
		versions.resize(h.id + 1);
	return Handle(h.id, versions[h.id]);
}

// ----------------------------------------------------------------------------

bool Versioned_Handle_Manager::used(const Handle handle) const
{
	return manager.used(handle) && versions[handle.id] == handle.version;
}

// ----------------------------------------------------------------------------


void Versioned_Handle_Manager::free(const Handle handle)
{
	manager.free(handle);
	++versions[handle.id];
}

// ----------------------------------------------------------------------------


void Versioned_Handle_Manager::free_all()
{
	manager.free_all();
	versions.clear();
}

// ----------------------------------------------------------------------------


std::vector<Handle> Versioned_Handle_Manager::get_used() const
{
	std::vector<Handle> result = manager.get_used();
	for (auto & h : result)
		h = Handle(h.id, versions[h.id]);
	return result;
}

// ----------------------------------------------------------------------------


u16 Versioned_Handle_Manager::get_version(const u16 id) const
{
	return id < versions.size() ? versions[id] : 0;
}

// ============================================================================
