// ============================================================================

#include <ctime>

#include "violet/time/timestamp.h"

using namespace vlt;

// ============================================================================

Timestamp Timestamp::now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}

// ============================================================================

Timestamp::Timestamp(const u64 secondsSinceEpoch) :
	m_time(std::chrono::seconds(secondsSinceEpoch))
{
}

// ----------------------------------------------------------------------------

Timestamp & Timestamp::operator+=(const u64 seconds)
{
	m_time += std::chrono::seconds(seconds);
	return *this;
}

// ----------------------------------------------------------------------------

u64 Timestamp::get_hours() const
{
	return std::chrono::duration_cast<std::chrono::hours>(m_time.time_since_epoch()).count() % 24;
}

// ----------------------------------------------------------------------------

u64 Timestamp::get_minutes() const
{
	return std::chrono::duration_cast<std::chrono::minutes>(m_time.time_since_epoch()).count() % 60;
}

// ----------------------------------------------------------------------------

u64 Timestamp::get_seconds() const
{
	return std::chrono::duration_cast<std::chrono::seconds>(m_time.time_since_epoch()).count() % 60;
}

// ----------------------------------------------------------------------------

std::string Timestamp::to_string() const
{
	char buf[30];
	std::time_t time = std::chrono::system_clock::to_time_t(m_time);
	std::tm * t = std::gmtime(&time);
	std::strftime(buf, sizeof(buf), "%H:%M:%S", t);
	return buf;
}

// ============================================================================
