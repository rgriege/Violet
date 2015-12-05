// ============================================================================

#include "violet/log/FileLogTarget.h"

using namespace Violet;

// ============================================================================

std::unique_ptr<FileLogTarget> FileLogTarget::create(const char * const filename)
{
	std::filebuf fb;
	return fb.open(filename, std::ios::out) ? std::make_unique<FileLogTarget>(std::move(fb)) : nullptr;
}

// ============================================================================

FileLogTarget::FileLogTarget(std::filebuf && file) :
	m_file(std::move(file)),
	m_stream(&m_file)
{
}

// ----------------------------------------------------------------------------

FileLogTarget::~FileLogTarget()
{
	m_stream << std::flush;
}

// ----------------------------------------------------------------------------

void FileLogTarget::log(const char * const entry)
{
	m_stream << entry << std::endl;
}

// ============================================================================
