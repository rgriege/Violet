#include "violet/test/Test.h"

#include <iostream>

using namespace Violet;

Test::Test(func f, std::string const name) :
	m_func(f),
	m_name(std::move(name))
{
}

bool Test::run() const
{
	std::string const error = m_func();
	bool const result = error.empty();
	std::cout << "\t" << m_name << ": " << (result ? "succeeded" : "failed (" + error + ")") << std::endl;
	return result;
}