#ifndef VIOLET_MEMORY_H
#define VIOLET_MEMORY_H

#include <memory>

namespace vlt
{
	template <typename T>
	std::unique_ptr<T> make_unique(void * ptr)
	{
		return std::unique_ptr<T>(static_cast<T*>(ptr));
	}
}

#endif
