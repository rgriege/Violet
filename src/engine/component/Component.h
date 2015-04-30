#ifndef VIOLET_COMPONENT_H__
#define VIOLET_COMPONENT_H__

#include "engine/entity/Entity.h"

namespace Violet
{
	class VIOLET_API Component
	{
	public:

		Component(const Entity & e) :
			m_entity(e)
		{
		}

		Component(Component && c) :
			m_entity(c.m_entity)
		{
		}

		Component & operator=(Component && other)
		{
			m_entity = other.m_entity;
			return *this;
		}

		Entity & getEntity()
		{
			return m_entity;
		}

		Entity const & getEntity() const
		{
			return m_entity;
		}

	private:

		Component(const Component &) = delete;
		Component & operator=(const Component &) = delete;

	private:

		Entity m_entity;
	};
}

#endif