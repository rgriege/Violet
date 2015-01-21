#ifndef SCRIPT_H
#define SCRIPT_H

#include "violet/core/Defines.h"

namespace Violet
{
	class AlterContext;
	class Entity;

	class VIOLET_API Script
	{
	public:

		virtual ~Script() = default;

		virtual void run(const char * procedure, const Entity & entity, AlterContext & context) const = 0;
	};
}

#endif