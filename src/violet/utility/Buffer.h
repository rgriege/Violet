#ifndef BUFFER_H
#define BUFFER_H

namespace Violet
{
	template <typename T, size_t N>
	class StackBuffer
	{
	public:

		StackBuffer() :
			m_data()
		{
		}

		operator T* () { return m_data; }

	private:

		T m_data[N];
	};

	template <typename T>
	class HeapBuffer
	{
	public:

		HeapBuffer(size_t n) :
			m_data(static_cast<T*>(malloc(n * sizeof(T))))
		{
		}

		~HeapBuffer()
		{
			free(m_data);
		}

		operator T* () { return m_data; }
		T* data() { return m_data; }

	private:

		T * const m_data;
	};
}

#endif
