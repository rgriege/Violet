#ifndef VIOLET_Log_H
#define VIOLET_Log_H

#include "violet/Defines.h"

#include <memory>

namespace Violet
{
	class LogTarget;

	class VIOLET_API Log
	{
	public:

		static uint32 installTarget(std::unique_ptr<LogTarget> && logTarget);
		static void removeTarget(uint32 id);

		static void log(const char * entry);
	};
}

#endif