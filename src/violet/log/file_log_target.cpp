// ============================================================================

#include "violet/log/file_log_target.h"

using namespace vlt;

// ============================================================================

std::unique_ptr<file_log_target> file_log_target::create(const char * const filename)
{
	std::filebuf fb;
	return fb.open(filename, std::ios::out) ? std::make_unique<file_log_target>(std::move(fb)) : nullptr;
}

// ============================================================================

file_log_target::file_log_target(std::filebuf && file) :
	m_file(std::move(file)),
	m_stream(&m_file)
{
}

// ----------------------------------------------------------------------------

file_log_target::~file_log_target()
{
	m_stream << std::flush;
}

// ----------------------------------------------------------------------------

void file_log_target::log(const char * const entry)
{
	m_stream << entry << std::endl;
}

// ============================================================================
