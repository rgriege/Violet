#ifndef UNIQUE_VAL_H
#define UNIQUE_VAL_H

#include <memory>

namespace Violet
{
	template <typename T>
	class unique_val 
	{
	public:

		template <typename U, typename... Args>
		static unique_val<T> create(Args&&... args);

	public:

		T & operator*() const;
		T * operator->() const;

	private:

		unique_val(std::unique_ptr<T> && ptr);

	private:

		std::unique_ptr<T> m_ptr;
	};
}

#include "violet/core/utility/unique_val.ipp"

#endif