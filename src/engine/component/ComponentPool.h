#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"

#include <map>
#include <vector>

namespace Violet
{
	template <typename ComponentType>
	class ComponentPool
	{
	public:

		static const char * getStaticLabel()
		{
			return ComponentType::getLabel();
		}

	public:

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

		typename std::vector<ComponentType>::iterator remove(Entity & entity)
		{
			auto const it = m_entityComponentMap.find(entity.getId());
			if (it != m_entityComponentMap.end())
			{
				auto const result = m_components->erase(m_components->begin() + it->second);
				m_entityComponentMap.erase(it);
				return result;
			}
			return m_components->end();
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
	};
}

#endif