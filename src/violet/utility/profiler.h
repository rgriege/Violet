#ifndef VIOLET_PROFILER_H
#define VIOLET_PROFILER_H

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "violet/core/defines.h"

namespace vlt
{
	struct Profiler
	{
	public:

		static void flush_cache();
		static Profiler & instance();

	public:

		struct Block
		{
		public:

			Block(std::string key, Profiler & profiler = instance());
			~Block();

		private:

			const std::string m_key;
			Profiler & m_profiler;
			const std::chrono::steady_clock::time_point m_startTime;
		};

	public:

		void add(const std::string & key, s64 microseconds);

		void report(std::ostream & os, bool clear = true);
		s64 report(std::string const & key, bool clear = true);

	private:

		std::unordered_map<std::string, s64> m_counters;
	};

	/*struct Metric
	{
	public:

		virtual void set(s64 value) = 0;
		virtual void update(r32 dt) = 0;
	};

	struct AverageMetric : public Metric
	{
	public:

		virtual void set(s64 value) override
		{

		}

	private:

		s64 m_value;
	};

	struct StatisticMetric : public Metric
	{
	public:

		virtual void set(s64 value) override
		{
			m_min = std::min(m_min, value);
			m_max = std::max(m_max, value);
			m_accumulation += value;
		}

		virtual void update(r32 dt) override
		{
			m_timeSinceLastReport += dt;
			if (m_reportInterval >= m_timeSinceLastReport)
			{
				Profiler::instance().add(m_label + "_min", m_min);
				Profiler::instance().add(m_label + "_max", m_max);
				Profiler::instance().add(m_label + "_avg", m_accumulation / static_cast<s64>(m_timeSinceLastReport * 1000000));
			}
		}

	private:

		std::string const m_label;
		s64 m_min;
		s64 m_max;
		s64 m_accumulation;
		r32 m_timeSinceLastReport;
		r32 m_reportInterval;
	};

	struct MetricsTracker
	{
	public:

		static void set(std::string const & Key, s64 value);
		static void update(r32 dt);

	private:

		static std::unordered_map<std::string, std::unique_ptr<Metric>> ms_metrics;
	};*/
}

#endif
