#ifndef VIOLET_FILE_LOG_TARGET_H
#define VIOLET_FILE_LOG_TARGET_H

#include <fstream>
#include <memory>
#include <streambuf>

#include "violet/log/log.h"

namespace vlt
{
	struct VIOLET_API File_Log_Target final : public Log_Target
	{
		std::filebuf file;
		std::ostream stream;

		static std::unique_ptr<File_Log_Target> create(const char * filename);

		File_Log_Target(std::filebuf && file);
		virtual ~File_Log_Target() override;

		virtual void log(const char * entry) override;

	};
}

#endif
