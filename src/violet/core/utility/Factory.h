#ifndef FACTORY_H
#define FACTORY_H

#include "violet/core/Defines.h"
#include "violet/core/utility/StringUtilities.h"

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

		ReturnType create(Label label, Args ... args)
		{
			auto producer = m_producers[label];
			return producer(std::forward<Args>(args)...);
		}

		void assign(Label label, const Producer & producer)
		{
			m_producers[label] = producer;
		}

		void remove(Label label)
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

		ReturnType create(const char * label, Args ... args)
		{
			auto producer = m_producers[label];
			return producer(args...);
		}

		void assign(const char * label, const Producer & producer)
		{
			m_producers[label] = producer;
		}

		void remove(const char * label)
		{
			m_producers.erase(label);
		}

	private:

		std::map<const char *, Producer, StringUtilities::Less> m_producers;
	};
}

#endif