#ifndef VIOLET_LentPtr_H
#define VIOLET_LentPtr_H

#include <memory>

namespace Violet
{
	template <typename T>
	class lent_ptr
	{
	public:

		lent_ptr(T * ptr);
		lent_ptr(const std::unique_ptr<T> & ptr);

		T & operator*() const;
		T * operator->() const;

		bool operator==(const T * rhs) const;
		bool operator!=(const T * rhs) const;

	private:

		T * m_ptr;
	};
}

#include "violet/utility/lent_ptr.inl"

#endif