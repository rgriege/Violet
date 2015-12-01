#ifndef TEST_EVALUATOR_H
#define TEST_EVALUATOR_H

#include <ostream>
#include <sstream>
#include <deque>
#include <algorithm>
#include <memory>

namespace Violet
{
	class TestEvaluator
	{
	private:

		class BufferBase
		{
		public:

			virtual void write(std::ostream & stream, int level) = 0;
		};

		class TestBuffer : public BufferBase
		{
		public:

			TestBuffer(std::string const & entry);

			virtual void write(std::ostream & stream, int level) override;

		private:

			std::string const m_entry;
		};

		class SuiteBuffer : public BufferBase
		{
		public:

			SuiteBuffer(SuiteBuffer * parentBuffer);

			virtual void write(std::ostream & stream, int level) override;

			void addEntry(std::string const & entry);
			SuiteBuffer * enterSuite();
			SuiteBuffer * exitSuite();

		private:

			SuiteBuffer * const m_parentBuffer;
			std::deque<std::unique_ptr<BufferBase>> m_buffers;
		};

	public:

		TestEvaluator(std::ostream & stream);
		~TestEvaluator();

		template <typename ResultType>
		bool evaluate(const char * name, const ResultType & desired, const ResultType & actual, bool negated = false);
		void enterSuite();
		void exitSuite();

	private:

		SuiteBuffer m_baseBuffer;
		SuiteBuffer * m_currentBuffer;
		std::ostream & m_stream;
	};

	template <typename ResultType>
	bool TestEvaluator::evaluate(const char * name, const ResultType & desired, const ResultType & actual, const bool negated)
	{
		const bool result = (desired == actual) ^ negated;
		std::stringstream ss;
		ss << name << ": ";
		if (result)
			ss << "passed";
		else
			ss << "failed (" << (negated ? "didn't expect " : "expected ") << desired << ", got " << actual << ")";
		m_currentBuffer->addEntry(ss.str());
		return result;
	}
}

#endif
