#ifndef VIOLET_Log_H
#define VIOLET_Log_H

#include "violet/Defines.h"
#include "violet/utility/unique_val.h"

namespace Violet
{
	class LogTarget;

	class VIOLET_API Log
	{
	public:

		static uint32 installTarget(unique_val<LogTarget> && logTarget);
		static void removeTarget(uint32 id);

		static void log(const char * entry);
	};
}

#endif