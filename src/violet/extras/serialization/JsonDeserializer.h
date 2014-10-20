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

		JsonDeserializer(std::istream & stream);

		virtual operator bool() const override;

	public:
		
		virtual JsonDeserializer & operator>>(bool & value) override;
		virtual JsonDeserializer & operator>>(uint8 & value) override;
		virtual JsonDeserializer & operator>>(int8 & value) override;
		virtual JsonDeserializer & operator>>(uint16 & value) override;
		virtual JsonDeserializer & operator>>(int16 & value) override;
		virtual JsonDeserializer & operator>>(uint32 & value) override;
		virtual JsonDeserializer & operator>>(int32 & value) override;
		virtual JsonDeserializer & operator>>(float & value) override;
		virtual JsonDeserializer & operator>>(double & value) override;
		virtual JsonDeserializer & operator>>(std::string & value) override;

	private:

		void moveToNextValue();

	private:

		Json::Value m_root;
		std::deque<std::pair<Json::ValueConstIterator, Json::ValueConstIterator>> m_valueIterators;
	};
}

#endif