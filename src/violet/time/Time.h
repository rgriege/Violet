#ifndef VIOLET_Time_H
#define VIOLET_Time_H

#include "violet/Defines.h"

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