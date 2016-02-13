#ifndef VIOLET_SCRIPT_FACTORY_H
#define VIOLET_SCRIPT_FACTORY_H

#include <memory>

#include "violet/core/defines.h"
#include "violet/script/script.h"
#include "violet/utility/factory.h"

namespace vlt
{
	struct VIOLET_API script_factory
	{
		typedef factory<const char *, std::unique_ptr<script>(const char *)> factory_type;

		static void assign(const char * label, const factory_type::producer & producer);
		static std::unique_ptr<script> create(const char * fileName);
		static void remove(const char * label);

	private:

		static factory_type ms_instance;

	private:

		script_factory() = delete;
		script_factory(const script_factory &) = delete;
		script_factory & operator=(const script_factory &) = delete;
	};
}

#endif