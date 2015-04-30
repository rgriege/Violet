#ifndef SYSTEM_H
#define SYSTEM_H

#include "engine/component/Component.h"
#include "engine/component/ComponentFactory.h"
#include "engine/entity/Entity.h"
#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"
#include "engine/system/SystemFactory.h"
#include "engine/template/TupleUtilities.h"

#include <vector>
#include <map>
#include <assert.h>

namespace Violet
{
	class Component;
	class Engine;
	class EntityFactory;

	class VIOLET_API System
    {
    public:

		System(const char * label) :
			m_label(label)
		{
		}

		virtual ~System() = default;

		const char * getLabel() const
		{
			return m_label;
		}

		virtual void bind(ComponentFactory & factory) {};
		virtual void unbind(ComponentFactory & factory) {};
		virtual void bind(EntityFactory & factory) {};
		virtual void unbind(EntityFactory & factory) {};
		virtual bool owns(const char * label) const { return false; }
		virtual bool has(const char * label, const Entity & entity) const { return false; }
		virtual Component & fetch(const char * label, const Entity & entity) {
			static Component s_component(entity);
			s_component = Component(entity);
			return s_component;
		}
		virtual const Component & fetch(const char * label, const Entity & entity) const {
			static Component s_component(entity);
			s_component = Component(entity);
			return s_component;
		}
		virtual void remove(const Entity & entity) {}
		virtual void update(float dt, Engine & engine) {}
		virtual void save(Serializer & serializer, const Entity & entity) const {}
		virtual void clear() {}

	private:

		const char * m_label;
    };

	template <typename ComponentType>
	class ComponentSystem : public System
	{
	public:

		static const char * getStaticLabel()
		{
			return ComponentType::getLabel();
		}

	public:

		ComponentSystem() :
			System(ComponentType::getLabel()),
			m_components(new std::vector<ComponentType>),
			m_entityComponentMap(),
			m_removed()
		{
		}

		ComponentSystem<ComponentType>(ComponentSystem<ComponentType> && other) :
			System(ComponentType::getLabel()),
			m_components(std::move(other.m_components)),
			m_entityComponentMap(std::move(other.m_entityComponentMap)),
			m_removed(std::move(other.m_removed))
		{
		}

		void create(Entity & entity, Deserializer & deserializer)
		{
			auto segment = deserializer.enterSegment(getStaticLabel());
			m_entityComponentMap.emplace(entity.getId(), m_components->size());
			m_components->emplace_back(entity, *segment);
		}

		template <typename... Args>
		void create(Entity & entity, Args&&... args)
		{
			m_entityComponentMap.emplace(entity.getId(), m_components->size());
			m_components->emplace_back(entity, std::forward<Args>(args)...);
		}

		template <typename ComponentType, typename... Args> //std::enable_if_t<has_type<Systems, ComponentSystem<ComponentType>>::value>* = nullptr
		void create(Entity & entity, Args&&... args)
		{
			return create(entity, std::forward<Args>(args)...);
		}

		virtual void bind(ComponentFactory & factory) override
		{
			factory.assign(getStaticLabel(), std::bind(&ComponentSystem<ComponentType>::create, this, std::placeholders::_1, std::placeholders::_2));
		}

		virtual void unbind(ComponentFactory & factory) override
		{
			factory.remove(getStaticLabel());
		}

		virtual bool owns(const char * label) const override
		{
			return strcmp(label, getStaticLabel()) == 0;
		}

		bool has(const Entity & entity) const
		{
			return m_entityComponentMap.find(entity.getId()) != m_entityComponentMap.end();
		}

		virtual bool has(const char * label, const Entity & entity) const override
		{
			assert(label == getStaticLabel());
			return has(entity);
		}

		virtual ComponentType & fetch(const char * label, const Entity & entity) override
		{
			assert(has(label, entity));
			return m_components->operator[](m_entityComponentMap[entity.getId()]);
		}

		virtual const ComponentType & fetch(const char * label, const Entity & entity) const override
		{
			assert(has(label, entity));
			auto const it = m_entityComponentMap.find(entity.getId());
			return m_components->operator[](it->second);
		}

		virtual void remove(const Entity & entity) override
		{
			m_removed.emplace_back(entity);
		}

