#ifndef VIOLET_FILE_LOG_TARGET_H
#define VIOLET_FILE_LOG_TARGET_H

#include "violet/log/log.h"

#include <fstream>
#include <streambuf>
#include <memory>

namespace vlt
{
	struct VIOLET_API file_log_target : public log_target
	{
		std::filebuf m_file;
		std::ostream m_stream;

		static std::unique_ptr<file_log_target> create(const char * filename);

		file_log_target(std::filebuf && file);
		virtual ~file_log_target() override;

		virtual void log(const char * entry) override;

	};
}

#endif