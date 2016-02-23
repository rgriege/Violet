#ifndef VIOLET_SHARED_LIBRARY_H
#define VIOLET_SHARED_LIBRARY_H

#include <memory>
#include <string>

#include "violet/core/defines.h"
#include "violet/utility/unique_val.h"

namespace vlt
{
	struct VIOLET_API Shared_Library final
	{
	private:

		struct Implementation;

	public:

		static std::shared_ptr<Shared_Library> load(const char * filename);
		static const char * get_suffix();

	public:

		~Shared_Library();

		std::string get_filename() const;
		void * get_method_ptr(const char * name);

	private:

		Shared_Library(unique_val<Implementation> && impl);

	private:

		unique_val<Implementation> m_impl;
	};
}

#endif
