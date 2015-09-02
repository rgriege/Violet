// ============================================================================

#include "violet/time/Time.h"

#include <ctime>

using namespace Violet;

// ============================================================================

Time Time::now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}

// ============================================================================

Time::Time(const uint64 secondsSinceEpoch) :
	m_time(std::chrono::seconds(secondsSinceEpoch))
{
}

// ----------------------------------------------------------------------------

Time & Time::operator+=(const uint64 seconds)
{
	m_time += std::chrono::seconds(seconds);
	return *this;
}

// ----------------------------------------------------------------------------

uint64 Time::getHours() const
{
	return std::chrono::duration_cast<std::chrono::hours>(m_time.time_since_epoch()).count() % 24;
}

// ----------------------------------------------------------------------------

uint64 Time::getMinutes() const
{
	return std::chrono::duration_cast<std::chrono::minutes>(m_time.time_since_epoch()).count() % 60;
}

// ----------------------------------------------------------------------------

uint64 Time::getSeconds() const
{
	return std::chrono::duration_cast<std::chrono::seconds>(m_time.time_since_epoch()).count() % 60;
}

// ----------------------------------------------------------------------------

std::string Time::toString() const
{
	char buf[30];
	std::time_t time = std::chrono::system_clock::to_time_t(m_time);
	std::tm * t = std::gmtime(&time);
	std::strftime(buf, sizeof(buf), "%H:%M:%S", t);
	return buf;
}

// ============================================================================
