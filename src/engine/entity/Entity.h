#ifndef VIOLET_Entity_H
#define VIOLET_Entity_H

#include "engine/Defines.h"

#include <functional>

namespace Violet
{
	class VIOLET_API Entity
	{
	public:

		static Entity INVALID;

	public:

		Entity(uint32 id, uint32 version);

		uint32 getId() const;
		uint32 getVersion() const;

	private:

		uint32 m_id : 24;
		uint32 m_version : 8;
	};

	bool VIOLET_API operator<(Entity lhs, Entity rhs);
	bool VIOLET_API operator==(Entity lhs, Entity rhs);
	bool VIOLET_API operator!=(Entity lhs, Entity rhs);
}

namespace std
{
	template<>
	struct hash<Violet::Entity>
	{
		std::size_t operator()(const Violet::Entity entity) const
		{
			return std::hash<uint32>()(entity.getId());
		}
	};
}

#endif