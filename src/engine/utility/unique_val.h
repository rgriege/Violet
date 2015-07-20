#ifndef UNIQUE_VAL_H
#define UNIQUE_VAL_H

#include <memory>

namespace Violet
{
	template <typename T>
	class unique_val 
	{
		template <typename T, typename ... Args>
		friend unique_val<T> make_unique_val(Args && ... args);

		template <typename U>
		friend class unique_val;
		
	public:

		unique_val(unique_val<T> && rhs);

		template <typename U>
		unique_val(unique_val<U> && rhs);

		unique_val<T> & operator=(unique_val<T> && rhs);

		T & get() const;
		T * ptr() const;

		T & operator*() const;
		T * operator->() const;

		operator T &() const;

	private:

		unique_val(std::unique_ptr<T> && ptr);

	private:

		std::unique_ptr<T> m_ptr;
	};

	template <typename T, typename ... Args>
	unique_val<T> make_unique_val(Args && ... args);
}

#include "engine/utility/unique_val.inl"

#endif