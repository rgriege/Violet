#ifndef JSON_DESERIALIZER_H
#define JSON_DESERIALIZER_H

#include "violet/core/serialization/Deserializer.h"

#include <istream>
#include <json/json.h>

namespace Violet
{
	class JsonDeserializer : public Deserializer
	{
	public:

		static void install();

	public:

		JsonDeserializer(std::istream & stream);
		JsonDeserializer(JsonDeserializer && other);
		JsonDeserializer(const JsonDeserializer &) = delete;
		JsonDeserializer & operator=(const JsonDeserializer &) = delete;

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

		Json::Value m_root;
		Json::ValueConstIterator m_position;
	};
}

#endif