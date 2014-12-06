#ifndef FACTORY_H
#define FACTORY_H

#include <functional>
#include <map>

namespace Violet
{
	template <typename Label, typename Signature>
	class Factory;

	template <typename Label, typename ReturnType, typename... Args>
	class Factory<Label, ReturnType(Args...)>
	{
	public:

		typedef std::function<ReturnType(Args...)> Producer;

	public:

		ReturnType create(Label label, Args... args)
		{
			auto producer = m_producers[label];
			return producer(args...);
		}

		void assign(Label label, const Producer & producer)
		{
			m_producers[label] = producer;
		}

	private:

		std::map<Label, Producer> m_producers;
	};
}

#endif