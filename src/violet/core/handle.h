#ifndef VIOLET_HANDLE_H
#define VIOLET_HANDLE_H

#include <functional>

#include "violet/core/defines.h"
#include "violet/core/types.h"

namespace vlt
{
	struct VIOLET_API handle
	{
		u16 id;
		u16 version;

		static const handle Invalid;
		
		handle();
		handle(u16 id, u16 version);
		
		bool is_valid() const;
	};

	VIOLET_API bool operator<(handle lhs, handle rhs);
	VIOLET_API bool operator==(handle lhs, handle rhs);
	VIOLET_API bool operator!=(handle lhs, handle rhs);
}

namespace std
{
	template <>
	struct hash<vlt::handle>
	{
		std::size_t operator()(const vlt::handle id) const
		{
			return std::hash<u32>()(*reinterpret_cast<const u32*>(&id));
		}
	};
}

#endif
