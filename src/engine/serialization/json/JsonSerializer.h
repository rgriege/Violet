#ifndef VIOLET_JsonSerializer_H
#define VIOLET_JsonSerializer_H

#include "engine/serialization/Serializer.h"
#include "engine/Defines.h"

#include <ostream>
#include <json/json.h>

namespace Violet
{
	class VIOLET_API JsonSerializer : public Serializer
	{
	public:

		static void install();

	public:

		JsonSerializer(std::ostream & stream);
		JsonSerializer(JsonSerializer && other);
		virtual ~JsonSerializer() override;

		virtual unique_val<Serializer> createSegment(const char * label) override;

		virtual void writeBoolean(const char * label, bool value) override;
		virtual void writeUint(const char * label, uint32 value) override;
		virtual void writeInt(const char * label, int value) override;
		virtual void writeFloat(const char * label, float value) override;
		virtual void writeDouble(const char * label, double value) override;
		virtual void writeString(const char * label, const char * value) override;

	private:

		std::ostream & m_stream;
		Json::Value m_value;
	};
}

#endif
