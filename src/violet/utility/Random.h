#ifndef VIOLET_Random_H
#define VIOLET_Random_H

#include "violet/Defines.h"

#include <random>

namespace Violet
{
	class VIOLET_API Random
	{
	public:

		static Random ms_generator;

	public:
		
		float generate0to1();

	private:

		std::random_device m_device;
	};
}

#endif