#ifndef SCRIPT_H
#define SCRIPT_H

namespace Violet
{
	class Script
	{
	public:

		virtual ~Script() = default;

		virtual void run() const = 0;
	};
}

#endif