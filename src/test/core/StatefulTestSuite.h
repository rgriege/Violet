#ifndef STATEFUL_TEST_SUITE_H
#define STATEFUL_TEST_SUITE_H

#include <utility>

namespace Violet
{
	template <typename State, typename... Tests>
	class StatefulTestSuite
	{
	public:

		StatefulTestSuite(const char * name, State && state, std::tuple<Tests...> && tests) :
			m_name(name),
			m_state(std::move(state)),
			m_tests(std::move(tests))
		{
		}

        StatefulTestSuite(StatefulTestSuite && rhs) :
            m_name(rhs.m_name),
            m_state(std::move(rhs.m_state)),
            m_tests(std::move(rhs.m_tests))
        {
        }

		template <typename Evaluator>
		bool evaluate(Evaluator & evaluator) const
		{
			evaluator.enterSuite();
			size_t actual = EvaluateHelper<decltype(m_tests), sizeof...(Tests)-1>::evaluate(m_tests, evaluator, m_state);
			const bool result = evaluator.evaluate(m_name, sizeof...(Tests), actual);
			evaluator.exitSuite();
			return result;
		}

	private:

		template <typename Tuple, size_t Index>
		struct EvaluateHelper
		{
		public:

			template <typename Evaluator>
			static size_t evaluate(const Tuple & t, Evaluator & evaluator, State & state)
			{
				const size_t previous = EvaluateHelper<Tuple, Index - 1>::evaluate(t, evaluator, state);
				return previous + std::get<Index>(t).evaluate(evaluator, state);
			}
		};

		template <typename Tuple>
		struct EvaluateHelper<Tuple, 0>
		{
		public:

			template <typename Evaluator>
			static size_t evaluate(const Tuple & t, Evaluator & evaluator, State & state)
			{
				return std::get<0>(t).evaluate(evaluator, state);
			}
		};

    private:

		StatefulTestSuite(const StatefulTestSuite &) = delete;

	private:

		const char * m_name;
		mutable State m_state;
		std::tuple<Tests...> m_tests;
	};
}

#endif
