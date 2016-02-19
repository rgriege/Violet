#ifndef VIOLET_GUARD_H
#define VIOLET_GUARD_H

namespace vlt
{
	template <typename T>
	struct guard
	{
		guard(const T & t)
		{
			T::bind(t);
		}

		~guard()
		{
			T::unbind();
		}
	};
}

#endif