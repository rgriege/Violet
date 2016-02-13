#ifndef VIOLET_SHARED_LIBRARY_H
#define VIOLET_SHARED_LIBRARY_H

#include <memory>
#include <string>

#include "violet/core/defines.h"
#include "violet/utility/unique_val.h"

namespace vlt
{
	struct VIOLET_API shared_library final
	{
	private:

		struct implementation;

	public:

		static std::shared_ptr<shared_library> load(const char * filename);
		static const char * get_suffix();

	public:

		~shared_library();

		std::string get_filename() const;
		void * get_method_ptr(const char * name);

	private:

		shared_library(unique_val<implementation> && impl);

	private:

		unique_val<implementation> m_impl;
	};
}

#endif
