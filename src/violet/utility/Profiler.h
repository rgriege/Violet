#ifndef VIOLET_Profiler_H
#define VIOLET_Profiler_H

#include "violet/Defines.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace Violet
{
	class Profiler
	{
	public:

		static void flushCache();
		static Profiler & getInstance();

	public:

		class Block
		{
		public:

			Block(std::string key, Profiler & profiler = getInstance());
			~Block();

		private:

			const std::string m_key;
			Profiler & m_profiler;
			const std::chrono::steady_clock::time_point m_startTime;
		};

	public:

		void add(const std::string & key, int64 microseconds);

		void report(std::ostream & os, bool clear = true);
		int64 report(std::string const & key, bool clear = true);

	private:

		std::unordered_map<std::string, int64> m_counters;
	};

	/*class Metric
	{
	public:

		virtual void set(int64 value) = 0;
		virtual void update(float dt) = 0;
	};

	class AverageMetric : public Metric
	{
	public:

		virtual void set(int64 value) override
		{

		}

	private:

		int64 m_value;
	};

	class StatisticMetric : public Metric
	{
	public:

		virtual void set(int64 value) override
		{
			m_min = std::min(m_min, value);
			m_max = std::max(m_max, value);
			m_accumulation += value;
		}

		virtual void update(float dt) override
		{
			m_timeSinceLastReport += dt;
			if (m_reportInterval >= m_timeSinceLastReport)
			{
				Profiler::getInstance().add(m_label + "_min", m_min);
				Profiler::getInstance().add(m_label + "_max", m_max);
				Profiler::getInstance().add(m_label + "_avg", m_accumulation / static_cast<int64>(m_timeSinceLastReport * 1000000));
			}
		}

	private:

		std::string const m_label;
		int64 m_min;
		int64 m_max;
		int64 m_accumulation;
		float m_timeSinceLastReport;
		float m_reportInterval;
	};

	class MetricsTracker
	{
	public:

		static void set(std::string const & key, int64 value);
		static void update(float dt);

	private:

		static std::unordered_map<std::string, std::unique_ptr<Metric>> ms_metrics;
	};*/
}

#endif