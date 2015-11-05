#ifndef VIOLET_Factory_H
#define VIOLET_Factory_H

// #include "violet/Defines.h"
#include "violet/utility/StringUtilities.h"

#include <assert.h>
#include <functional>
#include <map>
#include <mutex>

namespace Violet
{
	template <typename Label, typename Signature>
	class Factory;

	template <typename Label, typename ReturnType, typename... Args>
	class Factory<Label, ReturnType(Args ...)>
	{
	public:

		typedef std::function<ReturnType(Args ...)> Producer;

	public:

		Factory() = default;

		Factory(const Factory & rhs) :
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

		void assign(const Label & label, const Producer & producer)
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

		std::map<Label, Producer> m_producers;
		mutable std::mutex m_mutex;
	};

	template <typename ReturnType, typename... Args>
	class Factory<const char *, ReturnType(Args ...)>
	{
	public:

		typedef std::function<ReturnType(Args ...)> Producer;

	public:

		Factory() = default;

		Factory(const Factory & rhs) :
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

		void assign(const char * const label, const Producer & producer)
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

		std::map<const char *, Producer, StringUtilities::Less> m_producers;
		mutable std::mutex m_mutex;
	};
}

#endif