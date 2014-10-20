#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include "violet/core/Defines.h"

#include <sstream>

namespace Violet
{
	class Deserializer
	{
	public:

		virtual ~Deserializer() {}

		virtual operator bool() const = 0;

		virtual void enterSegment(const char * label) = 0;
		virtual void leaveSegment() = 0;

		virtual bool getBoolean(const char * label) = 0;
		virtual uint32 getUint(const char * label) = 0;
		virtual int32 getInt(const char * label) = 0;
		virtual float getFloat(const char * label) = 0;
		virtual double getDouble(const char * label) = 0;
		virtual const char * getString(const char * label) = 0;
	};
}

#endif
