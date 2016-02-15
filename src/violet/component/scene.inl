// ============================================================================

#include "violet/template/tuple_utilities.h"

#include <algorithm>

// ============================================================================

namespace vlt
{
	namespace detail
	{
		template <typename ComponentType, typename... ComponentTypes>
		struct BeginHelper
		{
			static std::tuple<component_pool::const_iterator<ComponentType>, component_pool::const_iterator<ComponentTypes>...> begin(const vlt::scene & manager)
			{
				return std::tuple_cat(BeginHelper<ComponentType>::begin(manager), BeginHelper<ComponentTypes...>::begin(manager));
			}
		};

		template <typename ComponentType>
		struct BeginHelper<ComponentType>
		{
			static std::tuple<component_pool::const_iterator<ComponentType>> begin(const vlt::scene & manager)
			{
				const component_pool * pool = manager.get_pool<ComponentType>();
				return std::make_tuple(pool->begin<ComponentType>());
			}
		};

		template <typename ComponentType, typename... ComponentTypes>
		struct EndHelper
		{
			static std::tuple<component_pool::const_iterator<ComponentType>, component_pool::const_iterator<ComponentTypes>...> end(const vlt::scene & manager)
			{
				return std::tuple_cat(EndHelper<ComponentType>::end(manager), EndHelper<ComponentTypes...>::end(manager));
			}
		};

		template <typename ComponentType>
		struct EndHelper<ComponentType>
		{
			static std::tuple<component_pool::const_iterator<ComponentType>> end(const vlt::scene & manager)
			{
				const component_pool * pool = manager.get_pool<ComponentType>();
				return std::make_tuple(pool->end<ComponentType>());
			}
		};
	}
}

// ============================================================================

template <typename... ComponentTypes>
vlt::scene::view<ComponentTypes...>::entity::entity(component_tuple _components, const handle _id) :
	components(_components),
	id(_id)
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
template <typename ComponentType>
const ComponentType & vlt::scene::view<ComponentTypes...>::entity::get() const
{
	return vlt::get<const ComponentType &>(components);
}

// ============================================================================

template <typename... ComponentTypes>
vlt::scene::view<ComponentTypes...>::iterator::iterator(iterator_tuple iterators) :
	m_iterators(iterators),
	m_entityId()
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::scene::view<ComponentTypes...>::iterator & vlt::scene::view<ComponentTypes...>::iterator::operator++()
{
	m_entityId = m_entityId.is_valid() ? handle(m_entityId.id + 1, m_entityId.version) : handle(0, 0);
	advance();
	return *this;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::scene::view<ComponentTypes...>::entity vlt::scene::view<ComponentTypes...>::iterator::operator*()
{
	return entity(std::forward_as_tuple(*vlt::get<component_pool::const_iterator<ComponentTypes>>(m_iterators)...), m_entityId);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool vlt::scene::view<ComponentTypes...>::iterator::operator==(const iterator & other) const
{
	return m_entityId == other.m_entityId;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool vlt::scene::view<ComponentTypes...>::iterator::operator!=(const iterator & other) const
{
	return !(*this == other);
}

// ============================================================================

template <typename... ComponentTypes>
void vlt::scene::view<ComponentTypes...>::iterator::advance()
{
	u32 count = 0;
	advance<0>(count);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
template <u32 Index>
void vlt::scene::view<ComponentTypes...>::iterator::advance(u32 & count)
{
	if (count != sizeof...(ComponentTypes))
	{
		std::get<Index>(m_iterators).advance_to(m_entityId);
		if (std::get<Index>(m_iterators))
		{
			if (std::get<Index>(m_iterators).get_entity_id() != m_entityId)
			{
				count = 1;
				m_entityId = std::get<Index>(m_iterators).get_entity_id();
			}
			else
				++count;

			advance<(Index + 1) % sizeof...(ComponentTypes)>(count);
		}
		else
		{
			count = sizeof...(ComponentTypes);
			m_entityId = vlt::handle::Invalid;
		}
	}
}

// ============================================================================

template <typename... ComponentTypes>
vlt::scene::view<ComponentTypes...>::view(const scene & manager) :
	m_manager(manager)
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::scene::view<ComponentTypes...>::iterator vlt::scene::view<ComponentTypes...>::begin()
{
	iterator iterator(detail::BeginHelper<ComponentTypes...>::begin(m_manager));
	return ++iterator;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename vlt::scene::view<ComponentTypes...>::iterator vlt::scene::view<ComponentTypes...>::end()
{
	return iterator(detail::EndHelper<ComponentTypes...>::end(m_manager));
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & vlt::scene::create_component(const handle entity_id, Args &&... args)
{
	component_pool * pool = get_pool<ComponentType>();
	return pool->create<ComponentType, Args...>(entity_id, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
vlt::component_pool * vlt::scene::get_pool()
{
	return get_pool(ComponentType::metadata->tag);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const vlt::component_pool * vlt::scene::get_pool() const
{
	return get_pool(ComponentType::metadata->tag);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool vlt::scene::has_component(const handle entity_id) const
{
	return get_pool<ComponentType>()->has(entity_id);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * vlt::scene::get_component(const handle entity_id)
{
	return get_pool<ComponentType>()->get<ComponentType>(entity_id);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * vlt::scene::get_component(const handle entity_id) const
{
	return get_pool<ComponentType>()->get<ComponentType>(entity_id);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
vlt::scene::view<ComponentTypes...> vlt::scene::get_entity_view() const
{
	return view<ComponentTypes...>(*this);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool vlt::scene::remove(const handle entity_id)
{
	return get_pool<ComponentType>()->remove(entity_id);
}

// ============================================================================

template <typename ComponentType>
void vlt::scene::install_component()
{
	install_component(ComponentType::metadata->tag, &scene::create_pool<ComponentType>);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
void vlt::scene::uninstall_component()
{
	uninstall_component(ComponentType::metadata->tag);
}

// ============================================================================

template <typename ComponentType>
vlt::component_pool vlt::scene::create_pool()
{
	return component_pool::create<ComponentType>();
}

// ============================================================================
