#ifndef VIOLET_LogConst_H
#define VIOLET_LogConst_H

#include <cstddef>

namespace Violet
{
	template <size_t N, size_t base = 2>
	struct log_floor
	{
		enum { value = 1 + log_floor<N / base, base>::value };
	};

	template <size_t base>
	struct log_floor<1, base>
	{
		enum { value = 0 };
	};

	template <size_t base>
	struct log_floor<0, base>
	{
		enum { value = 0 };
	};

	template <size_t N, size_t base = 2>
	struct log_ceil
	{
		enum { value = 1 + log_floor<N - 1, base>::value };
	};
}

#endif