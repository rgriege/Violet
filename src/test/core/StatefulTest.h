#ifndef STATEFUL_TEST_H
#define STATEFUL_TEST_H

#include <functional>

namespace Violet
{
	template <typename ResultType, typename State>
	class StatefulTest
	{
	public:

		typedef std::function<ResultType(State &)> Predicate;

		StatefulTest(const char * name, const ResultType & desired, const Predicate & predicate, const bool negated = false) :
			m_name(name),
			m_desired(desired),
			m_predicate(predicate),
			m_negated(negated)
		{
		}

        StatefulTest(StatefulTest && rhs) :
            m_name(std::move(rhs.m_name)),
            m_desired(std::move(rhs.m_desired)),
            m_predicate(std::move(rhs.m_predicate)),
			m_negated(rhs.m_negated)
        {
        }

		template <typename TestEvaluator>
		bool evaluate(TestEvaluator & evaluator, State & state) const 
		{
			ResultType const actual = m_predicate(state);
			return evaluator.evaluate(m_name.c_str(), m_desired, actual, m_negated);
		}

    private:

		StatefulTest(const StatefulTest &) = delete;

	private:

		std::string m_name;
		ResultType m_desired;
		Predicate m_predicate;
		bool m_negated;
	};
}

#endif
