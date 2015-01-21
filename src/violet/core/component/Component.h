#ifndef VIOLET_COMPONENT_H__
#define VIOLET_COMPONENT_H__

#include "violet/core/entity/Entity.h"

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

		Component(const Component &) = delete;
		Component & operator=(const Component &) = delete;
		virtual ~Component() = default;

	public:

		const Entity m_entity;
	};
}

#endif