#ifndef VIOLET_TIMESTAMP_H
#define VIOLET_TIMESTAMP_H

#include "violet/core/defines.h"

#include <chrono>
#include <string>

namespace vlt
{
	struct VIOLET_API timestamp
	{
		static timestamp now();

		timestamp(u64 secondsSinceEpoch);

		timestamp & operator+=(u64 seconds);

		u64 get_hours() const;
		u64 get_minutes() const;
		u64 get_seconds() const;

		std::string to_string() const;

	private:

		std::chrono::time_point<std::chrono::system_clock> m_time;
	};
}

#endif