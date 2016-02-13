#ifndef VIOLET_SYSTEM_FACTORY_H
#define VIOLET_SYSTEM_FACTORY_H

#include "violet/utility/factory.h"

#include <memory>

namespace vlt
{
	struct deserializer;
	struct system;
	struct VIOLET_API system_factory : public factory<const char *, void(deserializer &)> {};
}

#endif