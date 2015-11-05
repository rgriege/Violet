#ifndef VIOLET_Factory_H
#define VIOLET_Factory_H

// #include "violet/Defines.h"
#include "violet/utility/StringUtilities.h"

#include <assert.h>
#include <functional>
#include <map>

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

		ReturnType create(const Label & label, Args ... args) const
		{
			auto it = m_producers.find(label);
			assert(it != m_producers.end());
			return (it->second)(std::forward<Args>(args)...);
		}

		void assign(const Label & label, const Producer & producer)
		{
			m_producers[label] = producer;
		}

		bool has(const Label & label) const
		{
			return m_producers.find(label) != m_producers.end();
		}

		void remove(const Label & label)
		{
			m_producers.erase(label);
		}

	private:

		std::map<Label, Producer> m_producers;
	};

	template <typename ReturnType, typename... Args>
	class Factory<const char *, ReturnType(Args ...)>
	{
	public:

		typedef std::function<ReturnType(Args ...)> Producer;

	public:

		ReturnType create(const char * const label, Args ... args) const
		{
			auto it = m_producers.find(label);
			assert(it != m_producers.end());
			return (it->second)(std::forward<Args>(args)...);
		}

		void assign(const char * const label, const Producer & producer)
		{
			m_producers[label] = producer;
		}

		bool has(const char * const label) const
		{
			return m_producers.find(label) != m_producers.end();
		}

		void remove(const char * const label)
		{
			m_producers.erase(label);
		}

	private:

		std::map<const char *, Producer, StringUtilities::Less> m_producers;
	};
}

#endif