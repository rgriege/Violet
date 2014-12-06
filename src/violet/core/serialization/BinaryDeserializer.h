#ifndef BINARY_DESERIALIZER_H
#define BINARY_DERERIALIZER_H

#include "violet/core/Defines.h"
#include "violet/core/serialization/Deserializer.h"

#include <istream>

namespace Violet
{
	class BinaryDeserializer : public Deserializer
	{
	public:

		BinaryDeserializer(std::istream & stream);

		virtual operator bool() const override;

		virtual std::unique_ptr<Deserializer> enterSegment(const char * label) override;
		virtual const char * nextLabel() const override;
		
		virtual bool getBoolean(const char * label) override;
		virtual uint32 getUint(const char * label) override;
		virtual int32 getInt(const char * label) override;
		virtual float getFloat(const char * label) override;
		virtual double getDouble(const char * label) override;
		virtual const char * getString(const char * label) override;

	private:

		std::istream & m_stream;
		std::string m_stringBuffer;
	};
}

#endif
