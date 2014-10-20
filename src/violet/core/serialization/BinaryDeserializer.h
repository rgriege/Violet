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

		virtual BinaryDeserializer & operator>>(bool & value) override;
		virtual BinaryDeserializer & operator>>(uint8 & value) override;
		virtual BinaryDeserializer & operator>>(int8 & value) override;
		virtual BinaryDeserializer & operator>>(uint16 & value) override;
		virtual BinaryDeserializer & operator>>(int16 & value) override;
		virtual BinaryDeserializer & operator>>(uint32 & value) override;
		virtual BinaryDeserializer & operator>>(int32 & value) override;
		virtual BinaryDeserializer & operator>>(float & value) override;
		virtual BinaryDeserializer & operator>>(double & value) override;
		virtual BinaryDeserializer & operator>>(std::string & value) override;

	private:

		std::istream & m_stream;
	};
}

#endif
