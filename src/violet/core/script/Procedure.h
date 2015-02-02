#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "violet/core/script/Script.h"
#include "violet/core/script/CppScript.h"
#include "violet/core/template/TupleUtilities.h"

#include <tuple>

namespace Violet
{
	class Procedure
	{
	public:

		template <typename ... Args>
		class ProcedureImpl : public Script::ProcedureBase
		{
		public:

			ProcedureImpl(const char * name, Args && ... args) :
				ProcedureBase(name),
				m_args(std::forward_as_tuple(args...))
			{
			}

			virtual void run(CppScript & script) override
			{
				void * method = script.getMethodPtr(m_name);
				if (method != nullptr)
				{
					if (script.getMemoryPtr() == nullptr)
					{
						auto m = (void(*)(Args...)) method;
						m(get<Args>(m_args)...);
					}
					else
					{
						auto m = (void(*)(Args..., void *)) method;
						m(get<Args>(m_args)..., script.getMemoryPtr());
					}
				}
			}

			virtual void run(LuaScript & script) override
			{
			}

		private:

			const std::tuple<Args...> m_args;
		};

	public:

		template <typename ... Args>
		static ProcedureImpl<Args...> create(const char * name, Args && ... args)
		{
			return ProcedureImpl<Args...>(name, std::forward<Args>(args)...);
		}
	};
}

#endif
