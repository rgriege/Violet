// ============================================================================

#include <assert.h>

#include "violet/component/component_pool.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

Component_Pool::Func_Table::Func_Table(load_fn _load, save_fn _save, destroy_fn _destroy) :
	load(_load),
	save(_save),
	destroy(_destroy)
{
}

// ============================================================================

Component_Pool::Component_Pool(Component_Pool && other) :
	metadata(other.metadata),
	ftable(std::move(other.ftable)),
	components(std::move(other.components)),
	ids(std::move(other.ids))
{
	other.ids.emplace_back(Handle::Invalid);
}

// ----------------------------------------------------------------------------

Component_Pool & Component_Pool::operator=(Component_Pool && other)
{
	assert(metadata == other.metadata);
	std::swap(ftable, other.ftable);
	std::swap(components, other.components);
	std::swap(ids, other.ids);
	return *this;
}

// ----------------------------------------------------------------------------

Component_Pool::~Component_Pool()
{
	for (u32 i = 0, end = size(); i < end; ++i)
		ftable->destroy(get(i));
}

// ----------------------------------------------------------------------------

void Component_Pool::load(Component_Deserializer & deserializer)
{
	while (deserializer.is_valid())
		ftable->load(*this, deserializer);
}

// ----------------------------------------------------------------------------

void Component_Pool::save(Serializer & serailizer) const
{
	serailizer.write_u32("version", metadata->version);
	for (u32 i = 0, end = size(); i < end; ++i)
	{
		serailizer.write_u32("id", ids[i].id);
		ftable->save(serailizer, get(i));
	}
}

// ----------------------------------------------------------------------------

u32 Component_Pool::save(Serializer & serailizer, const std::vector<Handle> & entity_ids) const
{
	u32 count = 0;
	serailizer.write_u32("version", metadata->version);
	for (u32 i = 0, end = size(); i < end; ++i)
	{
		const Handle id = ids[i];
		if (std::binary_search(entity_ids.cbegin(), entity_ids.cend(), id))
		{
			serailizer.write_u32("id", id.id);
			ftable->save(serailizer, get(i));
			++count;
		}
	}
	return count;
}

// ----------------------------------------------------------------------------

bool Component_Pool::has(const Handle entity_id) const
{
	return get_index(entity_id) != size();
}

// ----------------------------------------------------------------------------

u32 Component_Pool::size() const
{
	return ids.size() - 1;
}

// ----------------------------------------------------------------------------

bool Component_Pool::remove(const Handle entity_id)
{
	assert(entity_id.is_valid());
	const u32 index = get_index(entity_id);
	const bool found = index != size();
	if (found)
	{
		const u32 component_index = index * metadata->size;
		ftable->destroy(&components[component_index]);
		components.erase(components.begin() + component_index, components.begin() + component_index + metadata->size);
		ids.erase(ids.begin() + index);
	}

	return found;
}

// ----------------------------------------------------------------------------

void Component_Pool::clear()
{
	for (u32 i = 0, end = size(); i < end; ++i)
		ftable->destroy(get(i));
	components.clear();
	ids.clear();
	ids.emplace_back(Handle::Invalid);
}

// ============================================================================

Component_Pool::Component_Pool(const Component_Metadata * metadata, std::unique_ptr<Func_Table> && ftable) :
	metadata(metadata),
	ftable(std::move(ftable)),
	components(),
	ids()
{
	ids.emplace_back(Handle::Invalid);
}

// ----------------------------------------------------------------------------

u32 Component_Pool::get_index(const Handle entity_id) const
{
	assert(entity_id.is_valid());

	const auto & idIt = std::lower_bound(ids.begin(), ids.end(), entity_id);
	return idIt != ids.end() && *idIt == entity_id ? static_cast<u32>(std::distance(ids.begin(), idIt)) : size();
}

// ----------------------------------------------------------------------------

void * Component_Pool::get(const u32 index)
{
	return components.data() + index * metadata->size;
}

// ----------------------------------------------------------------------------

const void * Component_Pool::get(const u32 index) const
{
	return components.data() + index * metadata->size;
}

// ----------------------------------------------------------------------------

std::pair<void *, bool>  Component_Pool::insert(const Handle entity_id)
{
	const auto & idIt = std::lower_bound(ids.begin(), ids.end(), entity_id);
	bool const maxId = idIt == ids.end();
	const u32 index = static_cast<u32>(std::distance(ids.begin(), idIt));
	const u32 component_index = index * metadata->size;
	if (!maxId && *idIt == entity_id)
		return std::make_pair(&components[component_index], true);

	ids.insert(idIt, entity_id);
	auto const componentIt = components.insert(components.begin() + component_index, metadata->size, 0);
	return std::make_pair(&*componentIt, false);
}

// ============================================================================
