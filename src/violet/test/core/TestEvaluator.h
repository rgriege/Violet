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

			TestBuffer(std::string const & entry) :
				m_entry(entry)
			{
			}

			virtual void write(std::ostream & stream, int level) override
			{
				stream << std::string(2 * level, ' ') << m_entry << std::endl;
			}

		private:

			std::string const m_entry;
		};

		class SuiteBuffer : public BufferBase
		{
		public:

			SuiteBuffer(SuiteBuffer * parentBuffer) :
				m_parentBuffer(parentBuffer),
				m_buffers()
			{
			}

			virtual void write(std::ostream & stream, int level) override
			{
				m_buffers.back()->write(stream, level);
				std::for_each(std::begin(m_buffers),
					          std::end(m_buffers) - 1,
							  [&](std::unique_ptr<BufferBase> const & buffer) { buffer->write(stream, level + 1); });
			}

			void addEntry(std::string const & entry)
			{
				m_buffers.emplace_back(new TestBuffer(entry));
			}

			SuiteBuffer * enterSuite()
			{
				auto subSuiteBuffer = new SuiteBuffer(this);
				m_buffers.emplace_back(subSuiteBuffer);
				return subSuiteBuffer;
			}

			SuiteBuffer * exitSuite()
			{
				return m_parentBuffer;
			}

		private:

			SuiteBuffer * const m_parentBuffer;
			std::deque<std::unique_ptr<BufferBase>> m_buffers;
		};

	public:

		TestEvaluator(std::ostream & stream) :
			m_baseBuffer(nullptr),
			m_currentBuffer(&m_baseBuffer),
			m_stream(stream)
		{
		}

		~TestEvaluator()
		{
			m_baseBuffer.addEntry("Test Results");
			m_baseBuffer.write(m_stream, 0);
		}

		template <typename ResultType>
		bool evaluate(const char * name, const ResultType & desired, const ResultType & actual)
		{
			const bool result = (desired == actual);
			std::stringstream ss;
			ss << name << ": ";
			if (result)
				ss << "passed";
			else
				ss << "failed (expected " << desired << ", got " << actual << ")";
			m_currentBuffer->addEntry(ss.str());
			return result;
		}

		void enterSuite()
		{
			m_currentBuffer = m_currentBuffer->enterSuite();
		}

		void exitSuite()
		{
			m_currentBuffer = m_currentBuffer->exitSuite();
		}

	private:

		SuiteBuffer m_baseBuffer;
		SuiteBuffer * m_currentBuffer;
		std::ostream & m_stream;
	};
}

#endif
