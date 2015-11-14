#ifndef VIOLET_LogTarget_H
#define VIOLET_LogTarget_H

#include "violet/Defines.h"

namespace Violet
{
	class VIOLET_API LogTarget
	{
	public:

		class VIOLET_API Guard
		{
		public:

			Guard(uint32 handle);
			~Guard();

		private:

			const uint32 m_handle;
		};

	public:

		virtual ~LogTarget() = default;

		virtual void log(const char * entry) = 0;
	};
}

#endif 