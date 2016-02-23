// ============================================================================ 
#include "violet/template/tuple_utilities.h"

#include <algorithm>

// ============================================================================

namespace vlt
{
	namespace detail
	{
		template <typename ComponentType, typename... ComponentTypes>
		struct Begin_Helper
		{
			static std::tuple<Component_Pool::const_iterator<ComponentType>, Component_Pool::const_iterator<ComponentTypes>...> begin(const vlt::Scene & manager)
			{
				return std::tuple_cat(Begin_Helper<ComponentType>::begin(manager), Begin_Helper<ComponentTypes...>::begin(manager));
			}
		};

		template <typename ComponentType>
		struct Begin_Helper<ComponentType>
		{
			static std::tuple<Component_Pool::const_iterator<ComponentType>> begin(const vlt::Scene & manager)
			{
				const Component_Pool * pool = manager.get_pool<ComponentType>();
				return std::make_tuple(pool->begin<ComponentType>());
			}
		};

		template <typename ComponentType, typename... ComponentTypes>
		struct End_Helper
		{
			static std::tuple<Component_Pool::const_iterator<ComponentType>, Component_Pool::const_iterator<ComponentTypes>...> end(const vlt::Scene & manager)
			{
				return std::tuple_cat(End_Helper<ComponentType>::end(manager), End_Helper<ComponentTypes...>::end(manager));
			}
		};

		template <typename ComponentType>
		struct End_Helper<ComponentType>
		{
			static std::tuple<Component_Pool::const_iterator<ComponentType>> end(const vlt::Scene & manager)
			{
				const Component_Pool * pool = manager.get_pool<ComponentType>();
				return std::make_tuple(pool->end<ComponentType>());
			}
		};
	}
}

// ============================================================================

template <typename... ComponentTypes>
vlt::Scene::View<ComponentTypes...>::Entity::Entity(component_tuple _components, const Handle _id) :
	components(_components),
	id(_id)
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
template <typename ComponentType>
const ComponentType & vlt::Scene::View<ComponentTypes...>::Entity::get() const
{
	return vlt::get<const ComponentType &>(components);
}

// ============================================================================

template <typename... ComponentTypes>
vlt::Scene::View<ComponentTypes...>::iterator::iterator(iterator_tuple _iterators) :
	iterators(_iterators),
	entity_id()
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::Scene::View<ComponentTypes...>::iterator & vlt::Scene::View<ComponentTypes...>::iterator::operator++()
{
	entity_id = entity_id.is_valid() ? Handle(entity_id.id + 1, entity_id.version) : Handle(0, 0);
	advance();
	return *this;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::Scene::View<ComponentTypes...>::Entity vlt::Scene::View<ComponentTypes...>::iterator::operator*()
{
	return Entity(std::forward_as_tuple(*vlt::get<Component_Pool::const_iterator<ComponentTypes>>(iterators)...), entity_id);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool vlt::Scene::View<ComponentTypes...>::iterator::operator==(const iterator & other) const
{
	return entity_id == other.entity_id;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool vlt::Scene::View<ComponentTypes...>::iterator::operator!=(const iterator & other) const
{
	return !(*this == other);
}

// ============================================================================

template <typename... ComponentTypes>
void vlt::Scene::View<ComponentTypes...>::iterator::advance()
{
	u32 count = 0;
	advance<0>(count);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
template <u32 Index>
void vlt::Scene::View<ComponentTypes...>::iterator::advance(u32 & count)
{
	if (count != sizeof...(ComponentTypes))
	{
		std::get<Index>(iterators).advance_to(entity_id);
		if (std::get<Index>(iterators))
		{
			if (std::get<Index>(iterators).get_entity_id() != entity_id)
			{
				count = 1;
				entity_id = std::get<Index>(iterators).get_entity_id();
			}
			else
				++count;

			advance<(Index + 1) % sizeof...(ComponentTypes)>(count);
		}
		else
		{
			count = sizeof...(ComponentTypes);
			entity_id = vlt::Handle::Invalid;
		}
	}
}

// ============================================================================

template <typename... ComponentTypes>
vlt::Scene::View<ComponentTypes...>::View(const Scene & _scene) :
	scene(_scene)
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::Scene::View<ComponentTypes...>::iterator vlt::Scene::View<ComponentTypes...>::begin()
{
	iterator iterator(detail::Begin_Helper<ComponentTypes...>::begin(scene));
	return ++iterator;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::Scene::View<ComponentTypes...>::iterator vlt::Scene::View<ComponentTypes...>::end()
{
	return iterator(detail::End_Helper<ComponentTypes...>::end(scene));
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & vlt::Scene::create_component(const Handle entity_id, Args &&... args)
{
	Component_Pool * pool = get_pool<ComponentType>();
	return pool->create<ComponentType, Args...>(entity_id, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::Component_Pool * vlt::Scene::get_pool()
{
	return get_pool(ComponentType::metadata->tag);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const vlt::Component_Pool * vlt::Scene::get_pool() const
{
	return get_pool(ComponentType::metadata->tag);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool vlt::Scene::has_component(const Handle entity_id) const
{
	return get_pool<ComponentType>()->has(entity_id);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * vlt::Scene::get_component(const Handle entity_id)
{
	return get_pool<ComponentType>()->get<ComponentType>(entity_id);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * vlt::Scene::get_component(const Handle entity_id) const
{
	return get_pool<ComponentType>()->get<ComponentType>(entity_id);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
vlt::Scene::View<ComponentTypes...> vlt::Scene::get_entity_view() const
{
	return View<ComponentTypes...>(*this);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool vlt::Scene::remove(const Handle entity_id)
{
	return get_pool<ComponentType>()->remove(entity_id);
}

// ============================================================================

template <typename ComponentType>
void vlt::Scene::install_component()
{
	install_component(ComponentType::metadata->tag, &Scene::create_pool<ComponentType>);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
void vlt::Scene::uninstall_component()
{
	uninstall_component(ComponentType::metadata->tag);
}

// ============================================================================

template <typename ComponentType>
vlt::Component_Pool vlt::Scene::create_pool()
{
	return Component_Pool::create<ComponentType>();
}

// ============================================================================
