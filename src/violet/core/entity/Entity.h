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

		Entity(uint32 id);

		uint32 getId() const;

	private:

		uint32 m_id;
	};
}

#endif