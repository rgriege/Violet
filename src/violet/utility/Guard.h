#ifndef GUARD_H
#define GUARD_H

namespace Violet
{
	template <typename T>
	class Guard
	{
	public:

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