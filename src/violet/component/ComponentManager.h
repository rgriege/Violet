#ifndef VIOLET_ComponentManager_H
#define VIOLET_ComponentManager_H

#include "violet/component/ComponentPool.h"
#include "violet/handle/VersionedHandleManager.h"
#include "violet/task/Thread.h"
#include "violet/utility/Factory.h"

#include <iterator>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace Violet
{
	class ComponentDeserializer;

	class VIOLET_API ComponentManager
	{
	public:

		template <typename... ComponentTypes>
		class View
		{
		public:

			typedef std::tuple<const ComponentTypes&...> component_tuple;
			typedef std::tuple<ComponentPool::const_iterator<ComponentTypes>...> iterator_tuple;

			class Entity
			{
			public:

				Entity(component_tuple components, EntityId id);

				template <typename ComponentType>
				const ComponentType & get() const;
				EntityId getId() const;

			private:

				const component_tuple m_components;
				EntityId m_id;
			};

			class Iterator : public std::iterator<std::input_iterator_tag, component_tuple>
			{
			public:

				explicit Iterator(iterator_tuple iterators);

				Iterator & operator++();
				Entity operator*();

				bool operator==(const Iterator & other) const;
				bool operator!=(const Iterator & other) const;

			private:

				void advance();
				template <uint32 Index>
				void advance(uint32 & count);

			private:

				iterator_tuple m_iterators;
				EntityId m_entityId;
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

		typedef Factory<Tag, ComponentPool()> PoolFactory;
		typedef Factory<Tag, void(ComponentPool &, ComponentDeserializer &)> ComponentsFactory;
		typedef Factory<Tag, uint32(const ComponentPool &, Serializer &, const std::vector<EntityId> &)> PoolSaveFactory;

	public:

		template <typename ComponentType>
		static void installComponent();
		static void installComponent(Tag const tag, const PoolFactory::Producer & producer, const ComponentsFactory::Producer & csProducer, const PoolSaveFactory::Producer & sProducer, Thread thread);

		template <typename ComponentType>
		static void uninstallComponent();
		static void uninstallComponent(Tag const tag);

	public:

		ComponentManager();
		ComponentManager(ComponentManager && other);
		ComponentManager & operator=(ComponentManager && other);
		~ComponentManager();

		std::vector<EntityId> load(const char * sceneName);
		void save(const char * sceneName) const;
		void save(const char * sceneName, std::vector<EntityId> entityIds) const;

		EntityId createEntity();
		template <typename ComponentType, typename... Args>
		ComponentType & createComponent(EntityId entityId, Args &&... args);

		template <typename ComponentType>
		ComponentPool * getPool();
		template <typename ComponentType>
		const ComponentPool * getPool() const;

		bool exists(EntityId entityId) const;
		template <typename ComponentType>
		bool hasComponent(EntityId entityId) const;

		template <typename ComponentType>
		ComponentType * getComponent(EntityId entityId);
		template <typename ComponentType>
		const ComponentType * getComponent(EntityId entityId) const;
		template <typename... ComponentTypes>
		View<ComponentTypes...> getEntityView() const;
		std::vector<EntityId> getEntityIds() const;
		EntityId::StorageType getEntityVersion(EntityId::StorageType id) const;
		
		template <typename ComponentType>
		bool remove(EntityId entityId);
		void removeAll(EntityId entityId);
		void removeAll(EntityId entityId) thread_const;
		void clear();

	private:

		template <typename ComponentType>
		static ComponentPool createPool();
		template <typename ComponentType>
		static void createComponents(ComponentPool & pool, ComponentDeserializer & deserializer);
		template <typename ComponentType>
		static uint32 savePool(const ComponentPool & pool, Serializer & serializer, const std::vector<EntityId> & entityIds);

	private:

		ComponentPool * getPool(Tag componentTag);
		const ComponentPool * getPool(Tag componentTag) const;

		ComponentManager(const ComponentManager &) = delete;
		ComponentManager & operator=(const ComponentManager &) = delete;

	private:

		static PoolFactory ms_poolFactory;
		static ComponentsFactory ms_componentsFactory;
		static PoolSaveFactory ms_saveFactory;
		static std::map<Tag, Thread> ms_poolThreads;

	private:

		VersionedHandleManager<EntityId> m_handleManager;
		std::vector<ComponentPool> m_pools;
	};

	template <typename... ComponentTypes> 
	using Entity = typename ComponentManager::View<ComponentTypes...>::Entity;
}

#include "violet/component/ComponentManager.inl"

#endif
