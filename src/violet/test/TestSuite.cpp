#include "violet/test/TestSuite.h"

#include <iostream>

using namespace Violet;

TestSuite::Result::Result(const uint32 succeeded, const uint32 total) :
	m_succeeded(succeeded),
	m_total(total)
{
}

TestSuite::Result::operator bool() const
{
	return m_succeeded == m_total;
}

TestSuite::TestSuite(const std::vector<Test> tests, std::string const name) :
	m_tests(std::move(tests)),
	m_name(std::move(name))
{
}

TestSuite::Result TestSuite::run()
{
	std::cout << "Running " << m_name << " tests" << std::endl;
	uint32 succeeded = 0;
	for(auto const & test : m_tests)
		if (test.run())
			++succeeded;

	std::cout << succeeded << "/" << m_tests.size() << " succeeded" << std::endl;

	return Result(succeeded, m_tests.size());
}