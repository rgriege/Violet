#ifndef STATELESS_TEST_H
#define STATELESS_TEST_H

namespace Violet
{
	template <typename ResultType>
	class StatelessTest
	{
	public:

		typedef ResultType (*Predicate)();

		StatelessTest(const char * name, const ResultType & desired, Predicate predicate) :
			m_name(name),
			m_desired(desired),
			m_predicate(predicate)
		{
		}

		StatelessTest(const StatelessTest &) = delete;

		template <typename TestEvaluator>
		bool evaluate(TestEvaluator & evaluator) const
		{
			ResultType const actual = m_predicate();
			return evaluator.evaluate(m_name, m_desired, actual);
		}

	private:

		const char * m_name;
		ResultType m_desired;
		Predicate m_predicate;
	};
}

#endif
