#ifndef VIOLET_CONSOLE_LOG_TARGET_H
#define VIOLET_CONSOLE_LOG_TARGET_H

#include "violet/core/defines.h"
#include "violet/log/log.h"

namespace vlt
{
	struct VIOLET_API Console_Log_Target final : public Log_Target
	{
		virtual ~Console_Log_Target() override;
		virtual void log(const char * entry) override;
	};
}

#endif
