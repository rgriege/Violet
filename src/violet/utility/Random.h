#ifndef VIOLET_RANDOM_H
#define VIOLET_RANDOM_H

#include "violet/core/defines.h"

#include <random>

namespace vlt
{
	struct VIOLET_API random
	{
		static random ms_generator;
				
		r32 generate_0_to_1();

	private:

		std::random_device m_device;
	};
}

#endif