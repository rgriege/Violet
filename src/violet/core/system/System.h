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
	class Engine;

	class VIOLET_API System
    {
    public:

		System(const char * label) :
			m_label(label)
		{
		}

		virtual ~System() = default;

		const char * getLabel()
		{
			return m_label;
		}

		virtual void update(float dt, Engine & engine) = 0;
		virtual void clear() = 0;

	private:

		const char * m_label;
    };

	template <typename Component>
	class ComponentSystem : public System
	{
	public:

		typedef Component ComponentType;

	public:

		static const char * getStaticLabel()
		{
			return Component::getLabel();
		}

	public:

		~ComponentSystem()
		{
			ComponentFactory::getInstance().remove(getStaticLabel());
		}

		void create(Entity & entity, Deserializer & deserializer)
		{
			auto segment = deserializer.enterSegment(getStaticLabel());
			m_entityComponentMap.emplace(entity.m_id, m_components->size());
			m_components->emplace_back(entity, *segment);
		}

		virtual void clear() override
		{
			m_entityComponentMap.clear();
			m_components->clear();
		}

		bool has(const Entity & entity)
		{
			return m_entityComponentMap.find(entity.m_id) != m_entityComponentMap.end();
		}

		Component & fetch(const Entity & entity)
		{
			assert(has(entity));
			return m_components->operator[](m_entityComponentMap[entity.m_id]);
		}

	protected:

		ComponentSystem() :
			System(Component::getLabel()),
			m_components(new std::vector<Component>)
		{
			ComponentFactory::getInstance().assign(getStaticLabel(), std::bind(&ComponentSystem<Component>::create, this, std::placeholders::_1, std::placeholders::_2));
		}

	protected:

		typedef std::unique_ptr<std::vector<Component>> Components;
		Components m_components;
		std::map<uint32, uint32> m_entityComponentMap;
	};
};

#endif