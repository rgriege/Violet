#ifndef VIOLET_SharedVal_H
#define VIOLET_SharedVal_H

#include <memory>

namespace Violet
{
	template <typename T>
	class shared_val
	{
		template <typename U, typename ... Args>
		friend shared_val<U> make_shared_val(Args && ... args);

		template <typename U>
		friend class shared_val;

	public:

		shared_val(const shared_val<T> & rhs);

		template <typename U>
		shared_val(const shared_val<U> & rhs);

		shared_val<T> & operator=(const shared_val<T> & rhs);

		T & get() const;
		T * ptr() const;

		T & operator*() const;
		T * operator->() const;

		operator T &() const;

	private:

		shared_val(std::shared_ptr<T> ptr);

	private:

		std::shared_ptr<T> m_ptr;
	};

	template <typename T, typename ... Args>
	shared_val<T> make_shared_val(Args && ... args);
}

#include "engine/utility/shared_val.inl"

#endif
