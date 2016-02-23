#ifndef VIOLET_GUARD_H
#define VIOLET_GUARD_H

namespace vlt
{
	template <typename T>
	struct Guard
	{
		Guard(const T & t)
		{
			T::bind(t);
		}

		~Guard()
		{
			T::unbind();
		}
	};
}

#endif
