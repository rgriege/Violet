#ifndef VIOLET_SCRIPT_FACTORY_H
#define VIOLET_SCRIPT_FACTORY_H

#include <memory>

#include "violet/core/defines.h"
#include "violet/script/script.h"
#include "violet/utility/factory.h"

namespace vlt
{
	struct VIOLET_API Script_Factory
	{
		typedef Factory<const char *, std::unique_ptr<Script>(const char *)> Factory_Type;

		static void assign(const char * label, const Factory_Type::Producer & producer);
		static std::unique_ptr<Script> create(const char * filename);
		static void remove(const char * label);

	private:

		Script_Factory() = delete;
		Script_Factory(const Script_Factory &) = delete;
		Script_Factory & operator=(const Script_Factory &) = delete;
	};
}

#endif
