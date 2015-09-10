#ifndef VIOLET_Serializer_H
#define VIOLET_Serializer_H

#include "violet/Defines.h"
#include "violet/utility/unique_val.h"

namespace Violet
{
	class VIOLET_API Serializer
	{
	public:

		virtual ~Serializer() = default;

		virtual unique_val<Serializer> createSegment(const char * label) = 0;

		virtual void writeBoolean(const char * label, bool value) = 0;
		virtual void writeUint(const char * label, uint32 value) = 0;
		virtual void writeInt(const char * label, int value) = 0;
		virtual void writeFloat(const char * label, float value) = 0;
		virtual void writeDouble(const char * label, double value) = 0;
		virtual void writeString(const char * label, const char * value) = 0;
	};
}

#endif
