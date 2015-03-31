#ifndef SYSTEM_H
#define SYSTEM_H

#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/template/TupleUtilities.h"

#include <vector>
#include <map>
#include <assert.h>

namespace Violet
{
	class Component;
	class Engine;

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

		virtual bool owns(const char * label) const = 0;
		virtual bool has(const char * label, const Entity & entity) const = 0;
		virtual Component & fetch(const char * label, const Entity & entity) = 0;
		virtual void update(float dt, Engine & engine) {}
		virtual void clear() = 0;

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
			m_components(new std::vector<ComponentType>)
		{
			ComponentFactory::getInstance().assign(getStaticLabel(), std::bind(&ComponentSystem<ComponentType>::create, this, std::placeholders::_1, std::placeholders::_2));
		}

		~ComponentSystem()
		{
			ComponentFactory::getInstance().remove(getStaticLabel());
		}

		void create(Entity & entity, Deserializer & deserializer)
		{
			auto segment = deserializer.enterSegment(getStaticLabel());
			m_entityComponentMap.emplace(entity.getId(), m_components->size());
			m_components->emplace_back(entity, *segment);
		}

		virtual bool owns(const char * label) const override
		{
			return strcmp(label, getStaticLabel()) == 0;
		}

		virtual bool has(const char * label, const Entity & entity) const override
		{
			assert(label == getStaticLabel());
			return m_entityComponentMap.find(entity.getId()) != m_entityComponentMap.end();
		}

		virtual ComponentType & fetch(const char * label, const Entity & entity) override
		{
			assert(has(label, entity));
			return m_components->operator[](m_entityComponentMap[entity.getId()]);
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

		ComponentSystem<ComponentType> & operator=(const ComponentSystem<ComponentType> &) = delete;
		
	private:

		typedef std::unique_ptr<std::vector<ComponentType>> Components;
		Components m_components;
		std::map<uint32, uint32> m_entityComponentMap;
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

		virtual ~MultiComponentSystem() override = default;

		template <typename ComponentType, std::enable_if_t<has_type<Systems, ComponentSystem<ComponentType>>::value>* = nullptr>
		void create(Entity & entity, Deserializer & deserializer)
		{
			auto system = get<ComponentSystem<ComponentType>>(m_systems);
			system.create(entity, deserializer);
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

		MultiComponentSystem<ComponentTypes...> & operator=(const MultiComponentSystem<ComponentTypes...> &) = delete;

	private:

		Systems m_systems;
		std::map<const char *, System *, StringUtilities::Less> m_systemDictionary;
	};
};

#endif