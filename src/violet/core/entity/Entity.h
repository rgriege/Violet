#ifndef ENTITY_H
#define ENTITY_H

#include "violet/core/Defines.h"

namespace Violet
{
	/**
	* A basic entity.
	*/
	class VIOLET_API Entity
	{
	public:

		Entity();
		Entity(const Entity & other);

	public:

		const uint32 m_id;
	};
}

#endif