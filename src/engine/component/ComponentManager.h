#ifndef VIOLET_ComponentManager_H
#define VIOLET_ComponentManager_H

#include "engine/component/ComponentPool.h"

#include <iterator>
#include <memory>
#include <tuple>

namespace Violet
{
	class VIOLET_API ComponentManager
	{
	public:
		
		template <typename... ComponentTypes>
		class Iterator : public std::iterator<std::input_iterator_tag, std::tuple<ComponentTypes&...>>
		{
		public:

			Iterator(const ComponentManager & manager, bool begin);

			Iterator<ComponentTypes...> & operator++();
			std::tuple<ComponentTypes&...> operator*();

			bool operator==(const Iterator<ComponentTypes...> & other) const;
			bool operator!=(const Iterator<ComponentTypes...> & other) const;

		private:

			std::tuple<ComponentPoolIterator<ComponentTypes>...> m_iterators;
			std::tuple<ComponentPoolIterator<ComponentTypes>...> m_ends;
			Entity m_entity;
		};

		template <typename... ComponentTypes>
		class View
		{
		private:

			static const size_t N = sizeof...(ComponentTypes);

		public:

			View(const ComponentManager & manager);

			Iterator<ComponentTypes...> begin();
			Iterator<ComponentTypes...> end();

		private:

			const ComponentManager & m_manager;
		};

	public:

		ComponentManager() = default;

		ComponentManager(ComponentManager && other);
		ComponentManager & operator=(ComponentManager && other);

		template <typename ComponentType, typename... Args>
		ComponentType & create(Entity entity, Args&&... args);

		template <typename ComponentType>
		bool has(Entity entity) const;

		template <typename ComponentType>
		ComponentType * get(Entity entity) const;
		template <typename... ComponentTypes>
		View<ComponentTypes...> getView() const;
		
		template <typename ComponentType>
		bool remove(Entity entity);
		bool removeAll(Entity entity);
		void clear();

	private:

		template <typename ComponentType>
		ComponentPool & getPool() const;

		ComponentManager(const ComponentManager &) = delete;
		ComponentManager & operator=(const ComponentManager &) = delete;

	private:

		mutable std::vector<ComponentPool> m_pools;
	};
}

#include "engine/component/ComponentManager.inl"

#endif