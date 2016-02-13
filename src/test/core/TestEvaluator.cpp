#include "test/core/TestEvaluator.h"

using namespace vlt;

TestEvaluator::TestBuffer::TestBuffer(std::string const & entry) :
	m_entry(entry)
{
}

void TestEvaluator::TestBuffer::write(std::ostream & stream, int level)
{
	stream << std::string(2 * level, ' ') << m_entry << std::endl;
}


TestEvaluator::SuiteBuffer::SuiteBuffer(SuiteBuffer * parentBuffer) :
	m_parentBuffer(parentBuffer),
	m_buffers()
{
}

void TestEvaluator::SuiteBuffer::write(std::ostream & stream, int level)
{
	m_buffers.back()->write(stream, level);
	std::for_each(std::begin(m_buffers),
		std::end(m_buffers) - 1,
		[&](std::unique_ptr<BufferBase> const & buffer) { buffer->write(stream, level + 1); });
}

void TestEvaluator::SuiteBuffer::addEntry(std::string const & entry)
{
	m_buffers.emplace_back(new TestBuffer(entry));
}

TestEvaluator::SuiteBuffer * TestEvaluator::SuiteBuffer::enterSuite()
{
	auto subSuiteBuffer = new SuiteBuffer(this);
	m_buffers.emplace_back(subSuiteBuffer);
	return subSuiteBuffer;
}

TestEvaluator::SuiteBuffer * TestEvaluator::SuiteBuffer::exitSuite()
{
	return m_parentBuffer;
}

TestEvaluator::TestEvaluator(std::ostream & stream) :
	m_baseBuffer(nullptr),
	m_currentBuffer(&m_baseBuffer),
	m_stream(stream)
{
}

TestEvaluator::~TestEvaluator()
{
	m_baseBuffer.addEntry("Test Results");
	m_baseBuffer.write(m_stream, 0);
}

void TestEvaluator::enterSuite()
{
	m_currentBuffer = m_currentBuffer->enterSuite();
}

void TestEvaluator::exitSuite()
{
	m_currentBuffer = m_currentBuffer->exitSuite();
}
