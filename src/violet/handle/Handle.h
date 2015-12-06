#ifndef VIOLET_Handle_H
#define VIOLET_Handle_H

#include "violet/Defines.h"

#include <functional>

namespace Violet
{
	class VIOLET_API Handle
	{
	public:

		static const Handle ms_invalid;

	public:

		Handle();
		Handle(uint32 id, uint32 version);

		uint32 getId() const;
		uint32 getVersion() const;

		bool isValid() const;

	private:

		uint32 m_id : 24;
		uint32 m_version : 8;
	};

	bool VIOLET_API operator<(Handle lhs, Handle rhs);
	bool VIOLET_API operator==(Handle lhs, Handle rhs);
	bool VIOLET_API operator!=(Handle lhs, Handle rhs);
}

namespace std
{
	template<>
	struct hash<Violet::Handle>
	{
		std::size_t operator()(const Violet::Handle handle) const
		{
			return std::hash<uint32>()((handle.getId() << 8) | handle.getVersion());
		}
	};
}

#endif