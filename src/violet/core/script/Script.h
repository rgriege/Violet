#ifndef SCRIPT_H
#define SCRIPT_H

namespace Violet
{
	class Entity;

	class Script
	{
	public:

		virtual ~Script() = default;

		virtual void run(const char * procedure, const Entity & entity) const = 0;
	};
}

#endif