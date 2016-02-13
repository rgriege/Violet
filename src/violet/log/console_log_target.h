#ifndef VIOLET_CONSOLE_LOG_TARGET_H
#define VIOLET_CONSOLE_LOG_TARGET_H

#include "violet/core/defines.h"
#include "violet/log/log.h"

namespace vlt
{
	struct VIOLET_API console_log_target final : public log_target
	{
		virtual ~console_log_target() override;
		virtual void log(const char * entry) override;
	};
}

#endif