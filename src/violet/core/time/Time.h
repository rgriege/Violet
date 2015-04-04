#ifndef TIME_H
#define TIME_H

#include "violet/core/Defines.h"

#include <chrono>
#include <string>

namespace Violet
{
	class VIOLET_API Time
	{
	public:

		static Time now();

	public:

		Time(uint64 secondsSinceEpoch);

		Time & operator+=(uint64 seconds);

		uint64 getHours() const;
		uint64 getMinutes() const;
		uint64 getSeconds() const;

		std::string toString() const;

	private:

		std::chrono::time_point<std::chrono::system_clock> m_time;
	};
}

#endif