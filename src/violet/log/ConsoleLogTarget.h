#ifndef VIOLET_ConsoleLogTarget_H
#define VIOLET_ConsoleLogTarget_H

#include "violet/log/LogTarget.h"

namespace Violet
{
	class VIOLET_API ConsoleLogTarget : public LogTarget
	{
	public:

		virtual ~ConsoleLogTarget() override;

		virtual void log(const char * entry) override;
	};
}

#endif