#ifndef SCRIPT_FACTORY_H
#define SCRIPT_FACTORY_H

#include "violet/core/utility/Factory.h"

#include <memory>

namespace Violet
{
	class Script;

	class VIOLET_API ScriptFactory : public Factory<const char *, std::unique_ptr<Script>(const char *)>
	{
	public:

		static ScriptFactory & getInstance();

	public:

		std::unique_ptr<Script> create(const char * filename);

	private:

		ScriptFactory() = default;
		ScriptFactory(const ScriptFactory &) = delete;
		ScriptFactory & operator=(const ScriptFactory &) = delete;

		typedef Factory<const char *, std::unique_ptr<Script>(const char *)> Base;
	};
}

#endif