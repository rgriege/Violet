#ifndef STATELESS_TEST_H
#define STATELESS_TEST_H

#include <functional>

namespace Violet
{
	template <typename ResultType>
	class StatelessTest
	{
	public:

		typedef std::function<ResultType()> Predicate;

		StatelessTest(const char * name, const ResultType & desired, const Predicate & predicate, const bool negated = false) :
			m_name(name),
			m_desired(desired),
			m_predicate(predicate),
			m_negated(negated)
		{
		}

        StatelessTest(StatelessTest && rhs) :
            m_name(std::move(rhs.m_name)),
            m_desired(std::move(rhs.m_desired)),
            m_predicate(std::move(rhs.m_predicate)),
			m_negated(rhs.m_negated)
        {
        }

		template <typename TestEvaluator>
		bool evaluate(TestEvaluator & evaluator) const
		{
			ResultType const actual = m_predicate();
			return evaluator.evaluate(m_name.c_str(), m_desired, actual, m_negated);
		}

    private:

		StatelessTest(const StatelessTest &) = delete;

	private:

		std::string m_name;
		ResultType m_desired;
		Predicate m_predicate;
		bool m_negated;
	};
}

#endif
