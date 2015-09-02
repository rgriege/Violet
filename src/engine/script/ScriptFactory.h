#ifndef VIOLET_ScriptFactory_H
#define VIOLET_ScriptFactory_H

#include "engine/Defines.h"
#include "engine/script/Script.h"
#include "engine/utility/Factory.h"

#include <memory>

namespace Violet
{
	class VIOLET_API ScriptFactory
	{
	public:

		typedef Factory<const char *, std::unique_ptr<Script>(const char *)> FactoryType;

	public:

		static void assign(const char * label, const FactoryType::Producer & producer);
		static std::unique_ptr<Script> create(const char * fileName);
		static void remove(const char * label);

	private:

		static FactoryType ms_instance;

	private:

		ScriptFactory() = delete;
		ScriptFactory(const ScriptFactory &) = delete;
		ScriptFactory & operator=(const ScriptFactory &) = delete;
	};
}

#endif