// ============================================================================

template <size_t MaxId, size_t MaxVersion>
const Violet::Handle<MaxId, MaxVersion> Violet::Handle<MaxId, MaxVersion>::ms_invalid;

// ============================================================================

template <size_t MaxId, size_t MaxVersion>
Violet::Handle<MaxId, MaxVersion>::Handle() :
    Handle(~0, ~0)
{
	static_assert(StorageSize <= sizeof(long long unsigned) * 8, "Exceeded storage size");
}

// ----------------------------------------------------------------------------

template <size_t MaxId, size_t MaxVersion>
Violet::Handle<MaxId, MaxVersion>::Handle(const StorageType id, const StorageType version) :
	m_id(id),
	m_version(version)
{
}

// ----------------------------------------------------------------------------

template <size_t MaxId, size_t MaxVersion>
typename Violet::Handle<MaxId, MaxVersion>::StorageType Violet::Handle<MaxId, MaxVersion>::getId() const
{
	return m_id;
}

// ----------------------------------------------------------------------------

template <size_t MaxId, size_t MaxVersion>
typename Violet::Handle<MaxId, MaxVersion>::StorageType Violet::Handle<MaxId, MaxVersion>::getVersion() const
{
	return m_version;
}

// ----------------------------------------------------------------------------

template <size_t MaxId, size_t MaxVersion>
typename Violet::Handle<MaxId, MaxVersion>::StorageType Violet::Handle<MaxId, MaxVersion>::getRaw() const
{
	return ((m_id << VersionStorageSize) | m_version);
}

// ----------------------------------------------------------------------------

template <size_t MaxId, size_t MaxVersion>
bool Violet::Handle<MaxId, MaxVersion>::isValid() const
{
	return getId() != MaxId - 1;
}

// ============================================================================

template <size_t MaxId, size_t MaxVersion>
bool Violet::operator<(const Handle<MaxId, MaxVersion> lhs, const Handle<MaxId, MaxVersion> rhs)
{
	return lhs.getRaw() < rhs.getRaw();
}

// ----------------------------------------------------------------------------

template <size_t MaxId, size_t MaxVersion>
bool Violet::operator==(const Handle<MaxId, MaxVersion> lhs, const Handle<MaxId, MaxVersion> rhs)
{
	return lhs.getId() == rhs.getId() && lhs.getVersion() == rhs.getVersion();
}

// ----------------------------------------------------------------------------

template <size_t MaxId, size_t MaxVersion>
bool Violet::operator!=(const Handle<MaxId, MaxVersion> lhs, const Handle<MaxId, MaxVersion> rhs)
{
	return !(lhs == rhs);
}

// ============================================================================
