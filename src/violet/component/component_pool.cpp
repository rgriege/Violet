// ============================================================================

#include <assert.h>

#include "violet/component/component_pool.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

component_pool::func_table::func_table(load_fn _load, save_fn _save, destroy_fn _destroy) :
	load(_load),
	save(_save),
	destroy(_destroy)
{
}

// ============================================================================

component_pool::component_pool(component_pool && other) :
	component_tag(other.component_tag),
	component_size(other.component_size),
	component_version(other.component_version),
	ftable(std::move(other.ftable)),
	components(std::move(other.components)),
	ids(std::move(other.ids))
{
	other.ids.emplace_back(handle::Invalid);
}

// ----------------------------------------------------------------------------

component_pool & component_pool::operator=(component_pool && other)
{
	assert(component_tag == other.component_tag);
	assert(component_size == other.component_size);
	assert(component_version == other.component_version);
	std::swap(ftable, other.ftable);
	std::swap(components, other.components);
	std::swap(ids, other.ids);
	return *this;
}

// ----------------------------------------------------------------------------

component_pool::~component_pool()
{
	for (u32 i = 0, end = size(); i < end; ++i)
		ftable->destroy(get(i));
}

// ----------------------------------------------------------------------------

tag component_pool::get_tag() const
{
	return component_tag;
}

// ----------------------------------------------------------------------------

void component_pool::load(component_deserializer & deserializer)
{
	while (deserializer.is_valid())
		ftable->load(*this, deserializer);
}

// ----------------------------------------------------------------------------

void component_pool::save(serializer & serailizer) const
{
	serailizer.write_u32("version", component_version);
	for (u32 i = 0, end = size(); i < end; ++i)
	{
		serailizer.write_u32("id", ids[i].id);
		ftable->save(serailizer, get(i));
	}
}

// ----------------------------------------------------------------------------

u32 component_pool::save(serializer & serailizer, const std::vector<handle> & entityIds) const
{
	u32 count = 0;
	serailizer.write_u32("version", component_version);
	for (u32 i = 0, end = size(); i < end; ++i)
	{
		const handle id = ids[i];
		if (std::binary_search(entityIds.cbegin(), entityIds.cend(), id))
		{
			serailizer.write_u32("id", id.id);
			ftable->save(serailizer, get(i));
			++count;
		}
	}
	return count;
}

// ----------------------------------------------------------------------------

bool component_pool::has(const handle entity_id) const
{
	return get_index(entity_id) != size();
}

// ----------------------------------------------------------------------------

u32 component_pool::size() const
{
	return ids.size() - 1;
}

// ----------------------------------------------------------------------------

bool component_pool::remove(const handle entity_id)
{
	assert(entity_id.is_valid());
	const u32 index = get_index(entity_id);
	const bool found = index != size();
	if (found)
	{
		const u32 component_index = index * component_size;
		ftable->destroy(&components[component_index]);
		components.erase(components.begin() + component_index, components.begin() + component_index + component_size);
		ids.erase(ids.begin() + index);
	}

	return found;
}

// ----------------------------------------------------------------------------

void component_pool::clear()
{
	for (u32 i = 0, end = size(); i < end; ++i)
		ftable->destroy(get(i));
	components.clear();
	ids.clear();
	ids.emplace_back(handle::Invalid);
}

// ============================================================================

component_pool::component_pool(const tag typeId, const u32 componentSize, const u32 componentVersion, std::unique_ptr<func_table> && ftable) :
	component_tag(typeId),
	component_size(componentSize),
	component_version(componentVersion),
	ftable(std::move(ftable)),
	components(),
	ids()
{
	ids.emplace_back(handle::Invalid);
}

// ----------------------------------------------------------------------------

u32 component_pool::get_index(const handle entity_id) const
{
	assert(entity_id.is_valid());

	const auto & idIt = std::lower_bound(ids.begin(), ids.end(), entity_id);
	return idIt != ids.end() && *idIt == entity_id ? static_cast<u32>(std::distance(ids.begin(), idIt)) : size();
}

// ----------------------------------------------------------------------------

component * component_pool::get(const u32 index)
{
	return reinterpret_cast<component*>(components.data() + index * component_size);
}

// ----------------------------------------------------------------------------

const component * component_pool::get(const u32 index) const
{
	return reinterpret_cast<const component*>(components.data() + index * component_size);
}

// ----------------------------------------------------------------------------

std::pair<void *, bool>  component_pool::insert(const handle entity_id)
{
	const auto & idIt = std::lower_bound(ids.begin(), ids.end(), entity_id);
	bool const maxId = idIt == ids.end();
	const u32 index = static_cast<u32>(std::distance(ids.begin(), idIt));
	const u32 component_index = index * component_size;
	if (!maxId && *idIt == entity_id)
		return std::make_pair(&components[component_index], true);

	ids.insert(idIt, entity_id);
	auto const componentIt = components.insert(components.begin() + component_index, component_size, 0);
	return std::make_pair(&*componentIt, false);
}

// ============================================================================
