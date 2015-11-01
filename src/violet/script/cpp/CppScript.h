#ifndef VIOLET_CppScript_H
#define VIOLET_CppScript_H

#include "violet/script/Script.h"

#include <memory>

namespace Violet
{
    class SharedLibrary;

	class VIOLET_API CppScript : public Script
	{
	public:

		class VIOLET_API Instance
		{
		public:

			Instance(CppScript & script);
			virtual ~Instance() = 0;

		protected:

			CppScript & m_script;
		};

	public:

		static void install();

	public:

		CppScript(const char * fileName);
		CppScript(CppScript && other);
		virtual ~CppScript() override;

		virtual const std::string & getFileName() const override;
		virtual bool isValid() const override;
		virtual void reload() override;

		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args) const;

	private:

		CppScript(const CppScript &) = delete;
		CppScript & operator=(const CppScript &) = delete;

		void * getMethodPtr(const char * name) const;

		void load();
		void unload();

	private:

		std::string m_fileName;
		std::shared_ptr<SharedLibrary> m_lib;
		std::unique_ptr<Instance> m_instance;
	};
}

#include "violet/script/cpp/CppScript.inl"

#ifdef _WIN32
#define VIOLET_SCRIPT_EXPORT extern "C" __declspec(dllexport)
#else
#define VIOLET_SCRIPT_EXPORT extern "C"
#endif

#endif
