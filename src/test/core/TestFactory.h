#ifndef TEST_FACTORY_H
#define TEST_FACTORY_H

#include "test/core/StatefulTest.h"
#include "test/core/StatefulTestSuite.h"
#include "test/core/StatelessTest.h"
#include "test/core/StatelessTestSuite.h"

namespace Violet
{
	class TestFactory
	{
	public:

		template <typename ResultType>
		static StatelessTest<ResultType> makeStateless(const char * name, const ResultType & desired, typename StatelessTest<ResultType>::Predicate predicate, const bool negated = false)
		{
			return StatelessTest<ResultType>(name, desired, predicate, negated);
		}

		template <typename State, typename ResultType>
		static StatefulTest<ResultType, State> makeStateful(const char * name, const ResultType & desired, typename StatefulTest<ResultType, State>::Predicate predicate, const bool negated = false)
		{
			return StatefulTest<ResultType, State>(name, desired, predicate);
		}
		
		template <typename... Tests>
		static StatelessTestSuite<Tests...> makeStatelessSuite(const char * name, std::tuple<Tests...> && tests)
		{
			return StatelessTestSuite<Tests...>(name, std::forward<std::tuple<Tests...>>(tests));
		}

		template <typename State, typename... Tests>
		static StatefulTestSuite<State, Tests...> makeStatefulSuite(const char * name, State && state, std::tuple<Tests...> && tests)
		{
			return StatefulTestSuite<State, Tests...>(name, std::forward<State>(state), std::forward<std::tuple<Tests...>>(tests));
		}
	};
}

#endif
