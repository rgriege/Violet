#ifndef VIOLET_DELEGATE_H
#define VIOLET_DELEGATE_H

#include <functional>

namespace vlt
{
	struct Delegate_Store
	{
		void * instance;
		void(*function)();

		Delegate_Store(void * instance, void(*function)()) :
			instance(instance),
			function(function)
		{
		}

		bool operator==(const Delegate_Store & rhs) const
		{
			return instance == rhs.instance && function == rhs.function;
		}
	};

	template <typename Signature>
	struct Delegate;
	template <typename ResultType, typename ... Args>
	struct Delegate<ResultType(Args...)> : public Delegate_Store
	{
		template <typename T, ResultType(T::*Member)(Args...)>
		static Delegate bind(T * instance)
		{
			ResultType(*function)(T*, Args...) = call_member<T, Member>;
			return Delegate(instance, function);
		}

	private:

		template <typename T, ResultType(T::*Member)(Args...)>
		static ResultType call_member(T * instance, Args ... args)
		{
			return (instance->*Member)(args...);
		}

	public:

		Delegate(const Delegate_Store & store) :
			Delegate_Store(store)
		{
		}

		Delegate(ResultType(*function)(Args...)) :
			Delegate_Store(nullptr, function)
		{
		}

		ResultType operator()(Args && ... args) const
		{
			return instance == nullptr
				? reinterpret_cast<ResultType(*)(Args...)>(function)(std::forward<Args>(args)...)
				: reinterpret_cast<ResultType(*)(void *, Args...)>(function)(instance, std::forward<Args>(args)...);
		}

	private:

		template <typename T>
		Delegate(T * instance, ResultType(*function)(T*, Args...)) :
			Delegate_Store(instance, reinterpret_cast<void(*)()>(function))
		{
		}
	};
}

#endif
