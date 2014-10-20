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
		
		virtual Deserializer & operator>>(bool & value) = 0;
		virtual Deserializer & operator>>(uint8 & value) = 0;
		virtual Deserializer & operator>>(int8 & value) = 0;
		virtual Deserializer & operator>>(uint16 & value) = 0;
		virtual Deserializer & operator>>(int16 & value) = 0;
		virtual Deserializer & operator>>(uint32 & value) = 0;
		virtual Deserializer & operator>>(int32 & value) = 0;
		virtual Deserializer & operator>>(float & value) = 0;
		virtual Deserializer & operator>>(double & value) = 0;
		virtual Deserializer & operator>>(std::string & value) = 0;
	};
}

#endif
