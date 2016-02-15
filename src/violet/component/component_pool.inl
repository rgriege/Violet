// ============================================================================

#include "violet/component/component_deserializer.h"

#include <algorithm>
#include <assert.h>

// ============================================================================

template <typename ComponentType, bool IsConst>
vlt::component_pool::Iterator<ComponentType, IsConst>::Iterator(pointer ptr, const handle * idPtr) :
	component_ptr(ptr),
	id_ptr(idPtr)
{
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::component_pool::Iterator<ComponentType, IsConst> & vlt::component_pool::Iterator<ComponentType, IsConst>::operator++()
{
	++component_ptr;
	++id_ptr;
	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::component_pool::Iterator<ComponentType, IsConst> & vlt::component_pool::Iterator<ComponentType, IsConst>::advance_to(const handle entity_id)
{
	while (*this && *id_ptr < entity_id)
	{
		++id_ptr;
		++component_ptr;
	}

	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
typename vlt::component_pool::Iterator<ComponentType, IsConst>::reference vlt::component_pool::Iterator<ComponentType, IsConst>::operator*()
{
	return *component_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
typename vlt::component_pool::Iterator<ComponentType, IsConst>::pointer vlt::component_pool::Iterator<ComponentType, IsConst>::operator->()
{
	return component_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::handle vlt::component_pool::Iterator<ComponentType, IsConst>::get_entity_id() const
{
	return *id_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::component_pool::Iterator<ComponentType, IsConst>::operator bool() const
{
	return id_ptr->is_valid();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
bool vlt::component_pool::Iterator<ComponentType, IsConst>::operator!=(const Iterator<ComponentType, IsConst> & other) const
{
	return component_ptr != other.component_ptr;
}

// ============================================================================

namespace vlt
{
	namespace detail
	{
		template <typename T>
		void loadComponent(component_pool & pool, component_deserializer & deserializer)
		{
			pool.create<T>(deserializer.get_entity_id("id"), deserializer);
		}

		template <typename T>
		void saveComponent(serializer & serializer, const T * component)
		{
			serializer << *component;
		}

		template <typename T>
		void deleteComponent(T * component)
		{
			component->~T();
		}
	}
}

template <typename ComponentType>
vlt::component_pool vlt::component_pool::create()
{
	func_table::load_fn load = (func_table::load_fn) &vlt::detail::loadComponent<ComponentType>;
	func_table::save_fn save = (func_table::save_fn) &vlt::detail::saveComponent<ComponentType>;
	func_table::destroy_fn destroy = (func_table::destroy_fn) &vlt::detail::deleteComponent<ComponentType>;
	auto ftable = std::make_unique<func_table>(load, save, destroy);
	return component_pool(ComponentType::metadata, std::move(ftable));
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & vlt::component_pool::create(const handle entity_id, Args && ... args)
{
	assert(ComponentType::metadata->tag == metadata->tag);
	auto result = insert(entity_id);
	assert(!result.second);

	ComponentType * component = new (result.first) ComponentType(entity_id, std::forward<Args>(args)...);
	return *component;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * vlt::component_pool::get(const handle entity_id)
{
	assert(ComponentType::metadata->tag == metadata->tag);
	const u32 index = get_index(entity_id);
	return index != size() ? static_cast<ComponentType*>(get(index)) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * vlt::component_pool::get(const handle entity_id) const
{
	assert(ComponentType::metadata->tag == metadata->tag);
	const u32 index = get_index(entity_id);
	return index != size() ? static_cast<const ComponentType*>(get(index)) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::component_pool::iterator<ComponentType> vlt::component_pool::begin()
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return iterator<ComponentType>(static_cast<ComponentType*>(get(0)), &ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::component_pool::const_iterator<ComponentType> vlt::component_pool::begin() const
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return const_iterator<ComponentType>(static_cast<const ComponentType*>(get(0)), &ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::component_pool::iterator<ComponentType> vlt::component_pool::end()
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return iterator<ComponentType>(static_cast<ComponentType*>(get(size())), &ids[size()]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::component_pool::const_iterator<ComponentType> vlt::component_pool::end() const
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return const_iterator<ComponentType>(static_cast<const ComponentType*>(get(size())), &ids[size()]);
}

// ============================================================================
