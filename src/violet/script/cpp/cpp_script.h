#ifndef VIOLET_CPP_SCRIPT_H
#define VIOLET_CPP_SCRIPT_H

#include <memory>

#include "violet/script/script.h"

namespace vlt
{
	struct Shared_Library;

	struct VIOLET_API Cpp_Script final : public Script
	{
		struct VIOLET_API Instance
		{
			Instance(Cpp_Script & Script);
			virtual ~Instance() = 0;

		protected:

			Cpp_Script & script;
		};

		static void install();

		Cpp_Script(const char * fileName);
		Cpp_Script(Cpp_Script && other);
		virtual ~Cpp_Script() override;

		virtual const std::string & get_filename() const override;
		virtual bool is_valid() const override;
		virtual void reload() override;

		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args) const;

	private:

		Cpp_Script(const Cpp_Script &) = delete;
		Cpp_Script & operator=(const Cpp_Script &) = delete;

		void * get_method_ptr(const char * name) const;

		void load();
		void unload();

	private:

		std::string m_fileName;
		std::shared_ptr<Shared_Library> m_lib;
		std::unique_ptr<Instance> m_instance;
	};
}

#include "violet/script/cpp/cpp_script.inl"

#ifdef _WIN32
#define VIOLET_SCRIPT_EXPORT extern "C" __declspec(dllexport)
#else
#define VIOLET_SCRIPT_EXPORT extern "C"
#endif

#endif
