// ============================================================================

#include "violet/log/file_log_target.h"

using namespace vlt;

// ============================================================================

std::unique_ptr<File_Log_Target> File_Log_Target::create(const char * const filename)
{
	std::filebuf fb;
	return fb.open(filename, std::ios::out) ? std::make_unique<File_Log_Target>(std::move(fb)) : nullptr;
}

// ============================================================================

File_Log_Target::File_Log_Target(std::filebuf && _file) :
	file(std::move(_file)),
	stream(&file)
{
}

// ----------------------------------------------------------------------------

File_Log_Target::~File_Log_Target()
{
	stream << std::flush;
}

// ----------------------------------------------------------------------------

void File_Log_Target::log(const char * const entry)
{
	stream << entry << std::endl;
}

// ============================================================================
