// ============================================================================

template <typename Handle>
Violet::VersionedHandleManager<Handle>::VersionedHandleManager() :
	m_manager(),
	m_versions()
{
}

// ----------------------------------------------------------------------------

template <typename Handle>
Violet::VersionedHandleManager<Handle>::~VersionedHandleManager()
{
}

// ----------------------------------------------------------------------------

template <typename Handle>
Violet::VersionedHandleManager<Handle>::VersionedHandleManager(VersionedHandleManager && other) :
	m_manager(std::move(other.m_manager)),
	m_versions(std::move(other.m_versions))
{
}

// ----------------------------------------------------------------------------

template <typename Handle>
Violet::VersionedHandleManager<Handle> & Violet::VersionedHandleManager<Handle>::operator=(VersionedHandleManager && other)
{
	std::swap(m_manager, other.m_manager);
	std::swap(m_versions, other.m_versions);
	return *this;
}

// ----------------------------------------------------------------------------

template <typename Handle>
Handle Violet::VersionedHandleManager<Handle>::create()
{
	const Handle handle = m_manager.create();
	if (handle.getId() >= m_versions.size())
		m_versions.resize(handle.getId() + 1);
	return Handle(handle.getId(), m_version[handle.getId()]);
}

// ----------------------------------------------------------------------------

template <typename Handle>
Handle Violet::VersionedHandleManager<Handle>::create(const typename Handle::StorageType desiredId)
{
	const Handle handle = m_manager.create(desiredId);
	if (handle.getId() >= m_versions.size())
		m_versions.resize(handle.getId() + 1);
	return Handle(handle.getId(), m_versions[handle.getId()]);
}

// ----------------------------------------------------------------------------

template <typename Handle>
void Violet::VersionedHandleManager<Handle>::free(const Handle entity)
{
	m_manager.free(entity);
	++m_versions[entity.getId()];
}

// ----------------------------------------------------------------------------

template <typename Handle>
void Violet::VersionedHandleManager<Handle>::freeAll()
{
	m_manager.freeAll();
	m_versions.clear();
}

// ----------------------------------------------------------------------------

template <typename Handle>
std::vector<Handle> Violet::VersionedHandleManager<Handle>::getUsed() const
{
	std::vector<Handle> result = m_manager.getUsed();
	for (auto & handle : result)
		handle = Handle(handle.getId(), m_versions[handle.getId()]);
	return result;
}

// ============================================================================
