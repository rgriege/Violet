#ifndef VIOLET_SYSTEM_FACTORY_H
#define VIOLET_SYSTEM_FACTORY_H

#include <memory>

#include "violet/utility/factory.h"

namespace vlt
{
	struct Deserializer;
	struct VIOLET_API System_Factory : public Factory<const char *, void(Deserializer &)> {};
}

#endif