		virtual void update(float /*dt*/, Engine & /*engine*/) override
		{
			for (auto const & entity : m_removed)
			{
				auto const it = m_entityComponentMap.find(entity.getId());
				if (it != m_entityComponentMap.end())
				{
					m_components->erase(m_components->begin() + it->second);
					m_entityComponentMap.erase(it);
				}
			}
			m_removed.clear();
		}

		virtual void save(Serializer & serializer, const Entity & entity) const override
		{
			if (has(entity))
			{
				const ComponentType & c = fetch(getStaticLabel(), entity);
				auto segment = serializer.createSegment(getStaticLabel());
				*segment << c;
			}
		}

		virtual void clear() override
		{
			m_entityComponentMap.clear();
			m_components->clear();
		}

		std::vector<ComponentType> & getComponents()
		{
			return *m_components;
		}

	private:

		ComponentSystem<ComponentType>(const ComponentSystem<ComponentType> &) = delete;
		ComponentSystem<ComponentType> & operator=(const ComponentSystem<ComponentType> &) = delete;
		
	private:

		typedef std::unique_ptr<std::vector<ComponentType>> Components;
		Components m_components;
		std::map<uint32, uint32> m_entityComponentMap;
		std::vector<Entity> m_removed;
	};

	template <typename ... ComponentTypes>
	class MultiComponentSystem : public System
	{
	private:

		typedef std::tuple<ComponentSystem<ComponentTypes>...> Systems;

	public:

		MultiComponentSystem(const char * label) :
			System(label),
			m_systems(),
			m_systemDictionary()
		{
			for_each([&](System & system) { m_systemDictionary[system.getLabel()] = &system; }, m_systems);
		}

		template <typename ComponentType, std::enable_if_t<has_type<Systems, ComponentSystem<ComponentType>>::value>* = nullptr>
		void create(Entity & entity, Deserializer & deserializer)
		{
			auto & system = get<ComponentSystem<ComponentType>>(m_systems);
			system.create(entity, deserializer);
		}

		template <typename ComponentType, typename... Args> //std::enable_if_t<has_type<Systems, ComponentSystem<ComponentType>>::value>* = nullptr
		void create(Entity & entity, Args&&... args)
		{
			auto & system = get<ComponentSystem<ComponentType>>(m_systems);
			system.create(entity, std::forward<Args>(args)...);
		}

		virtual void bind(ComponentFactory & factory) override
		{
			for_each([&](System & system) { system.bind(factory); }, m_systems);
		}

		virtual void unbind(ComponentFactory & factory) override
		{
			for_each([&](System & system) { system.unbind(factory); }, m_systems);
		}

		virtual bool owns(const char * label) const override
		{
			return m_systemDictionary.find(label) != m_systemDictionary.end();
		}

		virtual bool has(const char * label, const Entity & entity) const override
		{
			assert(owns(label));
			auto it = m_systemDictionary.find(label);
			return it != m_systemDictionary.end() && it->second->has(label, entity);
		}

		virtual Component & fetch(const char * label, const Entity & entity) override
		{
			assert(has(label, entity));
			auto it = m_systemDictionary.find(label);
			assert(it != m_systemDictionary.end());
			return it->second->fetch(label, entity);
		}

		virtual void save(Serializer & serializer, const Entity & entity) const override
		{
			for_each([&](const System & system) { system.save(serializer, entity); }, m_systems);
		}

		virtual void clear() override
		{
			for_each([](System & system) { system.clear(); }, m_systems);
		}

	protected:

		template <typename ComponentType>
		std::vector<ComponentType> & getComponents()
		{
			static_assert(has_type<decltype(m_systems), ComponentSystem<ComponentType>>::value, "MultiComponentSystem owns no such ComponentType");
			return get<ComponentSystem<ComponentType>>(m_systems).getComponents();
		}

	private:

		MultiComponentSystem<ComponentTypes...>(const MultiComponentSystem<ComponentTypes...> &) = delete;
		MultiComponentSystem<ComponentTypes...> & operator=(const MultiComponentSystem<ComponentTypes...> &) = delete;

	private:

		Systems m_systems;
		std::map<const char *, System *, StringUtilities::Less> m_systemDictionary;
	};
};

#endif