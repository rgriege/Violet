#ifndef TEST_H
#define TEST_H

#include <string>

namespace Violet
{
	class Test
	{
	public:

		typedef std::string (*func)();

	public:

		Test(func f, std::string name);

		bool run() const;

	private:

		func m_func;
		std::string m_name;
	};
}

#endif