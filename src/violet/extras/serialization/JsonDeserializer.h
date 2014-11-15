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
		JsonDeserializer(const JsonDeserializer & other);

		virtual operator bool() const override;

		virtual void enterSegment(const char * label) override;
		virtual void leaveSegment() override;
		
		virtual bool getBoolean(const char * label) override;
		virtual uint32 getUint(const char * label) override;
		virtual int32 getInt(const char * label) override;
		virtual float getFloat(const char * label) override;
		virtual double getDouble(const char * label) override;
		virtual const char * getString(const char * label) override;

	private:

		Json::Value m_root;
		std::deque<std::pair<Json::Value *, uint32>> m_stack;
		bool m_valid;
	};
}

#endif