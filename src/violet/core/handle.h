#ifndef VIOLET_HANDLE_H
#define VIOLET_HANDLE_H

#include <functional>

#include "violet/core/defines.h"
#include "violet/core/types.h"

namespace vlt
{
	struct VIOLET_API Handle
	{
		u16 id;
		u16 version;

		static const Handle Invalid;
		
		Handle();
		Handle(u16 id, u16 version);
		
		bool is_valid() const;
	};

	VIOLET_API bool operator<(Handle lhs, Handle rhs);
	VIOLET_API bool operator==(Handle lhs, Handle rhs);
	VIOLET_API bool operator!=(Handle lhs, Handle rhs);
}

namespace std
{
	template <>
	struct hash<vlt::Handle>
	{
		std::size_t operator()(const vlt::Handle id) const
		{
			return std::hash<u32>()(*reinterpret_cast<const u32*>(&id));
		}
	};
}

#endif
