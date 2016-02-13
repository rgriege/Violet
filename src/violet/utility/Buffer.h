#ifndef VIOLET_BUFFER_H
#define VIOLET_BUFFER_H

namespace vlt
{
	template <typename T, size_t N>
	struct stack_buffer
	{
		T data[N];

		stack_buffer() :
			data()
		{
		}

		operator T* () { return data; }
	};

	template <typename T>
	struct heap_buffer
	{
		T * const data;

		heap_buffer(size_t n) :
			data(static_cast<T*>(malloc(n * sizeof(T))))
		{
		}

		~heap_buffer()
		{
			free(data);
		}

		operator T* () { return data; }
	};
}

#endif
