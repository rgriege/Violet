#ifndef VIOLET_SystemFactory_H
#define VIOLET_SystemFactory_H

#include "violet/utility/Factory.h"

#include <memory>

namespace Violet
{
	class Deserializer;
	class System;
	class VIOLET_API SystemFactory : public Factory<const char *, std::unique_ptr<System>(Deserializer &)> {};
}

#endif