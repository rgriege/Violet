#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include "violet/core/Defines.h"
#include "violet/test/Test.h"

#include <string>
#include <vector>

namespace Violet
{
	class TestSuite
	{
	public:

		class Result
		{
		public:

			Result(uint32 succeeded, uint32 total);

		public:

			operator bool() const;

		public:

			const uint32 m_succeeded;
			const uint32 m_total;
		};

	public:

		TestSuite(std::vector<Test> tests, std::string name);

		Result run();

	private:

		std::vector<Test> m_tests;
		std::string m_name;
	};
}

#endif