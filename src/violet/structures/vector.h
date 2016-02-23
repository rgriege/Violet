#ifndef VIOLET_VECTOR_H
#define VIOLET_VECTOR_H

#include <assert.h>

namespace vlt
{
	template <typename T, typename Allocator = std::allocator<T>>
	struct Vector
	{
	public:

		typedef T value_type;
		typedef Allocator allocator_type;
		typedef u32 size_type;
		typedef std::ptrdiff_t difference_type;
		typedef value_type * pointer;
		typedef const value_type * const_pointer;
		typedef value_type & reference;
		typedef const value_type & const_reference;

	public:

		template <bool IsConst>
		struct Iterator
		{
		public:

			typedef std::conditional_t<IsConst, typename Vector::const_pointer, typename Vector::pointer> pointer;
			typedef std::conditional_t<IsConst, typename Vector::const_reference, typename Vector::reference> reference;

		public:

			explicit Iterator(pointer ptr);

			Iterator<IsConst> & operator++();

			reference operator*();
			pointer operator->();

			bool operator==(const Iterator<IsConst> & rhs) const;
			bool operator!=(const Iterator<IsConst> & rhs) const;

		private:

			pointer m_ptr;
		};

		typedef Iterator<false> iterator;
		typedef Iterator<true> const_iterator;

	public:

		Vector();
		Vector(std::initializer_list<value_type> list);
		Vector(const Vector & rhs);
		Vector(Vector && rhs);
		~Vector();
		Vector & operator=(const Vector & rhs);
		Vector & operator=(Vector && rhs);

		reference operator[](size_type index);
		const_reference operator[](size_type index) const;

		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;

		pointer data();
		const_pointer data() const;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		bool empty() const;
		size_type size() const;
		void reserve(size_type capacity);
		size_type capacity() const;

		 void clear();
		template <typename ... Args>
		void emplace_back(Args && ... args);
		void resize(size_type size);
		void swap(Vector & rhs);

	private:

		void grow();

	private:

		pointer m_data;
		allocator_type m_allocator;
		size_type m_size;
		size_type m_capacity;
	};
}

#include "violet/structures/vector.inl"

#endif
