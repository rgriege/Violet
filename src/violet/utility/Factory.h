#ifndef VIOLET_FACTORY_H
#define VIOLET_FACTORY_H

#include "violet/utility/string_utilities.h"

#include <assert.h>
#include <functional>
#include <map>
#include <mutex>

namespace vlt
{
	template <typename Label, typename Signature>
	struct factory;

	template <typename Label, typename ReturnType, typename... Args>
	struct factory<Label, ReturnType(Args ...)>
	{
		typedef std::function<ReturnType(Args ...)> producer;

	public:

		factory() = default;

		factory(const factory & rhs) :
			m_producers(rhs.m_producers),
			m_mutex()
		{
		}

		ReturnType create(const Label & label, Args ... args) const
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			auto it = m_producers.find(label);
			assert(it != m_producers.end());
			return (it->second)(std::forward<Args>(args)...);
		}

		void assign(const Label & label, const producer & producer)
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			m_producers[label] = producer;
		}

		bool has(const Label & label) const
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			return m_producers.find(label) != m_producers.end();
		}

		void remove(const Label & label)
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			m_producers.erase(label);
		}

	private:

		std::map<Label, producer> m_producers;
		mutable std::mutex m_mutex;
	};

	template <typename ReturnType, typename... Args>
	struct factory<const char *, ReturnType(Args ...)>
	{
	public:

		typedef std::function<ReturnType(Args ...)> producer;

	public:

		factory() = default;

		factory(const factory & rhs) :
			m_producers(rhs.m_producers),
			m_mutex()
		{
		}

		ReturnType create(const char * const label, Args ... args) const
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			auto it = m_producers.find(label);
			assert(it != m_producers.end());
			return (it->second)(std::forward<Args>(args)...);
		}

		void assign(const char * const label, const producer & producer)
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			m_producers[label] = producer;
		}

		bool has(const char * const label) const
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			return m_producers.find(label) != m_producers.end();
		}

		void remove(const char * const label)
		{
			const std::lock_guard<std::mutex> guard(m_mutex);
			m_producers.erase(label);
		}

	private:

		std::map<const char *, producer, string_utilities::Less> m_producers;
		mutable std::mutex m_mutex;
	};
}

#endif