#ifndef VIOLET_DELEGATE_H
#define VIOLET_DELEGATE_H

namespace vlt
{
	struct delegate_store
	{
		void * instance;
		void(*function)();

		delegate_store(void * instance, void(*function)()) :
			instance(instance),
			function(function)
		{
		}

		bool operator==(const delegate_store & rhs) const
		{
			return instance == rhs.instance && function == rhs.function;
		}
	};

	template <typename Signature>
	struct delegate;
	template <typename ResultType, typename ... Args>
	struct delegate<ResultType(Args...)> : public delegate_store
	{
		template <typename T, ResultType(T::*Member)(Args...)>
		static delegate bind(T * instance)
		{
			ResultType(*function)(T*, Args...) = call_member<T, Member>;
			return delegate(instance, function);
		}

	private:

		template <typename T, ResultType(T::*Member)(Args...)>
		static ResultType call_member(T * instance, Args ... args)
		{
			return (instance->*Member)(args...);
		}

	public:

		delegate(const delegate_store & store) :
			delegate_store(store)
		{
		}

		delegate(ResultType(*function)(Args...)) :
			delegate_store(nullptr, function)
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
		delegate(T * instance, ResultType(*function)(T*, Args...)) :
			delegate_store(instance, reinterpret_cast<void(*)()>(function))
		{
		}
	};
}

#endif
