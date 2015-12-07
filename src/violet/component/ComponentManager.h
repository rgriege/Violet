#ifndef VIOLET_ComponentManager_H
#define VIOLET_ComponentManager_H

#include "violet/component/ComponentPool.h"
#include "violet/handle/Handle.h"
#include "violet/task/Thread.h"
#include "violet/utility/Factory.h"

#include <iterator>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace Violet
{
	class Deserializer;
	class HandleManager;

	class VIOLET_API ComponentManager
	{
	public:

		template <bool is_const, typename... ComponentTypes>
		class View
		{
		public:

			typedef std::conditional_t<is_const, std::tuple<const ComponentTypes&...>, std::tuple<ComponentTypes&...>> component_tuple;
			typedef std::conditional_t<is_const,
				std::tuple<ComponentPool::const_iterator<ComponentTypes>...>,
				std::tuple<ComponentPool::iterator<ComponentTypes>...>> iterator_tuple;

			class Iterator : public std::iterator<std::input_iterator_tag, component_tuple>
			{
			public:

				explicit Iterator(iterator_tuple iterators);

				Iterator & operator++();
				component_tuple operator*();
				Handle getEntityId() const;

				bool operator==(const Iterator & other) const;
				bool operator!=(const Iterator & other) const;

			private:

				void advance();
				template <uint32 Index>
				void advance(uint32 & count);

			private:

				iterator_tuple m_iterators;
				Handle m_entityId;
			};

		private:

			static const size_t N = sizeof...(ComponentTypes);

		public:

			explicit View(const ComponentManager & manager);

			Iterator begin();
			Iterator end();

		private:

			const ComponentManager & m_manager;
		};

	public:

		typedef Factory<std::string, ComponentPool(ComponentManager &)> PoolFactory;
		typedef Factory<std::string, void(ComponentManager &, Deserializer &, const std::unordered_map<uint32, Handle> &)> ComponentsFactory;

	public:

		template <typename ComponentType>
		static void installComponent();
		static void installComponent(Tag const tag, const PoolFactory::Producer & producer, const ComponentsFactory::Producer & csProducer, Thread thread);

		template <typename ComponentType>
		static void uninstallComponent();
		static void uninstallComponent(Tag const tag);

	public:

		ComponentManager();

		/*ComponentManager(ComponentManager && other);
		ComponentManager & operator=(ComponentManager && other);*/

		void load(HandleManager & handleManager, const char * sceneName);

		template <typename ComponentType, typename... Args>
		ComponentType & createComponent(Handle entityId, Args &&... args);

		template <typename ComponentType>
		ComponentPool * getPool();
		template <typename ComponentType>
		const ComponentPool * getPool() const;

		template <typename ComponentType>
		bool hasComponent(Handle entityId) const;

		template <typename ComponentType>
		ComponentType * getComponent(Handle entityId);
		template <typename ComponentType>
		const ComponentType * getComponent(Handle entityId) const;
		template <typename... ComponentTypes>
		View<true, ComponentTypes...> getEntityView() const;
		
		template <typename ComponentType>
		bool remove(Handle entityId);
		bool removeAll(Handle entityId);
		void clear();

	private:

		template <typename ComponentType>
		static ComponentPool factoryCreatePool(ComponentManager & manager);
		template <typename ComponentType>
		static void factoryCreateComponents(ComponentManager & manager, Deserializer & deserializer, const std::unordered_map<uint32, Handle> &);

	private:

		ComponentPool * getPool(Tag componentTag);
		const ComponentPool * getPool(Tag componentTag) const;

		ComponentManager(const ComponentManager &) = delete;
		ComponentManager & operator=(const ComponentManager &) = delete;

	private:

		static PoolFactory ms_poolFactory;
		static ComponentsFactory ms_componentsFactory;
		static std::map<Tag, Thread> ms_poolThreads;

	private:

		std::vector<ComponentPool> m_pools;
	};
}

#include "violet/component/ComponentManager.inl"

#endif