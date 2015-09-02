#ifndef VIOLET_JsonDeserializer_H
#define VIOLET_JsonDeserializer_H

#include "violet/serialization/Deserializer.h"
#include "violet/Defines.h"

#include <istream>
#include <json/json.h>

namespace Violet
{
	class VIOLET_API JsonDeserializer : public Deserializer
	{
	public:

		static void install();

	public:

		JsonDeserializer(std::istream & stream);
		JsonDeserializer(JsonDeserializer && other);
		virtual ~JsonDeserializer() override = default;

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