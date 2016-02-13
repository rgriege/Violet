// ============================================================================

#include <ctime>

#include "violet/time/timestamp.h"

using namespace vlt;

// ============================================================================

timestamp timestamp::now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}

// ============================================================================

timestamp::timestamp(const u64 secondsSinceEpoch) :
	m_time(std::chrono::seconds(secondsSinceEpoch))
{
}

// ----------------------------------------------------------------------------

timestamp & timestamp::operator+=(const u64 seconds)
{
	m_time += std::chrono::seconds(seconds);
	return *this;
}

// ----------------------------------------------------------------------------

u64 timestamp::get_hours() const
{
	return std::chrono::duration_cast<std::chrono::hours>(m_time.time_since_epoch()).count() % 24;
}

// ----------------------------------------------------------------------------

u64 timestamp::get_minutes() const
{
	return std::chrono::duration_cast<std::chrono::minutes>(m_time.time_since_epoch()).count() % 60;
}

// ----------------------------------------------------------------------------

u64 timestamp::get_seconds() const
{
	return std::chrono::duration_cast<std::chrono::seconds>(m_time.time_since_epoch()).count() % 60;
}

// ----------------------------------------------------------------------------

std::string timestamp::to_string() const
{
	char buf[30];
	std::time_t timestamp = std::chrono::system_clock::to_time_t(m_time);
	std::tm * t = std::gmtime(&timestamp);
	std::strftime(buf, sizeof(buf), "%H:%M:%S", t);
	return buf;
}

// ============================================================================
