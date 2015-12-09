#ifndef VIOLET_Delegate_H
#define VIOLET_Delegate_H

namespace Violet
{
	class DelegateStore
	{
	public:

		bool operator==(const DelegateStore & rhs) const
		{
			return m_instance == rhs.m_instance && m_function == rhs.m_function;
		}

	protected:

		DelegateStore(void * instance, void * function) :
			m_instance(instance),
			m_function(function)
		{
		}

	protected:

		void * m_instance;
		void * m_function;
	};

	template <typename Signature>
	class Delegate;
	template <typename ResultType, typename ... Args>
	class Delegate<ResultType(Args...)> : public DelegateStore
	{
	public:

		template <typename T, typename ResultType(T::*Member)(Args...)>
		static Delegate bind(T * instance)
		{
			ResultType(*function)(T*, Args...) = callMember<T, Member>;
			return Delegate(instance, function);
		}

	private:

		template <typename T, typename ResultType(T::*Member)(Args...)>
		static ResultType callMember(T * instance, Args ... args)
		{
			return (instance->*Member)(args...);
		}

	public:

		Delegate(const DelegateStore & store) :
			DelegateStore(store)
		{
		}

		Delegate(ResultType(*function)(Args...)) :
			DelegateStore(nullptr, function)
		{
		}

		ResultType operator()(Args && ... args) const
		{
			return m_instance == nullptr
				? static_cast<ResultType(*)(Args...)>(m_function)(std::forward<Args>(args)...)
				: static_cast<ResultType(*)(void *, Args...)>(m_function)(m_instance, std::forward<Args>(args)...);
		}

	private:

		template <typename T>
		Delegate(T * instance, ResultType(*function)(T*, Args...)) :
			DelegateStore(instance, function)
		{
		}
	};
}

#endif