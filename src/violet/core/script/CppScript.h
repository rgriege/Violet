#ifndef CPP_SCRIPT_H
#define CPP_SCRIPT_H

#include "violet/core/Defines.h"
#include "violet/core/script/Script.h"

#include "Windows.h"
#include <functional>

namespace Violet
{
	/*template <typename ... Args>
	class CppProcedureImpl : public Procedure
	{
	public:

		CppProcedureImpl(HMODULE lib, const char * name) :
			m_methodPtr(GetProcAddress(m_lib, m_name)),
			m_args()
		{
		}

		virtual void run() override
		{
			m_methodPtr(std::get<Args>(m_args)...);
		}

		template <typename T>
		void bind(T * t)
		{
			std::get<T*>(m_args) = t;
		}

	private:

		typedef void(*MethodPtr)(Args ... args);
		MethodPtr m_methodPtr;
		std::tuple<Args...> m_args;
	};*/

	class VIOLET_API CppScript : public Script
	{
	public:

		class VIOLET_API Allocator
		{
			friend class CppScript;

		public:

			template <typename T>
			void * allocate()
			{
				return allocate(sizeof(T));
			}

		private:

			Allocator();
			~Allocator();

			void * allocate(size_t size);
			void * fetch() const;

		private:

			void * m_memory;
		};

	public:

		static void install();

	public:

		CppScript(const char * filename);

	public:

		virtual ~CppScript() override;

		virtual std::string getFilename() const override;
		virtual void run(ProcedureBase & procedure) override;
		virtual void reload() override;

		void * getMethodPtr(const char * name) const;
		void * getMemoryPtr() const;

	private:

		std::string getFilenameWithPath() const;

		void load(const char * filename);
		void unload();

	private:

		HMODULE m_lib;
		Allocator m_allocator;
	};
}

#endif