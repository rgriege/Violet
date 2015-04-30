#ifndef ASSERT_H
#define ASSERT_H

#include <assert.h>

namespace Violet
{
	template <typename T>
	void assert_equal(const T & lhs, const T & rhs)
	{
		assert(lhs == rhs);
	}

	template <typename T, typename Pr, typename Arg>
	void assert_equal(const T & lhs, const T & rhs, const Pr & pr, const Arg & arg)
	{
		if (lhs != rhs)
		{
			pr(arg);
			assert(false);
		}
	}
}

#endif