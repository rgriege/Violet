#ifndef VIOLET_BUFFER_H
#define VIOLET_BUFFER_H

namespace vlt
{
	template <typename T, size_t N>
	struct Stack_Buffer
	{
		T data[N];

		Stack_Buffer() :
			data()
		{
		}

		operator T* () { return data; }
	};

	template <typename T>
	struct Heap_Buffer
	{
		T * const data;

		Heap_Buffer(size_t n) :
			data(static_cast<T*>(malloc(n * sizeof(T))))
		{
		}

		~Heap_Buffer()
		{
			free(data);
		}

		operator T* () { return data; }
	};
}

#endif
