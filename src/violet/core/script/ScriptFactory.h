#ifndef SCRIPT_FACTORY_H
#define SCRIPT_FACTORY_H

#include "violet/core/script/Script.h"
#include "violet/core/utility/Factory.h"

#include <iosfwd>
#include <memory>

namespace Violet
{
	class ScriptFactory : public Factory<const char *, std::unique_ptr<Script>(std::istream &)>
	{
	public:

		static ScriptFactory & getInstance();

	public:

		std::unique_ptr<Script> create(const char * filename);

	private:

		ScriptFactory() = default;
		ScriptFactory(const ScriptFactory &) = delete;
		ScriptFactory & operator=(const ScriptFactory &) = delete;

		typedef Factory<const char *, std::unique_ptr<Script>(std::istream &)> Base;
	};
}

#endif