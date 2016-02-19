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

		factory() = default;

		factory(const factory & rhs) :
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

		void assign(const Label & label, const producer & producer)
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

		std::map<Label, producer> producers;
		mutable std::mutex mutex;
	};

	template <typename ReturnType, typename... Args>
	struct factory<const char *, ReturnType(Args ...)>
	{
	public:

		typedef std::function<ReturnType(Args ...)> producer;

	public:

		factory() = default;

		factory(const factory & rhs) :
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

		void assign(const char * const label, const producer & producer)
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

		std::map<const char *, producer, string_utilities::Less> producers;
		mutable std::mutex mutex;
	};
}

#endif