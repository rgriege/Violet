#ifndef ENTITY_H
#define ENTITY_H

#include "violet/core/Defines.h"

namespace Violet
{
	/**
	* A basic entity.
	*/
	class Entity
	{
	public:

		Entity();

	private:

		Entity(const Entity &) = delete;
		Entity & operator=(const Entity &) = delete;

	public:

		uint32 m_id;
	};
}

#endif