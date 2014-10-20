#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

template <>
const char * Deserializer::get<const char *>()
{
	return getValue();
}