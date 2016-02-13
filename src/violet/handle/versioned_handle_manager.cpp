// ============================================================================

#include "violet/handle/versioned_handle_manager.h"

using namespace vlt;

// ============================================================================

versioned_handle_manager::versioned_handle_manager() :
	m_manager(),
	m_versions()
{
}

// ----------------------------------------------------------------------------


versioned_handle_manager::~versioned_handle_manager()
{
}

// ----------------------------------------------------------------------------


versioned_handle_manager::versioned_handle_manager(versioned_handle_manager && other) :
	m_manager(std::move(other.m_manager)),
	m_versions(std::move(other.m_versions))
{
}

// ----------------------------------------------------------------------------


versioned_handle_manager & versioned_handle_manager::operator=(versioned_handle_manager && other)
{
	std::swap(m_manager, other.m_manager);
	std::swap(m_versions, other.m_versions);
	return *this;
}

// ----------------------------------------------------------------------------


handle versioned_handle_manager::create()
{
	const handle h = m_manager.create();
	if (h.id >= m_versions.size())
		m_versions.resize(h.id + 1);
	return handle(h.id, m_versions[h.id]);
}

// ----------------------------------------------------------------------------

bool versioned_handle_manager::used(const handle handle) const
{
	return m_manager.used(handle) && m_versions[handle.id] == handle.version;
}

// ----------------------------------------------------------------------------


void versioned_handle_manager::free(const handle entity)
{
	m_manager.free(entity);
	++m_versions[entity.id];
}

// ----------------------------------------------------------------------------


void versioned_handle_manager::freeAll()
{
	m_manager.freeAll();
	m_versions.clear();
}

// ----------------------------------------------------------------------------


std::vector<handle> versioned_handle_manager::getUsed() const
{
	std::vector<handle> result = m_manager.getUsed();
	for (auto & h : result)
		h = handle(h.id, m_versions[h.id]);
	return result;
}

// ----------------------------------------------------------------------------


u16 versioned_handle_manager::get_version(const u16 id) const
{
	return id < m_versions.size() ? m_versions[id] : 0;
}

// ============================================================================
