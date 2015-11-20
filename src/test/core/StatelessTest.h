#ifndef STATELESS_TEST_H
#define STATELESS_TEST_H

namespace Violet
{
	template <typename ResultType>
	class StatelessTest
	{
	public:

		typedef ResultType (*Predicate)();

		StatelessTest(const char * name, const ResultType & desired, Predicate predicate, const bool negated = false) :
			m_name(name),
			m_desired(desired),
			m_predicate(predicate),
			m_negated(negated)
		{
		}

        StatelessTest(StatelessTest && rhs) :
            m_name(rhs.m_name),
            m_desired(std::move(rhs.m_desired)),
            m_predicate(std::move(rhs.m_predicate)),
			m_negated(rhs.m_negated)
        {
        }

		template <typename TestEvaluator>
		bool evaluate(TestEvaluator & evaluator) const
		{
			ResultType const actual = m_predicate();
			return evaluator.evaluate(m_name, m_desired, actual, m_negated);
		}

    private:

		StatelessTest(const StatelessTest &) = delete;

	private:

		const char * m_name;
		ResultType m_desired;
		Predicate m_predicate;
		bool m_negated;
	};
}

#endif
