#ifndef VIOLET_CPP_SCRIPT_H
#define VIOLET_CPP_SCRIPT_H

#include <memory>

#include "violet/script/script.h"

namespace vlt
{
	struct shared_library;

	struct VIOLET_API cpp_script final : public script
	{
		struct VIOLET_API instance
		{
			instance(cpp_script & script);
			virtual ~instance() = 0;

		protected:

			cpp_script & script;
		};

		static void install();

		cpp_script(const char * fileName);
		cpp_script(cpp_script && other);
		virtual ~cpp_script() override;

		virtual const std::string & get_filename() const override;
		virtual bool is_valid() const override;
		virtual void reload() override;

		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args) const;

	private:

		cpp_script(const cpp_script &) = delete;
		cpp_script & operator=(const cpp_script &) = delete;

		void * get_method_ptr(const char * name) const;

		void load();
		void unload();

	private:

		std::string m_fileName;
		std::shared_ptr<shared_library> m_lib;
		std::unique_ptr<instance> m_instance;
	};
}

#include "violet/script/cpp/cpp_script.inl"

#ifdef _WIN32
#define VIOLET_SCRIPT_EXPORT extern "C" __declspec(dllexport)
#else
#define VIOLET_SCRIPT_EXPORT extern "C"
#endif

#endif
