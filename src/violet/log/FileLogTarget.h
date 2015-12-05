#ifndef VIOLET_FileLogTarget_H
#define VIOLET_FileLogTarget_H

#include "violet/log/LogTarget.h"

#include <fstream>
#include <streambuf>
#include <memory>

namespace Violet
{
	class VIOLET_API FileLogTarget : public LogTarget
	{
	public:

		static std::unique_ptr<FileLogTarget> create(const char * filename);

	public:

		FileLogTarget(std::filebuf && file);
		virtual ~FileLogTarget() override;

		virtual void log(const char * entry) override;

	private:

		std::filebuf m_file;
		std::ostream m_stream;
	};
}

#endif