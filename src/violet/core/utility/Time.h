#ifndef TIME_H
#define TIME_H

#include "violet/core/Defines.h"

namespace Violet
{
	class VIOLET_API Time
	{
	public:

		static long unsigned getTimeInMilliseconds();
		static double getTimeInSeconds();
	};
}

#endif
