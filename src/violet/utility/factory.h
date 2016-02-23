#ifndef VIOLET_FACTORY_H
#define VIOLET_FACTORY_H

#include <assert.h>
#include <functional>
#include <map>
#include <mutex>

#include "violet/utility/string_utilities.h"

namespace vlt
{
	template <typename Label, typename Signature>
	struct Factory;

	template <typename Label, typename ReturnType, typename... Args>
	struct Factory<Label, ReturnType(Args ...)>
	{
		typedef std::function<ReturnType(Args ...)> Producer;

		std::map<Label, Producer> producers;
		mutable std::mutex mutex;

		Factory() = default;

		Factory(const Factory & rhs) :
			producers(rhs.producers),
			mutex()
		{
		}

		ReturnType create(const Label & label, Args ... args) const
		{
			const std::lock_guard<std::mutex> guard(mutex);
			auto it = producers.find(label);
			assert(it != producers.end());
			return (it->second)(std::forward<Args>(args)...);
		}

		void assign(const Label & label, const Producer & producer)
		{
			const std::lock_guard<std::mutex> guard(mutex);
			producers[label] = producer;
		}

		bool has(const Label & label) const
		{
			const std::lock_guard<std::mutex> guard(mutex);
			return producers.find(label) != producers.end();
		}

		void remove(const Label & label)
		{
			const std::lock_guard<std::mutex> guard(mutex);
			producers.erase(label);
		}
	};

	template <typename ReturnType, typename... Args>
	struct Factory<const char *, ReturnType(Args ...)>
	{
		typedef std::function<ReturnType(Args ...)> Producer;

		std::map<const char *, Producer, String_Utilities::Less> producers;
		mutable std::mutex mutex;

		Factory() = default;

		Factory(const Factory & rhs) :
			producers(rhs.producers),
			mutex()
		{
		}

		ReturnType create(const char * const label, Args ... args) const
		{
			const std::lock_guard<std::mutex> guard(mutex);
			auto it = producers.find(label);
			assert(it != producers.end());
			return (it->second)(std::forward<Args>(args)...);
		}

		void assign(const char * const label, const Producer & producer)
		{
			const std::lock_guard<std::mutex> guard(mutex);
			producers[label] = producer;
		}

		bool has(const char * const label) const
		{
			const std::lock_guard<std::mutex> guard(mutex);
			return producers.find(label) != producers.end();
		}

		void remove(const char * const label)
		{
			const std::lock_guard<std::mutex> guard(mutex);
			producers.erase(label);
		}
	};
}

#endif
