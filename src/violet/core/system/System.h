#ifndef SYSTEM_H
#define SYSTEM_H

#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/SystemFactory.h"

#include <vector>
#include <map>

namespace Violet
{
	class AlterContext;

    class System
    {
    public:

		virtual ~System() = default;
		virtual void create(Entity & entity, Deserializer & deserializer) = 0;
		virtual const char * getLabel() = 0;
		virtual void update(float dt, AlterContext & context) = 0;
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

		virtual void create(Entity & entity, Deserializer & deserializer) override
		{
			auto segment = deserializer.enterSegment(Component::getLabel());
			m_entityComponentMap.emplace(entity.m_id, m_components.size());
			m_components.emplace_back(entity, *segment);
		}

		virtual const char * getLabel() override
		{
			return getStaticLabel();
		}

		Component & fetch(const Entity & entity)
		{
			return m_components[m_entityComponentMap[entity.m_id]];
		}

	protected:

		typedef std::vector<Component> Components;
		Components m_components;
		std::map<uint32, uint32> m_entityComponentMap;
	};
};

#endif