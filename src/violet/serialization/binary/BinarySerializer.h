#ifndef VIOLET_BinarySerializer_H
#define VIOLET_BinarySerializer_H

#include "violet/serialization/Serializer.h"

#include <ostream>

namespace Violet
{
	class VIOLET_API BinarySerializer : public Serializer
	{
	public:

		static void install();

	public:

		BinarySerializer(std::ostream & stream);
		virtual ~BinarySerializer() override = default;

		virtual unique_val<Serializer> createSegment(const char * label) override;

		virtual void writeBoolean(const char * label, bool value) override;
		virtual void writeUint(const char * label, uint32 value) override;
		virtual void writeInt(const char * label, int value) override;
		virtual void writeFloat(const char * label, float value) override;
		virtual void writeDouble(const char * label, double value) override;
		virtual void writeString(const char * label, const char * value) override;

	private:

		std::ostream & m_stream;
	};
}

#endif