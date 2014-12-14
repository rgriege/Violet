#include "violet/core/script/ScriptFactory.h"

#include "violet/core/utility/StringUtilities.h"

#include <fstream>

using namespace Violet;

ScriptFactory & ScriptFactory::getInstance()
{
	static ScriptFactory ms_factory;
	return ms_factory;
}

std::unique_ptr<Script> ScriptFactory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::in))
		return nullptr;

	return Base::create(StringUtilities::right(filename, '.'), std::istream(&fb));
}