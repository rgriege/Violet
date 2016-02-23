#ifndef VIOLET_LOG_H
#define VIOLET_LOG_H

#include "violet/core/defines.h"
#include "violet/core/types.h"

#include <memory>

namespace vlt
{
	struct VIOLET_API Log_Target
	{
		struct VIOLET_API Guard
		{
			const u32 handle;

			Guard(u32 handle);
			~Guard();
		};

		virtual ~Log_Target() = default;
		virtual void log(const char * entry) = 0;
	};

	VIOLET_API void log(const char * entry);
	VIOLET_API u32 install_log_target(std::unique_ptr<Log_Target> && target);
	VIOLET_API void remove_log_target(u32 id);
}

#endif
