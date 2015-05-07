#ifndef VIOLET_System_H
#define VIOLET_System_H

#include "engine/Defines.h"

namespace Violet
{
	class Engine;

	class VIOLET_API System
    {
    public:

		System(const char * label);
		virtual ~System() = default;

		virtual void update(float dt, Engine & engine) = 0;

		const char * getLabel() const;

	private:

		const char * m_label;
    };
};

#endif