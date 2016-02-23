#ifndef VIOLET_RANDOM_H
#define VIOLET_RANDOM_H

#include <random>

#include "violet/core/defines.h"

namespace vlt
{
	struct VIOLET_API Random
	{
		static Random ms_generator;
				
		r32 generate_0_to_1();

	private:

		std::random_device m_device;
	};
}

#endif
