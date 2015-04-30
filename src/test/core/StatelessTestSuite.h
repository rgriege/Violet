#ifndef STATELESS_TEST_SUITE_H
#define STATELESS_TEST_SUITE_H

#include <utility>

namespace Violet
{
	template <typename... Tests>
	class StatelessTestSuite
	{
	public:

		StatelessTestSuite(const char * name, std::tuple<Tests...> tests) :
			m_name(name),
			m_tests(tests)
		{
		}

		StatelessTestSuite(const StatelessTestSuite &) = delete;

		template <typename Evaluator>
		bool evaluate(Evaluator & evaluator) const
		{
			evaluator.enterSuite();
			size_t actual = EvaluateHelper<decltype(m_tests), sizeof...(Tests)-1>::evaluate(m_tests, evaluator);
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
			static size_t evaluate(const Tuple & t, Evaluator & evaluator)
			{
				const size_t previous = EvaluateHelper<Tuple, Index - 1>::evaluate(t, evaluator);
				return previous + std::get<Index>(t).evaluate(evaluator);
			}
		};

		template <typename Tuple>
		struct EvaluateHelper<Tuple, 0>
		{
		public:

			template <typename Evaluator>
			static size_t evaluate(const Tuple & t, Evaluator & evaluator)
			{
				return std::get<0>(t).evaluate(evaluator);
			}
		};

	private:

		const char * m_name;
		std::tuple<Tests...> m_tests;
	};
}

#endif
