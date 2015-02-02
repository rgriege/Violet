#ifndef SCRIPT_H
#define SCRIPT_H

#include "violet/core/Defines.h"
#include "violet/core/template/TupleUtilities.h"

namespace Violet
{
	class CppScript;
	class LuaScript;
	
	class VIOLET_API Script
	{
	public:

		class ProcedureBase
		{
		public:

			ProcedureBase(const char * name) :
				m_name(name)
			{
			}

			virtual ~ProcedureBase() = default;

			virtual void run(CppScript & script) = 0;
			virtual void run(LuaScript & script) = 0;

		protected:

			const char * const m_name;
		};

	public:

		virtual ~Script() = default;

		virtual void run(ProcedureBase & procedure) = 0;
	};
}

#endif