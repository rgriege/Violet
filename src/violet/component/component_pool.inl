// ============================================================================

#include "violet/component/component_deserializer.h"

#include <algorithm>
#include <assert.h>

// ============================================================================

template <typename ComponentType, bool IsConst>
vlt::Component_Pool::Iterator<ComponentType, IsConst>::Iterator(pointer ptr, const Handle * idPtr) :
	component_ptr(ptr),
	id_ptr(idPtr)
{
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::Component_Pool::Iterator<ComponentType, IsConst> & vlt::Component_Pool::Iterator<ComponentType, IsConst>::operator++()
{
	++component_ptr;
	++id_ptr;
	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::Component_Pool::Iterator<ComponentType, IsConst> & vlt::Component_Pool::Iterator<ComponentType, IsConst>::advance_to(const Handle entity_id)
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
typename vlt::Component_Pool::Iterator<ComponentType, IsConst>::reference vlt::Component_Pool::Iterator<ComponentType, IsConst>::operator*()
{
	return *component_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
typename vlt::Component_Pool::Iterator<ComponentType, IsConst>::pointer vlt::Component_Pool::Iterator<ComponentType, IsConst>::operator->()
{
	return component_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::Handle vlt::Component_Pool::Iterator<ComponentType, IsConst>::get_entity_id() const
{
	return *id_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
vlt::Component_Pool::Iterator<ComponentType, IsConst>::operator bool() const
{
	return id_ptr->is_valid();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool IsConst>
bool vlt::Component_Pool::Iterator<ComponentType, IsConst>::operator!=(const Iterator<ComponentType, IsConst> & other) const
{
	return component_ptr != other.component_ptr;
}

// ============================================================================

namespace vlt
{
	namespace detail
	{
		template <typename T>
		void loadComponent(Component_Pool & pool, Component_Deserializer & deserializer)
		{
			pool.create<T>(deserializer.get_entity_id("id"), deserializer);
		}

		template <typename T>
		void saveComponent(Serializer & serializer, const T * component)
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
vlt::Component_Pool vlt::Component_Pool::create()
{
	Func_Table::load_fn load = (Func_Table::load_fn) &vlt::detail::loadComponent<ComponentType>;
	Func_Table::save_fn save = (Func_Table::save_fn) &vlt::detail::saveComponent<ComponentType>;
	Func_Table::destroy_fn destroy = (Func_Table::destroy_fn) &vlt::detail::deleteComponent<ComponentType>;
	auto ftable = std::make_unique<Func_Table>(load, save, destroy);
	return Component_Pool(ComponentType::metadata, std::move(ftable));
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & vlt::Component_Pool::create(const Handle entity_id, Args && ... args)
{
	assert(ComponentType::metadata->tag == metadata->tag);
	auto result = insert(entity_id);
	assert(!result.second);

	ComponentType * component = new (result.first) ComponentType(entity_id, std::forward<Args>(args)...);
	return *component;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * vlt::Component_Pool::get(const Handle entity_id)
{
	assert(ComponentType::metadata->tag == metadata->tag);
	const u32 index = get_index(entity_id);
	return index != size() ? static_cast<ComponentType*>(get(index)) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * vlt::Component_Pool::get(const Handle entity_id) const
{
	assert(ComponentType::metadata->tag == metadata->tag);
	const u32 index = get_index(entity_id);
	return index != size() ? static_cast<const ComponentType*>(get(index)) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::Component_Pool::iterator<ComponentType> vlt::Component_Pool::begin()
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return iterator<ComponentType>(static_cast<ComponentType*>(get(0)), &ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::Component_Pool::const_iterator<ComponentType> vlt::Component_Pool::begin() const
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return const_iterator<ComponentType>(static_cast<const ComponentType*>(get(0)), &ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::Component_Pool::iterator<ComponentType> vlt::Component_Pool::end()
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return iterator<ComponentType>(static_cast<ComponentType*>(get(size())), &ids[size()]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::Component_Pool::const_iterator<ComponentType> vlt::Component_Pool::end() const
{
	assert(ComponentType::metadata->tag == metadata->tag);
	return const_iterator<ComponentType>(static_cast<const ComponentType*>(get(size())), &ids[size()]);
}

// ============================================================================
