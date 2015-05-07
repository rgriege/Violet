#ifndef VIOLET_Deserializer_H
#define VIOLET_Deserializer_H

#include "engine/Defines.h"

#include <memory>

namespace Violet
{
	class VIOLET_API Deserializer
	{
	public:

		Deserializer() = default;
		virtual ~Deserializer() = default;

		virtual operator bool() const = 0;

		virtual std::unique_ptr<Deserializer> enterSegment(const char * label) = 0;
		virtual const char * nextLabel() const = 0;

		virtual bool getBoolean(const char * label) = 0;
		virtual uint32 getUint(const char * label) = 0;
		virtual int32 getInt(const char * label) = 0;
		virtual float getFloat(const char * label) = 0;
		virtual double getDouble(const char * label) = 0;
		virtual const char * getString(const char * label) = 0;

	private:

		Deserializer(const Deserializer &) = delete;
		Deserializer & operator=(const Deserializer &) = delete;
	};
}

#endif
